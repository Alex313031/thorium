"""
MIT License

Copyright (c) 2023 David Buchanan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

This is widevine_fixup.py version 0.1. Find the latest version here:
https://gist.github.com/DavidBuchanan314/c6b97add51b97e4c3ee95dc890f9e3c8
"""

import sys

if len(sys.argv) != 3:
	print(f"Usage: {sys.argv[0]} input.so output.so")
	exit()

"""
aarch64 widevine builds currently only support 4k page sizes.

This script fixes that, by pre-padding the LOAD segments so that they meet
the alignment constraints required by the loader, and then fixing up the
relevant header offsets to keep the file valid.

It also injects two functions that are not exported from arch's libgcc, into
the empty space at the end of the .text segment. This avoids any LD_PRELOAD
workarounds. (The injected functions are __aarch64_ldadd4_acq_rel
and __aarch64_swp4_acq_rel)

This script is tested against aarch64 libwidevinecdm.so version 4.10.2557.0, with
BuildID b71fd3342a03fdb140e1f7a757960df219d63625 and sha256sum
42ddbedb40f079a1dd63147456c12eb1ee0e986a0cd1a8b26e57ad9d119adbb4

You can conveniently obtain it, and all dependencies, by installing
https://aur.archlinux.org/packages/widevine-aarch64

This process is fragile, and may not work as-is on future revisions of widevine.

IMPORTANT NOTE: On systems with >4k page size (e.g. Apple Silicon devices),
Using the patched binary *significantly* weakens the security of your web browser,
in two ways. Firstly, it disables the RELRO security mitigation, and secondly it
creates a RWX mapping.

I'm looking into ways around this, but it will require more advanced patching.

P.S. If you want to watch Netflix, you will need to spoof a ChromeOS useragent
"""

import ctypes

class Elf64_Ehdr(ctypes.Structure):
	_fields_ = [
		('e_ident', ctypes.c_ubyte * 16),
		('e_type', ctypes.c_uint16),
		('e_machine', ctypes.c_uint16),
		('e_version', ctypes.c_uint32),
		('e_entry', ctypes.c_uint64),
		('e_phoff', ctypes.c_uint64),
		('e_shoff', ctypes.c_uint64),
		('e_flags', ctypes.c_uint32),
		('e_ehsize', ctypes.c_uint16),
		('e_phentsize', ctypes.c_uint16),
		('e_phnum', ctypes.c_uint16),
		('e_shentsize', ctypes.c_uint16),
		('e_shnum', ctypes.c_uint16),
		('e_shstrndx', ctypes.c_uint16),
	]

class Elf64_Phdr(ctypes.Structure):
	_fields_ = [
		('p_type', ctypes.c_uint32),
		('p_flags', ctypes.c_uint32),
		('p_offset', ctypes.c_uint64),
		('p_vaddr', ctypes.c_uint64),
		('p_paddr', ctypes.c_uint64),
		('p_filesz', ctypes.c_uint64),
		('p_memsz', ctypes.c_uint64),
		('p_align', ctypes.c_uint64),
	]

class P_FLAGS:
	""" Flag values for the p_flags field of program headers
	"""
	PF_X=0x1
	PF_W=0x2
	PF_R=0x4
	PF_MASKOS=0x00FF0000
	PF_MASKPROC=0xFF000000

class PT:
	PT_NULL=0
	PT_LOAD=1
	PT_DYNAMIC=2
	PT_INTERP=3
	PT_NOTE=4
	PT_SHLIB=5
	PT_PHDR=6
	PT_TLS=7
	PT_LOOS=0x60000000
	PT_HIOS=0x6fffffff

	PT_GNU_EH_FRAME=0x6474e550
	PT_GNU_STACK=0x6474e551
	PT_GNU_RELRO=0x6474e552
	PT_GNU_PROPERTY=0x6474e553

