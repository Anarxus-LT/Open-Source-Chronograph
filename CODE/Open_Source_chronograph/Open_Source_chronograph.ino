/*

 ___________ _____ _   _   _____  _____ _   _______  _____  _____      
|  _  | ___ \  ___| \ | | /  ___||  _  | | | | ___ \/  __ \|  ___|     
| | | | |_/ / |__ |  \| | \ `--. | | | | | | | |_/ /| /  \/| |__       
| | | |  __/|  __|| . ` |  `--. \| | | | | | |    / | |    |  __|      
\ \_/ / |   | |___| |\  | /\__/ /\ \_/ / |_| | |\ \ | \__/\| |___      
 \___/\_|   \____/\_| \_/ \____/  \___/ \___/\_| \_| \____/\____/      
                                                                       
                                                                       
 _____  _   _ ______ _____ _   _ _____ _____ ______  ___  ______ _   _ 
/  __ \| | | || ___ \  _  | \ | |  _  |  __ \| ___ \/ _ \ | ___ \ | | |
| /  \/| |_| || |_/ / | | |  \| | | | | |  \/| |_/ / /_\ \| |_/ / |_| |
| |    |  _  ||    /| | | | . ` | | | | | __ |    /|  _  ||  __/|  _  |
| \__/\| | | || |\ \\ \_/ / |\  \ \_/ / |_\ \| |\ \| | | || |   | | | |
 \____/\_| |_/\_| \_|\___/\_| \_/\___/ \____/\_| \_\_| |_/\_|   \_| |_/
                                                                       
==========================================================================
Read LICENCE
Description: An open source chronograph used to measure the speed and 
            energy of projectiles. Mainly intended for airsoft.
Author: Lykourgos Tanious

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
#define Trigger1 11
#define Trigger2 12
#define BuzzerPin 10
#define SensorDistance 100 // in mm


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Variable decleration
String batper = "";
int bat = 0;
float vbat= 0;
int selector = 0;
float bbweight = 0.23;
float speedmet = 0;
float powerj = 0;
int rps = 0;
int fps = 0;
int shotcounter = 0;
int UIupdate = 0;
unsigned long UIlastUpdate = 0;
volatile bool ButtUpInt = false;
volatile bool ButDownInt = false;
volatile bool ButMenuInt = false;
// time2-time1 1025 gives 320fps for tests
volatile unsigned long time1 = 0;
volatile unsigned long time2 = 0;
unsigned long firstshottime = 0;

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(2000);
  display.clearDisplay();
  //Init buzzer
  // Make sure it does not draw more than 12 mA, use resistor if needed
  pinMode (BuzzerPin, OUTPUT_12MA);
  // Boot splash
  Splash();
  // Main display UI init
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
  display.print("RPS: " + String(rps));
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

  //Init IR Triggers
  pinMode (Trigger1, INPUT);
  pinMode (Trigger2, INPUT);

  //Launch second core
  multicore_launch_core1(core1_IR_Triggers);

}

void loop() {
  // menu button selector handling
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
      break;
  }
  // Button interupt functions
  if (ButMenuInt == true){
    ButMenu();
  }else if (ButDownInt == true){
    ButDown();
  }else if (ButtUpInt == true){
    ButUp();
  }

  // Measure RPS
  if ((shotcounter == 1) && (firstshottime == 0)){
    firstshottime=millis();
  }
  if (((millis()-firstshottime)>1000) && shotcounter != 0){
    rps = shotcounter;
    rpsdisp();
    shotcounter = 0;
    firstshottime = 0;
    message("Wait");
    delay(2000);
    message("Go !");
  }

  // update UI after 10s
  if (UIupdate == 0){
    UIlastUpdate=millis();
    UIupdate = 1;
  }
  if ((millis()-UIlastUpdate)>10000){
    update_battery();
    message("");
    UIupdate = 0;
  }

  // Detect failed read after 1s
  if (((micros() - time1) > 1000000) && time1 != 0){
    time1 = 0;
    time2 = 0;
    message("Retry");
    tone(BuzzerPin, 500, 750);
  }
  if (((micros() - time2) > 1000000) && time2 != 0){
    time1 = 0;
    time2 = 0;
    message("Retry");
    tone(BuzzerPin, 500, 750);
  }


}

void core1_IR_Triggers(){
  while (true){
      while(digitalRead(Trigger1)==LOW);
      while(digitalRead(Trigger1)==HIGH);
      time1=micros();
      while(digitalRead(Trigger2)==LOW);
      while(digitalRead(Trigger2)==HIGH);
      time2=micros();
      if (time1 != 0 && time2 != 0){
      //Got a sucessful read
      fps = floor(0.32808398950131235/((float(time2)-float(time1))/1000000.0));
      message("PEW !");
      fpsdisp(fps);
      msdisp(fps);
      powdisp();
      time1 = 0;
      time2 = 0;
      shotcounter++;
      tone(BuzzerPin, 3000, 500);
      }
  }
}


void rpsdisp(){
  display.fillRect(27, 36, 53, 10, BLACK);
  display.setTextSize(1);
  display.setCursor(27,36);
  display.print(" " + String(rps));
  display.display();
}

void powdisp(){
  powerj = 0.5 * float(bbweight/1000.0) * sq(speedmet);
  display.fillRect(27, 26, 53, 10, BLACK);
  display.setTextSize(1);
  display.setCursor(27,26);
  display.print(" " + String(powerj, 2) + "J");
  display.display();
}

void msdisp(int fps){
  speedmet = fps * 0.3048;
  display.fillRect(27, 16, 53, 10, BLACK);
  display.setTextSize(1);
  display.setCursor(27,16);
  display.print(" " + String(speedmet, 1) + "m/s");
  display.display();
}

void fpsdisp(int fps){
  String fpsstring = String(fps);
  display.fillRect(90, 40, 128, 64, BLACK);
  display.setTextSize(2);
  display.setCursor(90,40);
  display.print(fps);
  display.display();
}

void message(String msg){
  delay(250); //without this delay the display glitches for some reason
  display.fillRect(5, 50, 65, 15, BLACK);
  display.setTextSize(1);
  display.setCursor(5,50);
  display.print(msg);
  display.display();
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
      powdisp();
      tone(BuzzerPin, 2000, 250);
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
      powdisp();
      tone(BuzzerPin, 2000, 250);
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
  tone(BuzzerPin, 1000, 250);
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
    tone(BuzzerPin, 3000, 150);
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

void bootsound(){
  digitalWrite(BuzzerPin, HIGH);
  delay(500);
  digitalWrite(BuzzerPin, LOW);
  delay(500);
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
  bootsound();
  display.clearDisplay();
  display.display();
}
