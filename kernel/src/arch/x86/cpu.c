


void halt(void) {
	while (1) {
		asm volatile("cli; hlt");
	}
}