class Elf64_Shdr(ctypes.Structure):
	_fields_ = [
		('sh_name', ctypes.c_uint32),
		('sh_type', ctypes.c_uint32),
		('sh_flags', ctypes.c_uint64),
		('sh_addr', ctypes.c_uint64),
		('sh_offset', ctypes.c_uint64),
		('sh_size', ctypes.c_uint64),
		('sh_link', ctypes.c_uint32),
		('sh_info', ctypes.c_uint32),
		('sh_addralign', ctypes.c_uint64),
		('sh_entsize', ctypes.c_uint64),
	]

class Elf64_Sym(ctypes.Structure):
	_fields_ = [
		('st_name', ctypes.c_uint32),
		('st_info', ctypes.c_ubyte),
		('st_other', ctypes.c_ubyte),
		('st_shndx', ctypes.c_uint16),
		('st_value', ctypes.c_uint64),
		('st_size', ctypes.c_uint64),
	]

class Elf64_Dyn(ctypes.Structure):
	_fields_ = [
		('d_tag', ctypes.c_uint64),
		('d_val', ctypes.c_uint64), # union with d_ptr
	]

class D_TAG: # XXX: this is very incomplete
	DT_NULL=0
	DT_NEEDED=1
	DT_SONAME=14

class Elf64_Rela(ctypes.Structure):
	_fields_ = [
		('r_offset', ctypes.c_uint64),
		#('r_info', ctypes.c_uint64),
		('r_type', ctypes.c_uint32),
		('r_symbol', ctypes.c_uint32),
		('r_addend', ctypes.c_int64),
	]

import mmap
TARGET_PAGE_SIZE = 0x4000
WEAKEN_SECURITY = mmap.PAGESIZE > 0x1000
inject_addr = None

if WEAKEN_SECURITY:
	print("It looks like you're running Asahi, or some other device with >4k page size.")
	print("We will need to weaken certain memory permissions accordingly.")

"""
0000000000000b24 <__aarch64_ldadd4_acq_rel>:
 b24:   2a0003e2        mov     w2, w0
 b28:   885ffc20        ldaxr   w0, [x1]
 b2c:   0b020003        add     w3, w0, w2
 b30:   8804fc23        stlxr   w4, w3, [x1]
 b34:   35ffffa4        cbnz    w4, b28 <__aarch64_ldadd4_acq_rel+0x4>
 b38:   d65f03c0        ret

0000000000000b3c <__aarch64_swp4_acq_rel>:
 b3c:   2a0003e2        mov     w2, w0
 b40:   885ffc20        ldaxr   w0, [x1]
 b44:   8803fc22        stlxr   w3, w2, [x1]
 b48:   35ffffc3        cbnz    w3, b40 <__aarch64_swp4_acq_rel+0x4>
 b4c:   d65f03c0        ret
"""

injected_code = bytes.fromhex("e203002a20fc5f880300020b23fc0488a4ffff35c0035fd6e203002a20fc5f8822fc0388c3ffff35c0035fd6")



with open(sys.argv[1], "rb") as infile:
	elf = bytearray(infile.read())
elf_length = len(elf)
elf += bytearray(0x100000) # pre-expand the buffer by more than enough

ehdr = Elf64_Ehdr.from_buffer(elf)

phdrs = [
	Elf64_Phdr.from_buffer(memoryview(elf)[ehdr.e_phoff + i * ehdr.e_phentsize:])
	for i in range(ehdr.e_phnum)
]

adjustments = []

def adjust_offset(x):
	for index, delta in adjustments:
		if x >= index:
			x += delta
	return x

