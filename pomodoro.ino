/*
Copyright (c) 2020 LHW - createskyblue
Arduino-Pomodoro is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details.

番茄钟程序版本:
V1.0.0
*/
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#define button 13
#define poten A1
#define rs 3
#define en 4
#define d4 5
#define d5 6
#define d6 7
#define d7 8
#define LEDPin 11
#define SoundPin 9
#define MaxMenuLevel 0
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
byte LEDLight;
byte MenuLevel = 0; //菜单深度
int PomTimer[4][3] = {{0, 25, 0}, {0, 5, 0}, {0, 0, 0}}; //番茄钟 休息时间 总学习时间 计时时间
unsigned long ObjectTime; //计时秒控制
unsigned long TimingStartTime;
unsigned long SysSleepTime;
unsigned long TunePlayWait; //播放某个音符的时间
/*
   误差控制
*/
unsigned long TimeCumulativeError[2][2]; //【历史记录】【看门狗中断时 ，计时时 】
unsigned long TimeCumulativeErrorAdd; //累积误差
bool WhoTimeCumulativeError; //历史记录指针

int HistoryPoten;
byte levels = 0;
float Tomato = 999;
const int LevelsTable[7] = {2, 6, 14, 28, 62, 186, 32767};
bool Sound = false;
byte TimingPlus;
bool PlayTone;
int TuneNum = 491;
byte LowPowerMode;
byte Track = 0; //曲目 0-车尼尔 op599,89 1-雨后重生 2-展望 3-晚安喵 4-国际歌 5-车尼尔 op599,60
byte TrackMax = 6; //多少首曲子
const int TrackList[] PROGMEM = {68, 140, 257, 491, 655, 826};
const int tune[] PROGMEM = {
  //车尼尔 op599,89
  330, 392, 440, 392, 330, 392, 262, 330, 0, 330, 392, 440, 392, 330, 392, 262, 330, 0, 294, 349, 392, 349, 294, 349, 497, 294, 0, 262, 330, 349, 330, 262, 330, 392, 262, 0, 330, 392, 440, 392, 330, 392, 262, 330, 0, 330, 392, 440, 392, 330, 392, 262, 330, 0, 294, 349, 392, 349, 294, 349, 497, 294, 0, 262, 392, 330, 392, 262,
  //雨后重生
  294, 587, 497, 0, 294, 587, 392, 0, 294, 440, 523, 0, 497, 440, 349, 0, 330, 349, 392, 0, 392, 440, 262, 0, 294, 330, 349, 0, 349, 392, 440, 0, 523, 497, 523, 587, 392, 0, 523, 497, 392, 0, 523, 497, 330, 0, 523, 497, 349, 0, 440, 262, 294, 0, 392, 330, 349, 392, 523, 0, 392, 349, 330, 440, 523, 0, 523, 497, 587, 698, 784, 988,
  //展望
  262, 392, 330, 349, 262, 392, 330, 0, 262, 392, 330, 349, 262, 392, 262, 0, 262, 330, 392, 0, 262, 330, 440, 0, 262, 330, 497, 0, 262, 330, 262, 0, 294, 330, 349, 330, 349, 392, 0, 392, 262, 294, 349, 497, 0, 497, 349, 392, 497, 497, 0, 880, 784, 659, 587, 0, 587, 0, 587, 659, 698, 0, 698, 784, 880, 0, 880, 523, 587, 698, 988, 988, 988, 880, 880, 0, 587, 497, 392, 0, 392, 497, 349, 0, 349, 440, 262, 0, 440, 262, 294, 0, 392, 440, 349, 0, 349, 294, 262, 0, 330, 392, 349, 0, 392, 330, 262, 0, 392, 330, 349, 0, 392, 262, 497, 587, 392,
  //晚安喵
  392, 659, 659, 0, 392, 587, 0, 587, 392, 523, 0, 523, 523, 587, 0, 392, 659, 0, 659, 392, 587, 0, 587, 392, 523, 0, 523, 497, 523, 0, 392, 392, 659, 0, 659, 659, 587, 0, 523, 497, 523, 497, 440, 392, 0, 392, 392, 523, 0, 523, 523, 497, 0, 497, 497, 523, 587, 659, 587, 0, 392, 392, 0, 659, 659, 784, 0, 784, 698, 659, 698, 0, 523, 0, 440, 497, 523, 0, 523, 587, 587, 0, 523, 587, 523, 0, 392, 659, 0, 659, 523, 587, 0, 587, 392, 523, 0, 523, 523, 587, 0, 392, 659, 0, 659, 392, 587, 0, 587, 392, 523, 0, 523, 497, 523, 0, 392, 392, 392, 0, 0, 0, 587, 0, 523, 497, 523, 0, 497, 440, 392, 0, 392, 392, 523, 0, 523, 523, 497, 0, 497, 497, 523, 587, 587, 587, 0, 392, 392, 659, 0, 659, 659, 784, 0, 784, 698, 659, 698, 0, 523, 0, 523, 0, 523, 0, 523, 0, 523, 0, 497, 523, 587, 523, 0, 392, 659, 0, 659, 392, 587, 0, 587, 392, 523, 0, 523, 523, 587, 0, 392, 659, 0, 659, 392, 587, 0, 587, 392, 523, 0, 523, 497, 523, 0, 392, 659, 0, 659, 392, 587, 0, 587, 392, 523, 0, 523, 523, 587, 0, 392, 659, 0, 659, 392, 587, 0, 587, 392, 523, 0, 523, 497, 523,
  //国际歌
  392, 523, 0, 497, 587, 523, 392, 330, 440, 0, 440, 349, 0, 440, 587, 0, 523, 497, 440, 392, 349, 349, 0, 330, 330, 0, 392, 523, 0, 497, 587, 523, 392, 330, 440, 0, 440, 349, 0, 440, 587, 523, 497, 587, 698, 497, 523, 0, 659, 587, 497, 0, 440, 497, 523, 440, 497, 0, 497, 392, 0, 392, 349, 392, 440, 0, 440, 587, 0, 523, 497, 0, 587, 587, 497, 392, 392, 349, 392, 659, 523, 0, 440, 497, 523, 497, 587, 523, 497, 440, 392, 0, 659, 587, 523, 0, 392, 330, 440, 0, 440, 349, 0, 587, 523, 497, 0, 497, 440, 392, 392, 0, 392, 659, 0, 587, 392, 523, 497, 0, 497, 440, 0, 497, 440, 587, 587, 0, 659, 587, 523, 0, 392, 330, 440, 0, 440, 349, 0, 587, 523, 497, 0, 497, 440, 392, 659, 0, 659, 698, 0, 659, 587, 659, 698, 0, 698, 659, 0, 659, 587, 0, 587, 523,
  //车尼尔 op599,60
  786, 495, 589, 495, 786, 495, 589, 495, 786, 495, 882, 278, 589, 278, 882, 278, 786, 278, 589, 278, 786, 278, 700, 221, 589, 221, 700, 221, 700, 221, 589, 221, 700, 221, 786, 278, 990, 589, 786, 589, 990, 589, 990, 589, 786, 589, 990, 589, 990, 589, 882, 589, 990, 589, 990, 589, 786, 589, 990, 589, 882, 786, 882, 1120, 882, 700, 661, 589, 661, 882, 786, 661, 589, 700, 882, 700, 882, 700, 589, 0, 589, 700, 882, 1120, 882, 700, 589, 700, 882, 1120, 882, 700, 589, 700, 882, 1120, 882, 700, 589, 700, 882, 1120, 882, 700, 589, 700, 882, 1120, 882, 700, 589, 700, 882, 1120, 882, 700, 589, 700, 882, 1120, 882, 700, 589, 700, 882, 1120, 882, 700, 990, 589, 786, 589, 990, 589, 990, 589, 786, 589, 990, 589, 990, 589, 786, 589, 990, 589, 1120, 786, 990, 786, 1120, 786, 990, 882, 1120, 990, 786, 661, 882, 786, 990, 882, 700, 589, 786, 589, 882, 786, 1120, 882, 786,

};
const int TuneDelay[] PROGMEM = {
  //车尼尔 op599,89
  140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
  //雨后重生
  440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440, 440,
  //展望
  480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 540, 540, 540, 480, 540, 540, 540, 480, 540, 540, 540, 540, 540, 540, 540, 540, 540, 540, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480, 480,
  //晚安喵
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  //国际歌
  542, 552, 436, 410, 407, 442, 352, 459, 461, 369, 440, 439, 360, 385, 399, 375, 428, 365, 370, 380, 460, 428, 382, 399, 421, 388, 437, 399, 373, 441, 397, 498, 430, 425, 401, 297, 335, 429, 390, 482, 398, 443, 453, 435, 516, 463, 444, 449, 429, 486, 409, 419, 446, 444, 454, 460, 702, 410, 402, 510, 416, 407, 434, 454, 430, 482, 401, 348, 306, 395, 453, 348, 443, 479, 485, 480, 420, 488, 454, 515, 499, 357, 420, 481, 422, 468, 520, 476, 450, 435, 579, 383, 432, 404, 461, 362, 423, 410, 511, 455, 380, 390, 374, 403, 427, 494, 485, 472, 431, 452, 486, 471, 518, 492, 449, 409, 412, 456, 427, 457, 409, 430, 476, 423, 477, 416, 444, 308, 374, 410, 463, 394, 433, 460, 464, 427, 456, 526, 410, 418, 437, 455, 367, 386, 393, 422, 482, 475, 417, 443, 383, 450, 524, 466, 472, 389, 442, 503, 334, 415, 545, 374, 405, 808,
  //车尼尔 op599,60
  220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220,
};

