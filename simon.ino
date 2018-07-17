//  Simon - Simon game made with Arduino
//  Copyright (C) 2018 Guillermo Diz
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <TimerOne.h>
#include <MultiFuncShield.h>

const char LEDS[3] = {10, 11, 12};
const char BTNS[3] = {A1, A2, A3};
const int NOTES[3] = {2093, 2349, 2637};
const int BEEP = 240;
const char LEDN = 3;
const char BTN = 3;
const char BUZZER = 3;

const char DFTLEDS = 3;
const int DFTMSTIME = 500;
const char MSPENALTY = 25;

enum States { show, wait, ok, fail };
States state = show;

int lvlMsTime = DFTMSTIME, lvlLeds = DFTLEDS;
char* nvlMap;
char* userMap;

void setup() {
  Timer1.initialize();
  MFS.initialize(&Timer1);
  
  for (int i = 0; i < LEDN; i++) {
    pinMode(LEDS[i], OUTPUT);
    digitalWrite(LEDS[i], HIGH);
  }

  for (int i = 0; i < BTN; i++) {
    pinMode(BTNS[i], INPUT);
  }

  pinMode(BUZZER, OUTPUT);
  
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  MFS.write("hola");
  delay(1500);
}

void loop() {
  switch (state) {  
    case show:
      fShow();
      break;
    case wait:
      fWait();
      break;
    case ok:
      fOk();
      break;
    case fail:
      fFail();
      break;
  }
}

void fShow() {
  //Create new maps
  nvlMap = new char[3];
  userMap = new char[3];

  for (int i = 0; i < lvlLeds; i++) {
    nvlMap[i] = random(0, LEDN);
  }

  //Showing alarm
  blinkAll(300, true);
  blinkAll(300, true);
  
  //Show the map
  for (int i = 0; i < lvlLeds; i++) {
    MFS.write(i + 1);
    blink(LEDS[nvlMap[i]], lvlMsTime, NOTES[nvlMap[i]]);
  }

  //Wait for user
  state = wait;
}

void fWait() {
  int btnLast = -1;
  int btnCount = 0;

  MFS.write("dale");
  
  while (btnCount != lvlLeds) {
    if (btnLast == -1) {
      for (int i = 0; i < BTN; i++) {
        if (!digitalRead(BTNS[i])) {
          userMap[btnCount] = i;
          btnLast = i;
          btnCount++;
          MFS.write(btnCount);
          blink(LEDS[i], 50, NOTES[i]);
        }
      }
    } else {
      if (digitalRead(BTNS[btnLast])) {
        btnLast = -1;
      }
    }

    delay(100);
  }

  //Check maps
  state = ok;
  for (int i = 0; i < lvlLeds; i++) {
    if (nvlMap[i] != userMap[i]) {
      state = fail;
      break;
    }
  }

  delete[] nvlMap;
  delete[] userMap;
}

void fOk() {
  MFS.write("uale");
  
  lvlMsTime -= MSPENALTY;
  if (lvlMsTime < MSPENALTY) {
    //Reset
    reset();
  } else {
    lvlLeds++;
  }
  
  state = show;
}

void fFail() {
  MFS.write("no");
  
  blinkAll(1000, BEEP);

  reset();
  state = show;
}

void reset() {
  lvlMsTime = DFTMSTIME;
  lvlLeds = DFTLEDS;
}

void blink(char led, uint16_t ms, int beep) {
  digitalWrite(led, LOW);
  if (beep > 0) tone(BUZZER, beep, ms);
  delay(ms);
  digitalWrite(led, HIGH);
  if (beep > 0) digitalWrite(BUZZER, HIGH);
  delay(ms / 2);
}

void blinkAll(uint16_t ms, int beep) {
  for (int i = 10; i < 14; i++) {
    digitalWrite(i, LOW);
  }
  if (beep > 0) tone(BUZZER, beep, ms);
  delay(ms);
    for (int i = 10; i < 14; i++) {
    digitalWrite(i, HIGH);
  }
  if (beep > 0) digitalWrite(BUZZER, HIGH);
  delay(ms);
}

