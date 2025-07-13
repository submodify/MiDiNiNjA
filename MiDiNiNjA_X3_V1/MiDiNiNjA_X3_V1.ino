/*
  Multi Channel Midi Input Bug
  A device to work aroung the Akai MPC multi channel input bug...
  Description.... TODO
*/

#include <MIDI.h>
#include <OneButton.h>

#define MIDI_OUT_PORTA 10   // Port 11 for MIDI Out Din A
#define MIDI_OUT_PORTB 11   // Port 12 for MIDI Out Din B
#define MIDI_OUT_PORTC 12   // Port 13 for MIDI Out Din C
#define LOOPBACKCABLE_1 13  // Port 14 for Loopback MIDI A
#define LOOPBACKCABLE_2 14  // Port 15 for Loopback MIDI B
#define REALTIMECABLE 15    // Port 16 for RealTime MIDI messages
#define BUTTONPIN 12
#define LEDPIN 13
#define BLINKRATE 150

MIDI_CREATE_INSTANCE(HardwareSerial, Serial5, MIDI_A);  //
MIDI_CREATE_INSTANCE(HardwareSerial, Serial4, MIDI_B);  //
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI_C);  //

elapsedMillis ledOnMillis;
elapsedMillis buttonMillis;

enum ledStates { MONITOR,
                 BLINK_ENABLE,
                 LED_OFF,
                 LED_ON };
enum ledStates ledState = MONITOR;

enum modes { MERGE = 0,
             EXCLUSIVE = 1 };
enum modes currentMode = MERGE;

uint8_t blinkCount = 0;
bool midiActivity = false;

OneButton button;