int ToThePower(int num, byte power) {
  int ANS = 1;
  for (power; power > 0; power--) ANS *= num;
  return ANS;
}
void PrintPomTimer() {
  for (byte x = 0; x < 4; x++) {
    for (byte y = 0; y < 3; y++) {
      Serial.print(PomTimer[x][y]);
      Serial.print(" , ");
    }
    Serial.println();
  }
}

void SysSleepOn() {
  SysSleepTime = millis();
}

void Wake_avr() {
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_disable();
}

void PrintTimeCumulativeError() {
  //Serial.print();
  Serial.println();
  Serial.print("系统时间 "); Serial.println(millis());
  Serial.print("指针 "); Serial.println(WhoTimeCumulativeError);
  Serial.print("  => "); Serial.print(!TimeCumulativeError[0][0]); Serial.print(" , "); Serial.println(!TimeCumulativeError[0][1]);
  Serial.print("累计 "); Serial.println(TimeCumulativeErrorAdd);
}

void setup_watchdog(int ii) {
  byte bb;
  if (ii > 9 ) ii = 9;
  bb = ii & 7;
  if (ii > 7) bb |= (1 << 5);
  bb |= (1 << WDCE);
  MCUSR &= ~(1 << WDRF);
  // start timed sequence
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  // set new watchdog timeout value
  WDTCSR = bb;
  WDTCSR |= _BV(WDIE);
}



