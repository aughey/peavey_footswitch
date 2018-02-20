/* 
Code for interfacing to the Peavey Vypr Amp with an Arduino through the MIDI port.
Written by John H. Aughey.

Copyright 2018 John H. Aughey

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
associated documentation files (the "Software"), to deal in the Software without restriction, including 
without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to 
the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

// Button are wired into sequential input pins
const int FirstButtonPin = 2;    // the number of the pushbutton pin
// LEDs are wired into sequential output pins (Currently not used)
const int FirstLEDPin = 4;      // the number of the LED pin

// The state of the onboard LED.  This is toggled every time a switch is pressed.
int visible_led = LOW;

// How many milliseconds we wait between button presses.  This helps with noisy switches.
long debounceDelay = 50;

// How many banks (switches) we have.
#define BANKS 2
int selected_bank = 0;
char bank_state[BANKS];
char switch_state[BANKS];

void setup() {
  for(int i=0;i<BANKS;++i) {
    bank_state[i] = 0;
  }

  for(int i=0;i<BANKS;++i) {
    pinMode(FirstButtonPin + i, INPUT);
    // enable pull-up resistors on pin switches
    digitalWrite(FirstButtonPin + i, HIGH);
    switch_state[i] = digitalRead(FirstButtonPin + i);

    pinMode(FirstLEDPin + i, OUTPUT);
    digitalWrite(FirstLEDPin + i, 0);
  }
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, visible_led);

  Serial.begin(31250);
}

void loop() {

  // Go through each bank (switch)
  for(int i=0;i<BANKS;++i) {
    int value = digitalRead(FirstButtonPin + i);
    // If this switch state has changed
    if(value != switch_state[i]) {
      // Remember it
      switch_state[i] = value;
      // Toggle or switch to this bank
      toggleOrSwitchBank(i);
      // This is a stupid simple debounce.  We don't do anything, we just wait
      delay(debounceDelay);
    }
  }
}

void toggleOrSwitchBank(int bank) {
  if(selected_bank == bank) {
    bank_state[bank] = !bank_state[bank];
  } else {
    selected_bank = bank;
  }
  patch(bank * 2 + bank_state[selected_bank]);
}

//MIDI COMMANDS
void patch(int instrument) {    
  Serial.write((byte)0xC0);
  Serial.write((byte)instrument);

  visible_led = !visible_led;
  digitalWrite(LED_BUILTIN, visible_led);
}

void controller(int controller, int value) {
  Serial.write((byte)0xB0);
  Serial.write((byte)controller);
  Serial.write((byte)value);
}

void noteOn(int key, int vel) {
  Serial.write((byte)0x90);
  Serial.write((byte)key);
  Serial.write((byte)vel);
}

void noteOff(int key) {
  Serial.write((byte)0x90);
  Serial.write((byte)key);
  Serial.write((byte)0x00);
}