for phdr in phdrs:
	phdr.p_offset = adjust_offset(phdr.p_offset)
	if phdr.p_type == PT.PT_LOAD:
		phdr.p_align = TARGET_PAGE_SIZE
		delta_needed = (phdr.p_vaddr - phdr.p_offset) % phdr.p_align
		if delta_needed:
			print(f"inserting {hex(delta_needed)} bytes at offset {hex(phdr.p_offset)}")
			if not inject_addr:
				pad_bytes = injected_code + bytes(delta_needed - len(injected_code))
				inject_addr = phdr.p_offset
				print(f"also injecting code at offset {hex(phdr.p_offset)}")
			else:
				pad_bytes = bytes(delta_needed)
			elf[phdr.p_offset:] = pad_bytes + elf[phdr.p_offset:-delta_needed]
			adjustments.append((phdr.p_offset, delta_needed))
			elf_length += delta_needed
			phdr.p_offset += delta_needed

			if WEAKEN_SECURITY:
				phdr.p_flags |= P_FLAGS.PF_X # XXX: this is a hack!!! (at the very least, we should apply it only to the mappings that need it)

	if WEAKEN_SECURITY and phdr.p_type == PT.PT_GNU_RELRO:
		print("neutering relro") # XXX: relro is a security mechanism
		phdr.p_type = PT.PT_NOTE

# the section headers have moved
ehdr.e_shoff = adjust_offset(ehdr.e_shoff)

shdrs = [
	Elf64_Shdr.from_buffer(memoryview(elf)[ehdr.e_shoff + i * ehdr.e_shentsize:])
	for i in range(ehdr.e_shnum)
]

for shdr in shdrs:
	shdr.sh_offset = adjust_offset(shdr.sh_offset)

strtab = shdrs[ehdr.e_shstrndx]

def resolve_string(elf, strtab, stridx, count=False):
	if count:
		str_start = strtab.sh_offset
		for _ in range(stridx):
			str_start = elf.index(b"\0", str_start) + 1
	else:
		str_start = strtab.sh_offset + stridx
	str_end = elf.index(b"\0", str_start)
	return bytes(elf[str_start:str_end])

shdr_by_name = {
	resolve_string(elf, strtab, shdr.sh_name): shdr
	for shdr in shdrs
}

# XXX: unfortunately this does not do anything useful!
# It doesn't hurt either, so I'm leaving it here just in case.
dynsym = shdr_by_name[b".dynsym"]
dynstr = shdr_by_name[b".dynstr"]
for i in range(0, dynsym.sh_size, dynsym.sh_entsize):
	sym = Elf64_Sym.from_buffer(memoryview(elf)[dynsym.sh_offset + i:])
	name = resolve_string(elf, dynstr, sym.st_name)
	if name in [b"__aarch64_ldadd4_acq_rel", b"__aarch64_swp4_acq_rel"]:
		print(f"weak binding {name}")
		sym.st_info = (sym.st_info & 0x0f) | (2 << 4) # STB_WEAK

"""
dynamic = shdr_by_name[b".dynamic"]
for i in range(0, dynamic.sh_size, dynamic.sh_entsize):
	dyn = Elf64_Dyn.from_buffer(memoryview(elf)[dynamic.sh_offset + i:])
	if dyn.d_tag == D_TAG.DT_SONAME:
		print("hijacking SONAME tag to point to NEEDED libgcc_hide.so")
		dyn.d_tag = D_TAG.DT_NEEDED
		dyn.d_val = inject_addr - dynstr.sh_offset
		dynstr.sh_size = (inject_addr - dynstr.sh_offset) + len(PATH_TO_INJECT) + 1
"""

rela_plt = shdr_by_name[b".rela.plt"]
for i in range(0, rela_plt.sh_size, rela_plt.sh_entsize):
	rela = Elf64_Rela.from_buffer(memoryview(elf)[rela_plt.sh_offset + i:])
	sym = resolve_string(elf, dynstr, rela.r_symbol, count=True)
	if sym in [b"__aarch64_ldadd4_acq_rel", b"__aarch64_swp4_acq_rel"]:
		print(f"patching {sym} plt reloc to point into injected code")
		rela.r_type = 1027 # R_AARCH64_RELATIVE
		rela.r_addend = inject_addr
		if sym == b"__aarch64_swp4_acq_rel":
			rela.r_addend += 6*4

with open(sys.argv[2], "wb") as outfile:
	outfile.write(memoryview(elf)[:elf_length])

