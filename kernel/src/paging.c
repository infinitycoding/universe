/*
	Copyright 2012 universe coding group (UCG) all rights reserved
	This file is part of the Universe Kernel.

    Universe Kernel is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    Universe Kernel is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Universe Kernel.  If not, see <http://www.gnu.org/licenses/>.



    Diese Datei ist ein Teil vom Universe Kernel.

    Das Universe Kernel ist Freie Software: Sie können es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder jeder späteren
    veröffentlichten Version, weiterverbreiten und/oder modifizieren.

    Das Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
    Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
    OHNE JEDE GEWÄHELEISTUNG, bereitgestellt; sogar ohne die implizite
    Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License für weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*/

/**
	@author Tom Slawik <tom.slawik@gmail.com>
*/

#include <paging.h>
#include <pmm.h>
#include "memory_layout.h"

#define BP_MACRO2(X) ((X) < (MEMORY_LAYOUT_DIRECT_MAPPED / 0x00400000) ? (((X) * 0x00400000) | PDE_4MB | PDE_WRITABLE | PDE_PRESENT) : 0)
#define BP_MACRO(X) (X >= (MEMORY_LAYOUT_KERNEL_START / 0x00400000) ? BP_MACRO2(X - MEMORY_LAYOUT_KERNEL_START / 0x00400000) : BP_MACRO2(X))

