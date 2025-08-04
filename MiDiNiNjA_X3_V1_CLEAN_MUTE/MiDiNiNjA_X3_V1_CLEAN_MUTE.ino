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

uint8_t blinkCount = 0;
bool midiActivity = false;

OneButton button;

bool clockDIN = true;
bool clockUSB = true;
bool chanMute[16];

inline void blinkLED() __attribute__((always_inline));
inline void blink(uint8_t) __attribute__((always_inline));
inline void checkButton() __attribute__((always_inline));

void setup() {
  pinMode(LEDPIN, OUTPUT);               // LED pin
  button.setup(BUTTONPIN, false, true);  // Button pin 12, no pullup, active low.
  button.attachLongPressStart(resetClockLock);
  resetClockLock();
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
    byte chan_index = channel - 1;

    if (channel) {                                           // If a Channel message
      if (!(type == midi::NoteOn && chanMute[chan_index])) {    // If NOT a (Note On && Muted)...... This still confuses me =)
        usbMIDI.send(type, data1, data2, 1, (chan_index));  // Use the Channel number to assign a USB port, and send on Midi Channel 1
        midiActivity = true;
      }
    } else if (clockDIN) {  // Send clock message if not already being sent by host USB
      switch (type) {       // If not a Channel message, look for Realtime messages Clock, Start, Continue, Stop only
        case midi::Clock:
        case midi::Start:
        case midi::Continue:
        case midi::Stop:
        case midi::SongPosition:
          MIDI_B.send((midi::MidiType)type, data1, data2, channel);  // Forward Realtime messages to MIDI DIN B (type needs to be cast to MidiType)
          MIDI_C.send((midi::MidiType)type, data1, data2, channel);  // Forward Realtime messages to MIDI DIN C (type needs to be cast to MidiType)
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

    if (channel) {                                         // If a Channel message
      usbMIDI.send(type, data1, data2, 1, (channel - 1));  // Use the Channel number to assign a USB port, and send on Midi Channel 1
      midiActivity = true;
    }
  }

  if (MIDI_C.read()) {  // Get a MIDI message from MIDI DIN C if available
    byte type = MIDI_C.getType();
    byte channel = MIDI_C.getChannel();
    byte data1 = MIDI_C.getData1();
    byte data2 = MIDI_C.getData2();

    if (channel) {                                         // If a Channel message
      usbMIDI.send(type, data1, data2, 1, (channel - 1));  // Use the Channel number to assign a USB port, and send on Midi Channel 1
      midiActivity = true;
    }
  }

  if (usbMIDI.read()) {  // Get a MIDI message from USB ports if available
    byte type = usbMIDI.getType();
    byte channel = usbMIDI.getChannel();
    byte data1 = usbMIDI.getData1();
    byte data2 = usbMIDI.getData2();
    byte cable = usbMIDI.getCable();
    midi::MidiType mtype = (midi::MidiType)type;  // Cast to midi::MidiType to avoid warnings.

    if (channel) {  // If a Channel message
      switch (cable) {
        case MIDI_OUT_PORTA:
          if (type == midi::NoteOn && data1 <= 15) {          // If Note On 0 to 15
            chanMute[data1] = true;                           // Mute relative Midi Channel
          } else if (type == midi::NoteOff && data1 <= 15) {  // If Note Off 0 to 15
            chanMute[data1] = false;                          // Unmute relative Midi Channel
          } else {
            MIDI_A.send(mtype, data1, data2, channel);  // If on USB port MIDI_OUT_PORTA, send to MIDI DIN A. Notes used for Mute are not transmitted.
          }
          midiActivity = true;
          break;
        case MIDI_OUT_PORTB:
          MIDI_B.send(mtype, data1, data2, channel);  // If on USB port MIDI_OUT_PORTB, send to MIDI DIN B
          midiActivity = true;
          break;
        case MIDI_OUT_PORTC:
          MIDI_C.send(mtype, data1, data2, channel);  // If on USB port MIDI_OUT_PORTC, send to MIDI DIN C
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
        case midi::SongPosition:
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

/* Reset Clock Assignment */
void resetClockLock() {
  memset(chanMute, false, 16);  // reset mutes
  clockDIN = true;
  clockUSB = true;
  blink(2);
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
