
void _start(void) {
short* videomem = (short*) 0xb8000;

    int i;
    for (i = 0; i < 3; i++) {
        *videomem++ = (0x07 << 8) | ('0' + i);
    }
   
    while(1);
}

