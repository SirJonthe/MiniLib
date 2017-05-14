#include "mglText.h"
#include "../MML/mmlFixed.h"

#define first_char mglFont_FirstChar
#define last_char  mglFont_LastChar

#define font_big_char_count_width  4
#define font_big_char_count_height 24
#define font_big_char_width_px     mglFontBig_CharWidthPx
#define font_big_char_height_px    mglFontBig_CharHeightPx

// XBM data format
#define font_big_width  40
#define font_big_height 384
static const unsigned char font_big_bits[] = {
	0xf8, 0x23, 0x1e, 0xf1, 0xd7, 0xf8, 0x23, 0x1e, 0x71, 0x80, 0xf8, 0x23,
	0x0e, 0x20, 0x80, 0xf8, 0xef, 0x0e, 0x20, 0xea, 0xf8, 0xff, 0x0f, 0x20,
	0xc0, 0xf8, 0xff, 0x1f, 0x31, 0x80, 0xf8, 0xff, 0x0f, 0x60, 0x80, 0xf8,
	0xff, 0x0f, 0xe0, 0x8a, 0xff, 0xff, 0x0f, 0x20, 0x80, 0xf8, 0xff, 0x1f,
	0x31, 0xc0, 0xf8, 0xff, 0x1f, 0x71, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0x07, 0x3f, 0xfe,
	0xc1, 0xc7, 0x03, 0x3e, 0xfe, 0xc0, 0xc7, 0x73, 0x7e, 0xff, 0xc0, 0xff,
	0x23, 0xfe, 0xff, 0xf8, 0x00, 0x02, 0xff, 0xff, 0xf8, 0x00, 0x86, 0xf8,
	0xff, 0xf8, 0x00, 0x02, 0xfc, 0xff, 0xf8, 0xff, 0x33, 0xfe, 0xff, 0xf8,
	0xc7, 0x73, 0xfc, 0xff, 0xc0, 0xc7, 0x03, 0xf8, 0xff, 0xc0, 0xc7, 0x07,
	0xf9, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xe0, 0xbf, 0xff, 0xff, 0xff, 0xc0, 0x0f, 0xfe,
	0xff, 0xff, 0xc0, 0x1f, 0x7f, 0xfc, 0xff, 0xc7, 0x1f, 0x7f, 0xfc, 0xff,
	0xc7, 0xef, 0x1e, 0xf0, 0xff, 0xc7, 0xff, 0x1f, 0xf0, 0xff, 0xc7, 0xff,
	0x1f, 0xf0, 0xff, 0xc7, 0xff, 0x7f, 0xfc, 0xff, 0xc0, 0xff, 0x7f, 0x3c,
	0xfe, 0xc0, 0xff, 0xff, 0x3f, 0xfe, 0xe0, 0xff, 0xff, 0x3f, 0xfe, 0xff,
	0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0x78, 0xc0, 0xff, 0xff, 0xff, 0x38, 0x80, 0xff, 0xff,
	0x7f, 0x3c, 0x80, 0xff, 0xff, 0x7f, 0x3c, 0x8e, 0x01, 0xff, 0x3f, 0x3e,
	0x86, 0x01, 0xff, 0x3f, 0x3e, 0x8a, 0x01, 0xff, 0x1f, 0x3f, 0x8c, 0xff,
	0xff, 0x1f, 0x3f, 0x8e, 0xff, 0xe3, 0x8f, 0x3f, 0x80, 0xff, 0xe3, 0x8f,
	0x3f, 0x80, 0xff, 0xe3, 0x8f, 0x7f, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc3, 0x03, 0x1c, 0x30,
	0x8e, 0xc1, 0x03, 0x08, 0x20, 0x8e, 0xc1, 0x03, 0x08, 0x20, 0x8e, 0xc7,
	0xff, 0x88, 0x23, 0x8e, 0xc7, 0x07, 0xf8, 0x21, 0x80, 0xc7, 0x03, 0xf8,
	0x30, 0x80, 0xc7, 0x03, 0xfc, 0x61, 0x80, 0xc7, 0xe3, 0x8f, 0xe3, 0x8f,
	0x01, 0x03, 0x08, 0xe0, 0x8f, 0x01, 0x03, 0x08, 0xe0, 0x8f, 0x01, 0x03,
	0x18, 0xf0, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x00, 0x06, 0x0c, 0x60, 0xc0, 0x00, 0x02, 0x0c,
	0x20, 0x80, 0x00, 0x02, 0x0c, 0x20, 0x80, 0xf8, 0xe3, 0xff, 0x23, 0x8e,
	0x00, 0x03, 0xfc, 0x23, 0x80, 0x00, 0x02, 0xf8, 0x71, 0xc0, 0x00, 0x02,
	0xf8, 0x31, 0x80, 0x3f, 0xe2, 0xf8, 0x31, 0x8e, 0x00, 0x02, 0xf8, 0x38,
	0x80, 0x00, 0x02, 0xf8, 0x38, 0x80, 0x00, 0x07, 0xfc, 0x78, 0xc0, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x01, 0xff, 0xff, 0xff, 0xff, 0x00, 0xfe, 0xff, 0xff, 0xff, 0x00, 0xfe,
	0xff, 0xff, 0xe3, 0x38, 0xe2, 0x8f, 0xff, 0xf1, 0x00, 0xe2, 0x8f, 0xff,
	0xf8, 0x00, 0xe2, 0x8f, 0x7f, 0xfc, 0x01, 0xfe, 0xff, 0xff, 0xf8, 0x3f,
	0xfe, 0xff, 0xff, 0xf1, 0x01, 0xe2, 0x8f, 0xff, 0xe3, 0x01, 0xe2, 0x8f,
	0xff, 0xff, 0x01, 0xe3, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x70,
	0xc0, 0xff, 0xff, 0x0f, 0x20, 0x80, 0x01, 0xc7, 0x0f, 0x20, 0x9f, 0x01,
	0x8f, 0x8f, 0x23, 0x83, 0x01, 0x1f, 0xff, 0x20, 0x81, 0xff, 0x3f, 0x7e,
	0x30, 0x99, 0x01, 0x1f, 0x7f, 0x3c, 0x81, 0x01, 0x8f, 0x7f, 0x3c, 0x83,
	0x01, 0xc7, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x7f, 0x3c, 0x80, 0xff, 0xff,
	0x7f, 0x7c, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x01, 0x03, 0x1c, 0x30, 0xc0, 0x00, 0x02, 0x08,
	0x20, 0x80, 0x00, 0x02, 0x08, 0x20, 0x80, 0x38, 0xe2, 0x88, 0x23, 0x8e,
	0x38, 0x02, 0x88, 0x3f, 0x8e, 0x00, 0x02, 0x8c, 0x3f, 0x8e, 0x00, 0x02,
	0x88, 0x3f, 0x8e, 0x00, 0xe2, 0x88, 0x23, 0x8e, 0x38, 0x02, 0x08, 0x20,
	0x80, 0x38, 0x02, 0x08, 0x20, 0x80, 0x38, 0x02, 0x1c, 0x30, 0xc0, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x00, 0x02, 0x18, 0x30, 0x8e, 0x00, 0x02, 0x08, 0x20, 0x8e, 0x00, 0x02,
	0x08, 0x20, 0x8e, 0xf8, 0xe3, 0x8f, 0x23, 0x8e, 0xc0, 0x03, 0x8f, 0x3f,
	0x80, 0xc0, 0x03, 0x8f, 0x20, 0x80, 0xc0, 0x03, 0x8f, 0x20, 0x80, 0xf8,
	0xe3, 0x8f, 0x23, 0x8e, 0x00, 0xe2, 0x0f, 0x20, 0x8e, 0x00, 0xe2, 0x0f,
	0x20, 0x8e, 0x00, 0xe2, 0x1f, 0x30, 0x8e, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x83, 0x03, 0x88, 0x23,
	0xfe, 0x83, 0x03, 0x88, 0x23, 0xfe, 0x83, 0x03, 0x88, 0x23, 0xfe, 0xc7,
	0xff, 0x88, 0x23, 0xfe, 0xc7, 0xff, 0x08, 0x20, 0xfe, 0xc7, 0xff, 0x08,
	0x30, 0xfe, 0xc7, 0xe3, 0x08, 0x20, 0xfe, 0xc7, 0xe3, 0x88, 0x23, 0xfe,
	0x83, 0x03, 0x88, 0x23, 0x80, 0x83, 0x03, 0x88, 0x23, 0x80, 0x83, 0x07,
	0x8c, 0x23, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x38, 0xe2, 0x18, 0x30, 0xc0, 0x10, 0xe2, 0x08,
	0x20, 0x80, 0x00, 0xe2, 0x08, 0x20, 0x80, 0x00, 0xc2, 0x88, 0x23, 0x8e,
	0x00, 0x82, 0x88, 0x23, 0x8e, 0x28, 0x02, 0x88, 0x23, 0x80, 0x38, 0x22,
	0x88, 0x23, 0x80, 0x38, 0x62, 0x88, 0x23, 0xc0, 0x38, 0xe2, 0x08, 0x20,
	0xfe, 0x38, 0xe2, 0x08, 0x20, 0xfe, 0x38, 0xe2, 0x18, 0x30, 0xfe, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x01, 0x03, 0x1c, 0x30, 0x80, 0x00, 0x02, 0x08, 0x20, 0x80, 0x00, 0x02,
	0x08, 0x20, 0x80, 0x38, 0xe2, 0x88, 0xff, 0xf1, 0x38, 0xe2, 0x08, 0xf0,
	0xf1, 0x38, 0x02, 0x08, 0xe0, 0xf1, 0x08, 0x02, 0x1c, 0xe0, 0xf1, 0x08,
	0x02, 0xf8, 0xe3, 0xf1, 0x00, 0xe2, 0x08, 0xe0, 0xf1, 0x00, 0xe2, 0x08,
	0xe0, 0xf1, 0x01, 0xe2, 0x18, 0xf0, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x38, 0xe2, 0x88, 0x23,
	0x8e, 0x38, 0xe2, 0x88, 0x23, 0x8e, 0x38, 0xe2, 0x88, 0x23, 0x8e, 0x38,
	0xe2, 0x88, 0x23, 0x84, 0x38, 0xe2, 0x88, 0x23, 0x80, 0x38, 0xe2, 0x88,
	0x62, 0xc0, 0x38, 0xe2, 0x08, 0x20, 0x80, 0x38, 0x42, 0x08, 0x20, 0x84,
	0x00, 0x06, 0x0c, 0x20, 0x8e, 0x00, 0x0e, 0x0e, 0x21, 0x8e, 0x01, 0x1f,
	0x8f, 0x23, 0x8e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x38, 0x02, 0x38, 0x30, 0xfe, 0x38, 0x02, 0x38,
	0x30, 0xfe, 0x38, 0x02, 0x38, 0x70, 0xfc, 0x38, 0xfe, 0x38, 0x7e, 0xfc,
	0x00, 0x0e, 0x38, 0xfe, 0xf8, 0x00, 0x06, 0x3c, 0xfe, 0xf8, 0x01, 0x03,
	0x3e, 0xfe, 0xf1, 0xc7, 0xe3, 0x3f, 0xfe, 0xf1, 0xc7, 0x03, 0x38, 0xf0,
	0xe3, 0xc7, 0x03, 0x38, 0xf0, 0xe3, 0xc7, 0x03, 0x38, 0xf0, 0xe3, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xc0, 0xbf, 0xff, 0xff, 0xf8, 0xc0, 0x1f, 0xff, 0xff, 0xf8, 0xc0, 0x0f,
	0xfe, 0xff, 0xf8, 0xc7, 0x4f, 0xfe, 0xff, 0xfb, 0xc7, 0xef, 0xfe, 0xff,
	0xff, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xc7,
	0xff, 0xff, 0xff, 0xff, 0xc0, 0xff, 0x0f, 0xe0, 0xff, 0xc0, 0xff, 0x0f,
	0xe0, 0xff, 0xc0, 0xff, 0x0f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff,
	0x8f, 0xff, 0xe3, 0xff, 0xff, 0x8f, 0xff, 0xe3, 0xff, 0xff, 0x8f, 0x01,
	0x03, 0x1c, 0x60, 0x80, 0x00, 0x02, 0x08, 0x20, 0x80, 0x00, 0x02, 0x08,
	0x20, 0x80, 0x38, 0xe2, 0x88, 0x3f, 0x8e, 0x38, 0xe2, 0x88, 0x3f, 0x8e,
	0x00, 0x02, 0x08, 0x20, 0x80, 0x00, 0x02, 0x08, 0x20, 0x80, 0x01, 0x02,
	0x1c, 0x60, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xfc, 0x3f, 0xfe, 0xff, 0x07, 0xfc,
	0x3f, 0xfe, 0xff, 0x07, 0xfc, 0x3f, 0xfe, 0x01, 0xc7, 0x1f, 0x20, 0xc0,
	0x00, 0x06, 0x0f, 0x20, 0x80, 0x38, 0x06, 0x0f, 0x20, 0x80, 0x00, 0x06,
	0x8f, 0x23, 0x8e, 0xf8, 0xc7, 0x8f, 0x23, 0x8e, 0x00, 0xc6, 0x0f, 0x20,
	0x8e, 0x00, 0xc6, 0x0f, 0x20, 0x8e, 0x01, 0xc6, 0x1f, 0x20, 0x8e, 0xff,
	0xff, 0xff, 0xe3, 0xff, 0xff, 0xff, 0x3f, 0xe0, 0xff, 0xff, 0xff, 0x3f,
	0xe0, 0xff, 0xff, 0xff, 0x3f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0x8f, 0x7f, 0xfc, 0xff, 0xff, 0x8f, 0x7f, 0xfc, 0xff, 0xff,
	0x8f, 0x63, 0xfc, 0xc7, 0x3f, 0x8e, 0x61, 0xfc, 0xc7, 0x3f, 0x8e, 0x70,
	0xfc, 0xff, 0xff, 0x0f, 0x78, 0xfc, 0xc7, 0x3f, 0x0e, 0x7c, 0xfc, 0xc7,
	0x3f, 0x0e, 0x78, 0xfc, 0xc7, 0x3f, 0x8e, 0x70, 0xe0, 0x83, 0x3f, 0x8e,
	0x61, 0xe0, 0x83, 0x3f, 0x8e, 0xe3, 0xe0, 0xff, 0x23, 0xfe, 0xff, 0xff,
	0xff, 0x03, 0xfe, 0xff, 0xff, 0xff, 0x03, 0xfe, 0xff, 0xff, 0xff, 0x07,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x10,
	0x03, 0x1c, 0x30, 0xc0, 0x00, 0x02, 0x08, 0x20, 0x80, 0x00, 0x02, 0x08,
	0x20, 0x80, 0x28, 0xe2, 0x88, 0x23, 0x8e, 0x38, 0xe2, 0x88, 0x23, 0x8e,
	0x38, 0xe2, 0x08, 0x20, 0x80, 0x38, 0xe2, 0x08, 0x20, 0x80, 0x38, 0xe2,
	0x18, 0x30, 0xc0, 0xff, 0xff, 0xff, 0x3f, 0xfe, 0xff, 0xff, 0xff, 0x3f,
	0xfe, 0xff, 0xff, 0xff, 0x3f, 0xfe, 0xff, 0xff, 0xff, 0x3f, 0xfe, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xfc, 0xff, 0xff, 0xff,
	0x7f, 0xfc, 0xff, 0xff, 0xff, 0x7f, 0xfc, 0x01, 0x46, 0x3c, 0x70, 0xfc,
	0x00, 0x06, 0x1c, 0x70, 0xf0, 0x00, 0x06, 0x9c, 0x7f, 0xf0, 0x38, 0x86,
	0x1f, 0x78, 0xf0, 0x38, 0xc6, 0x3f, 0x70, 0xfc, 0x00, 0xc6, 0xff, 0x73,
	0xe0, 0x00, 0xc6, 0x1f, 0x70, 0xe0, 0x01, 0xc6, 0x1f, 0xf8, 0xe0, 0x3f,
	0xfe, 0xff, 0xff, 0xff, 0x3f, 0xfe, 0xff, 0xff, 0xff, 0x3f, 0xfe, 0xff,
	0xff, 0xff, 0x3f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0x38, 0xe2, 0x88, 0x23, 0x8e, 0x38, 0xe2, 0x88, 0x23,
	0x8e, 0x38, 0xe2, 0x88, 0x63, 0xc4, 0x38, 0xe2, 0x88, 0xe3, 0xe0, 0x38,
	0x42, 0x88, 0xe2, 0xe0, 0x00, 0x06, 0x0c, 0x60, 0xc4, 0x00, 0x0e, 0x0e,
	0x20, 0x8e, 0x01, 0x1e, 0x1f, 0x30, 0x8e, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xf0,
	0xf8, 0xff, 0xff, 0x3f, 0xf0, 0xf8, 0xff, 0xff, 0x3f, 0xf0, 0xf8, 0x38,
	0x02, 0x38, 0xfe, 0xf8, 0x38, 0x02, 0x78, 0xfe, 0xf8, 0x38, 0x7e, 0x38,
	0xff, 0xf8, 0x38, 0x1e, 0x7c, 0xfe, 0xf8, 0x38, 0x06, 0x3f, 0xfe, 0xf8,
	0x00, 0xc2, 0x3f, 0xf0, 0xf8, 0x00, 0x02, 0x38, 0xf0, 0xf8, 0x01, 0x02,
	0x78, 0xf0, 0xf8, 0x3f, 0xfe, 0xff, 0xff, 0xff, 0x03, 0xfe, 0xff, 0xff,
	0xff, 0x03, 0xfe, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x5f, 0x0f, 0x20, 0x80, 0xc0, 0x0f, 0xef,
	0xaf, 0xbf, 0xc0, 0x0f, 0xef, 0xaf, 0xbf, 0xc7, 0xaf, 0xef, 0xaf, 0xbf,
	0xe7, 0xff, 0xef, 0xaf, 0xbf, 0xcf, 0xff, 0xef, 0xaf, 0xbf, 0xe7, 0xff,
	0xef, 0xaf, 0xbf, 0xc7, 0xff, 0xef, 0xaf, 0xbf, 0xc0, 0xff, 0xef, 0xaf,
	0xbf, 0xc0, 0xff, 0xef, 0xaf, 0xbf, 0xe0, 0xff, 0x0f, 0x20, 0x80, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

#define font_small_char_count_width   4
#define font_small_char_count_height  24
#define font_small_char_width_px      mglFontSmall_CharWidthPx
#define font_small_char_height_px     mglFontSmall_CharHeightPx

// XBM data format
#define font_small_width  24
#define font_small_height 144
static const unsigned char font_small_bits[] = {
	0xff, 0xff, 0xff, 0x7d, 0xbd, 0x06, 0x7d, 0x1d, 0xa4, 0xfd, 0xbf, 0x06,
	0xff, 0x1f, 0x2c, 0xfd, 0xbf, 0x06, 0xff, 0xff, 0xff, 0x77, 0xdc, 0xef,
	0xff, 0xde, 0xf7, 0x41, 0xf5, 0xf7, 0x7f, 0xfb, 0xf7, 0x77, 0xf4, 0xef,
	0xff, 0xff, 0xff, 0xfd, 0xff, 0xff, 0xfb, 0xbe, 0xff, 0x7b, 0x1d, 0xff,
	0xfb, 0xbe, 0xef, 0xfd, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0x05,
	0xff, 0xff, 0x76, 0xf1, 0x7f, 0x57, 0xff, 0xbf, 0x77, 0x7f, 0xdf, 0x07,
	0xff, 0xff, 0xff, 0x77, 0x10, 0x74, 0xf3, 0xf7, 0x75, 0x77, 0x30, 0x04,
	0x77, 0xff, 0x7d, 0x41, 0x10, 0x7c, 0xff, 0xff, 0xff, 0x41, 0x10, 0x04,
	0x7d, 0xff, 0x75, 0x41, 0xf0, 0x04, 0x5f, 0xf7, 0x75, 0x41, 0xf0, 0x05,
	0xff, 0xff, 0xff, 0xc1, 0xff, 0xdf, 0x5d, 0xbf, 0xef, 0xc1, 0xff, 0xf7,
	0x5f, 0xbf, 0xef, 0xdf, 0xdf, 0xdf, 0xff, 0xff, 0xff, 0x7f, 0x1f, 0x04,
	0xf1, 0xfe, 0x75, 0xff, 0x1d, 0x34, 0xf1, 0xfe, 0x57, 0x7f, 0xdf, 0x17,
	0xff, 0xff, 0xff, 0x41, 0x38, 0x84, 0x5d, 0xdb, 0x77, 0x41, 0xd0, 0x77,
	0x5d, 0xd7, 0x77, 0x5d, 0x10, 0x04, 0xff, 0xff, 0xff, 0x41, 0x10, 0x74,
	0x7d, 0xdf, 0x77, 0x61, 0x58, 0x04, 0x7d, 0xdf, 0x75, 0x41, 0x1f, 0x74,
	0xff, 0xff, 0xff, 0x41, 0xd0, 0xf5, 0xf7, 0xd7, 0xf5, 0xf7, 0x17, 0xf6,
	0x77, 0xd7, 0xf5, 0x41, 0xd0, 0x05, 0xff, 0xff, 0xff, 0x49, 0x17, 0x04,
	0x55, 0xd6, 0x75, 0x5d, 0xd5, 0x05, 0x5d, 0xd3, 0xf5, 0x5d, 0x17, 0xf4,
	0xff, 0xff, 0xff, 0x41, 0x30, 0x04, 0x5d, 0xd7, 0xdf, 0x5d, 0x10, 0xdc,
	0x4d, 0xfb, 0xdd, 0x41, 0x17, 0xde, 0xff, 0xff, 0xff, 0x5d, 0xd7, 0x75,
	0x5d, 0xd7, 0x75, 0x5d, 0xd7, 0x8d, 0xdd, 0x5a, 0x75, 0xc1, 0x9d, 0x74,
	0xff, 0xff, 0xff, 0x5d, 0x90, 0xf7, 0xdd, 0xd7, 0xef, 0xeb, 0xd8, 0xdf,
	0x77, 0xdf, 0xbf, 0x77, 0x90, 0x7f, 0xff, 0xff, 0xff, 0xf9, 0xfe, 0xf7,
	0x7b, 0xfd, 0xef, 0xfb, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xf9, 0x1f, 0xfc,
	0xff, 0xff, 0xff, 0x41, 0x38, 0x84, 0x5d, 0xdb, 0x77, 0x41, 0xd0, 0x77,
	0x5d, 0xd7, 0x77, 0x5d, 0x10, 0x04, 0xff, 0xff, 0xff, 0x41, 0x10, 0x74,
	0x7d, 0xdf, 0x77, 0x61, 0x58, 0x04, 0x7d, 0xdf, 0x75, 0x41, 0x1f, 0x74,
	0xff, 0xff, 0xff, 0x41, 0xd0, 0xf5, 0xf7, 0xd7, 0xf5, 0xf7, 0x17, 0xf6,
	0x77, 0xd7, 0xf5, 0x41, 0xd0, 0x05, 0xff, 0xff, 0xff, 0x49, 0x17, 0x04,
	0x55, 0xd6, 0x75, 0x5d, 0xd5, 0x05, 0x5d, 0xd3, 0xf5, 0x5d, 0x17, 0xf4,
	0xff, 0xff, 0xff, 0x41, 0x30, 0x04, 0x5d, 0xd7, 0xdf, 0x5d, 0x10, 0xdc,
	0x4d, 0xfb, 0xdd, 0x41, 0x17, 0xde, 0xff, 0xff, 0xff, 0x5d, 0xd7, 0x75,
	0x5d, 0xd7, 0x75, 0x5d, 0xd7, 0x8d, 0xdd, 0x5a, 0x75, 0xc1, 0x9d, 0x74,
	0xff, 0xff, 0xff, 0x5d, 0x30, 0xf7, 0xdd, 0xb7, 0xf7, 0xeb, 0xd8, 0xf7,
	0x77, 0xbf, 0xf7, 0x77, 0x30, 0xf7, 0xff, 0xff, 0xff, 0xf9, 0x1a, 0x04,
	0x7b, 0x1d, 0x04, 0xf7, 0x1f, 0x04, 0xfb, 0x1f, 0x04, 0xf9, 0x1f, 0x04
};

mtlByte mglExtractStencilBit(const mtlByte *stencil_bits, int num_bits_width, int x, int y)
{
	// Don't use CHAR_BITS:
	// Each byte in the XBM format has 8 bits of data regardless of if the byte is larger than 8 bits
	//stencil_bits += y * (num_bits_width >> 3);
	//stencil_bits += x >> 3;
	//unsigned char bit = mtlReadBit(*stencil_bits, x & 7);
	mtlByte bit = mtlReadBit<mtlByte>(stencil_bits, x + y * num_bits_width);
	return bit != 0 ? 0x0 : 0xff;
}

void mglDrawChar(char ch, const mtlByte *stencil_bits, int font_width, int char_count_width, int char_width, int char_height, mtlByte *dst, int dst_bpp, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale)
{
	typedef mml::fixed<unsigned long long,16> fixed;

	unsigned char *dst0 = dst;

	int char_width0  = char_width;
	int char_height0 = char_height;
	char_width  *= scale;
	char_height *= scale;

	if (ch < first_char || ch > last_char) {
		ch = last_char + 1;
	}

	int ch_index = ch - first_char;
	int ch_x = (ch_index % char_count_width) * char_width0;
	int ch_y = (ch_index / char_count_width) * char_height0;

	int start_i = x < 0 ? -x : 0;
	int clip_x  = x < 0 ?  0 : x;
	int start_j = y < 0 ? -y : 0;
	int clip_y  = y < 0 ?  0 : y;
	int end_i   = (x + char_width)  >= dst_w ? dst_w - x : char_width;
	int end_j   = (y + char_height) >= dst_h ? dst_h - y : char_height;

	// Rendering and scaling using a shift per pixel
	int size_i  = end_i - start_i;
	int size_j  = end_j - start_j;

	if (size_i <= 1 || size_j <= 1) { // watch out for division by zero
		return;
	}

	fixed iscale   = fixed(scale);
	fixed ix       = fixed(start_i) / iscale;
	fixed iy       = fixed(start_j) / iscale;
	fixed ix_start = ix;
	fixed idelta_x = (fixed(end_i) / iscale - ix) / fixed(size_i - 1);
	fixed idelta_y = (fixed(end_j) / iscale - iy) / fixed(size_j - 1);

	for (int j = 0; j < size_j; ++j) {
		dst = dst0 + (clip_x + (clip_y + j) * dst_w) * dst_bpp;
		unsigned int bit_row = ch_y + (unsigned int)iy;
		for (int i = 0; i < size_i; ++i) {
			unsigned int bit_col = ch_x + (unsigned int)ix;
			if (mglExtractStencilBit(stencil_bits, font_width, bit_col, bit_row) != 0) {
				dst[dst_order.index.r] = r;
				dst[dst_order.index.g] = g;
				dst[dst_order.index.b] = b;
			}
			dst += dst_bpp;
			ix += idelta_x;
		}
		ix = ix_start;
		iy += idelta_y;
	}
}

void mglDrawCharBig(char ch, mtlByte *dst, int dst_bytes_per_pixel, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale)
{
	mglDrawChar(
		ch,
		font_big_bits, font_big_width,
		font_big_char_count_width,
		font_big_char_width_px, font_big_char_height_px,
		dst, dst_bytes_per_pixel, dst_order, dst_w, dst_h,
		x, y,
		r, g, b,
		scale
	);
}

void mglDrawCharSmall(char ch, mtlByte *dst, int dst_bytes_per_pixel, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale)
{
	mglDrawChar(
		ch,
		font_small_bits, font_small_width,
		font_small_char_count_width,
		font_small_char_width_px, font_small_char_height_px,
		dst, dst_bytes_per_pixel, dst_order, dst_w, dst_h,
		x, y,
		r, g, b,
		scale
	);
}