uint8_t chanLock[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
bool clockDIN = true;
bool clockUSB = true;

//Filter/Remap matrix MIDI A:  CH  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
uint8_t chanFilterA[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };        // 1 == Allow, 0 == Block
uint8_t cableRemapA[] = { 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };  // USB port to remap channel to.

//Filter/Remap matrix MIDI B:  CH  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
uint8_t chanFilterB[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };        // 1 == Allow, 0 == Block
uint8_t cableRemapB[] = { 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };  // USB port to remap channel to.

//Filter/Remap matrix MIDI C:  CH  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
uint8_t chanFilterC[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };        // 1 == Allow, 0 == Block
uint8_t cableRemapC[] = { 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };  // USB port to remap channel to.

inline void blinkLED() __attribute__((always_inline));
inline void blink(uint8_t) __attribute__((always_inline));
inline void checkButton() __attribute__((always_inline));
inline void blinkMode() __attribute__((always_inline));


void setup() {

  pinMode(LEDPIN, OUTPUT);               // LED pin
  button.setup(BUTTONPIN, false, true);  // Button pin 12, no pullup, active low.
  button.attachLongPressStart(resetChannelLock);
  //button.attachDoubleClick(toggleMode); // DIsabled for now.
  resetChannelLock();
  MIDI_A.begin(MIDI_CHANNEL_OMNI);
  MIDI_B.begin(MIDI_CHANNEL_OMNI);
  MIDI_C.begin(MIDI_CHANNEL_OMNI);

  MIDI_A.turnThruOff();
  MIDI_B.turnThruOff();
  MIDI_C.turnThruOff();
}


void loop() {


  if (MIDI_A.read()) {  // Get a MIDI message from MIDI DIN A if available
    byte type = MIDI_A.getType();
    byte channel = MIDI_A.getChannel();
    byte data1 = MIDI_A.getData1();
    byte data2 = MIDI_A.getData2();

    if (chanFilterA[channel]) {  // If a Channel message, and not being filtered
      if (chanLock[channel]) {   // Assign to this MIDI port if not already assigned
        chanFilterB[channel] = 0;
        chanFilterC[channel] = 0;
        chanLock[channel] = 0;
      }
      usbMIDI.send(type, data1, data2, 1, cableRemapA[channel]);  // Use the Channel number to assign a USB port, and send on Midi Channel 1
      midiActivity = true;
    } else if (clockDIN) {  // Send clock message if not already being sent by host USB
      switch (type) {       // If not a Channel message, look for Realtime messages Clock, Start, Continue, Stop only
        case midi::Clock:
        case midi::Start:
        case midi::Continue:
        case midi::Stop:
          MIDI_B.send((midi::MidiType)type, data1, data2, channel);  // Forward Realtime messages to MIDI DIN B (type needs to be cast to MidiType)
          MIDI_C.send((midi::MidiType)type, data1, data2, channel);  // Forward Realtime messages to MIDI DIN B (type needs to be cast to MidiType)
          usbMIDI.send(type, data1, data2, channel, REALTIMECABLE);  // Forward Realtime messages to the USB Realtime Cable port
          if (clockUSB) clockUSB = false;
          if (type == midi::Start) midiActivity = true;
          break;
        default:
          break;
      }
    }
  }

  if (MIDI_B.read()) {  // Get a MIDI message from MIDI DIN B if available
    byte type = MIDI_B.getType();
    byte channel = MIDI_B.getChannel();
    byte data1 = MIDI_B.getData1();
    byte data2 = MIDI_B.getData2();

    if (chanFilterB[channel]) {  // If a Channel message, and not being filtered
      if (chanLock[channel]) {   // Assign to this MIDI port if not already assigned
        chanFilterA[channel] = 0;
        chanFilterC[channel] = 0;
        chanLock[channel] = 0;
      }
      usbMIDI.send(type, data1, data2, 1, cableRemapB[channel]);  // Use the Channel number to assign a USB port, and send on Midi Channel 1
      midiActivity = true;
    }
  }

  if (MIDI_C.read()) {  // Get a MIDI message from MIDI DIN C if available
    byte type = MIDI_C.getType();
    byte channel = MIDI_C.getChannel();
    byte data1 = MIDI_C.getData1();
    byte data2 = MIDI_C.getData2();

    if (chanFilterC[channel]) {  // If a Channel message, and not being filtered
      if (chanLock[channel]) {   // Assign to this MIDI port if not already assigned
        chanFilterA[channel] = 0;
        chanFilterB[channel] = 0;
        chanLock[channel] = 0;
      }
      usbMIDI.send(type, data1, data2, 1, cableRemapC[channel]);  // Use the Channel number to assign a USB port, and send on Midi Channel 1
      midiActivity = true;
    }
  }


  if (usbMIDI.read()) {  // Get a MIDI message from USB ports if available
    byte type = usbMIDI.getType();
    byte channel = usbMIDI.getChannel();
    byte data1 = usbMIDI.getData1();
    byte data2 = usbMIDI.getData2();
    byte cable = usbMIDI.getCable();
    midi::MidiType mtype = (midi::MidiType)type;

    if (channel) {  // If a Channel message
      switch (cable) {
        case MIDI_OUT_PORTA:
          MIDI_A.send(mtype, data1, data2, channel);  // If on USB port 1, send to MIDI DIN A
          midiActivity = true;
          break;
        case MIDI_OUT_PORTB:
          MIDI_B.send(mtype, data1, data2, channel);  // If on USB port 5, send to MIDI DIN B
          midiActivity = true;
          break;
        case MIDI_OUT_PORTC:
          MIDI_C.send(mtype, data1, data2, channel);  // If on USB port 9, send to MIDI DIN C
          midiActivity = true;
          break;
        case LOOPBACKCABLE_1:
          if (channel == 16) {                                     // If on MIDI Channel 16
            usbMIDI.send(type, data1, data2, 1, LOOPBACKCABLE_1);  // Send data back to USB on LoopBack cable A, MIDI Channel 1
            midiActivity = true;
          }
          break;
        case LOOPBACKCABLE_2:
          if (channel == 16) {                                     // If on MIDI Channel 16
            usbMIDI.send(type, data1, data2, 1, LOOPBACKCABLE_2);  // Send data back to USB on LoopBack cable B, MIDI Channel 1
            midiActivity = true;
          }
          break;
        default:
          break;
      }
    } else if (clockUSB && (cable == REALTIMECABLE)) {  //Send clock message if not already being sent by DIN, and If USB port is assigned to RealTime messages
      switch (type) {
        case midi::Clock:
        case midi::Start:
        case midi::Continue:
        case midi::Stop:
          MIDI_A.send(mtype, data1, data2, channel);  // Send to MIDI DIN A
          MIDI_B.send(mtype, data1, data2, channel);  // Send to MIDI DIN B
          MIDI_C.send(mtype, data1, data2, channel);  // Send to MIDI DIN C
          if (clockDIN) clockDIN = false;
          if (mtype == midi::Start) midiActivity = true;
          break;
        default:
          break;
      }
    }
  }

  blinkLED();

  checkButton();
}


// ##### FUNCTIONS #####

/* Reset the Arrays used for dynamic port assignment. */
void resetChannelLock() {
  memset(chanLock, currentMode, 17);
  memset(chanFilterA + 1, 1, 16);
  memset(chanFilterB + 1, 1, 16);
  memset(chanFilterC + 1, 1, 16);
  clockDIN = true;
  clockUSB = true;
  blinkMode();
}

void blinkLED() {
  switch (ledState) {
    case MONITOR:
      if (midiActivity) {                // Blink the LED when any activity has happened
        digitalWriteFast(LEDPIN, HIGH);  // LED on
        ledOnMillis = 0;
        midiActivity = false;
      }
      if (ledOnMillis > 15) {
        digitalWriteFast(LEDPIN, LOW);  // LED off
      }
      break;
    case BLINK_ENABLE:
      digitalWriteFast(LEDPIN, LOW);
      ledOnMillis = 0;
      ledState = LED_OFF;
      break;
    case LED_OFF:
      if (blinkCount) {
        if (ledOnMillis > BLINKRATE) {
          digitalWriteFast(LEDPIN, HIGH);
          ledState = LED_ON;
          ledOnMillis = 0;
        }
      } else {
        ledState = MONITOR;
        ledOnMillis = 0;
      }
      break;
    case LED_ON:
      if (ledOnMillis > BLINKRATE) {
        digitalWriteFast(LEDPIN, LOW);
        ledState = LED_OFF;
        ledOnMillis = 0;
        blinkCount--;
      }
      break;
    default:
      break;
  }
}

void blink(uint8_t i) {
  ledState = BLINK_ENABLE;
  blinkCount = i;
}

void checkButton() {
  // Check for button press every 25ms.
  if (buttonMillis > 25) {
    button.tick();
    buttonMillis = 0;
  }
}

void blinkMode() {
  currentMode ? blink(4) : blink(2);
}

void toggleMode() {
  if (currentMode == EXCLUSIVE) {
    currentMode = MERGE;
  } else {
    currentMode = EXCLUSIVE;
  }
  resetChannelLock();
}