void Sleep_avr() {
  // 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
  // 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
  setup_watchdog(6 + LowPowerMode);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  /*
     记录累积误差时间结束
  */
  PrintTimeCumulativeError();
  TimeCumulativeError[WhoTimeCumulativeError][1] = millis();
  WhoTimeCumulativeError = !WhoTimeCumulativeError;
  sleep_mode();                        // System sleeps here
}



void SysSleep() {
  if (millis() < SysSleepTime + 5000) {
    if (TuneNum >= pgm_read_word_near(&TrackList[Track])) analogWrite(LEDPin, LEDLight);
    Wake_avr();
  } else {
    analogWrite(LEDPin, 0);
    Sleep_avr();
  }
}

void RandomClear() {
  for (byte i = 0; i < 20; i++) {
    for (byte y = 0; y < 2; y++)
      for (byte x = 0; x < 16; x++) {
        lcd.setCursor(x, y);
        lcd.print(random(0, 9));
      }
    delay(50);
    SysSleepOn();
    SysSleep();
  }
  lcd.clear();
}

void ClearTimeCumulativeError() {
  //Serial.println("清除历史误差");
  for (byte i = 0; i < 2; i++) {
    //清除历史数据
    TimeCumulativeError[i][0] = 0;
    TimeCumulativeError[i][1] = 0;
  }
}

bool ScintillationFreqency(byte T) {
  return (millis() % (T) > T / 2.0);
}

