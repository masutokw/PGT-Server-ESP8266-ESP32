#include "conf.h"
#include "focus.h"
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include "mount.h"
#include "focus.h"
extern mount_t *telescope;
extern int  focuspeed;
extern int  focuspeed_low;
extern int focusmax;
const uint16_t kRecvPin = 2;

IRrecv irrecv(kRecvPin);
decode_results results;
uint32_t last, lastpresstime;
char ir_state = 0;
void ir_init(void) {

  irrecv.enableIRIn();  // Start the receiver


}

void ir_read(void) {
  uint32_t  code;
  if (irrecv.decode(&results)) {
    if (results.value == 0xFFFFFFFFFFFFFFFF) code = 0xFFFF; else
      last = code = results.command;
    switch (code) {
      // print() & println() can't handle printing long longs. (uint64_t)
      case 0x16: mount_move(telescope, 'e'); lastpresstime = millis(); ir_state = 1; break;
      case 0x14: mount_move(telescope, 'w'); lastpresstime = millis(); ir_state = 1; break;
      case 0x15: mount_stop(telescope, 'w'); mount_stop(telescope, 's'); lastpresstime = millis(); ir_state = 1; break;
      case 0x11: mount_move(telescope, 'n'); lastpresstime = millis(); ir_state = 1; break; ;
      case 0x19: mount_move(telescope, 's'); lastpresstime = millis(); ir_state = 1; break;
      case 0x0A: lastpresstime = millis(); ir_state = 1; gotofocuser(telescope->azmotor, 100000, focuspeed); break;
      case 0x06: lastpresstime = millis(); ir_state = 1; gotofocuser(telescope->azmotor, 0, focuspeed); break;
      case 1:  case 2: telescope->srate = code - 1; break;
      case 4: case 5: telescope->srate = code - 2 ; break;
      case 0xFFFF:  lastpresstime = millis(); ir_state = 1; break; //mount_move(telescope, 's');
      default: ir_state = 0; break;
    }
    // last = code;
    irrecv.resume();  // Receive the next value
  } else if ((millis() - lastpresstime > 200) && (ir_state)) {
    ir_state = 0;
    switch (last) {
      case 0x16: case 0x14: mount_stop(telescope, 'w'); break;
      case 0x11: case 0x19: mount_stop(telescope, 's'); break;
      case 0x06: case 0x0A: stopfocuser(telescope->azmotor);; break;
      default: break;
      
    }
    last = 0xFFFF;
  }
}
