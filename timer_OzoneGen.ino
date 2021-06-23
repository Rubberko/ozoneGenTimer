#include "U8glib.h"

#define relay 4
#define startStop 3
#define buttonSwitch 2

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	

int firstMinutes;
int firstSeconds;
volatile int tMinutes;
volatile int tSeconds;
char minuty_string[8];
char sekundy_string[8];
char firstSeconds_string[8];
static unsigned long lastTick;
static unsigned long lastTick2;
volatile int buttonCounter;
volatile bool timerReady;
volatile bool firstTimerReady;
unsigned long lastInterrupt1;
unsigned long lastInterrupt2;

void setup(void) {
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  } else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
      u8g.setColorIndex(3);         // max intensity
  } else if ( u8g.getMode() == U8G_MODE_BW ) {
      u8g.setColorIndex(1);         // pixel on
  } else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
      u8g.setHiColorByRGB(255,255,255);
  }

  firstMinutes = 0;
  firstSeconds = 30;
  tMinutes = 30;
  tSeconds = 0;
  timerReady = true;
  firstTimerReady = true;
  buttonCounter = 1;
  
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  pinMode(startStop, INPUT_PULLUP);
  pinMode(buttonSwitch, INPUT_PULLUP);
  
  EIFR = (1 << INTF1);
  attachInterrupt(digitalPinToInterrupt(startStop), startStopInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(buttonSwitch), modeSwitchInterrupt, RISING);
}


void loop(void) {
  if(firstTimerReady == false) {
    if (firstSeconds > -1) {
      if (millis() - lastTick2 >= 1000) {     
        lastTick2 = millis();
        firstSeconds--;
      }
    }
    if (firstMinutes > 0) {
      if (firstSeconds <= -1) {
        firstMinutes--;
        firstSeconds = 59;
      }
    }
    if (firstSeconds == -1) {
      firstSeconds = 0;
    }
  }

  if(firstMinutes == 0 && firstSeconds == 0) {
    timerReady = false;
    firstTimerReady = true;
  }
  
  if(timerReady == false) {
    digitalWrite(relay, HIGH);
    if (tSeconds > -1) {
      if (millis() - lastTick >= 1000) {     
        lastTick = millis();
        tSeconds--;
      }
    }
    if (tMinutes > 0) {
      if (tSeconds <= -1) {
        tMinutes--;
        tSeconds = 59;
      }
    }
    if (tSeconds == -1) {
      tSeconds = 0;
    }
  }
  if(tMinutes == 0 && tSeconds ==0) {
    digitalWrite(relay, LOW);
    firstMinutes = 0;
    firstSeconds = 30;
    timerReady = true;
    firstTimerReady = true;
    buttonCounter = 0;
  }
  
  itoa(tSeconds, sekundy_string, 10);
  itoa(tMinutes, minuty_string, 10);
  itoa(firstSeconds, firstSeconds_string, 10);

  addZero(minuty_string);
  addZero(sekundy_string);
  addZero(firstSeconds_string);
  
  if(!firstTimerReady) {
    u8g.firstPage();  
    do {
      drawFirstTimer(firstSeconds_string);
    } while( u8g.nextPage());
  } else if(tMinutes >= 100) {
          u8g.firstPage();  
          do {
            drawBigger(minuty_string, sekundy_string);
          } while( u8g.nextPage());
          } else {
             u8g.firstPage();  
              do {
                draw(minuty_string, sekundy_string);
              } while( u8g.nextPage());
          }
  delay(50);
}

void addZero(char *pole) {
  if (pole[1] == '\0') {
    pole[1] = pole[0];
    pole[0] = '0';
    pole[2] = '\0';
  }
}

void draw(char *minuty, char *sekundy) {    
  u8g.setFont(u8g_font_fub49n);
  u8g.setFontPosTop();
  u8g.drawStr( 1, 5, minuty);

  u8g.setFontPosTop();
  u8g.setFont(u8g_font_fub30n);
  u8g.drawStr( 80, 35, sekundy);
}

void drawBigger(char *minuty, char *sekundy) {    
  u8g.setFont(u8g_font_fub42n);
  u8g.setFontPosTop();
  u8g.drawStr( -5, 10, minuty);

  u8g.setFontPosTop();
  u8g.setFont(u8g_font_fub20n);
  u8g.drawStr( 95, 25, sekundy);
}

void drawFirstTimer(char *sekundy) {    
  u8g.setFont(u8g_font_fub49n);
  u8g.setFontPosTop();
  u8g.drawStr( 25, 5, sekundy);
}

void startStopInterrupt() { 
  if(millis() - lastInterrupt1 > 200) {    
    if(timerReady && firstTimerReady) {
      firstTimerReady = false;
    } else {
        setup();
    }
    lastInterrupt1 = millis();
  }
}

void modeSwitchInterrupt() {
  if(millis() - lastInterrupt2 > 200) {    
    if(timerReady) {
      buttonCounter++; 
      if(buttonCounter >= 5) {
         buttonCounter = 1;
        }
    }
    if(timerReady) {
      switch(buttonCounter) {
        case 1:
          tMinutes = 30;
          tSeconds = 0;
          break;
        case 2:
          tMinutes = 60;
          tSeconds = 0;
          break;  
        case 3: 
          tMinutes = 90;
          tSeconds = 0;
          break;
        case 4: 
          tMinutes = 120;
          tSeconds = 0;
          break;  
      }
    }
    lastInterrupt2 = millis();
  }
}