void ShowPomodoroTime(byte Timemode, byte WhatNum, bool StudyOrRest) {
  /*
    Timemode 0 番茄钟 1休息时间
    lcd.print("");
    PomTimer[][]
    PomTimer[3][3] = {{0, 25, 0}, {0, 0, 5}}; //番茄钟 休息时间 总学习时间
  */

  //ClearRow(0,1,16);
  lcd.setCursor(0, 1);
  if (!StudyOrRest) lcd.print("Study   "); else lcd.print("Rest    ");
  for (byte i = 0; i < 3; i++) {
    lcd.setCursor(8 + i * 3, 1);
    lcd.print("  ");
    if (i != 2) lcd.print(":");
    lcd.setCursor(8 + i * 3, 1);
    if (WhatNum == i && ScintillationFreqency(50)) lcd.print(F("  ")); else lcd.print(PomTimer[Timemode][i]);

  }
}
void ClearRow(byte x, byte y, byte ClearLong) {
  if (ClearLong == 0) ClearLong = 16;
  lcd.setCursor(x, y);
  for (byte i = 0; i < ClearLong; i++) lcd.print(" ");
}
void ShowPrompt() {
  if (levels >= 5) lcd.print("$"); else lcd.print("#");
}
void ShowPomodoroMode(bool mode, bool pause) {
  ClearRow(0, 0, 16);
  lcd.setCursor(0, 0);
    if (!pause) if (!mode) lcd.print(F("Pomodoro")); else lcd.println(F("Timing  "));
  lcd.setCursor(8, 0);
  ShowPrompt();
  lcd.print(Tomato);
}
bool WaittingButton(int CheckDelay) {
  delay(CheckDelay);
  return digitalRead(button);
}

byte LongButton() {
  if (WaittingButton(50)) {
    SysSleepOn();
    SysSleep();
    lcd.setCursor(0, 0);
    for (byte i = 0; i < 16;) {
      if (WaittingButton(50)) {
        noTone(SoundPin);
        lcd.write(0xFF);
        i++;
        delay(100);
      } else {
        lcd.clear();
        if (i < 4) {
          return 0;
        } else if (i < 8) return 1; else return 2;
      }
    }
    lcd.clear();
    delay(150);
    return 3;
  }
}
void DelTimeCumulativeError() {
  //TimeCumulativeError[!WhoTimeCumulativeError][0] = 0;
  //TimeCumulativeError[!WhoTimeCumulativeError][1] = 0;
  //启动延时3秒
  if (TimingStartTime > ToThePower(2, LowPowerMode)) {
    //计算累积误差
    TimeCumulativeErrorAdd += TimeCumulativeError[!WhoTimeCumulativeError][1] - TimeCumulativeError[!WhoTimeCumulativeError][0];
    //用累积误差兑换一秒的时间
    for (TimeCumulativeErrorAdd / 1000; TimeCumulativeErrorAdd / 1000 > 0; TimeCumulativeErrorAdd -= 1000) TimingPlus++;
  } else TimeCumulativeErrorAdd = 0;
}

void TotalTimeTimerPlus() {
  PomTimer[2][2]++;
  for (byte ii = 1; ii < 3; ii++) {
    if (PomTimer[2][ii] > 59) {
      PomTimer[2][ii] = 0;
      PomTimer[2][ii - 1]++;
    }
  }
}
void mc_levels() {
  //PomTimer[2][0] ：学习总小时
  levels = 0;
  for (byte i = 0; i < sizeof(LevelsTable) / sizeof(LevelsTable[0]); i++) if (PomTimer[2][0] >= LevelsTable[i]) {
      Serial.println(PomTimer[2][0] + String(" > ") + LevelsTable[i] + String("  i=") + i);
      levels++;
    }
}
void RewardTomato() {
  if (TimingStartTime == 60) { //防止恶意调时间作弊
    Tomato += (1 + (levels - 1) * 0.2) * 0.2;
    if (Tomato > 999) Tomato = 999;
    PomTimer[2][2]++;
    mc_levels();
  }
}
void BeginSoundEffect() {
  if (Track - 1 >= 0) TuneNum = pgm_read_word_near(&TrackList[Track - 1]); else TuneNum = 0;
  TunePlayWait = millis() + pgm_read_word_near(&TuneDelay[TuneNum]);
}
void EndSound() {
  TuneNum = pgm_read_word_near(&TrackList[Track]);
  noTone(SoundPin);
}

