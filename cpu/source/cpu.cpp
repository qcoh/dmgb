#include "cpu.h"

namespace cpu {

namespace {

template <u8 Sel>
inline u8& reg(cpu& cpu, mmu_ref mmu) {
	const u8 masked = Sel;
	if constexpr (masked == 0) {
		return cpu.b;
	} else if (masked == 1) {
		return cpu.c;
	} else if (masked == 2) {
		return cpu.d;
	} else if (masked == 3) {
		return cpu.e;
	} else if (masked == 4) {
		return cpu.h;
	} else if (masked == 5) {
		return cpu.l;
	} else if (masked == 6) {
		return mmu[cpu.hl];
	} else /* if (masked == 7) */ {
		return cpu.a;
	}
}

template <u8 Op>
void ld(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Dst = (Op >> 3) & 0x7;
	constexpr u8 Src = (Op & 0x7);
	reg<Dst>(cpu, mmu) = reg<Src>(cpu, mmu);
}

template <u8 Op>
void add(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Src = (Op & 0x7);
	const u8 src = reg<Src>(cpu, mmu);

	cpu.hf = ((cpu.a & 0xf) + (src & 0xf)) > 0xf;
	int temp = cpu.a +  src;
	cpu.cf = temp > 0xff;
	cpu.a = static_cast<u8>(temp);
	cpu.zf = cpu.a == 0;
	cpu.nf = false;
}

template <u8 Op>
void adc(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Src = (Op & 0x7);
	const u8 src = reg<Src>(cpu, mmu);

	cpu.hf = ((cpu.a & 0xf) + (src & 0xf) + cpu.cf) > 0xf;
	int temp = cpu.a + src + cpu.cf;
	cpu.cf = temp > 0xff;
	cpu.a = static_cast<u8>(temp);
	cpu.zf = cpu.a == 0;
	cpu.nf = 0;
}

template <u8 Op>
void sub(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Src = (Op & 0x7);
	const u8 src = reg<Src>(cpu, mmu);

	cpu.hf = (cpu.a & 0xf) < (src & 0xf);
	cpu.cf = cpu.a < src;
	cpu.a = static_cast<u8>(cpu.a - src);
	cpu.zf = cpu.a == 0;
	cpu.nf = true;
}

template <u8 Op>
void sbc(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Src = (Op & 0x7);
	const u8 src = reg<Src>(cpu, mmu);

	cpu.hf = (cpu.a & 0xf) < ((src & 0xf) + cpu.cf);
	int temp = cpu.a - src - cpu.cf;
	cpu.cf = temp < 0;
	cpu.a = static_cast<u8>(temp);
	cpu.zf = cpu.a == 0;
	cpu.nf = true;
}

}

void step(cpu& cpu, mmu_ref mmu) noexcept {
	const u8 op = mmu[cpu.pc];

	switch (op) {
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
	case 0x08:
	case 0x09:
	case 0x0a:
	case 0x0b:
	case 0x0c:
	case 0x0d:
	case 0x0e:
	case 0x0f:
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x14:
	case 0x15:
	case 0x16:
	case 0x17:
	case 0x18:
	case 0x19:
	case 0x1a:
	case 0x1b:
	case 0x1c:
	case 0x1d:
	case 0x1e:
	case 0x1f:
	case 0x20:
	case 0x21:
	case 0x22:
	case 0x23:
	case 0x24:
	case 0x25:
	case 0x26:
	case 0x27:
	case 0x28:
	case 0x29:
	case 0x2a:
	case 0x2b:
	case 0x2c:
	case 0x2d:
	case 0x2e:
	case 0x2f:
	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:
	case 0x34:
	case 0x35:
	case 0x36:
	case 0x37:
	case 0x38:
	case 0x39:
	case 0x3a:
	case 0x3b:
	case 0x3c:
	case 0x3d:
	case 0x3e:
	case 0x3f:
	case 0x40: ld<0x40>(cpu, mmu); break;
	case 0x41: ld<0x41>(cpu, mmu); break;
	case 0x42: ld<0x42>(cpu, mmu); break;
	case 0x43: ld<0x43>(cpu, mmu); break;
	case 0x44: ld<0x44>(cpu, mmu); break;
	case 0x45: ld<0x45>(cpu, mmu); break;
	case 0x46: ld<0x46>(cpu, mmu); break;
	case 0x47: ld<0x47>(cpu, mmu); break;
	case 0x48: ld<0x48>(cpu, mmu); break;
	case 0x49: ld<0x49>(cpu, mmu); break;
	case 0x4a: ld<0x4a>(cpu, mmu); break;
	case 0x4b: ld<0x4b>(cpu, mmu); break;
	case 0x4c: ld<0x4c>(cpu, mmu); break;
	case 0x4d: ld<0x4d>(cpu, mmu); break;
	case 0x4e: ld<0x4e>(cpu, mmu); break;
	case 0x4f: ld<0x4f>(cpu, mmu); break;
	case 0x50: ld<0x50>(cpu, mmu); break;
	case 0x51: ld<0x51>(cpu, mmu); break;
	case 0x52: ld<0x52>(cpu, mmu); break;
	case 0x53: ld<0x53>(cpu, mmu); break;
	case 0x54: ld<0x54>(cpu, mmu); break;
	case 0x55: ld<0x55>(cpu, mmu); break;
	case 0x56: ld<0x56>(cpu, mmu); break;
	case 0x57: ld<0x57>(cpu, mmu); break;
	case 0x58: ld<0x58>(cpu, mmu); break;
	case 0x59: ld<0x59>(cpu, mmu); break;
	case 0x5a: ld<0x5a>(cpu, mmu); break;
	case 0x5b: ld<0x5b>(cpu, mmu); break;
	case 0x5c: ld<0x5c>(cpu, mmu); break;
	case 0x5d: ld<0x5d>(cpu, mmu); break;
	case 0x5e: ld<0x5e>(cpu, mmu); break;
	case 0x5f: ld<0x5f>(cpu, mmu); break;
	case 0x60: ld<0x60>(cpu, mmu); break;
	case 0x61: ld<0x61>(cpu, mmu); break;
	case 0x62: ld<0x62>(cpu, mmu); break;
	case 0x63: ld<0x63>(cpu, mmu); break;
	case 0x64: ld<0x64>(cpu, mmu); break;
	case 0x65: ld<0x65>(cpu, mmu); break;
	case 0x66: ld<0x66>(cpu, mmu); break;
	case 0x67: ld<0x67>(cpu, mmu); break;
	case 0x68: ld<0x68>(cpu, mmu); break;
	case 0x69: ld<0x69>(cpu, mmu); break;
	case 0x6a: ld<0x6a>(cpu, mmu); break;
	case 0x6b: ld<0x6b>(cpu, mmu); break;
	case 0x6c: ld<0x6c>(cpu, mmu); break;
	case 0x6d: ld<0x6d>(cpu, mmu); break;
	case 0x6e: ld<0x6e>(cpu, mmu); break;
	case 0x6f: ld<0x6f>(cpu, mmu); break;
	case 0x70: ld<0x70>(cpu, mmu); break;
	case 0x71: ld<0x71>(cpu, mmu); break;
	case 0x72: ld<0x72>(cpu, mmu); break;
	case 0x73: ld<0x73>(cpu, mmu); break;
	case 0x74: ld<0x74>(cpu, mmu); break;
	case 0x75: ld<0x75>(cpu, mmu); break;
	case 0x76: /* halt */ break;
	case 0x77: ld<0x77>(cpu, mmu); break;
	case 0x78: ld<0x78>(cpu, mmu); break;
	case 0x79: ld<0x79>(cpu, mmu); break;
	case 0x7a: ld<0x7a>(cpu, mmu); break;
	case 0x7b: ld<0x7b>(cpu, mmu); break;
	case 0x7c: ld<0x7c>(cpu, mmu); break;
	case 0x7d: ld<0x7d>(cpu, mmu); break;
	case 0x7e: ld<0x7e>(cpu, mmu); break;
	case 0x7f: ld<0x7f>(cpu, mmu); break;
	case 0x80: add<0x80>(cpu, mmu); break;
	case 0x81: add<0x81>(cpu, mmu); break;
	case 0x82: add<0x82>(cpu, mmu); break;
	case 0x83: add<0x83>(cpu, mmu); break;
	case 0x84: add<0x84>(cpu, mmu); break;
	case 0x85: add<0x85>(cpu, mmu); break;
	case 0x86: add<0x86>(cpu, mmu); break;
	case 0x87: add<0x87>(cpu, mmu); break;
	case 0x88: adc<0x88>(cpu, mmu); break;
	case 0x89: adc<0x89>(cpu, mmu); break;
	case 0x8a: adc<0x8a>(cpu, mmu); break;
	case 0x8b: adc<0x8b>(cpu, mmu); break;
	case 0x8c: adc<0x8c>(cpu, mmu); break;
	case 0x8d: adc<0x8d>(cpu, mmu); break;
	case 0x8e: adc<0x8e>(cpu, mmu); break;
	case 0x8f: adc<0x8f>(cpu, mmu); break;
	case 0x90: sub<0x90>(cpu, mmu); break;
	case 0x91: sub<0x91>(cpu, mmu); break;
	case 0x92: sub<0x92>(cpu, mmu); break;
	case 0x93: sub<0x93>(cpu, mmu); break;
	case 0x94: sub<0x94>(cpu, mmu); break;
	case 0x95: sub<0x95>(cpu, mmu); break;
	case 0x96: sub<0x96>(cpu, mmu); break;
	case 0x97: sub<0x97>(cpu, mmu); break;
	case 0x98: sbc<0x98>(cpu, mmu); break;
	case 0x99: sbc<0x99>(cpu, mmu); break;
	case 0x9a: sbc<0x9a>(cpu, mmu); break;
	case 0x9b: sbc<0x9b>(cpu, mmu); break;
	case 0x9c: sbc<0x9c>(cpu, mmu); break;
	case 0x9d: sbc<0x9d>(cpu, mmu); break;
	case 0x9e: sbc<0x9e>(cpu, mmu); break;
	case 0x9f: sbc<0x9f>(cpu, mmu); break;
	case 0xa0:
	case 0xa1:
	case 0xa2:
	case 0xa3:
	case 0xa4:
	case 0xa5:
	case 0xa6:
	case 0xa7:
	case 0xa8:
	case 0xa9:
	case 0xaa:
	case 0xab:
	case 0xac:
	case 0xad:
	case 0xae:
	case 0xaf:
	case 0xb0:
	case 0xb1:
	case 0xb2:
	case 0xb3:
	case 0xb4:
	case 0xb5:
	case 0xb6:
	case 0xb7:
	case 0xb8:
	case 0xb9:
	case 0xba:
	case 0xbb:
	case 0xbc:
	case 0xbd:
	case 0xbe:
	case 0xbf:
	case 0xc0:
	case 0xc1:
	case 0xc2:
	case 0xc3:
	case 0xc4:
	case 0xc5:
	case 0xc6:
	case 0xc7:
	case 0xc8:
	case 0xc9:
	case 0xca:
	case 0xcb:
	case 0xcc:
	case 0xcd:
	case 0xce:
	case 0xcf:
	case 0xd0:
	case 0xd1:
	case 0xd2:
	case 0xd3:
	case 0xd4:
	case 0xd5:
	case 0xd6:
	case 0xd7:
	case 0xd8:
	case 0xd9:
	case 0xda:
	case 0xdb:
	case 0xdc:
	case 0xdd:
	case 0xde:
	case 0xdf:
	case 0xe0:
	case 0xe1:
	case 0xe2:
	case 0xe3:
	case 0xe4:
	case 0xe5:
	case 0xe6:
	case 0xe7:
	case 0xe8:
	case 0xe9:
	case 0xea:
	case 0xeb:
	case 0xec:
	case 0xed:
	case 0xee:
	case 0xef:
	case 0xf0:
	case 0xf1:
	case 0xf2:
	case 0xf3:
	case 0xf4:
	case 0xf5:
	case 0xf6:
	case 0xf7:
	case 0xf8:
	case 0xf9:
	case 0xfa:
	case 0xfb:
	case 0xfc:
	case 0xfd:
	case 0xfe:
	case 0xff:
		   break;
	}

}

}
