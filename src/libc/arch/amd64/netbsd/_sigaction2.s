
# This syscall cannot be autogenerated because it receives more than
# 4 arguments

	.text
	.globl	_sigaction2

_sigaction2:
	mov	$0x154,%eax
	mov	%rcx,%r10
	syscall
	retq