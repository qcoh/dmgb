#include "cpu.h"

namespace cpu {

namespace {

template <u8 Sel>
u8& reg8(cpu& cpu, mmu_ref mmu) {
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

// T can either be word (bc, de, hl) or u16 (sp)
template <u8 Sel>
auto& reg16(cpu& cpu) {
	// it seems return type deduction does not work with
	// if constexpr -- else if -- else
	if constexpr (0 <= Sel && Sel < 3) {
		return (Sel == 0) ? cpu.bc : (Sel == 1) ? cpu.de : cpu.hl;
	} else {
		return cpu.sp;
	}
}

// when pushing and popping af is used instead of sp
template <u8 Sel>
auto& regp(cpu& cpu) {
	if constexpr (Sel == 0) {
		return cpu.bc;
	} else if (Sel == 1) {
		return cpu.de;
	} else if (Sel == 2) {
		return cpu.hl;
	} else {
		return cpu.af;
	}
}

template <u8 Op>
void ld(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Dst = (Op >> 3) & 0x7;
	constexpr u8 Src = (Op & 0x7);
	reg8<Dst>(cpu, mmu) = reg8<Src>(cpu, mmu);
}

template <u8 Op>
void add(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Src = (Op & 0x7);
	const u8 src = reg8<Src>(cpu, mmu);

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
	const u8 src = reg8<Src>(cpu, mmu);

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
	const u8 src = reg8<Src>(cpu, mmu);

	cpu.hf = (cpu.a & 0xf) < (src & 0xf);
	cpu.cf = cpu.a < src;
	cpu.a = static_cast<u8>(cpu.a - src);
	cpu.zf = cpu.a == 0;
	cpu.nf = true;
}

template <u8 Op>
void sbc(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Src = (Op & 0x7);
	const u8 src = reg8<Src>(cpu, mmu);

	cpu.hf = (cpu.a & 0xf) < ((src & 0xf) + cpu.cf);
	int temp = cpu.a - src - cpu.cf;
	cpu.cf = temp < 0;
	cpu.a = static_cast<u8>(temp);
	cpu.zf = cpu.a == 0;
	cpu.nf = true;
}

template <u8 Op>
void and_(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Src = (Op & 0x7);
	const u8 src = reg8<Src>(cpu, mmu);

	cpu.a = cpu.a & src;
	cpu.zf = cpu.a == 0;
	cpu.nf = false;
	cpu.hf = true;
	cpu.cf = false;
}

template <u8 Op>
void xor_(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Src = (Op & 0x7);
	const u8 src = reg8<Src>(cpu, mmu);

	cpu.a = cpu.a ^ src;
	cpu.zf = cpu.a == 0;
	cpu.nf = false;
	cpu.hf = false;
	cpu.cf = false;
}

template <u8 Op>
void or_(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Src = (Op & 0x7);
	const u8 src = reg8<Src>(cpu, mmu);

	cpu.a = cpu.a | src;
	cpu.zf = cpu.a == 0;
	cpu.nf = false;
	cpu.hf = false;
	cpu.cf = false;

}


template <u8 Op>
void cp(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Src = (Op & 0x7);
	const u8 src = reg8<Src>(cpu, mmu);

	cpu.zf = (cpu.a - src) == 0;
	cpu.nf = true;
	cpu.hf = (cpu.a & 0xf) < (src & 0xf);
	cpu.cf = cpu.a < src;
}


template <u8 Op>
void bit(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Src = (Op & 0x7);
	const u8 src = reg8<Src>(cpu, mmu);
	constexpr u8 Mask = static_cast<u8>(1 << ((Op >> 3) & 0x7));

	cpu.zf = (src & Mask);
	cpu.nf = false;
	cpu.hf = true;
}


template <u8 Op>
void res(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Tgt = (Op & 0x7);
	u8& tgt = reg8<Tgt>(cpu, mmu);
	constexpr u8 Mask = static_cast<u8>(~(1 << ((Op >> 3) & 0x7)));

	tgt = tgt & Mask;
}


template <u8 Op>
void set(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Tgt = (Op & 0x7);
	u8& tgt = reg8<Tgt>(cpu, mmu);
	constexpr u8 Mask = static_cast<u8>(1 << ((Op >> 3) & 0x7));

	tgt = tgt | Mask;
}


template <u8 Op>
void rlc(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Tgt = (Op & 0x7);
	u8& tgt = reg8<Tgt>(cpu, mmu);

	cpu.cf = tgt & (1 << 7);
	tgt = (tgt << 1) | cpu.cf;
	cpu.zf = tgt == 0;
	cpu.nf = false;
	cpu.hf = false;
}

template <u8 Op>
void rrc(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Tgt = (Op & 0x7);
	u8& tgt = reg8<Tgt>(cpu, mmu);

	cpu.cf = tgt & 1;
	tgt = (tgt >> 1) | (cpu.cf << 7);
	cpu.zf = tgt == 0;
	cpu.nf = false;
	cpu.hf = false;
}

template <u8 Op>
void rl(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Tgt = (Op & 0x7);
	u8& tgt = reg8<Tgt>(cpu, mmu);

	const bool temp = cpu.cf;
	cpu.cf = tgt & (1 << 7);
	tgt = (tgt << 1) | temp;
	cpu.zf = tgt == 0;
	cpu.nf = false;
	cpu.hf = false;
}

template <u8 Op>
void rr(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Tgt = (Op & 0x7);
	u8& tgt = reg8<Tgt>(cpu, mmu);

	const bool temp = cpu.cf;
	cpu.cf = tgt & 1;
	tgt = (tgt >> 1) | (temp << 7);
	cpu.zf = tgt == 0;
	cpu.nf = false;
	cpu.hf = false;
}

template <u8 Op>
void sla(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Tgt = (Op & 0x7);
	u8& tgt = reg8<Tgt>(cpu, mmu);

	cpu.cf = tgt & (1 << 7);
	tgt = tgt << 1;
	cpu.zf = tgt == 0;
	cpu.nf = false;
	cpu.hf = false;
}


template <u8 Op>
void sra(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Tgt = (Op & 0x7);
	u8& tgt = reg8<Tgt>(cpu, mmu);

	const u8 temp = tgt & (1 << 7);
	cpu.cf = tgt & 1;
	tgt = (tgt >> 1) | temp;
	cpu.zf = tgt == 0;
	cpu.nf = false;
	cpu.hf = false;
}

template <u8 Op>
void swap(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Tgt = (Op & 0x7);
	u8& tgt = reg8<Tgt>(cpu, mmu);

	tgt = (tgt << 4) | (tgt >> 4);
	cpu.zf = tgt == 0;
	cpu.nf = false;
	cpu.hf = false;
	cpu.cf = false;
}

template <u8 Op>
void srl(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Tgt = (Op & 0x7);
	u8& tgt = reg8<Tgt>(cpu, mmu);

	cpu.cf = tgt & 1;
	tgt = tgt >> 1;
	cpu.zf = tgt == 0;
	cpu.nf = false;
	cpu.hf = false;
}

void prefix_cb(cpu& cpu, mmu_ref mmu) {
	const u8 op = mmu[cpu.pc + 1];

	switch (op) {
	case 0x0: rlc<0x0>(cpu, mmu); break;
	case 0x1: rlc<0x1>(cpu, mmu); break;
	case 0x2: rlc<0x2>(cpu, mmu); break;
	case 0x3: rlc<0x3>(cpu, mmu); break;
	case 0x4: rlc<0x4>(cpu, mmu); break;
	case 0x5: rlc<0x5>(cpu, mmu); break;
	case 0x6: rlc<0x6>(cpu, mmu); break;
	case 0x7: rlc<0x7>(cpu, mmu); break;
	case 0x8: rrc<0x8>(cpu, mmu); break;
	case 0x9: rrc<0x9>(cpu, mmu); break;
	case 0xa: rrc<0xa>(cpu, mmu); break;
	case 0xb: rrc<0xb>(cpu, mmu); break;
	case 0xc: rrc<0xc>(cpu, mmu); break;
	case 0xd: rrc<0xd>(cpu, mmu); break;
	case 0xe: rrc<0xe>(cpu, mmu); break;
	case 0xf: rrc<0xf>(cpu, mmu); break;
	case 0x10: rl<0x10>(cpu, mmu); break;
	case 0x11: rl<0x11>(cpu, mmu); break;
	case 0x12: rl<0x12>(cpu, mmu); break;
	case 0x13: rl<0x13>(cpu, mmu); break;
	case 0x14: rl<0x14>(cpu, mmu); break;
	case 0x15: rl<0x15>(cpu, mmu); break;
	case 0x16: rl<0x16>(cpu, mmu); break;
	case 0x17: rl<0x17>(cpu, mmu); break;
	case 0x18: rr<0x18>(cpu, mmu); break;
	case 0x19: rr<0x19>(cpu, mmu); break;
	case 0x1a: rr<0x1a>(cpu, mmu); break;
	case 0x1b: rr<0x1b>(cpu, mmu); break;
	case 0x1c: rr<0x1c>(cpu, mmu); break;
	case 0x1d: rr<0x1d>(cpu, mmu); break;
	case 0x1e: rr<0x1e>(cpu, mmu); break;
	case 0x1f: rr<0x1f>(cpu, mmu); break;
	case 0x20: sla<0x20>(cpu, mmu); break;
	case 0x21: sla<0x21>(cpu, mmu); break;
	case 0x22: sla<0x22>(cpu, mmu); break;
	case 0x23: sla<0x23>(cpu, mmu); break;
	case 0x24: sla<0x24>(cpu, mmu); break;
	case 0x25: sla<0x25>(cpu, mmu); break;
	case 0x26: sla<0x26>(cpu, mmu); break;
	case 0x27: sla<0x27>(cpu, mmu); break;
	case 0x28: sra<0x28>(cpu, mmu); break;
	case 0x29: sra<0x29>(cpu, mmu); break;
	case 0x2a: sra<0x2a>(cpu, mmu); break;
	case 0x2b: sra<0x2b>(cpu, mmu); break;
	case 0x2c: sra<0x2c>(cpu, mmu); break;
	case 0x2d: sra<0x2d>(cpu, mmu); break;
	case 0x2e: sra<0x2e>(cpu, mmu); break;
	case 0x2f: sra<0x2f>(cpu, mmu); break;
	case 0x30: swap<0x30>(cpu, mmu); break;
	case 0x31: swap<0x31>(cpu, mmu); break;
	case 0x32: swap<0x32>(cpu, mmu); break;
	case 0x33: swap<0x33>(cpu, mmu); break;
	case 0x34: swap<0x34>(cpu, mmu); break;
	case 0x35: swap<0x35>(cpu, mmu); break;
	case 0x36: swap<0x36>(cpu, mmu); break;
	case 0x37: swap<0x37>(cpu, mmu); break;
	case 0x38: srl<0x38>(cpu, mmu); break;
	case 0x39: srl<0x39>(cpu, mmu); break;
	case 0x3a: srl<0x3a>(cpu, mmu); break;
	case 0x3b: srl<0x3b>(cpu, mmu); break;
	case 0x3c: srl<0x3c>(cpu, mmu); break;
	case 0x3d: srl<0x3d>(cpu, mmu); break;
	case 0x3e: srl<0x3e>(cpu, mmu); break;
	case 0x3f: srl<0x3f>(cpu, mmu); break;
	case 0x40: bit<0x40>(cpu, mmu); break;
	case 0x41: bit<0x41>(cpu, mmu); break;
	case 0x42: bit<0x42>(cpu, mmu); break;
	case 0x43: bit<0x43>(cpu, mmu); break;
	case 0x44: bit<0x44>(cpu, mmu); break;
	case 0x45: bit<0x45>(cpu, mmu); break;
	case 0x46: bit<0x46>(cpu, mmu); break;
	case 0x47: bit<0x47>(cpu, mmu); break;
	case 0x48: bit<0x48>(cpu, mmu); break;
	case 0x49: bit<0x49>(cpu, mmu); break;
	case 0x4a: bit<0x4a>(cpu, mmu); break;
	case 0x4b: bit<0x4b>(cpu, mmu); break;
	case 0x4c: bit<0x4c>(cpu, mmu); break;
	case 0x4d: bit<0x4d>(cpu, mmu); break;
	case 0x4e: bit<0x4e>(cpu, mmu); break;
	case 0x4f: bit<0x4f>(cpu, mmu); break;
	case 0x50: bit<0x50>(cpu, mmu); break;
	case 0x51: bit<0x51>(cpu, mmu); break;
	case 0x52: bit<0x52>(cpu, mmu); break;
	case 0x53: bit<0x53>(cpu, mmu); break;
	case 0x54: bit<0x54>(cpu, mmu); break;
	case 0x55: bit<0x55>(cpu, mmu); break;
	case 0x56: bit<0x56>(cpu, mmu); break;
	case 0x57: bit<0x57>(cpu, mmu); break;
	case 0x58: bit<0x58>(cpu, mmu); break;
	case 0x59: bit<0x59>(cpu, mmu); break;
	case 0x5a: bit<0x5a>(cpu, mmu); break;
	case 0x5b: bit<0x5b>(cpu, mmu); break;
	case 0x5c: bit<0x5c>(cpu, mmu); break;
	case 0x5d: bit<0x5d>(cpu, mmu); break;
	case 0x5e: bit<0x5e>(cpu, mmu); break;
	case 0x5f: bit<0x5f>(cpu, mmu); break;
	case 0x60: bit<0x60>(cpu, mmu); break;
	case 0x61: bit<0x61>(cpu, mmu); break;
	case 0x62: bit<0x62>(cpu, mmu); break;
	case 0x63: bit<0x63>(cpu, mmu); break;
	case 0x64: bit<0x64>(cpu, mmu); break;
	case 0x65: bit<0x65>(cpu, mmu); break;
	case 0x66: bit<0x66>(cpu, mmu); break;
	case 0x67: bit<0x67>(cpu, mmu); break;
	case 0x68: bit<0x68>(cpu, mmu); break;
	case 0x69: bit<0x69>(cpu, mmu); break;
	case 0x6a: bit<0x6a>(cpu, mmu); break;
	case 0x6b: bit<0x6b>(cpu, mmu); break;
	case 0x6c: bit<0x6c>(cpu, mmu); break;
	case 0x6d: bit<0x6d>(cpu, mmu); break;
	case 0x6e: bit<0x6e>(cpu, mmu); break;
	case 0x6f: bit<0x6f>(cpu, mmu); break;
	case 0x70: bit<0x70>(cpu, mmu); break;
	case 0x71: bit<0x71>(cpu, mmu); break;
	case 0x72: bit<0x72>(cpu, mmu); break;
	case 0x73: bit<0x73>(cpu, mmu); break;
	case 0x74: bit<0x74>(cpu, mmu); break;
	case 0x75: bit<0x75>(cpu, mmu); break;
	case 0x76: bit<0x76>(cpu, mmu); break;
	case 0x77: bit<0x77>(cpu, mmu); break;
	case 0x78: bit<0x78>(cpu, mmu); break;
	case 0x79: bit<0x79>(cpu, mmu); break;
	case 0x7a: bit<0x7a>(cpu, mmu); break;
	case 0x7b: bit<0x7b>(cpu, mmu); break;
	case 0x7c: bit<0x7c>(cpu, mmu); break;
	case 0x7d: bit<0x7d>(cpu, mmu); break;
	case 0x7e: bit<0x7e>(cpu, mmu); break;
	case 0x7f: bit<0x7f>(cpu, mmu); break;
	case 0x80: res<0x80>(cpu, mmu); break;
	case 0x81: res<0x81>(cpu, mmu); break;
	case 0x82: res<0x82>(cpu, mmu); break;
	case 0x83: res<0x83>(cpu, mmu); break;
	case 0x84: res<0x84>(cpu, mmu); break;
	case 0x85: res<0x85>(cpu, mmu); break;
	case 0x86: res<0x86>(cpu, mmu); break;
	case 0x87: res<0x87>(cpu, mmu); break;
	case 0x88: res<0x88>(cpu, mmu); break;
	case 0x89: res<0x89>(cpu, mmu); break;
	case 0x8a: res<0x8a>(cpu, mmu); break;
	case 0x8b: res<0x8b>(cpu, mmu); break;
	case 0x8c: res<0x8c>(cpu, mmu); break;
	case 0x8d: res<0x8d>(cpu, mmu); break;
	case 0x8e: res<0x8e>(cpu, mmu); break;
	case 0x8f: res<0x8f>(cpu, mmu); break;
	case 0x90: res<0x90>(cpu, mmu); break;
	case 0x91: res<0x91>(cpu, mmu); break;
	case 0x92: res<0x92>(cpu, mmu); break;
	case 0x93: res<0x93>(cpu, mmu); break;
	case 0x94: res<0x94>(cpu, mmu); break;
	case 0x95: res<0x95>(cpu, mmu); break;
	case 0x96: res<0x96>(cpu, mmu); break;
	case 0x97: res<0x97>(cpu, mmu); break;
	case 0x98: res<0x98>(cpu, mmu); break;
	case 0x99: res<0x99>(cpu, mmu); break;
	case 0x9a: res<0x9a>(cpu, mmu); break;
	case 0x9b: res<0x9b>(cpu, mmu); break;
	case 0x9c: res<0x9c>(cpu, mmu); break;
	case 0x9d: res<0x9d>(cpu, mmu); break;
	case 0x9e: res<0x9e>(cpu, mmu); break;
	case 0x9f: res<0x9f>(cpu, mmu); break;
	case 0xa0: res<0xa0>(cpu, mmu); break;
	case 0xa1: res<0xa1>(cpu, mmu); break;
	case 0xa2: res<0xa2>(cpu, mmu); break;
	case 0xa3: res<0xa3>(cpu, mmu); break;
	case 0xa4: res<0xa4>(cpu, mmu); break;
	case 0xa5: res<0xa5>(cpu, mmu); break;
	case 0xa6: res<0xa6>(cpu, mmu); break;
	case 0xa7: res<0xa7>(cpu, mmu); break;
	case 0xa8: res<0xa8>(cpu, mmu); break;
	case 0xa9: res<0xa9>(cpu, mmu); break;
	case 0xaa: res<0xaa>(cpu, mmu); break;
	case 0xab: res<0xab>(cpu, mmu); break;
	case 0xac: res<0xac>(cpu, mmu); break;
	case 0xad: res<0xad>(cpu, mmu); break;
	case 0xae: res<0xae>(cpu, mmu); break;
	case 0xaf: res<0xaf>(cpu, mmu); break;
	case 0xb0: res<0xb0>(cpu, mmu); break;
	case 0xb1: res<0xb1>(cpu, mmu); break;
	case 0xb2: res<0xb2>(cpu, mmu); break;
	case 0xb3: res<0xb3>(cpu, mmu); break;
	case 0xb4: res<0xb4>(cpu, mmu); break;
	case 0xb5: res<0xb5>(cpu, mmu); break;
	case 0xb6: res<0xb6>(cpu, mmu); break;
	case 0xb7: res<0xb7>(cpu, mmu); break;
	case 0xb8: res<0xb8>(cpu, mmu); break;
	case 0xb9: res<0xb9>(cpu, mmu); break;
	case 0xba: res<0xba>(cpu, mmu); break;
	case 0xbb: res<0xbb>(cpu, mmu); break;
	case 0xbc: res<0xbc>(cpu, mmu); break;
	case 0xbd: res<0xbd>(cpu, mmu); break;
	case 0xbe: res<0xbe>(cpu, mmu); break;
	case 0xbf: res<0xbf>(cpu, mmu); break;
	case 0xc0: set<0xc0>(cpu, mmu); break;
	case 0xc1: set<0xc1>(cpu, mmu); break;
	case 0xc2: set<0xc2>(cpu, mmu); break;
	case 0xc3: set<0xc3>(cpu, mmu); break;
	case 0xc4: set<0xc4>(cpu, mmu); break;
	case 0xc5: set<0xc5>(cpu, mmu); break;
	case 0xc6: set<0xc6>(cpu, mmu); break;
	case 0xc7: set<0xc7>(cpu, mmu); break;
	case 0xc8: set<0xc8>(cpu, mmu); break;
	case 0xc9: set<0xc9>(cpu, mmu); break;
	case 0xca: set<0xca>(cpu, mmu); break;
	case 0xcb: set<0xcb>(cpu, mmu); break;
	case 0xcc: set<0xcc>(cpu, mmu); break;
	case 0xcd: set<0xcd>(cpu, mmu); break;
	case 0xce: set<0xce>(cpu, mmu); break;
	case 0xcf: set<0xcf>(cpu, mmu); break;
	case 0xd0: set<0xd0>(cpu, mmu); break;
	case 0xd1: set<0xd1>(cpu, mmu); break;
	case 0xd2: set<0xd2>(cpu, mmu); break;
	case 0xd3: set<0xd3>(cpu, mmu); break;
	case 0xd4: set<0xd4>(cpu, mmu); break;
	case 0xd5: set<0xd5>(cpu, mmu); break;
	case 0xd6: set<0xd6>(cpu, mmu); break;
	case 0xd7: set<0xd7>(cpu, mmu); break;
	case 0xd8: set<0xd8>(cpu, mmu); break;
	case 0xd9: set<0xd9>(cpu, mmu); break;
	case 0xda: set<0xda>(cpu, mmu); break;
	case 0xdb: set<0xdb>(cpu, mmu); break;
	case 0xdc: set<0xdc>(cpu, mmu); break;
	case 0xdd: set<0xdd>(cpu, mmu); break;
	case 0xde: set<0xde>(cpu, mmu); break;
	case 0xdf: set<0xdf>(cpu, mmu); break;
	case 0xe0: set<0xe0>(cpu, mmu); break;
	case 0xe1: set<0xe1>(cpu, mmu); break;
	case 0xe2: set<0xe2>(cpu, mmu); break;
	case 0xe3: set<0xe3>(cpu, mmu); break;
	case 0xe4: set<0xe4>(cpu, mmu); break;
	case 0xe5: set<0xe5>(cpu, mmu); break;
	case 0xe6: set<0xe6>(cpu, mmu); break;
	case 0xe7: set<0xe7>(cpu, mmu); break;
	case 0xe8: set<0xe8>(cpu, mmu); break;
	case 0xe9: set<0xe9>(cpu, mmu); break;
	case 0xea: set<0xea>(cpu, mmu); break;
	case 0xeb: set<0xeb>(cpu, mmu); break;
	case 0xec: set<0xec>(cpu, mmu); break;
	case 0xed: set<0xed>(cpu, mmu); break;
	case 0xee: set<0xee>(cpu, mmu); break;
	case 0xef: set<0xef>(cpu, mmu); break;
	case 0xf0: set<0xf0>(cpu, mmu); break;
	case 0xf1: set<0xf1>(cpu, mmu); break;
	case 0xf2: set<0xf2>(cpu, mmu); break;
	case 0xf3: set<0xf3>(cpu, mmu); break;
	case 0xf4: set<0xf4>(cpu, mmu); break;
	case 0xf5: set<0xf5>(cpu, mmu); break;
	case 0xf6: set<0xf6>(cpu, mmu); break;
	case 0xf7: set<0xf7>(cpu, mmu); break;
	case 0xf8: set<0xf8>(cpu, mmu); break;
	case 0xf9: set<0xf9>(cpu, mmu); break;
	case 0xfa: set<0xfa>(cpu, mmu); break;
	case 0xfb: set<0xfb>(cpu, mmu); break;
	case 0xfc: set<0xfc>(cpu, mmu); break;
	case 0xfd: set<0xfd>(cpu, mmu); break;
	case 0xfe: set<0xfe>(cpu, mmu); break;
	case 0xff: set<0xff>(cpu, mmu); break;
	}
}

template <u8 Op>
void ld_d16(cpu& cpu, const u16 imw) {
	constexpr u8 Tgt = (Op >> 4 ) & 0x3;

	auto& r16 = reg16<Tgt>(cpu); 
	r16 = imw;
}

template <u8 Op>
void inc(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Tgt = (Op >> 3 ) & 0x7;
	u8& tgt = reg8<Tgt>(cpu, mmu);

	cpu.hf = (tgt & 0xf) == 0xf;
	tgt = tgt + 1;
	cpu.zf = tgt == 0;
	cpu.nf = false;
}

template <u8 Op>
void dec(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Tgt = (Op >> 3 ) & 0x7;
	u8& tgt = reg8<Tgt>(cpu, mmu);

	cpu.hf = (tgt & 0xf) == 0;
	tgt = tgt - 1;
	cpu.zf = tgt == 0;
	cpu.nf = true;
}

template <u8 Op>
void inc16(cpu& cpu) {
	constexpr u8 Tgt = (Op >> 4 ) & 0x3;

	auto& r16 = reg16<Tgt>(cpu); 
	r16 = static_cast<u16>(r16 + 1);
}

template <u8 Op>
void dec16(cpu& cpu) {
	constexpr u8 Tgt = (Op >> 4 ) & 0x3;

	auto& r16 = reg16<Tgt>(cpu); 
	r16 = static_cast<u16>(r16 - 1);
}

template <u8 Op>
void ld_d8(cpu& cpu, mmu_ref mmu, const u8 imb) {
	constexpr u8 Tgt = (Op >> 3 ) & 0x7;
	reg8<Tgt>(cpu, mmu) = imb;
}

template <u8 Op>
void pop(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Tgt = (Op >> 6);
	auto& r16 = regp<Tgt>(cpu);

	r16 = static_cast<u16>(mmu[cpu.sp] | (mmu[cpu.sp+1] >> 8));
	cpu.sp += 2;
}

template <u8 Op>
void push(cpu& cpu, mmu_ref mmu) {
	constexpr u8 Tgt = (Op >> 6);
	const auto r16 = regp<Tgt>(cpu);

	mmu[cpu.sp-1] = static_cast<u8>(r16 >> 8);
	mmu[cpu.sp-2] = static_cast<u8>(r16);
	cpu.sp -= 2;
}

template <u8 Op>
void add_hl(cpu& cpu) {
	constexpr u8 Src = (Op >> 4) & 0x3;
	const u16 src = reg16<Src>(cpu);

	cpu.nf = false;
	cpu.hf = ((cpu.hl & 0xfff) + (src & 0xfff)) > 0xfff;
	cpu.cf = (cpu.hl + src) > 0xffff;

	cpu.hl = static_cast<u16>(cpu.hl + src);
}

}

void step(cpu& cpu, mmu_ref mmu) noexcept {
	const u8 op = mmu[cpu.pc];

	const u8 imb = mmu[cpu.pc+1];
	const word imw{mmu[cpu.pc+2], mmu[cpu.pc+1]};

	switch (op) {
	case 0x00: /* nop */ break;
	case 0x01: ld_d16<0x01>(cpu, imw); break;
	case 0x02:
	case 0x03: inc16<0x03>(cpu); break;
	case 0x04: inc<0x04>(cpu, mmu); break;
	case 0x05: dec<0x05>(cpu, mmu); break;
	case 0x06: ld_d8<0x06>(cpu, mmu, imb); break;
	case 0x07:
	case 0x08:
	case 0x09: add_hl<0x09>(cpu); break;
	case 0x0a:
	case 0x0b: dec16<0x0b>(cpu); break;
	case 0x0c: inc<0x0c>(cpu, mmu); break;
	case 0x0d: dec<0x0d>(cpu, mmu); break;
	case 0x0e: ld_d8<0x0e>(cpu, mmu, imb); break;
	case 0x0f:
	case 0x10: /* stop, ignore for now */ break;
	case 0x11: ld_d16<0x11>(cpu, imw); break;
	case 0x12:
	case 0x13: inc16<0x13>(cpu); break;
	case 0x14: inc<0x14>(cpu, mmu); break;
	case 0x15: dec<0x15>(cpu, mmu); break;
	case 0x16: ld_d8<0x16>(cpu, mmu, imb); break;
	case 0x17:
	case 0x18:
	case 0x19: add_hl<0x19>(cpu); break;
	case 0x1a:
	case 0x1b: dec16<0x1b>(cpu); break;
	case 0x1c: inc<0x1c>(cpu, mmu); break;
	case 0x1d: dec<0x1d>(cpu, mmu); break;
	case 0x1e: ld_d8<0x1e>(cpu, mmu, imb); break;
	case 0x1f:
	case 0x20:
	case 0x21: ld_d16<0x21>(cpu, imw); break;
	case 0x22:
	case 0x23: inc16<0x23>(cpu); break;
	case 0x24: inc<0x24>(cpu, mmu); break;
	case 0x25: dec<0x25>(cpu, mmu); break;
	case 0x26: ld_d8<0x26>(cpu, mmu, imb); break;
	case 0x27:
	case 0x28:
	case 0x29: add_hl<0x29>(cpu); break;
	case 0x2a:
	case 0x2b: dec16<0x2b>(cpu); break;
	case 0x2c: inc<0x2c>(cpu, mmu); break;
	case 0x2d: dec<0x2d>(cpu, mmu); break;
	case 0x2e: ld_d8<0x2e>(cpu, mmu, imb); break;
	case 0x2f:
	case 0x30:
	case 0x31: ld_d16<0x31>(cpu, imw); break;
	case 0x32:
	case 0x33: inc16<0x33>(cpu); break;
	case 0x34: inc<0x34>(cpu, mmu); break;
	case 0x35: dec<0x35>(cpu, mmu); break;
	case 0x36: ld_d8<0x36>(cpu, mmu, imb); break;
	case 0x37:
	case 0x38:
	case 0x39: add_hl<0x39>(cpu); break;
	case 0x3a:
	case 0x3b: dec16<0x3b>(cpu); break;
	case 0x3c: inc<0x3c>(cpu, mmu); break;
	case 0x3d: dec<0x3d>(cpu, mmu); break;
	case 0x3e: ld_d8<0x3e>(cpu, mmu, imb); break;
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
	case 0xa0: and_<0xa0>(cpu, mmu); break;
	case 0xa1: and_<0xa1>(cpu, mmu); break;
	case 0xa2: and_<0xa2>(cpu, mmu); break;
	case 0xa3: and_<0xa3>(cpu, mmu); break;
	case 0xa4: and_<0xa4>(cpu, mmu); break;
	case 0xa5: and_<0xa5>(cpu, mmu); break;
	case 0xa6: and_<0xa6>(cpu, mmu); break;
	case 0xa7: and_<0xa7>(cpu, mmu); break;
	case 0xa8: xor_<0xa8>(cpu, mmu); break;
	case 0xa9: xor_<0xa9>(cpu, mmu); break;
	case 0xaa: xor_<0xaa>(cpu, mmu); break;
	case 0xab: xor_<0xab>(cpu, mmu); break;
	case 0xac: xor_<0xac>(cpu, mmu); break;
	case 0xad: xor_<0xad>(cpu, mmu); break;
	case 0xae: xor_<0xae>(cpu, mmu); break;
	case 0xaf: xor_<0xaf>(cpu, mmu); break;
	case 0xb0: or_<0xb0>(cpu, mmu); break;
	case 0xb1: or_<0xb1>(cpu, mmu); break;
	case 0xb2: or_<0xb2>(cpu, mmu); break;
	case 0xb3: or_<0xb3>(cpu, mmu); break;
	case 0xb4: or_<0xb4>(cpu, mmu); break;
	case 0xb5: or_<0xb5>(cpu, mmu); break;
	case 0xb6: or_<0xb6>(cpu, mmu); break;
	case 0xb7: or_<0xb7>(cpu, mmu); break;
	case 0xb8: cp<0xb8>(cpu, mmu); break;
	case 0xb9: cp<0xb9>(cpu, mmu); break;
	case 0xba: cp<0xba>(cpu, mmu); break;
	case 0xbb: cp<0xbb>(cpu, mmu); break;
	case 0xbc: cp<0xbc>(cpu, mmu); break;
	case 0xbd: cp<0xbd>(cpu, mmu); break;
	case 0xbe: cp<0xbe>(cpu, mmu); break;
	case 0xbf: cp<0xbf>(cpu, mmu); break;
	case 0xc0:
	case 0xc1: pop<0xc1>(cpu, mmu); break;
	case 0xc2:
	case 0xc3:
	case 0xc4:
	case 0xc5: push<0xc5>(cpu, mmu); break;
	case 0xc6:
	case 0xc7:
	case 0xc8:
	case 0xc9:
	case 0xca:
	case 0xcb: prefix_cb(cpu, mmu); break;
	case 0xcc:
	case 0xcd:
	case 0xce:
	case 0xcf:
	case 0xd0:
	case 0xd1: pop<0xd1>(cpu, mmu); break;
	case 0xd2:
	case 0xd3: /* undefined */ break;
	case 0xd4:
	case 0xd5: push<0xd5>(cpu, mmu); break;
	case 0xd6:
	case 0xd7:
	case 0xd8:
	case 0xd9:
	case 0xda:
	case 0xdb: /* undefined */ break;
	case 0xdc:
	case 0xdd: /* undefined */ break;
	case 0xde:
	case 0xdf:
	case 0xe0:
	case 0xe1: pop<0xe1>(cpu, mmu); break;
	case 0xe2:
	case 0xe3: /* undefined */ break;
	case 0xe4: /* undefined */ break;
	case 0xe5: push<0xe5>(cpu, mmu); break;
	case 0xe6:
	case 0xe7:
	case 0xe8:
	case 0xe9:
	case 0xea:
	case 0xeb: /* undefined */ break;
	case 0xec: /* undefined */ break;
	case 0xed: /* undefined */ break;
	case 0xee:
	case 0xef:
	case 0xf0:
	case 0xf1: pop<0xf1>(cpu, mmu); break;
	case 0xf2:
	case 0xf3:
	case 0xf4: /* undefined */ break;
	case 0xf5: push<0xf5>(cpu, mmu); break;
	case 0xf6:
	case 0xf7:
	case 0xf8:
	case 0xf9:
	case 0xfa:
	case 0xfb:
	case 0xfc: /* undefined */ break;
	case 0xfd: /* undefined */ break;
	case 0xfe:
	case 0xff:
		   break;
	}

}

}
