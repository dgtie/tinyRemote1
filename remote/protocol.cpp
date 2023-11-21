namespace
{

char toggle;

char *rc6_bits(char *p, char c) {
  const char bit2[] = { 0x58, 0x68, (char)0x98, (char)0xa8 };
  char d = c >> 6;
  *p++ = bit2[d & 3];
  d = c >> 4;
  *p++ = bit2[d & 3];
  d = c >> 2;
  *p++ = bit2[d & 3];
  *p++ = bit2[c & 3];
  return p;
}

char *sony20_bits(char *p, int b) {
  const char bit2[] = { (char)0xA8, (char)0xD4, (char)0xB4, (char)0xDA };
  while (b != 1) { *p++ = bit2[b & 3]; b >>= 2; }
  return p;
}

char *nec_bits(char *p, char c) {
  for (char i = 0; i < 8; i++) {
    *p++ = c & 1 ? 0x88 : 0xA0;
    c >>= 1;
  }
  return p;
}

}//anonymous

void rc6(char *p, unsigned char a, unsigned char c) {
  *p++ = 93;		// 36 kHz
  *p++ = 16;		// 444 us
  *p++ = 0xFE;		// leader mark
  *p++ = 0x28;		// leader space + SB
  *p++ = 0x56;		// mode 0
  *p++ = (toggle ^= 1) ? 0xC8 : 0x38;
  p = rc6_bits(p, a);
  p = rc6_bits(p, c);
  *p++ = *p++ = 101;
  *p++ = 0x80;		// jump back to leader
  *p = 14;
}

void sony20(char *p, char a, char c, char extended) {
  *p++ = 83;
  *p++ = 24;
  *p++ = 0xF4;
  p = sony20_bits(p, (a << 7) | c | 0x1000);
  p = sony20_bits(p, extended | 0x100);
  *p++ = 39;
  *p++ = 0x80;
  *p = 12;
}

void nec(char *p, char a, char c) {
  *p++ = 88;
  *p++ = 21;
  *p++ = 161;
  *p++ = 17;
  p = nec_bits(p, a);
  p = nec_bits(p, ~a);
  p = nec_bits(p, c);
  p = nec_bits(p, ~c);
  *p++ = 0x82;
  *p++ = 127;
  *p++ = 161;
  *p++ = 0xa0;
  *p++ = *p++ = 109;
  *p++ = 0x80;
  *p = 5;
}