pde_t const BOOT_PDE[1024] __attribute__ ((aligned (PAGE_SIZE))) = {
		BP_MACRO(0x000), BP_MACRO(0x001), BP_MACRO(0x002), BP_MACRO(0x003),
		BP_MACRO(0x004), BP_MACRO(0x005), BP_MACRO(0x006), BP_MACRO(0x007),
		BP_MACRO(0x008), BP_MACRO(0x009), BP_MACRO(0x00A), BP_MACRO(0x00B),
		BP_MACRO(0x00C), BP_MACRO(0x00D), BP_MACRO(0x00E), BP_MACRO(0x00F),

		BP_MACRO(0x010), BP_MACRO(0x011), BP_MACRO(0x012), BP_MACRO(0x013),
		BP_MACRO(0x014), BP_MACRO(0x015), BP_MACRO(0x016), BP_MACRO(0x017),
		BP_MACRO(0x018), BP_MACRO(0x019), BP_MACRO(0x01A), BP_MACRO(0x01B),
		BP_MACRO(0x01C), BP_MACRO(0x01D), BP_MACRO(0x01E), BP_MACRO(0x01F),

		BP_MACRO(0x020), BP_MACRO(0x021), BP_MACRO(0x022), BP_MACRO(0x023),
		BP_MACRO(0x024), BP_MACRO(0x025), BP_MACRO(0x026), BP_MACRO(0x027),
		BP_MACRO(0x028), BP_MACRO(0x029), BP_MACRO(0x02A), BP_MACRO(0x02B),
		BP_MACRO(0x02C), BP_MACRO(0x02D), BP_MACRO(0x02E), BP_MACRO(0x02F),

		BP_MACRO(0x030), BP_MACRO(0x031), BP_MACRO(0x032), BP_MACRO(0x033),
		BP_MACRO(0x034), BP_MACRO(0x035), BP_MACRO(0x036), BP_MACRO(0x037),
		BP_MACRO(0x038), BP_MACRO(0x039), BP_MACRO(0x03A), BP_MACRO(0x03B),
		BP_MACRO(0x03C), BP_MACRO(0x03D), BP_MACRO(0x03E), BP_MACRO(0x03F),

		BP_MACRO(0x040), BP_MACRO(0x041), BP_MACRO(0x042), BP_MACRO(0x043),
		BP_MACRO(0x044), BP_MACRO(0x045), BP_MACRO(0x046), BP_MACRO(0x047),
		BP_MACRO(0x048), BP_MACRO(0x049), BP_MACRO(0x04A), BP_MACRO(0x04B),
		BP_MACRO(0x04C), BP_MACRO(0x04D), BP_MACRO(0x04E), BP_MACRO(0x04F),

		BP_MACRO(0x050), BP_MACRO(0x051), BP_MACRO(0x052), BP_MACRO(0x053),
		BP_MACRO(0x054), BP_MACRO(0x055), BP_MACRO(0x056), BP_MACRO(0x057),
		BP_MACRO(0x058), BP_MACRO(0x059), BP_MACRO(0x05A), BP_MACRO(0x05B),
		BP_MACRO(0x05C), BP_MACRO(0x05D), BP_MACRO(0x05E), BP_MACRO(0x05F),

		BP_MACRO(0x060), BP_MACRO(0x061), BP_MACRO(0x062), BP_MACRO(0x063),
		BP_MACRO(0x064), BP_MACRO(0x065), BP_MACRO(0x066), BP_MACRO(0x067),
		BP_MACRO(0x068), BP_MACRO(0x069), BP_MACRO(0x06A), BP_MACRO(0x06B),
		BP_MACRO(0x06C), BP_MACRO(0x06D), BP_MACRO(0x06E), BP_MACRO(0x06F),

		BP_MACRO(0x070), BP_MACRO(0x071), BP_MACRO(0x072), BP_MACRO(0x073),
		BP_MACRO(0x074), BP_MACRO(0x075), BP_MACRO(0x076), BP_MACRO(0x077),
		BP_MACRO(0x078), BP_MACRO(0x079), BP_MACRO(0x07A), BP_MACRO(0x07B),
		BP_MACRO(0x07C), BP_MACRO(0x07D), BP_MACRO(0x07E), BP_MACRO(0x07F),

		BP_MACRO(0x080), BP_MACRO(0x081), BP_MACRO(0x082), BP_MACRO(0x083),
		BP_MACRO(0x084), BP_MACRO(0x085), BP_MACRO(0x086), BP_MACRO(0x087),
		BP_MACRO(0x088), BP_MACRO(0x089), BP_MACRO(0x08A), BP_MACRO(0x08B),
		BP_MACRO(0x08C), BP_MACRO(0x08D), BP_MACRO(0x08E), BP_MACRO(0x08F),

		BP_MACRO(0x090), BP_MACRO(0x091), BP_MACRO(0x092), BP_MACRO(0x093),
		BP_MACRO(0x094), BP_MACRO(0x095), BP_MACRO(0x096), BP_MACRO(0x097),
		BP_MACRO(0x098), BP_MACRO(0x099), BP_MACRO(0x09A), BP_MACRO(0x09B),
		BP_MACRO(0x09C), BP_MACRO(0x09D), BP_MACRO(0x09E), BP_MACRO(0x09F),

		BP_MACRO(0x0A0), BP_MACRO(0x0A1), BP_MACRO(0x0A2), BP_MACRO(0x0A3),
		BP_MACRO(0x0A4), BP_MACRO(0x0A5), BP_MACRO(0x0A6), BP_MACRO(0x0A7),
		BP_MACRO(0x0A8), BP_MACRO(0x0A9), BP_MACRO(0x0AA), BP_MACRO(0x0AB),
		BP_MACRO(0x0AC), BP_MACRO(0x0AD), BP_MACRO(0x0AE), BP_MACRO(0x0AF),

		BP_MACRO(0x0B0), BP_MACRO(0x0B1), BP_MACRO(0x0B2), BP_MACRO(0x0B3),
		BP_MACRO(0x0B4), BP_MACRO(0x0B5), BP_MACRO(0x0B6), BP_MACRO(0x0B7),
		BP_MACRO(0x0B8), BP_MACRO(0x0B9), BP_MACRO(0x0BA), BP_MACRO(0x0BB),
		BP_MACRO(0x0BC), BP_MACRO(0x0BD), BP_MACRO(0x0BE), BP_MACRO(0x0BF),

		BP_MACRO(0x0C0), BP_MACRO(0x0C1), BP_MACRO(0x0C2), BP_MACRO(0x0C3),
		BP_MACRO(0x0C4), BP_MACRO(0x0C5), BP_MACRO(0x0C6), BP_MACRO(0x0C7),
		BP_MACRO(0x0C8), BP_MACRO(0x0C9), BP_MACRO(0x0CA), BP_MACRO(0x0CB),
		BP_MACRO(0x0CC), BP_MACRO(0x0CD), BP_MACRO(0x0CE), BP_MACRO(0x0CF),

		BP_MACRO(0x0D0), BP_MACRO(0x0D1), BP_MACRO(0x0D2), BP_MACRO(0x0D3),
		BP_MACRO(0x0D4), BP_MACRO(0x0D5), BP_MACRO(0x0D6), BP_MACRO(0x0D7),
		BP_MACRO(0x0D8), BP_MACRO(0x0D9), BP_MACRO(0x0DA), BP_MACRO(0x0DB),
		BP_MACRO(0x0DC), BP_MACRO(0x0DD), BP_MACRO(0x0DE), BP_MACRO(0x0DF),

		BP_MACRO(0x0E0), BP_MACRO(0x0E1), BP_MACRO(0x0E2), BP_MACRO(0x0E3),
		BP_MACRO(0x0E4), BP_MACRO(0x0E5), BP_MACRO(0x0E6), BP_MACRO(0x0E7),
		BP_MACRO(0x0E8), BP_MACRO(0x0E9), BP_MACRO(0x0EA), BP_MACRO(0x0EB),
		BP_MACRO(0x0EC), BP_MACRO(0x0ED), BP_MACRO(0x0EE), BP_MACRO(0x0EF),

		BP_MACRO(0x0F0), BP_MACRO(0x0F1), BP_MACRO(0x0F2), BP_MACRO(0x0F3),
		BP_MACRO(0x0F4), BP_MACRO(0x0F5), BP_MACRO(0x0F6), BP_MACRO(0x0F7),
		BP_MACRO(0x0F8), BP_MACRO(0x0F9), BP_MACRO(0x0FA), BP_MACRO(0x0FB),
		BP_MACRO(0x0FC), BP_MACRO(0x0FD), BP_MACRO(0x0FE), BP_MACRO(0x0FF),

		BP_MACRO(0x100), BP_MACRO(0x101), BP_MACRO(0x102), BP_MACRO(0x103),
		BP_MACRO(0x104), BP_MACRO(0x105), BP_MACRO(0x106), BP_MACRO(0x107),
		BP_MACRO(0x108), BP_MACRO(0x109), BP_MACRO(0x10A), BP_MACRO(0x10B),
		BP_MACRO(0x10C), BP_MACRO(0x10D), BP_MACRO(0x10E), BP_MACRO(0x10F),

		BP_MACRO(0x110), BP_MACRO(0x111), BP_MACRO(0x112), BP_MACRO(0x113),
		BP_MACRO(0x114), BP_MACRO(0x115), BP_MACRO(0x116), BP_MACRO(0x117),
		BP_MACRO(0x118), BP_MACRO(0x119), BP_MACRO(0x11A), BP_MACRO(0x11B),
		BP_MACRO(0x11C), BP_MACRO(0x11D), BP_MACRO(0x11E), BP_MACRO(0x11F),

		BP_MACRO(0x120), BP_MACRO(0x121), BP_MACRO(0x122), BP_MACRO(0x123),
		BP_MACRO(0x124), BP_MACRO(0x125), BP_MACRO(0x126), BP_MACRO(0x127),
		BP_MACRO(0x128), BP_MACRO(0x129), BP_MACRO(0x12A), BP_MACRO(0x12B),
		BP_MACRO(0x12C), BP_MACRO(0x12D), BP_MACRO(0x12E), BP_MACRO(0x12F),

		BP_MACRO(0x130), BP_MACRO(0x131), BP_MACRO(0x132), BP_MACRO(0x133),
		BP_MACRO(0x134), BP_MACRO(0x135), BP_MACRO(0x136), BP_MACRO(0x137),
		BP_MACRO(0x138), BP_MACRO(0x139), BP_MACRO(0x13A), BP_MACRO(0x13B),
		BP_MACRO(0x13C), BP_MACRO(0x13D), BP_MACRO(0x13E), BP_MACRO(0x13F),

		BP_MACRO(0x140), BP_MACRO(0x141), BP_MACRO(0x142), BP_MACRO(0x143),
		BP_MACRO(0x144), BP_MACRO(0x145), BP_MACRO(0x146), BP_MACRO(0x147),
		BP_MACRO(0x148), BP_MACRO(0x149), BP_MACRO(0x14A), BP_MACRO(0x14B),
		BP_MACRO(0x14C), BP_MACRO(0x14D), BP_MACRO(0x14E), BP_MACRO(0x14F),

		BP_MACRO(0x150), BP_MACRO(0x151), BP_MACRO(0x152), BP_MACRO(0x153),
		BP_MACRO(0x154), BP_MACRO(0x155), BP_MACRO(0x156), BP_MACRO(0x157),
		BP_MACRO(0x158), BP_MACRO(0x159), BP_MACRO(0x15A), BP_MACRO(0x15B),
		BP_MACRO(0x15C), BP_MACRO(0x15D), BP_MACRO(0x15E), BP_MACRO(0x15F),

		BP_MACRO(0x160), BP_MACRO(0x161), BP_MACRO(0x162), BP_MACRO(0x163),
		BP_MACRO(0x164), BP_MACRO(0x165), BP_MACRO(0x166), BP_MACRO(0x167),
		BP_MACRO(0x168), BP_MACRO(0x169), BP_MACRO(0x16A), BP_MACRO(0x16B),
		BP_MACRO(0x16C), BP_MACRO(0x16D), BP_MACRO(0x16E), BP_MACRO(0x16F),

		BP_MACRO(0x170), BP_MACRO(0x171), BP_MACRO(0x172), BP_MACRO(0x173),
		BP_MACRO(0x174), BP_MACRO(0x175), BP_MACRO(0x176), BP_MACRO(0x177),
		BP_MACRO(0x178), BP_MACRO(0x179), BP_MACRO(0x17A), BP_MACRO(0x17B),
		BP_MACRO(0x17C), BP_MACRO(0x17D), BP_MACRO(0x17E), BP_MACRO(0x17F),

		BP_MACRO(0x180), BP_MACRO(0x181), BP_MACRO(0x182), BP_MACRO(0x183),
		BP_MACRO(0x184), BP_MACRO(0x185), BP_MACRO(0x186), BP_MACRO(0x187),
		BP_MACRO(0x188), BP_MACRO(0x189), BP_MACRO(0x18A), BP_MACRO(0x18B),
		BP_MACRO(0x18C), BP_MACRO(0x18D), BP_MACRO(0x18E), BP_MACRO(0x18F),

		BP_MACRO(0x190), BP_MACRO(0x191), BP_MACRO(0x192), BP_MACRO(0x193),
		BP_MACRO(0x194), BP_MACRO(0x195), BP_MACRO(0x196), BP_MACRO(0x197),
		BP_MACRO(0x198), BP_MACRO(0x199), BP_MACRO(0x19A), BP_MACRO(0x19B),
		BP_MACRO(0x19C), BP_MACRO(0x19D), BP_MACRO(0x19E), BP_MACRO(0x19F),

		BP_MACRO(0x1A0), BP_MACRO(0x1A1), BP_MACRO(0x1A2), BP_MACRO(0x1A3),
		BP_MACRO(0x1A4), BP_MACRO(0x1A5), BP_MACRO(0x1A6), BP_MACRO(0x1A7),
		BP_MACRO(0x1A8), BP_MACRO(0x1A9), BP_MACRO(0x1AA), BP_MACRO(0x1AB),
		BP_MACRO(0x1AC), BP_MACRO(0x1AD), BP_MACRO(0x1AE), BP_MACRO(0x1AF),

		BP_MACRO(0x1B0), BP_MACRO(0x1B1), BP_MACRO(0x1B2), BP_MACRO(0x1B3),
		BP_MACRO(0x1B4), BP_MACRO(0x1B5), BP_MACRO(0x1B6), BP_MACRO(0x1B7),
		BP_MACRO(0x1B8), BP_MACRO(0x1B9), BP_MACRO(0x1BA), BP_MACRO(0x1BB),
		BP_MACRO(0x1BC), BP_MACRO(0x1BD), BP_MACRO(0x1BE), BP_MACRO(0x1BF),

		BP_MACRO(0x1C0), BP_MACRO(0x1C1), BP_MACRO(0x1C2), BP_MACRO(0x1C3),
		BP_MACRO(0x1C4), BP_MACRO(0x1C5), BP_MACRO(0x1C6), BP_MACRO(0x1C7),
		BP_MACRO(0x1C8), BP_MACRO(0x1C9), BP_MACRO(0x1CA), BP_MACRO(0x1CB),
		BP_MACRO(0x1CC), BP_MACRO(0x1CD), BP_MACRO(0x1CE), BP_MACRO(0x1CF),

		BP_MACRO(0x1D0), BP_MACRO(0x1D1), BP_MACRO(0x1D2), BP_MACRO(0x1D3),
		BP_MACRO(0x1D4), BP_MACRO(0x1D5), BP_MACRO(0x1D6), BP_MACRO(0x1D7),
		BP_MACRO(0x1D8), BP_MACRO(0x1D9), BP_MACRO(0x1DA), BP_MACRO(0x1DB),
		BP_MACRO(0x1DC), BP_MACRO(0x1DD), BP_MACRO(0x1DE), BP_MACRO(0x1DF),

		BP_MACRO(0x1E0), BP_MACRO(0x1E1), BP_MACRO(0x1E2), BP_MACRO(0x1E3),
		BP_MACRO(0x1E4), BP_MACRO(0x1E5), BP_MACRO(0x1E6), BP_MACRO(0x1E7),
		BP_MACRO(0x1E8), BP_MACRO(0x1E9), BP_MACRO(0x1EA), BP_MACRO(0x1EB),
		BP_MACRO(0x1EC), BP_MACRO(0x1ED), BP_MACRO(0x1EE), BP_MACRO(0x1EF),

		BP_MACRO(0x1F0), BP_MACRO(0x1F1), BP_MACRO(0x1F2), BP_MACRO(0x1F3),
		BP_MACRO(0x1F4), BP_MACRO(0x1F5), BP_MACRO(0x1F6), BP_MACRO(0x1F7),
		BP_MACRO(0x1F8), BP_MACRO(0x1F9), BP_MACRO(0x1FA), BP_MACRO(0x1FB),
		BP_MACRO(0x1FC), BP_MACRO(0x1FD), BP_MACRO(0x1FE), BP_MACRO(0x1FF),

		BP_MACRO(0x200), BP_MACRO(0x201), BP_MACRO(0x202), BP_MACRO(0x203),
		BP_MACRO(0x204), BP_MACRO(0x205), BP_MACRO(0x206), BP_MACRO(0x207),
		BP_MACRO(0x208), BP_MACRO(0x209), BP_MACRO(0x20A), BP_MACRO(0x20B),
		BP_MACRO(0x20C), BP_MACRO(0x20D), BP_MACRO(0x20E), BP_MACRO(0x20F),

		BP_MACRO(0x210), BP_MACRO(0x211), BP_MACRO(0x212), BP_MACRO(0x213),
		BP_MACRO(0x214), BP_MACRO(0x215), BP_MACRO(0x216), BP_MACRO(0x217),
		BP_MACRO(0x218), BP_MACRO(0x219), BP_MACRO(0x21A), BP_MACRO(0x21B),
		BP_MACRO(0x21C), BP_MACRO(0x21D), BP_MACRO(0x21E), BP_MACRO(0x21F),

		BP_MACRO(0x220), BP_MACRO(0x221), BP_MACRO(0x222), BP_MACRO(0x223),
		BP_MACRO(0x224), BP_MACRO(0x225), BP_MACRO(0x226), BP_MACRO(0x227),
		BP_MACRO(0x228), BP_MACRO(0x229), BP_MACRO(0x22A), BP_MACRO(0x22B),
		BP_MACRO(0x22C), BP_MACRO(0x22D), BP_MACRO(0x22E), BP_MACRO(0x22F),

		BP_MACRO(0x230), BP_MACRO(0x231), BP_MACRO(0x232), BP_MACRO(0x233),
		BP_MACRO(0x234), BP_MACRO(0x235), BP_MACRO(0x236), BP_MACRO(0x237),
		BP_MACRO(0x238), BP_MACRO(0x239), BP_MACRO(0x23A), BP_MACRO(0x23B),
		BP_MACRO(0x23C), BP_MACRO(0x23D), BP_MACRO(0x23E), BP_MACRO(0x23F),

		BP_MACRO(0x240), BP_MACRO(0x241), BP_MACRO(0x242), BP_MACRO(0x243),
		BP_MACRO(0x244), BP_MACRO(0x245), BP_MACRO(0x246), BP_MACRO(0x247),
		BP_MACRO(0x248), BP_MACRO(0x249), BP_MACRO(0x24A), BP_MACRO(0x24B),
		BP_MACRO(0x24C), BP_MACRO(0x24D), BP_MACRO(0x24E), BP_MACRO(0x24F),

		BP_MACRO(0x250), BP_MACRO(0x251), BP_MACRO(0x252), BP_MACRO(0x253),
		BP_MACRO(0x254), BP_MACRO(0x255), BP_MACRO(0x256), BP_MACRO(0x257),
		BP_MACRO(0x258), BP_MACRO(0x259), BP_MACRO(0x25A), BP_MACRO(0x25B),
		BP_MACRO(0x25C), BP_MACRO(0x25D), BP_MACRO(0x25E), BP_MACRO(0x25F),

		BP_MACRO(0x260), BP_MACRO(0x261), BP_MACRO(0x262), BP_MACRO(0x263),
		BP_MACRO(0x264), BP_MACRO(0x265), BP_MACRO(0x266), BP_MACRO(0x267),
		BP_MACRO(0x268), BP_MACRO(0x269), BP_MACRO(0x26A), BP_MACRO(0x26B),
		BP_MACRO(0x26C), BP_MACRO(0x26D), BP_MACRO(0x26E), BP_MACRO(0x26F),

		BP_MACRO(0x270), BP_MACRO(0x271), BP_MACRO(0x272), BP_MACRO(0x273),
		BP_MACRO(0x274), BP_MACRO(0x275), BP_MACRO(0x276), BP_MACRO(0x277),
		BP_MACRO(0x278), BP_MACRO(0x279), BP_MACRO(0x27A), BP_MACRO(0x27B),
		BP_MACRO(0x27C), BP_MACRO(0x27D), BP_MACRO(0x27E), BP_MACRO(0x27F),

		BP_MACRO(0x280), BP_MACRO(0x281), BP_MACRO(0x282), BP_MACRO(0x283),
		BP_MACRO(0x284), BP_MACRO(0x285), BP_MACRO(0x286), BP_MACRO(0x287),
		BP_MACRO(0x288), BP_MACRO(0x289), BP_MACRO(0x28A), BP_MACRO(0x28B),
		BP_MACRO(0x28C), BP_MACRO(0x28D), BP_MACRO(0x28E), BP_MACRO(0x28F),

		BP_MACRO(0x290), BP_MACRO(0x291), BP_MACRO(0x292), BP_MACRO(0x293),
		BP_MACRO(0x294), BP_MACRO(0x295), BP_MACRO(0x296), BP_MACRO(0x297),
		BP_MACRO(0x298), BP_MACRO(0x299), BP_MACRO(0x29A), BP_MACRO(0x29B),
		BP_MACRO(0x29C), BP_MACRO(0x29D), BP_MACRO(0x29E), BP_MACRO(0x29F),

		BP_MACRO(0x2A0), BP_MACRO(0x2A1), BP_MACRO(0x2A2), BP_MACRO(0x2A3),
		BP_MACRO(0x2A4), BP_MACRO(0x2A5), BP_MACRO(0x2A6), BP_MACRO(0x2A7),
		BP_MACRO(0x2A8), BP_MACRO(0x2A9), BP_MACRO(0x2AA), BP_MACRO(0x2AB),
		BP_MACRO(0x2AC), BP_MACRO(0x2AD), BP_MACRO(0x2AE), BP_MACRO(0x2AF),

		BP_MACRO(0x2B0), BP_MACRO(0x2B1), BP_MACRO(0x2B2), BP_MACRO(0x2B3),
		BP_MACRO(0x2B4), BP_MACRO(0x2B5), BP_MACRO(0x2B6), BP_MACRO(0x2B7),
		BP_MACRO(0x2B8), BP_MACRO(0x2B9), BP_MACRO(0x2BA), BP_MACRO(0x2BB),
		BP_MACRO(0x2BC), BP_MACRO(0x2BD), BP_MACRO(0x2BE), BP_MACRO(0x2BF),

		BP_MACRO(0x2C0), BP_MACRO(0x2C1), BP_MACRO(0x2C2), BP_MACRO(0x2C3),
		BP_MACRO(0x2C4), BP_MACRO(0x2C5), BP_MACRO(0x2C6), BP_MACRO(0x2C7),
		BP_MACRO(0x2C8), BP_MACRO(0x2C9), BP_MACRO(0x2CA), BP_MACRO(0x2CB),
		BP_MACRO(0x2CC), BP_MACRO(0x2CD), BP_MACRO(0x2CE), BP_MACRO(0x2CF),

		BP_MACRO(0x2D0), BP_MACRO(0x2D1), BP_MACRO(0x2D2), BP_MACRO(0x2D3),
		BP_MACRO(0x2D4), BP_MACRO(0x2D5), BP_MACRO(0x2D6), BP_MACRO(0x2D7),
		BP_MACRO(0x2D8), BP_MACRO(0x2D9), BP_MACRO(0x2DA), BP_MACRO(0x2DB),
		BP_MACRO(0x2DC), BP_MACRO(0x2DD), BP_MACRO(0x2DE), BP_MACRO(0x2DF),

		BP_MACRO(0x2E0), BP_MACRO(0x2E1), BP_MACRO(0x2E2), BP_MACRO(0x2E3),
		BP_MACRO(0x2E4), BP_MACRO(0x2E5), BP_MACRO(0x2E6), BP_MACRO(0x2E7),
		BP_MACRO(0x2E8), BP_MACRO(0x2E9), BP_MACRO(0x2EA), BP_MACRO(0x2EB),
		BP_MACRO(0x2EC), BP_MACRO(0x2ED), BP_MACRO(0x2EE), BP_MACRO(0x2EF),

		BP_MACRO(0x2F0), BP_MACRO(0x2F1), BP_MACRO(0x2F2), BP_MACRO(0x2F3),
		BP_MACRO(0x2F4), BP_MACRO(0x2F5), BP_MACRO(0x2F6), BP_MACRO(0x2F7),
		BP_MACRO(0x2F8), BP_MACRO(0x2F9), BP_MACRO(0x2FA), BP_MACRO(0x2FB),
		BP_MACRO(0x2FC), BP_MACRO(0x2FD), BP_MACRO(0x2FE), BP_MACRO(0x2FF),

		BP_MACRO(0x300), BP_MACRO(0x301), BP_MACRO(0x302), BP_MACRO(0x303),
		BP_MACRO(0x304), BP_MACRO(0x305), BP_MACRO(0x306), BP_MACRO(0x307),
		BP_MACRO(0x308), BP_MACRO(0x309), BP_MACRO(0x30A), BP_MACRO(0x30B),
		BP_MACRO(0x30C), BP_MACRO(0x30D), BP_MACRO(0x30E), BP_MACRO(0x30F),

		BP_MACRO(0x310), BP_MACRO(0x311), BP_MACRO(0x312), BP_MACRO(0x313),
		BP_MACRO(0x314), BP_MACRO(0x315), BP_MACRO(0x316), BP_MACRO(0x317),
		BP_MACRO(0x318), BP_MACRO(0x319), BP_MACRO(0x31A), BP_MACRO(0x31B),
		BP_MACRO(0x31C), BP_MACRO(0x31D), BP_MACRO(0x31E), BP_MACRO(0x31F),

		BP_MACRO(0x320), BP_MACRO(0x321), BP_MACRO(0x322), BP_MACRO(0x323),
		BP_MACRO(0x324), BP_MACRO(0x325), BP_MACRO(0x326), BP_MACRO(0x327),
		BP_MACRO(0x328), BP_MACRO(0x329), BP_MACRO(0x32A), BP_MACRO(0x32B),
		BP_MACRO(0x32C), BP_MACRO(0x32D), BP_MACRO(0x32E), BP_MACRO(0x32F),

		BP_MACRO(0x330), BP_MACRO(0x331), BP_MACRO(0x332), BP_MACRO(0x333),
		BP_MACRO(0x334), BP_MACRO(0x335), BP_MACRO(0x336), BP_MACRO(0x337),
		BP_MACRO(0x338), BP_MACRO(0x339), BP_MACRO(0x33A), BP_MACRO(0x33B),
		BP_MACRO(0x33C), BP_MACRO(0x33D), BP_MACRO(0x33E), BP_MACRO(0x33F),

		BP_MACRO(0x340), BP_MACRO(0x341), BP_MACRO(0x342), BP_MACRO(0x343),
		BP_MACRO(0x344), BP_MACRO(0x345), BP_MACRO(0x346), BP_MACRO(0x347),
		BP_MACRO(0x348), BP_MACRO(0x349), BP_MACRO(0x34A), BP_MACRO(0x34B),
		BP_MACRO(0x34C), BP_MACRO(0x34D), BP_MACRO(0x34E), BP_MACRO(0x34F),

		BP_MACRO(0x350), BP_MACRO(0x351), BP_MACRO(0x352), BP_MACRO(0x353),
		BP_MACRO(0x354), BP_MACRO(0x355), BP_MACRO(0x356), BP_MACRO(0x357),
		BP_MACRO(0x358), BP_MACRO(0x359), BP_MACRO(0x35A), BP_MACRO(0x35B),
		BP_MACRO(0x35C), BP_MACRO(0x35D), BP_MACRO(0x35E), BP_MACRO(0x35F),

		BP_MACRO(0x360), BP_MACRO(0x361), BP_MACRO(0x362), BP_MACRO(0x363),
		BP_MACRO(0x364), BP_MACRO(0x365), BP_MACRO(0x366), BP_MACRO(0x367),
		BP_MACRO(0x368), BP_MACRO(0x369), BP_MACRO(0x36A), BP_MACRO(0x36B),
		BP_MACRO(0x36C), BP_MACRO(0x36D), BP_MACRO(0x36E), BP_MACRO(0x36F),

		BP_MACRO(0x370), BP_MACRO(0x371), BP_MACRO(0x372), BP_MACRO(0x373),
		BP_MACRO(0x374), BP_MACRO(0x375), BP_MACRO(0x376), BP_MACRO(0x377),
		BP_MACRO(0x378), BP_MACRO(0x379), BP_MACRO(0x37A), BP_MACRO(0x37B),
		BP_MACRO(0x37C), BP_MACRO(0x37D), BP_MACRO(0x37E), BP_MACRO(0x37F),

		BP_MACRO(0x380), BP_MACRO(0x381), BP_MACRO(0x382), BP_MACRO(0x383),
		BP_MACRO(0x384), BP_MACRO(0x385), BP_MACRO(0x386), BP_MACRO(0x387),
		BP_MACRO(0x388), BP_MACRO(0x389), BP_MACRO(0x38A), BP_MACRO(0x38B),
		BP_MACRO(0x38C), BP_MACRO(0x38D), BP_MACRO(0x38E), BP_MACRO(0x38F),

		BP_MACRO(0x390), BP_MACRO(0x391), BP_MACRO(0x392), BP_MACRO(0x393),
		BP_MACRO(0x394), BP_MACRO(0x395), BP_MACRO(0x396), BP_MACRO(0x397),
		BP_MACRO(0x398), BP_MACRO(0x399), BP_MACRO(0x39A), BP_MACRO(0x39B),
		BP_MACRO(0x39C), BP_MACRO(0x39D), BP_MACRO(0x39E), BP_MACRO(0x39F),

		BP_MACRO(0x3A0), BP_MACRO(0x3A1), BP_MACRO(0x3A2), BP_MACRO(0x3A3),
		BP_MACRO(0x3A4), BP_MACRO(0x3A5), BP_MACRO(0x3A6), BP_MACRO(0x3A7),
		BP_MACRO(0x3A8), BP_MACRO(0x3A9), BP_MACRO(0x3AA), BP_MACRO(0x3AB),
		BP_MACRO(0x3AC), BP_MACRO(0x3AD), BP_MACRO(0x3AE), BP_MACRO(0x3AF),

		BP_MACRO(0x3B0), BP_MACRO(0x3B1), BP_MACRO(0x3B2), BP_MACRO(0x3B3),
		BP_MACRO(0x3B4), BP_MACRO(0x3B5), BP_MACRO(0x3B6), BP_MACRO(0x3B7),
		BP_MACRO(0x3B8), BP_MACRO(0x3B9), BP_MACRO(0x3BA), BP_MACRO(0x3BB),
		BP_MACRO(0x3BC), BP_MACRO(0x3BD), BP_MACRO(0x3BE), BP_MACRO(0x3BF),

		BP_MACRO(0x3C0), BP_MACRO(0x3C1), BP_MACRO(0x3C2), BP_MACRO(0x3C3),
		BP_MACRO(0x3C4), BP_MACRO(0x3C5), BP_MACRO(0x3C6), BP_MACRO(0x3C7),
		BP_MACRO(0x3C8), BP_MACRO(0x3C9), BP_MACRO(0x3CA), BP_MACRO(0x3CB),
		BP_MACRO(0x3CC), BP_MACRO(0x3CD), BP_MACRO(0x3CE), BP_MACRO(0x3CF),

		BP_MACRO(0x3D0), BP_MACRO(0x3D1), BP_MACRO(0x3D2), BP_MACRO(0x3D3),
		BP_MACRO(0x3D4), BP_MACRO(0x3D5), BP_MACRO(0x3D6), BP_MACRO(0x3D7),
		BP_MACRO(0x3D8), BP_MACRO(0x3D9), BP_MACRO(0x3DA), BP_MACRO(0x3DB),
		BP_MACRO(0x3DC), BP_MACRO(0x3DD), BP_MACRO(0x3DE), BP_MACRO(0x3DF),

		BP_MACRO(0x3E0), BP_MACRO(0x3E1), BP_MACRO(0x3E2), BP_MACRO(0x3E3),
		BP_MACRO(0x3E4), BP_MACRO(0x3E5), BP_MACRO(0x3E6), BP_MACRO(0x3E7),
		BP_MACRO(0x3E8), BP_MACRO(0x3E9), BP_MACRO(0x3EA), BP_MACRO(0x3EB),
		BP_MACRO(0x3EC), BP_MACRO(0x3ED), BP_MACRO(0x3EE), BP_MACRO(0x3EF),

		BP_MACRO(0x3F0), BP_MACRO(0x3F1), BP_MACRO(0x3F2), BP_MACRO(0x3F3),
		BP_MACRO(0x3F4), BP_MACRO(0x3F5), BP_MACRO(0x3F6), BP_MACRO(0x3F7),
		BP_MACRO(0x3F8), BP_MACRO(0x3F9), BP_MACRO(0x3FA), BP_MACRO(0x3FB),
		BP_MACRO(0x3FC), BP_MACRO(0x3FD), BP_MACRO(0x3FE), BP_MACRO(0x3FF)
};