void SoundEffect() {
  if (Sound && TuneNum < pgm_read_word_near(&TrackList[Track])) {
    SysSleepOn();
    if (TuneNum < pgm_read_word_near(&TrackList[Track]) && !PlayTone) {
      //Serial.print(TuneNum); Serial.print(" -> "); Serial.println(pgm_read_word_near(&tune[TuneNum]));
      tone(SoundPin, pgm_read_word_near(&tune[TuneNum]));
      PlayTone = 1;
    }
    if (millis() > TunePlayWait && TuneNum < pgm_read_word_near(&TrackList[Track])) {
      PlayTone = 0;
      TunePlayWait = millis() + pgm_read_word_near(&TuneDelay[TuneNum]);
      noTone(SoundPin);//停止当前音符，进入下一音符
      TuneNum++;
    }
  } else {
    noTone(SoundPin);
  }
}

void PomodoroExecute(bool mode, bool state) {
  byte ButtonState;
  bool pause = false;
  lcd.setCursor(0, 0);
  lcd.print(F("READY "));
  delay(1000);
  TimingStartTime = 0;
  ObjectTime = millis() / 1000;
  ClearTimeCumulativeError();
  wdt_reset();
  while (PomTimer[3][0] + PomTimer[3][1] + PomTimer[3][2] > 0 && PomTimer[3][0] >= 0 || mode) {
    ShowPomodoroTime(3, 255, state);
    ShowPomodoroMode(mode, pause);
    //判断输入
    ButtonState = LongButton();
    if (ButtonState > 0) {
      if (ButtonState != 3) {
        pause = !pause;
      } else {
        RandomClear();
        break;
      }
    }
    //暂停功能
    if (!pause) {

      //计时器
      for (TimingPlus; TimingPlus > 0; TimingPlus--) {
        //DelTimeCumulativeError();  已被证实无效
        ObjectTime = millis() / 1000;
        TotalTimeTimerPlus();
        PomTimer[3][2]--;
        if (mode) PomTimer[3][2] += 2;
        if (TimingStartTime < 60) TimingStartTime++;
        for (byte i = 1; i < 3; i++) {
          if (PomTimer[3][i] < 0) {
            PomTimer[3][i] = 59;
            PomTimer[3][i - 1]--;
            RewardTomato();
          }
          if (PomTimer[3][i] > 59) {
            PomTimer[3][i] = 0;
            PomTimer[3][i - 1]++;
            if (state != 1) RewardTomato();
          }
        }
      }
      SoundEffect(); //声效程序
    } else {
      TimingPlus = 0;
      //暂停时执行这里
      lcd.setCursor(0, 0);
      ShowPrompt();
      if (ScintillationFreqency(500)) lcd.print("PAUSE");
      delay(200);
    }
    SysSleep();
  }

}
void ShowLevel() {
  while (1) {
    SoundEffect();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.setCursor(0, 1);
    lcd.print("LV"); lcd.print(levels);
    lcd.print("");
    ShowPrompt();
    lcd.print(Tomato);
    lcd.print(" "); lcd.print(PomTimer[2][0]); lcd.print(":"); lcd.print(PomTimer[2][1]);
    lcd.setCursor(0, 0);
    if (ScintillationFreqency(500)) {
      lcd.setCursor(0, 0);
      switch (levels) {
        case 0:
          lcd.print(F("Z Zero,the start"));
        case 1:
          lcd.print(F("Y -Yes"));
          break;
        case 2:
          lcd.print(F("K -Knowledge"));
          break;
        case 3:
          lcd.print(F("P -Patience"));
          break;
        case 4:
          lcd.print(F("Q -Quit"));
          break;
        case 5:
          lcd.print(F("X -Xfactor"));
          break;
        case 6:
          lcd.print(F("A -Ackrowledging"));
          break;
        case 7:
          lcd.print(F("D -Developer"));
          break;
      }
    } else  ClearRow(0, 0, 16);
    if (LongButton() >= 1) break;
  }
  RandomClear();
}

