#include <Arduino.h>
#include <Wire.h>
#include "mount.h"
extern mount_t *telescope;
#include "nunchuck.h"
#define ADDRESS 0x52
int chuckbuffer[6];
int lastx, lasty;
void nunchuck_init(int sda, int scl)
{
  Wire.begin(sda, scl);
  Wire.beginTransmission(ADDRESS);
  Wire.write(0xF0);
  Wire.write(0x55);
  Wire.endTransmission();
  delay(1);
  Wire.beginTransmission(ADDRESS);
  Wire.write(0xFB);
  Wire.write(0x00);
  Wire.endTransmission();

  delay(1);
}

void nunchuck_read(void)
{ int count = 0;


  Wire.requestFrom(ADDRESS, 6);
  while (Wire.available())
  {
    chuckbuffer[count] = Wire.read();
    count++;
  }

  if (lastx != (chuckbuffer[0] /= 86)) {
    telescope->srate = chuckbuffer[5]&0x03;
    switch (chuckbuffer[0]) {
      case 0 : mount_move(telescope, 'e'); //Serial.println("Left");
        break;
      case 1 : mount_stop(telescope, 'w'); //Serial.println("CenterX");
        break;
      case 2 :  mount_move(telescope, 'w'); //Serial.println("Rigth");
        break;
      default:  break;

    }
  }

  lastx = chuckbuffer[0];
  if (lasty != (chuckbuffer[1]  /= 86)) {
    telescope->srate = chuckbuffer[5]&0x03;
    switch (chuckbuffer[1] ) {
      case 0 :  mount_move(telescope, 's'); //Serial.println("Down");
        break;
      case 1 :  mount_stop(telescope, 's'); // Serial.println("CenterY");
        break;
      case 2 : mount_move(telescope, 'n'); //Serial.println("Up");
        break;
      default:  break;

    }

  }
  lasty = chuckbuffer[1];

  Wire.beginTransmission(ADDRESS);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();

  delay(1);
}