pd_t *pd_kernel;
pd_t *pd_current;

void INIT_PAGING(void)
{
	pd_kernel = pd_create();
	/*pd_map_range(
			pd_kernel,
			0x00000000,
			0x00000000,
			PTE_WRITABLE,
			MEMORY_LAYOUT_DIRECT_MAPPED / PAGE_SIZE
	);*/
	pd_map_range(
			pd_kernel,
			0x00000000,
			MEMORY_LAYOUT_KERNEL_START,
			PTE_WRITABLE,
			MEMORY_LAYOUT_DIRECT_MAPPED / PAGE_SIZE
	);
	pd_switch(pd_kernel, 0);
	pd_enable_paging();
}

pd_t *pd_create(void)
{
	pd_t *pd = (pd_t *)pmm_alloc_page();
	memset(pd, 0, sizeof(pd_t));
	return pd;
}

void pd_destroy(pd_t *pd)
{
	int pt;
	for (pt = 0; pt < PD_LENGTH; ++pt) {
		if (pd->entries[pt] & PDE_PRESENT) {
			pmm_mark_page_as_free((paddr_t)pd->entries[pt]);
		}
	}
	pmm_mark_page_as_free((paddr_t)pd);
}

void pd_map(pd_t *pd, paddr_t pframe, vaddr_t vframe, uint8_t flags)
{
	pt_t *pt = NULL;

	if (pd->entries[PDE_INDEX(vframe)] & PDE_PRESENT) {
		pt = (pt_t *)((uint32_t)pd->entries[PDE_INDEX(vframe)] & PDE_FRAME);
	} else {
		pt = (pt_t *)pmm_alloc_page();
		memset(pt, 0, sizeof(pt_t));
		pd->entries[PDE_INDEX(vframe)] = (uint32_t)pt | PDE_PRESENT;
	}

	pt->entries[PTE_INDEX(vframe)] = (uint32_t)pframe | PTE_PRESENT | (flags & 0xFFF);

	pd_flush_tlb(vframe);
}

