enum args {
	AREG_CS = AMAX,
	AREG_DS,
	AREG_SS,
	AREG_ES,
	AREG_FS,
	AREG_GS,

	AREG_EFLAGS,
	AREG_CF,
	AREG_PF,
	AREG_AF,
	AREG_ZF,
	AREG_SF,
	AREG_TF,
	AREG_IF,
	AREG_DF,
	AREG_OF,
	AREG_IOPL,
	AREG_NT,
	AREG_RF,
	AREG_VM,
	AREG_AC,
	AREG_VIF,
	AREG_VIP,
	AREG_ID,

	AREG_AX,
	AREG_AL,
	AREG_AH,
	AREG_EAX,
	AREG_RAX,

	AREG_BX,
	AREG_BL,
	AREG_BH,
	AREG_EBX,
	AREG_RBX,

	AREG_CX,
	AREG_CL,
	AREG_CH,
	AREG_ECX,
	AREG_RCX,

	AREG_DX,
	AREG_DL,
	AREG_DH,
	AREG_EDX,
	AREG_RDX,

	AREG_SI,
	AREG_SIL,
	AREG_ESI,
	AREG_RSI,
	AREG_DI,
	AREG_DIL,
	AREG_EDI,
	AREG_RDI,

	AREG_SP,
	AREG_SPL,
	AREG_ESP,
	AREG_RSP,

	AREG_BP,
	AREG_BPL,
	AREG_EBP,
	AREG_RBP,

	AREG_R0,
	AREG_MM0,
	AREG_R1,
	AREG_MM1,
	AREG_R2,
	AREG_MM2,
	AREG_R3,
	AREG_MM3,
	AREG_R4,
	AREG_MM4,
	AREG_R5,
	AREG_MM5,
	AREG_R6,
	AREG_MM6,
	AREG_R7,
	AREG_MM7,

	AREG_R8,
	AREG_R8L,
	AREG_R8W,
	AREG_R9,
	AREG_R9L,
	AREG_R9W,
	AREG_R10,
	AREG_R10L,
	AREG_R10W,
	AREG_R11,
	AREG_R11L,
	AREG_R11W,
	AREG_R12,
	AREG_R12L,
	AREG_R12W,
	AREG_R13,
	AREG_R13L,
	AREG_R13W,
	AREG_R14,
	AREG_R14L,
	AREG_R14W,
	AREG_R15,
	AREG_R15L,
	AREG_R15W,

	AREG_XMM0,
	AREG_XMM1,
	AREG_XMM2,
	AREG_XMM3,
	AREG_XMM4,
	AREG_XMM5,
	AREG_XMM6,
	AREG_XMM7,
	AREG_XMM8,
	AREG_XMM9,
	AREG_XMM10,
	AREG_XMM11,
	AREG_XMM12,
	AREG_XMM13,
	AREG_XMM14,
	AREG_XMM15,

	AREG_YMM0,
	AREG_YMM1,
	AREG_YMM2,
	AREG_YMM3,
	AREG_YMM4,
	AREG_YMM5,
	AREG_YMM6,
	AREG_YMM7,
	AREG_YMM8,
	AREG_YMM9,
	AREG_YMM10,
	AREG_YMM11,
	AREG_YMM12,
	AREG_YMM13,
	AREG_YMM14,
	AREG_YMM15,

	AREG_MXCSR,

	AREG_R8CLASS,   /* register class for 8 bit registers in i286 */
	AREG_R16CLASS,   /* register class for 16 bit registers in i286 */
};

enum class {
	R8CLASS  = 1 << 0,
	R16CLASS = 1 << 1,
};