void ClearEEPROM() {
  for (byte i = 4; i >= 1; i--) {
    lcd.clear();
    lcd.print("COUNT DOWN ");
    lcd.print(i - 1);
    delay(1000);
  }
  for (int i = 0; i < EEPROM.length(); i++) {
    lcd.setCursor(0, 0);
    for (byte ii = 0; ii <= map(i, 0, 1024, 0, 15); ii++) lcd.write(0xFF);

    lcd.setCursor(0, 1);
    lcd.print("Address ");
    lcd.print(i);
    lcd.print("=>0");
    EEPROM.update(i, 0);
  }
  lcd.clear();
  lcd.print("Complete!");
  delay(1000);
  lcd.clear();
}
void CheckEEPROM(bool mode) {
  byte ECheck = EEPROM.read(0);
  byte MyCheck = 0;
  byte data;
  lcd.setCursor(0, 1);
  lcd.print("Keep Power . . .");
  for (int i = 1; i < EEPROM.length(); i++) {
    data = EEPROM.read(i);
    if (levels >= 7) {
      for (byte ii = 0; ii <= map(i, 0, 1024, 0, 15); ii++) lcd.write(0xFF);
      lcd.setCursor(0, 1);
      lcd.print(i);
      lcd.print("/");
      lcd.print(EEPROM.length());
      lcd.print(":");
      lcd.print(data);
      Serial.print("====== EEPROM ======");
      Serial.print("Address ");
      Serial.print(i);
      Serial.print("=>");
      Serial.println(data);
    }
    MyCheck += data;
  }
  lcd.clear();
  if (!mode) {
    if (ECheck == MyCheck) {
      lcd.print("SAV safe!");
      delay(200);
    } else {
      lcd.print("SAV OOPS!!! E:M");
      lcd.setCursor(0, 1);
      lcd.print(ECheck); lcd.print(" != "); lcd.print(MyCheck);
      delay(1000);
      //ClearEEPROM();
    }
  } else {
    EEPROM.update(0, MyCheck);
    lcd.print("Complete!");
    delay(500);
  }
  lcd.clear();
}

void EEPROMSave() {
  lcd.clear();
  lcd.print("Saving EEPROM");
  delay(500);
  /*  地址    用途
     509-513 T币
     514 LEDLight
     515 Sound
     516 LowPowerMode
     521-532 为 番茄钟 休息时间 总学习时间 计时时间
     533-539 选曲 Track
     0为校验码 使用累加校验
  */
  EEPROM.put(509, Tomato);
  EEPROM.update(514, LEDLight);
  EEPROM.update(515, Sound);
  EEPROM.update(516, LowPowerMode);
  EEPROM.update(533, Track);
  int EEPROMAddress = 521;
  for (byte x = 0; x < 4; x++)
    for (byte y = 0; y < 3; y++) {
      EEPROM.update(EEPROMAddress, PomTimer[x][y]);
      EEPROMAddress++;
    }
  CheckEEPROM(1);
}

void EEPROMLoad() {
  lcd.clear();
  lcd.print("Loading EEPROM");
  delay(500);
  EEPROM.get(509, Tomato);
  if (Tomato < 0) Tomato = 0;
  if (Tomato > 999) Tomato = 999;
  LEDLight = EEPROM.read(514);
  SysSleep();
  Sound = EEPROM.read(515);
  LowPowerMode = EEPROM.read(516);
  Track = EEPROM.read(533);
  int EEPROMAddress = 521;
  for (byte x = 0; x < 4; x++)
    for (byte y = 0; y < 3; y++) {
      PomTimer[x][y] = EEPROM.read(EEPROMAddress);
      EEPROMAddress++;
    }
  CheckEEPROM(0);

}
void Pomodoro(bool mode) {
  RandomClear();
  /*
     unsigned int PomTimer[4][3] = {{0, 25, 0}, {0, 5, 0}, {186, 0, 0}}; //番茄钟 休息时间 总学习时间 计时时间
     模式0 倒计时 模式1 正计时
  */
  BeginSoundEffect();
  for (byte iii = 0; iii < 2 - mode; iii++) {
    if (mode) for (byte i = 0; i < 3; i++) PomTimer[3][i] = 0; else {
      PomTimer[3][0] = PomTimer[iii][0];
      PomTimer[3][1] = PomTimer[iii][1];
      PomTimer[3][2] = PomTimer[iii][2];
    }

    TimingPlus = 0;
    PomodoroExecute(mode, iii);
    noTone(SoundPin);
    RandomClear();
    for (byte i = 0; i < random(3, 10); i++) {
      analogWrite(LEDPin, 255);
      lcd.print(F("!!!Great"));
      delay(250);
      lcd.clear();
      analogWrite(LEDPin, 0);
      delay(250);
    }

    BeginSoundEffect();
    if (iii == 1) RewardTomato(); //休息奖励
    RandomClear();
  }
  EEPROMSave();
  ShowLevel();

}


