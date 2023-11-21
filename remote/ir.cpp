#include <avr/io.h>

namespace
{

char bits, count;
const char *ptr;
bool repeat;

void ir_off(void) { ptr = 0; TCB0.CTRLA = 0; }

void next(void) {
  if (*ptr == (char)0x80) {
    if (repeat) ptr -= (unsigned)ptr[1];
    else return ir_off();
  }
  if (!(bits = *ptr++)) ir_off();
  else {
    if (bits & 1) {
      count = (bits & 0x7f) >> 1;
      count--;
      bits &= 0x80;
    }
  }
}

}//anonymous

void ir_tca0_isr(void) {
  if (!ptr) return;
  if (bits & 0x80) { TCB0.CTRLB |= TCB_CCMPEN_bm; PORTA.OUTSET = PIN7_bm; }
  else { TCB0.CTRLB &= ~TCB_CCMPEN_bm; PORTA.OUTCLR = PIN7_bm; }
  if (count) count--;
  else {
    if (bits & 0x3f) bits <<= 1;
    else next();
  }
}

void ir_stop(void) {
  repeat = false;
  while (TCB0.CTRLA);
}

void ir_start(const char *p) {
  bits = count = 0;
  repeat = true;
  unsigned c = *p++;
  TCB0.CCMPL = c - 1;			// set carrier
  TCB0.CCMPH = 30;
  TCB0.CTRLB = TCB_CNTMODE_PWM8_gc;
  TCA0.SINGLE.PER = c * *p++;		// set bit width
  TCB0.CTRLA = TCB_ENABLE_bm;
  ptr = p;				// point to ir stream
}
