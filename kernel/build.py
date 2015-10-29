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

def compile_c_file(includes, ipath):
	opath = "{}.o".format(ipath)
	cccall = "i686-universe-gcc -Wall -g -nostdinc -fno-stack-protector -fno-builtin-log -fno-builtin {} {} -c -o {}".format(includes, ipath, opath)
	print(cccall)
	if(not os.system(cccall) == 0):
		print("BUILD ERROR. Aborting")
		exit()
	return opath

def compile_asm_file(includes, ipath):
	tpath = "{}.pp".format(ipath)
	opath = "{}.o".format(ipath)

	cccall = "i686-universe-gcc -Wall -x assembler-with-cpp -P -E {} {} -o {}".format(includes, ipath, tpath)
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

def compile_module_files(mpath, includes):
	ofiles = []
	for fpath in os.listdir(mpath):
		inpath = ""
		for i in includes:
			inpath = "{} -I{}/include".format(inpath, i)

		if(fpath.endswith(".c")):
			ipath = "{}/{}".format(mpath, fpath)
			opath = compile_c_file(inpath, ipath)
			ofiles.append(opath)
		if(fpath.endswith(".asm")):
			ipath = "{}/{}".format(mpath, fpath)
			opath = compile_asm_file(inpath, ipath)
			ofiles.append(opath)
	return ofiles

def build_module(mpath, includes=[]):
	ofiles = []
	if(not mpath in includes):
		includes.append(mpath)

	if(not mpath in built):
		print("building module {}..".format(mpath))
		built.append(mpath)

		for submod in dependices(mpath):
			includes, sof = build_module(submod, includes)
			ofiles.extend(sof)

		ofiles.extend(compile_module_files(mpath, includes))
		print("built module {}".format(mpath))
	return includes, ofiles

# compile all
include, ofiles = build_module(".")
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

