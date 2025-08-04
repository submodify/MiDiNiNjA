// To give your project a unique name, this code must be
// placed into a .c file (its own tab).  It can not be in
// a .cpp file or your main sketch (the .ino file).

#include "usb_names.h"

// Edit these lines to create your own name.  The length must
// match the number of characters in your custom name.

#define VENDOR_NAME {'M','i','D','i','N','i','N','j','A'}
#define VENDOR_NAME_LEN 9

#define MIDI_NAME   {'X','3'}
#define MIDI_NAME_LEN  2

// Edit these lines to create port names.
// ***IMPORTANT*** ----> Port names can only be used with modifications to usb_desc.c & usb_names.h

#define MIDI_PORT1_NAME   {'C','h','0','1'}
#define MIDI_PORT1_NAME_LEN 4

#define MIDI_PORT2_NAME   {'C','h','0','2'}
#define MIDI_PORT2_NAME_LEN 4

#define MIDI_PORT3_NAME   {'C','h','0','3'}
#define MIDI_PORT3_NAME_LEN 4

#define MIDI_PORT4_NAME   {'C','h','0','4'}
#define MIDI_PORT4_NAME_LEN 4

#define MIDI_PORT5_NAME   {'C','h','0','5'}
#define MIDI_PORT5_NAME_LEN 4

#define MIDI_PORT6_NAME   {'C','h','0','6'}
#define MIDI_PORT6_NAME_LEN 4

#define MIDI_PORT7_NAME   {'C','h','0','7'}
#define MIDI_PORT7_NAME_LEN 4

#define MIDI_PORT8_NAME   {'C','h','0','8'}
#define MIDI_PORT8_NAME_LEN 4

#define MIDI_PORT9_NAME   {'C','h','0','9'}
#define MIDI_PORT9_NAME_LEN 4

#define MIDI_PORT10_NAME  {'C','h','1','0'}
#define MIDI_PORT10_NAME_LEN 4

#define MIDI_PORT11_NAME  {'C','h','1','1','/','O','u','t','A'}
#define MIDI_PORT11_NAME_LEN 9

#define MIDI_PORT12_NAME  {'C','h','1','2','/','O','u','t','B'}
#define MIDI_PORT12_NAME_LEN 9

#define MIDI_PORT13_NAME  {'C','h','1','3','/','O','u','t','C'}
#define MIDI_PORT13_NAME_LEN 9

#define MIDI_PORT14_NAME  {'C','h','1','4','/','L','B','K','1'}
#define MIDI_PORT14_NAME_LEN 9

#define MIDI_PORT15_NAME  {'C','h','1','5','/','L','B','K','2'}
#define MIDI_PORT15_NAME_LEN 9

#define MIDI_PORT16_NAME  {'C','h','1','6','/','S','Y','N','C'}
#define MIDI_PORT16_NAME_LEN 9



// Do not change this part.  This exact format is required by USB.

struct usb_string_descriptor_struct usb_string_manufacturer_name = {
        2 + VENDOR_NAME_LEN * 2,
        3,
        VENDOR_NAME
};

struct usb_string_descriptor_struct usb_string_product_name = {
        2 + MIDI_NAME_LEN * 2,
        3,
        MIDI_NAME
};

struct usb_string_descriptor_struct usb_string_midi_port1 = {
        2 + MIDI_PORT1_NAME_LEN * 2,
        3,
        MIDI_PORT1_NAME
};

struct usb_string_descriptor_struct usb_string_midi_port2 = {
        2 + MIDI_PORT2_NAME_LEN * 2,
        3,
        MIDI_PORT2_NAME
};

struct usb_string_descriptor_struct usb_string_midi_port3 = {
        2 + MIDI_PORT3_NAME_LEN * 2,
        3,
        MIDI_PORT3_NAME
};

struct usb_string_descriptor_struct usb_string_midi_port4 = {
        2 + MIDI_PORT4_NAME_LEN * 2,
        3,
        MIDI_PORT4_NAME
};

struct usb_string_descriptor_struct usb_string_midi_port5 = {
        2 + MIDI_PORT5_NAME_LEN * 2,
        3,
        MIDI_PORT5_NAME
};

struct usb_string_descriptor_struct usb_string_midi_port6 = {
        2 + MIDI_PORT6_NAME_LEN * 2,
        3,
        MIDI_PORT6_NAME
};

struct usb_string_descriptor_struct usb_string_midi_port7 = {
        2 + MIDI_PORT7_NAME_LEN * 2,
        3,
        MIDI_PORT7_NAME
};

struct usb_string_descriptor_struct usb_string_midi_port8 = {
        2 + MIDI_PORT8_NAME_LEN * 2,
        3,
        MIDI_PORT8_NAME
};

struct usb_string_descriptor_struct usb_string_midi_port9 = {
        2 + MIDI_PORT9_NAME_LEN * 2,
        3,
        MIDI_PORT9_NAME
};

struct usb_string_descriptor_struct usb_string_midi_port10 = {
        2 + MIDI_PORT10_NAME_LEN * 2,
        3,
        MIDI_PORT10_NAME
};

struct usb_string_descriptor_struct usb_string_midi_port11 = {
        2 + MIDI_PORT11_NAME_LEN * 2,
        3,
        MIDI_PORT11_NAME
};

struct usb_string_descriptor_struct usb_string_midi_port12 = {
        2 + MIDI_PORT12_NAME_LEN * 2,
        3,
        MIDI_PORT12_NAME
};

struct usb_string_descriptor_struct usb_string_midi_port13 = {
        2 + MIDI_PORT13_NAME_LEN * 2,
        3,
        MIDI_PORT13_NAME
};

struct usb_string_descriptor_struct usb_string_midi_port14 = {
        2 + MIDI_PORT14_NAME_LEN * 2,
        3,
        MIDI_PORT14_NAME
};

struct usb_string_descriptor_struct usb_string_midi_port15 = {
        2 + MIDI_PORT15_NAME_LEN * 2,
        3,
        MIDI_PORT15_NAME
};

struct usb_string_descriptor_struct usb_string_midi_port16 = {
        2 + MIDI_PORT16_NAME_LEN * 2,
        3,
        MIDI_PORT16_NAME
};
