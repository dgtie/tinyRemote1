#include <avr/interrupt.h>

const char* ptr;
unsigned mark;
char line[] = "000 000\r\n";

void __attribute__((used, naked, section(".init8"))) init(void) {
  TCB0.CTRLA = TCB_CLKSEL_CLKDIV2_gc;	// 0.6 us
  TCB0.CTRLB = TCB_CNTMODE_INT_gc;	// overflow interrupt
  TCB0.CCMP = 65535;			// 39 ms
  TCB0.CNT = 0;
  PORTA.INTFLAGS = PIN4_bm;
  PORTA.PIN4CTRL = PORT_ISC_BOTHEDGES_gc;
  TCB0.INTCTRL = TCB_CAPT_bm;
  TCB0.CTRLA |= TCB_ENABLE_bm;
  PORTB.DIRSET = PIN2_bm;
  USART0.BAUD = 116;
  USART0.CTRLB = USART_TXEN_bm;
}

int main(void) {
  sei();
  while (1) ;
}

bool uart_print(const char *p) {
  if (USART0.CTRLA & USART_DREIE_bm) return false;
  ptr = p;
  return (USART0.CTRLA |= USART_DREIE_bm);
}

void toString(char *p, unsigned u) {
  unsigned q = u / 10;
  unsigned r = u - q * 10;
  p[2] = r + '0';
  u = q / 10;
  r = q - u * 10;
  p[1] = r + '0';
  p[0] = u + '0';
}

void print(unsigned space) {
  if (mark) {
    toString(line, mark / 40);
    toString(&line[4], space / 40);
    uart_print(line);
  }
}

ISR(USART0_DRE_vect) {
  if (*ptr) USART0.TXDATAL = *ptr++;
  else USART0.CTRLA &= ~USART_DREIE_bm;
}

ISR(TCB0_INT_vect) {
  TCB0.INTFLAGS = TCB_CAPT_bm;	// clear IF
  print(0);
  mark = 0;
}

ISR(PORTA_PORT_vect) {
  unsigned cnt = TCB0.CNT;
  TCB0.CNT = 0;
  PORTA.INTFLAGS = PIN4_bm;
  if (PORTA.IN & PIN4_bm) mark = cnt;
  else print(cnt);
}
