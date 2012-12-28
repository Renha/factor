namespace factor
{

#define FACTOR_CPU_STRING "arm"

#define FRAME_RETURN_ADDRESS(frame,vm) *(XT *)(vm->frame_successor(frame) + 1)

/* In the instruction sequence:

   MOVS r3,...
   B blah

   the offset from the immediate operand to MOVS to the instruction after
   the branch is one instruction. */
static const fixnum xt_tail_pic_offset = 4;

inline static void check_call_site(cell return_address)
{
	u32 insn = *(u32 *)return_address;
	/* Check that condition is `always` */
	FACTOR_ASSERT((insn >> 28) == 14);
	/* Check that instruction is `branch` */
	FACTOR_ASSERT(((insn >> 25) & 0x7 ) == 0x5);
}

static const u32 b_mask = 0xffffff;

inline static void *get_call_target(cell return_address)
{
	return_address -= 4;
	check_call_site(return_address);

	u32 insn = *(u32 *)return_address;
	u32 addr = ((insn & b_mask) << 2);
	if (addr >> 23)
	{
	    addr |= 0xFF000000;
	}
	return (void *)(addr + return_address);
}

inline static void set_call_target(cell return_address, void *target)
{
	return_address -= 4;
	check_call_site(return_address);

	u32 insn = *(u32 *)(return_address);

	fixnum relative_address = ((cell)target - return_address - 8);
	insn = ((insn & ~b_mask) | (relative_address & b_mask));
	*(u32 *)(return_address) = insn;

	/* Flush the cache line containing the call we just patched */
	flush_icache(return_address, 1);
}

inline static bool tail_call_site_p(cell return_address)
{
	return_address -= 4;
	u32 insn = *(u32 *)return_address;
	return (insn & (1<<24)) == 0;
}

inline static unsigned int fpu_status(unsigned int status)
{
	unsigned int r = 0;

	if (status & (1<<16))
		r |= FP_TRAP_INVALID_OPERATION;
	if (status & (1<<18))
		r |= FP_TRAP_OVERFLOW;
	if (status & (1<<19))
		r |= FP_TRAP_UNDERFLOW;
	if (status & (1<< 1))
		r |= FP_TRAP_ZERO_DIVIDE;
	if (status & (1<<20))
		r |= FP_TRAP_INEXACT;

	return r;
}

static inline void flush_icache(cell start, cell len)
{
	int result;

	/* result = syscall(__ARM_NR_cacheflush,start,start + len,0); */

	__asm__ __volatile__ (
		"mov     r0, %1\n"
		"sub     r1, %2, #1\n"
		"mov     r2, #0\n"
		"swi     " __sys1(__ARM_NR_cacheflush) "\n"
		"mov     %0, r0\n"
		: "=r" (result)
		: "r" (start), "r" (start + len)
		: "r0","r1","r2");

	if(result < 0)
		critical_error("flush_icache() failed",result);

}
}
