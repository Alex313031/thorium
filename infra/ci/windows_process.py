#!/usr/bin/env python3
"""Stop the entire native build tree before saving a Windows checkpoint."""

import codecs
import ctypes
from ctypes import wintypes
from pathlib import Path
import signal
import subprocess
import sys
import time


class BasicLimits(ctypes.Structure):
    _fields_ = [("process_time", ctypes.c_int64), ("job_time", ctypes.c_int64),
                ("flags", wintypes.DWORD), ("min_working_set", ctypes.c_size_t),
                ("max_working_set", ctypes.c_size_t), ("active_process_limit", wintypes.DWORD),
                ("affinity", ctypes.c_size_t), ("priority", wintypes.DWORD),
                ("scheduling_class", wintypes.DWORD)]


class IOCounters(ctypes.Structure):
    _fields_ = [(name, ctypes.c_uint64) for name in
                ("read_count", "write_count", "other_count", "read_bytes", "write_bytes", "other_bytes")]


class ExtendedLimits(ctypes.Structure):
    _fields_ = [("basic", BasicLimits), ("io", IOCounters),
                ("process_memory", ctypes.c_size_t), ("job_memory", ctypes.c_size_t),
                ("peak_process_memory", ctypes.c_size_t), ("peak_job_memory", ctypes.c_size_t)]


class Accounting(ctypes.Structure):
    _fields_ = [(name, ctypes.c_int64) for name in ("user", "kernel", "period_user", "period_kernel")] + [
        (name, wintypes.DWORD) for name in ("faults", "total", "active", "terminated")]


class Job:
    def __init__(self):
        self.api = ctypes.WinDLL("kernel32", use_last_error=True)
        declarations = {
            "CreateJobObjectW": ([ctypes.c_void_p, wintypes.LPCWSTR], wintypes.HANDLE),
            "SetInformationJobObject": ([wintypes.HANDLE, ctypes.c_int, ctypes.c_void_p, wintypes.DWORD], wintypes.BOOL),
            "QueryInformationJobObject": ([wintypes.HANDLE, ctypes.c_int, ctypes.c_void_p, wintypes.DWORD, ctypes.c_void_p], wintypes.BOOL),
            "AssignProcessToJobObject": ([wintypes.HANDLE, wintypes.HANDLE], wintypes.BOOL),
            "OpenProcess": ([wintypes.DWORD, wintypes.BOOL, wintypes.DWORD], wintypes.HANDLE),
            "CloseHandle": ([wintypes.HANDLE], wintypes.BOOL),
            "GetConsoleProcessList": ([ctypes.POINTER(wintypes.DWORD), wintypes.DWORD], wintypes.DWORD),
            "AllocConsole": ([], wintypes.BOOL),
        }
        for name, (arguments, result) in declarations.items():
            function = getattr(self.api, name)
            function.argtypes, function.restype = arguments, result
        # Actions can start a shell without an attached console. CTRL_BREAK
        # requires the sender and child group to share one, even with file logs.
        console_process = wintypes.DWORD()
        if not self.api.GetConsoleProcessList(ctypes.byref(console_process), 1):
            self.check(self.api.AllocConsole())
        self.handle = self.check(self.api.CreateJobObjectW(None, None))
        limits = ExtendedLimits()
        limits.basic.flags = 0x2000  # JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE
        try:
            self.check(self.api.SetInformationJobObject(self.handle, 9, ctypes.byref(limits), ctypes.sizeof(limits)))
        except BaseException:
            self.close()
            raise

    @staticmethod
    def check(result):
        if not result:
            raise ctypes.WinError(ctypes.get_last_error())
        return result

    def assign(self, pid):
        # PROCESS_SET_QUOTA | PROCESS_TERMINATE; no broad process access needed.
        process = self.check(self.api.OpenProcess(0x0100 | 0x0001, False, pid))
        try:
            self.check(self.api.AssignProcessToJobObject(self.handle, process))
        finally:
            self.api.CloseHandle(process)

    def active(self):
        information = Accounting()
        self.check(self.api.QueryInformationJobObject(
            self.handle, 1, ctypes.byref(information), ctypes.sizeof(information), None))
        return information.active

    def close(self):
        if self.handle:
            self.api.CloseHandle(self.handle)
            self.handle = None


def stop(process, job):
    if process.poll() is None:
        process.send_signal(signal.CTRL_BREAK_EVENT)
    until = time.monotonic() + 120
    while job.active() and time.monotonic() < until:
        process.poll()
        time.sleep(0.2)
    if job.active():
        job.close()  # Kill the whole tree, including detached compiler children.
        process.wait()
        raise RuntimeError("build did not stop cleanly; keeping the previous checkpoint")
    return process.wait()


def run_command(command, cwd, env, deadline, log_path, forward_log):
    job = Job()
    process = None
    with log_path.open("wb") as log, log_path.open("rb") as live_log:
        decoder = codecs.getincrementaldecoder("utf-8")(errors="replace")
        try:
            # The gate prevents the wrapper from spawning children before it
            # belongs to our job. Descendants automatically inherit membership.
            process = subprocess.Popen([sys.executable, str(Path(__file__).resolve()), *command],
                                       cwd=cwd, env=env, stdin=subprocess.PIPE, stdout=log,
                                       stderr=subprocess.STDOUT,
                                       creationflags=subprocess.CREATE_NEW_PROCESS_GROUP)
            job.assign(process.pid)
            process.stdin.write(b"1")
            process.stdin.close()
            heartbeat = time.monotonic()
            while process.poll() is None:
                forward_log(live_log, decoder)
                if time.monotonic() - heartbeat >= 60:
                    print(f"Building; {max(0, deadline - time.monotonic()) / 60:.0f} minutes left in this stage", flush=True)
                    heartbeat = time.monotonic()
                if time.monotonic() >= deadline:
                    if process.poll() is not None:
                        break
                    code = stop(process, job)
                    if code == 0:
                        return "complete", code
                    if code != 130:
                        raise subprocess.CalledProcessError(code, command)
                    return "checkpointed", code
                time.sleep(0.5)
            code = process.wait()
            if code:
                raise subprocess.CalledProcessError(code, command)
            if job.active():
                stop(process, job)
                raise RuntimeError("build exited with live child processes")
            return "complete", code
        except BaseException:
            if process is not None and job.handle and job.active():
                stop(process, job)
            raise
        finally:
            job.close()
            # Assignment can fail before the gate opens. Do not leave it waiting.
            if process is not None and process.poll() is None:
                process.kill()
                process.wait()
            if process is not None and not process.stdin.closed:
                process.stdin.close()
            forward_log(live_log, decoder, final=True)


def gated_build():
    interrupted = False

    def on_break(signum, frame):
        nonlocal interrupted
        interrupted = True

    signal.signal(signal.SIGBREAK, on_break)
    if sys.stdin.buffer.read(1) != b"1":
        return 1
    code = subprocess.call(sys.argv[1:], stdin=subprocess.DEVNULL)
    # Other failures remain failures even when the deadline expires nearby.
    if interrupted and code in (130, 0xC000013A, -1073741510):
        return 130
    return code


if __name__ == "__main__":
    sys.exit(gated_build())