int PotenGet() {
  if (analogRead(poten) > HistoryPoten + 3 || analogRead(poten) < HistoryPoten - 3) {
    HistoryPoten = analogRead(poten);
    SysSleepOn();
    SysSleep();
  }
  return analogRead(poten);
}
void EnterLEDLight() {
  while (LongButton() == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("EnterLEDLight");
    lcd.setCursor(0, 1);
    for (byte i = 0; i < map(PotenGet(), 0, 1023, 1, 16); i++) lcd.write(0xFF);
    LEDLight = map(PotenGet(), 0, 1024, 0, 255);
    analogWrite(LEDPin, LEDLight);
    delay(50);
  }
  EEPROMSave();
}
void ShowTomato() {
  ShowPrompt();
  lcd.print(Tomato);
}
bool PayTomato(byte price) {
  if (Tomato - price > 0) {
    int Ans = Tomato - price;
    for (Tomato; Tomato > Ans; Tomato--) {
      // Serial.print(Tomato);Serial.print(" : ");Serial.println(Ans);
      lcd.clear();
      lcd.setCursor(4, 0);
      ShowTomato();
      delay(300);
    }
    EEPROMSave();
    lcd.setCursor(0, 1);
    lcd.print("Payment Success");
    delay(1500);
    return 1;
  } else {
    lcd.clear();
    lcd.setCursor(4, 0);
    ShowTomato();
    lcd.setCursor(0, 1);
    lcd.print("Not enough 5T");
    delay(3000);
    return 0;
  }
}

void SongSet() {
  byte HaveTrack = Track;
  lcd.clear();
  lcd.print("Change song need");
  lcd.setCursor(0, 1);
  lcd.print("5 Tomato ");
  ShowTomato();
  delay(3000);
  lcd.clear();
  byte state = LongButton();
  while (state != 3) {
    state = LongButton();
    if (state == 2) BeginSoundEffect();
    if (state == 1) EndSound();
    SoundEffect();
    lcd.setCursor(0, 0);
    lcd.print("Soung Choose");
    lcd.setCursor(0, 1);
    lcd.print(" -->");
    Track = map(PotenGet(), 0, 1023, 0, TrackMax);
    if (Track >= TrackMax) Track = TrackMax - 1;
    lcd.print(Track);
  }
  if (Track != HaveTrack) if (!PayTomato(5)) Track = HaveTrack;
}




void SoundEffectSet() {
  while (LongButton() == 0) {
    SysSleep();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SoundEffectSet");
    lcd.setCursor(0, 1);
    if (PotenGet() < 512) lcd.print("Enable"); else lcd.print("Disable");
    Sound = PotenGet() < 512;
    delay(50);
  }
  EEPROMSave();
}



bool CheckButton(int CheckDelay) {
  if (digitalRead(button)) {
    delay(CheckDelay);
    return !digitalRead(button);
  }
  return 0;
}


void EnterTime() {
  lcd.setCursor(0, 0);
  lcd.print(F("Pomodoro set"));
  //InputProgress 0小时 1分钟 2秒 3小时 4分钟 5秒
  for (byte y = 0; y < 2; y++) {
    for (byte x = 0; x < 3;) {
      lcd.clear();
      SysSleep();
      PomTimer[y][x] = map(PotenGet(), 0, 1023, 0, 60);
      if (PomTimer[y][x] >= 60) PomTimer[y][x] = 59;
      ShowPomodoroTime(y, x, y);
      if (LongButton() > 0) {
        x++;
      }
      delay(150);
    }
  }
  EEPROMSave();
  lcd.clear();
  lcd.print(F("OK!"));
  delay(1000);
  RandomClear();
}




ISR(WDT_vect) {
  /*
     记录开始累积时间误差
  */
  TimeCumulativeError[WhoTimeCumulativeError][0] = millis();
  delay(50);
  PotenGet();
  TimingPlus += ToThePower(2, LowPowerMode);
}