void pd_unmap(pd_t *pd, vaddr_t frame)
{
	pt_t *pt = (pt_t *)((uint32_t)pd->entries[PDE_INDEX(frame)] & PDE_FRAME);
	pt->entries[PTE_INDEX(frame)] = 0;
	pd_flush_tlb(frame);
}

void pd_map_range(pd_t *pd, paddr_t pframe, vaddr_t vframe, uint8_t flags, unsigned int pages)
{
	int p;
	for (p = 0; p < pages; ++p) {
		pd_map(pd, pframe + PAGE_ADDR(p), vframe + PAGE_ADDR(p), flags);
	}
}

vaddr_t pd_map_fast(paddr_t frame, uint8_t flags)
{
	int t, e;

	for (t = 0; t < PD_LENGTH; ++t)
	{
		vaddr_t vframe;

		if (pd_current->entries[t] & PDE_PRESENT)
		{
			pt_t *pt = (pt_t *)(pd_current->entries[t] & PDE_FRAME);

			for (e = 0; e < PT_LENGTH; ++e)
			{
				if (!(pt->entries[e] & PTE_PRESENT))
				{
					vframe = (t << 22) + (e << 12);
					pd_map(pd_current, frame, vframe, flags);
					return vframe;
				}
			}
		} else {
			vframe = (t << 22);
			pd_map(pd_current, frame, vframe, flags);
			return vframe;
		}
	}

	/* too less memory */
	return -1;
}

pd_t *pd_get(void)
{
	return pd_current;
}

void pd_switch(pd_t *pd, uint8_t flags)
{
	pd_current = pd;
	uint32_t descriptor = ((paddr_t)pd & PD_FRAME) | flags;
	asm volatile ("mov %0, %%cr3" : : "r" (descriptor));
}

static inline void pd_enable_paging(void)
{
	asm volatile (
		"mov %cr0, %eax;"
		"or $0x80000000, %eax;"
		"mov %eax, %cr0"
	);
}

static inline void pd_flush_tlb(vaddr_t addr)
{
	asm volatile("invlpg (%0)" : : "r" (addr) : "memory");
}
