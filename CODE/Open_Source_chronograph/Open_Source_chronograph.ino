/*



*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "pico/multicore.h"



#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define UpButtonPin 13
#define DownButtonPin 14
#define MenuButtonPin 15


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


String batper = "";
int bat = 0;
float vbat= 0;
int selector = 0;
float bbweight = 0.23;
float speedmet = 0;
float powerj = 0;
float rps = 0;
int fps = 0;
volatile bool ButtUpInt = false;
volatile bool ButDownInt = false;
volatile bool ButMenuInt = false;

void setup() {
  Serial.begin(9200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(2000);
  display.clearDisplay();
  // Boot splash
  Splash();
  // Main display
  display.drawLine(0, 14, 128, 14, WHITE);
  update_battery();
  display.setTextSize(1);
  display.setCursor(5,3);
  display.print("BB: " + String(bbweight) + "g");
  display.setCursor(5,16);
  display.print("SPD: " + String(speedmet) + "m/s");
  display.setCursor(5,26);
  display.print("POW: " + String(powerj) + "J");
  display.setCursor(5,36);
  display.print("RPS: " + String(powerj));
  display.drawLine(80, 16, 80, 64, WHITE);
  display.setCursor(90,20);
  display.print("FPS");
  display.setTextSize(2);
  display.setCursor(90,40);
  display.print(fps);
  display.display();

  //init buttons
  pinMode (UpButtonPin, INPUT_PULLUP);
  pinMode (DownButtonPin, INPUT_PULLUP);
  pinMode (MenuButtonPin, INPUT_PULLUP);
  attachInterrupt(UpButtonPin, ButtUpInt_f, LOW );
  attachInterrupt(DownButtonPin, ButDownInt_f, LOW );
  attachInterrupt(MenuButtonPin, ButMenuInt_f, LOW );

  //Launch second core
    multicore_launch_core1(core1_UI_updater);

}

void loop() {
  switch(selector){
    case 0:
      break;
    case 1:
      display.fillRect(0, 0, 25, 10, BLACK);
      display.display();
      delay(250);
      display.setTextSize(1);
      display.setCursor(5,3);
      display.print("BB: ");
      display.display();
      delay(250);
      //Serial.println("blink");
      break;
  }
  if (ButMenuInt == true){
    ButMenu();
  }else if (ButDownInt == true){
    ButDown();
  }else if (ButtUpInt == true){
    ButUp();
  }

}

void core1_UI_updater(){
  while (true){
      update_battery();
      delay(10000);
  }
}

void ButtUpInt_f(){
  ButtUpInt = true;
}
void ButDownInt_f(){
  ButDownInt = true;
}
void ButMenuInt_f(){
  ButMenuInt = true;
}

void ButUp(){
  switch(selector){
    case 0:
      break;
    case 1:
      bbweight += 0.01;
      updatebb_weight();
      break;
  }
  ButtUpInt = false;
}

void ButDown(){
  switch(selector){
    case 0:
      break;
    case 1:
      bbweight -= 0.01;
      updatebb_weight();
      break;
  }
  ButDownInt = false;
}

void ButMenu(){
  selector++;
  if (selector>1){
    selector=0;
  }
  delay(200);
  ButMenuInt = false;
}

void updatebb_weight(){
  display.fillRect(25, 0, 50, 10, BLACK);
  display.setTextSize(1);
  display.setCursor(5,3);
  display.print("BB: " + String(bbweight) + "g");
  display.display();
  delay(200);
}

void update_battery(){
  vbat = analogRead(29)/100.0;
  bat = ceil(((vbat-3)/(4.2-3))*100);
  //Serial.println("raw:  "+ String(analogRead(29)));
  //Serial.println("vbat:  "+ String(vbat));
  //Serial.println("bat:  "+ String(bat));

  if (bat == 100){
    batper = ""+String(bat) + "%";
  }else if (bat < 100 && bat >= 10){
    batper = " "+String(bat) + "%";
  }else if (bat>1 && bat < 10){
    batper = "  "+String(bat) + "%";
  }else if (bat > 100){
    batper = " EXT";
  }else if (bat < 1){
     batper = " LOW";
     display.fillRoundRect(100, 1, 30, 11, 2, BLACK);
     display.display();
     delay(1000);
  }
  display.fillRoundRect(100, 1, 25, 11, 2, BLACK);
  display.drawRoundRect(100, 1, 25, 11, 2, WHITE);
  display.fillRect(125, 5, 2, 4, WHITE);
  display.setTextSize(0);
  display.setTextColor(WHITE);
  display.setCursor(101, 3);
  display.println(batper);
  display.display();
}

void Splash(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(40, 0);
  display.println("ANARXUS");
  display.setTextSize(2);
  display.setCursor(35, 20);
  display.println("OPEN");
  display.setCursor(25, 40);
  display.println("CHRONO");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
}