void MenuLevelUpAndDown(byte UP, byte DOWN) {
  switch (LongButton()) {
    case 1:
      MenuLevel = DOWN;
      RandomClear();
      break;
    case 2:
      MenuLevel = DOWN;
      RandomClear();
      break;
    case 3:
      MenuLevel = UP;
      RandomClear();
      break;
  }
}

void PowerSet() {
  while (LongButton() == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LowPowerMode");
    lcd.setCursor(0, 1);
    lcd.print("WDT Walk Up ");
    LowPowerMode = map(PotenGet(), 0, 1023, 0, 3);
    lcd.print(ToThePower(2, LowPowerMode));
    lcd.print(" s");
    delay(200);
  }
  EEPROMSave();
}

void Menu() {
  MenuLevel = 0;
  byte TheReturn;
  char buffer[8];
  while (1) {
    SysSleep();
    EndSound();
    int x;
    if (MenuLevel % 2) x = map(PotenGet(), 0, 1023, 0, 63); else x = map(PotenGet(), 0, 1023, 63, 0);
    ClearRow(0, 1, 16);
    lcd.setCursor(6, 0);
    if ((x - (x / 16) * 16) > 4 && (x - (x / 16) * 16) < 11) lcd.print(F("OwO!")); else lcd.print(F("Cuo?"));
    lcd.setCursor(x - (x / 16) * 16, 1);
    switch (MenuLevel) {
      case 0:
        switch (x / 16) {
          case 0:
            lcd.print(F("Pomodoro"));
            if (LongButton() > 0) Pomodoro(0);
            break;
          case 1:
            lcd.print(F("Timing"));
            if (LongButton() > 0) Pomodoro(1);
            break;
          case 2:
            lcd.print(F("Level"));
            if (LongButton() > 0) {
              RandomClear();
              ShowLevel();
            }
            break;
          case 3:
            lcd.print(F("Set"));
            if (LongButton() > 0) {
              RandomClear();
              MenuLevel = 254;
            }
            break;
            //case 3:
            //lcd.print(F(". . ."));
            //MenuLevelUpAndDown();
            // break;
        }
        break;

      case 254:
        switch (x / 16) {
          case 0:
            lcd.print(F("Up/DOWN"));
            MenuLevelUpAndDown(253, 0);
            break;
          case 1:
            lcd.print(F("Sound Effect Set"));
            if (LongButton() > 0) {
              RandomClear();
              SoundEffectSet();
            }
            break;
          case 2:
            lcd.print(F("Pomodoro Set"));
            if (LongButton() == 3) {
              RandomClear();
              EnterTime();
            }
            break;
          case 3:
            lcd.print(F("Screen Light"));
            if (LongButton() > 0) {
              RandomClear();
              EnterLEDLight();
            }
            break;
        }
        break;
      case 253:
        switch (x / 16) {
          case 0:
            lcd.print(F("EEPROM"));
            MenuLevelUpAndDown(252, 252);
            break;
          case 1:
            lcd.print(F("Songs SHOP"));
            if (LongButton() > 0) {
              RandomClear();
              SongSet();
            }
            break;
          case 2:
            lcd.print(F("POWER"));
            if (LongButton() > 1) {
              RandomClear();
              PowerSet();
            }
            break;
          case 3:
            lcd.print(F("BACK"));
            MenuLevelUpAndDown(254, 254);
            break;
        }
        break;
      case 252:
        switch (x / 16) {
          case 3:
            lcd.print(F("Very dangerous!!!"));
            break;
          case 2:
            lcd.print(F("lose everything"));
            break;
          case 1:
            lcd.print(F("-Clear EEPROM-"));
            if (LongButton() == 3) ClearEEPROM();
            break;
          case 0:
            lcd.print(F("BACK"));
            MenuLevelUpAndDown(253, 253);
            break;
        }
        break;
        /*
              case 000:
                switch (x / 16) {
                  case 0:
                    break;
                  case 1:
                    break;
                  case 2:
                    break;
                  case 3:
                    break;
                }
                break;
        */

    }
  }
}



void setup() {
  Serial.begin(115200);
  pinMode(button, INPUT);
  pinMode(SoundPin, OUTPUT);
  lcd.begin(16, 2);
  lcd.print("OK!");
  EEPROMLoad();
  mc_levels();
  //Tomato = 22;
}

void loop() {
  /*
    switch (Menu()) {
    case 0:
      Pomodoro();
    }
  */
  Menu();
}
