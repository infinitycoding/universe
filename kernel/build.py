#!/bin/python

import os
import sys

print("Welcome to the universe build script!\n")

built = []

def dependices(mpath):
	deppath = "{}/dep.lst".format(mpath)
	if(os.path.isfile(deppath)):
		listf = open(deppath)
		deplst = []
		for line in listf.read().split('\n'):
			if(os.path.isdir(line)):
				deplst.append(line)
		return deplst
	else:
		return []

def compile_c_file(cccall, ipath):
	opath = "{}.o".format(ipath)
	cccall = "{} {} -c -o {}".format(cccall, ipath, opath)
	print(cccall)
	if(not os.system(cccall) == 0):
		print("BUILD ERROR. Aborting")
		exit()
	return opath

def compile_asm_file(cccall, ipath):
	tpath = "{}.pp".format(ipath)
	opath = "{}.o".format(ipath)

	cccall = "{} -x assembler-with-cpp -P -E {} -o {}".format(cccall, ipath, tpath)
	print(cccall)
	if(not os.system(cccall) == 0):
		print("BUILD ERROR. Aborting")
		exit()

	asmcall = "nasm -felf32 {} -o {}".format(tpath, opath)
	print(asmcall)
	if(not os.system(asmcall) == 0):
		print("BUILD ERROR. Aborting")
		exit()
	return opath

def compile_module_files(mpath, cccall):
	ofiles = []
	for fpath in os.listdir(mpath):
		if(fpath.endswith(".c")):
			ipath = "{}/{}".format(mpath, fpath)
			opath = compile_c_file(cccall, ipath)
			ofiles.append(opath)
		if(fpath.endswith(".asm")):
			ipath = "{}/{}".format(mpath, fpath)
			opath = compile_asm_file(cccall, ipath)
			ofiles.append(opath)
	return ofiles

def build_module(mpath, cccall):
	ofiles = []
	cccall = "{} -I{}/include".format(cccall, mpath)
	if(not mpath in built):
		print("building module {}..".format(mpath))
		built.append(mpath)

		for submod in dependices(mpath):
			cccall, sof = build_module(submod, cccall)
			ofiles.extend(sof)

		ofiles.extend(compile_module_files(mpath, cccall))
		print("built module {}".format(mpath))
	return cccall, ofiles

# compile all
cccall, ofiles = build_module(".", "i686-universe-gcc -Wall -g -nostdinc -fno-stack-protector -fno-builtin-log -fno-builtin")
# generate string
ostr = ""
for of in ofiles:
	ostr = "{} {}".format(ostr, of)

# generate linker script
os.system("i686-universe-gcc -x assembler-with-cpp -P -E -o linkerscript.x linkerscript.gen")

# link kernel
ldcall = "i686-universe-ld -Tlinkerscript.x -o kernel32.elf {}".format(ostr)
print(ldcall)
os.system(ldcall)

