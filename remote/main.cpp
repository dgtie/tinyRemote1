#include <avr/interrupt.h>

void key_standby(void);
char key_read(void);
void ir_tca0_isr(void), ir_stop(void), ir_start(const char*);
void rc6(char *p, unsigned char a, unsigned char c);

namespace
{

const unsigned char code[] = { 88, 90, 92, 91, 16, 89, 63, 17, 13, 84,
                               255, 12 };

void __attribute__((used, naked, section(".init8"))) init(void) {
  TCA0.SINGLE.PER = 1667;	// 500us (~ bit width of IR data)
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
  TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm;
  PORTA.DIRSET = PIN7_bm;
  SLPCTRL.CTRLA = SLPCTRL_SMODE_PDOWN_gc | SLPCTRL_SEN_bm;
//  PORTB.DIRSET = PIN2_bm;
//  USART0.BAUD = 116;
//  USART0.CTRLB = USART_TXEN_bm;
}

volatile unsigned char tick;

void wait(unsigned char t) { 	// wait for "t" ticks
  t += tick;
  while (t != tick);
}

}//anonymous

int main(void) {
  char buffer[100];		// holds the ir bit stream
  sei();
  while (1) {
    key_standby();		// set pin change interrupt
    asm volatile ("sleep");	// to be wake up by pressing a key
    wait(20);			// skip key bouncing (~ 10 ms)
    char k = key_read();	// read key
//    USART0.TXDATAL = k + '0';
//    wait(2);
    if (k > 11) continue;	// loop back with invalid key
    unsigned char c = code[k];	// get ir code
    if (c != 255) {		// skip for null
      rc6(buffer, 4, c);	// set up bit stream in buffer
      ir_start(buffer);		// send out stream in background
    }
    char k1 = k;		// wait for key release
    while (k1 == k) {
      wait(20);			// de-bouncing
      k1 = key_read();		// read key
    }
    ir_stop();			// end ir stream
  }
}

ISR(TCA0_OVF_vect) {
  TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;	// clear IF
  ir_tca0_isr();				// send one bit ir data
  tick++;					// for wait()
}
