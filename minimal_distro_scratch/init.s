.text
.global _start
_start:

	mov x8, #142		// SYS_reboot
	ldr 	w0, =0xfee1dead	// magic 1
	ldr 	w1, =0x28121969	// magic 2 (see manpage)
	ldr  	w2, =0x01234567	// LINUX_REBOOT_CMD_RESTART
	svc	#0
	//mov  	x0, #0		// first arg: return 0
	mov 	x8, #93		// SYS_exit
	svc 	#0
