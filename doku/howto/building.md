# Building Guide

## Dependencies
##### Compiling
* gcc
* nasm
* cdrkit/genisoimage
* make

##### Code Processing
* astyle
* splint

## Basic Building
1. Set your processor architecture and computer plaform in `/makefile`
2. `make dependencies` fetches all git submodules and builds the specific cross-compilter
3. `make all` finally builds the kernel and generates a iso-image

## Testing
Testing universe requires anything compatible with your universe platform. Maybe real hardware or a VM (e.g. Qemu)

##### Run In Qemu
simply with the makefile:
`make qemu`

