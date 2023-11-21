#include <avr/interrupt.h>

#define KEY_0 0x02
#define KEY_1 0x0c
#define KEY_2 0x08
#define KEY_3 0x04
#define KEY_4 0x30
#define KEY_5 0x20
#define KEY_6 0x10
#define KEY_7 (char)0xc0
#define KEY_8 (char)0x80
#define KEY_9 0x40
#define KEY_L 0x03
#define KEY_R 0x01
#define KEY_U 0

#define SENSE_PINS (PIN6_bm|PIN3_bm|PIN2_bm|PIN1_bm)

namespace
{

const char key[] = { KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5,
                     KEY_6, KEY_7, KEY_8, KEY_9, KEY_L, KEY_R, KEY_U };

void __attribute__((optimize("O0"))) pinctrl(char c) {
  PORTA.PIN1CTRL = PORTA.PIN2CTRL = PORTA.PIN3CTRL = PORTA.PIN6CTRL = c;
}

char read(char key, char pb) {
  PORTB.DIRCLR = pb;
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  if (!(PORTA.IN & PIN3_bm)) key += 64;
  if (!(PORTA.IN & PIN2_bm)) key += 16;
  if (!(PORTA.IN & PIN1_bm)) key += 4;
  if (!(PORTA.IN & PIN6_bm)) key++;
  return key;
}

}//anonymous

void key_standby(void) {
  PORTB.DIRSET = PIN1_bm | PIN0_bm;
  PORTA.INTFLAGS = SENSE_PINS;
  pinctrl(PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc);
}

char key_read(void) {
  char k = read(read(read(0, 0), PIN1_bm), PIN0_bm);
  PORTB.DIRSET = PIN1_bm | PIN0_bm;
  char c;
  for (c = 0; c < 13; c++) if (k == key[c]) break;
  return c;
}

ISR(PORTA_PORT_vect) {
  if (PORTA.INTFLAGS & SENSE_PINS) pinctrl(PORT_PULLUPEN_bm);
}
