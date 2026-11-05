#!/usr/bin/env python3
"""Preserve the full source tree and exact timestamps on NTFS."""

from decimal import Decimal
from functools import cache
import os
from pathlib import Path
import shutil
import subprocess
import tarfile


@cache
def windows_file_api():
    import ctypes
    from ctypes import wintypes

    api = ctypes.WinDLL("kernel32", use_last_error=True)
    api.CreateFileW.argtypes = [wintypes.LPCWSTR, wintypes.DWORD, wintypes.DWORD,
                               ctypes.c_void_p, wintypes.DWORD, wintypes.DWORD, wintypes.HANDLE]
    api.CreateFileW.restype = wintypes.HANDLE
    api.SetFileTime.argtypes = [wintypes.HANDLE, ctypes.POINTER(wintypes.FILETIME),
                               ctypes.POINTER(wintypes.FILETIME), ctypes.POINTER(wintypes.FILETIME)]
    api.SetFileTime.restype = wintypes.BOOL
    api.CloseHandle.argtypes = [wintypes.HANDLE]
    api.CloseHandle.restype = wintypes.BOOL
    return api


def restore_windows_link_time(path, ns):
    """Set the link's own timestamp on Python versions without Windows lutime."""
    import ctypes
    from ctypes import wintypes

    api = windows_file_api()
    # Do not resolve the path: that would select the link target. Extended paths
    # also cover source entries longer than MAX_PATH and dangling symlinks.
    name = os.path.abspath(path)
    if not name.startswith("\\\\?\\"):
        name = "\\\\?\\UNC\\" + name[2:] if name.startswith("\\\\") else "\\\\?\\" + name
    ticks = ns // 100 + 116444736000000000  # 100 ns intervals since 1601-01-01.
    if not 0 <= ticks < 2 ** 64:
        raise ValueError(f"timestamp is outside the Windows FILETIME range: {path}")
    timestamp = wintypes.FILETIME(ticks & 0xffffffff, ticks >> 32)
    # FILE_WRITE_ATTRIBUTES; share read/write/delete; OPEN_EXISTING;
    # FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS.
    handle = api.CreateFileW(name, 0x0100, 0x0007, None, 3, 0x00200000 | 0x02000000, None)
    if handle == ctypes.c_void_p(-1).value:
        raise ctypes.WinError(ctypes.get_last_error())
    try:
        if not api.SetFileTime(handle, None, ctypes.byref(timestamp), ctypes.byref(timestamp)):
            raise ctypes.WinError(ctypes.get_last_error())
    finally:
        api.CloseHandle(handle)


def make_directory(path):
    path = Path(path).resolve()
    path.mkdir(parents=True, exist_ok=False)
    # The full export contains case-distinct filenames, including unused host
    # dependencies. Preserve them instead of pruning source for Windows.
    subprocess.run(["fsutil.exe", "file", "setCaseSensitiveInfo", str(path), "enable"], check=True)


def restore_time(path, member):
    value = member.pax_headers.get("mtime", str(member.mtime))
    ns = int(Decimal(value) * 1_000_000_000)
    if os.name == "nt" and os.utime not in os.supports_follow_symlinks:
        if member.issym():
            restore_windows_link_time(path, ns)
        else:
            os.utime(path, ns=(ns, ns))
    else:
        os.utime(path, ns=(ns, ns), follow_symlinks=False)


def extract_tar(package, destination):
    destination = Path(destination).resolve()
    links, directories = [], []
    for original in package:
        member = tarfile.data_filter(original, str(destination))
        if member is None:
            continue
        path = destination / member.name
        if member.isdir():
            path.mkdir(parents=True, exist_ok=True)
            directories.append((path, original))
        elif member.isfile():
            path.parent.mkdir(parents=True, exist_ok=True)
            if path.exists():
                path.chmod(0o600)
                path.unlink()
            with package.extractfile(original) as src, path.open("wb") as out:
                shutil.copyfileobj(src, out, 1024 * 1024)
            restore_time(path, original)
        elif member.issym() or member.islnk():
            links.append((path, member))
        else:
            raise ValueError(f"unsupported archive entry: {member.name}")
    # On Windows symlinks need the target type. Delay them until their targets
    # exist, including chains of directory symlinks, without copying file data.
    while links:
        remaining = []
        for path, member in links:
            target = (path.parent if member.issym() else destination) / member.linkname
            if not target.resolve().is_relative_to(destination):
                raise ValueError(f"escaping archive link: {member.name}")
            if not target.exists():
                remaining.append((path, member))
                continue
            path.parent.mkdir(parents=True, exist_ok=True)
            path.unlink(missing_ok=True)
            if member.issym():
                path.symlink_to(member.linkname, target_is_directory=target.is_dir())
            else:
                os.link(target, path)
            restore_time(path, member)
        if len(remaining) == len(links):
            # Dangling symlinks in unused upstream data remain symlinks. Their
            # target type cannot be inferred; a trailing slash denotes a dir.
            for path, member in remaining:
                if not member.issym():
                    raise ValueError(f"unresolved hardlink: {member.name}")
                path.parent.mkdir(parents=True, exist_ok=True)
                path.unlink(missing_ok=True)
                path.symlink_to(member.linkname, target_is_directory=member.linkname.endswith("/"))
                restore_time(path, member)
            break
        links = remaining
    for path, member in reversed(directories):
        restore_time(path, member)


def add_tree(package, root):
    root = Path(root)
    pending = [root]
    while pending:
        path = pending.pop()
        name = path.relative_to(root).as_posix() if path != root else "."
        member = package.gettarinfo(str(path), arcname=name)
        ns = path.lstat().st_mtime_ns
        member.pax_headers["mtime"] = f"{ns // 1_000_000_000}.{ns % 1_000_000_000:09d}"
        if member.isfile():
            with path.open("rb") as stream:
                package.addfile(member, stream)
        else:
            package.addfile(member)
        if member.isdir():
            pending.extend(path.iterdir())


def pack_checkpoint(work, destination):
    with destination.open("wb") as output:
        process = subprocess.Popen(["zstd", "-T2", "-3", "-c"], stdin=subprocess.PIPE, stdout=output)
        try:
            with tarfile.open(fileobj=process.stdin, mode="w|", format=tarfile.PAX_FORMAT) as package:
                add_tree(package, work)
            process.stdin.close()
            if process.wait():
                raise RuntimeError("checkpoint compression failed")
        finally:
            if process.poll() is None:
                process.kill()
                process.wait()


def restore_checkpoint(payload, work):
    make_directory(work)
    process = subprocess.Popen(["zstd", "-d", "-c", str(payload)], stdout=subprocess.PIPE)
    try:
        with tarfile.open(fileobj=process.stdout, mode="r|") as package:
            extract_tar(package, work)
        # Consume tar padding so zstd can finish and verify its checksum.
        while process.stdout.read(1024 * 1024):
            pass
        if process.wait():
            raise RuntimeError("checkpoint decompression failed")
    finally:
        process.stdout.close()
        if process.poll() is None:
            process.kill()
            process.wait()
