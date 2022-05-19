#ifndef __IMAGE_H
#define __IMAGE_H

#include "Arduino.h"

const uint8_t image_1[] = {
0x00,0x00,0xC0,0x03,0xF0,0x0F,0x3C,0x3C,0x0E,0x70,0x07,0xE0,0x01,0x80,0xC0,0x03,
0xF0,0x0F,0x78,0x1E,0x1C,0x38,0x0C,0x30,0x00,0x00,0x80,0x01,0xC0,0x03,0xC0,0x03,/*"wifi"*/ };
const uint8_t image_2[] = {
0xE0,0x07,0x20,0x04,0xA0,0x05,0xA0,0x05,0xA0,0x05,0xA0,0x05,0xA0,0x05,0xA0,0x05,
0xA0,0x05,0x90,0x09,0x88,0x11,0xC8,0x13,0xC8,0x13,0x88,0x11,0x30,0x0C,0xC0,0x03,/*"温度图标"*/};
const uint8_t image_3[] = {
0x80,0x01,0xC0,0x03,0x60,0x06,0x30,0x0C,0x18,0x18,0x18,0x18,0x0C,0x30,0x66,0x64,
0x66,0x66,0x06,0x63,0x86,0x61,0xC6,0x6C,0x66,0x6C,0x0C,0x30,0x38,0x1C,0xE0,0x07,/*"湿度图标"*/};
const uint8_t image_4[] = {
0xC0,0x03,0x60,0x06,0x20,0x04,0x20,0x04,0xA0,0x05,0xA0,0x05,0xA0,0x05,0xA0,0x05,
0xB0,0x0D,0x90,0x09,0x98,0x19,0xC8,0x13,0xC8,0x13,0x98,0x19,0x30,0x0C,0xC0,0x03,/*"温度图标2"*/};
const uint8_t image_5[] = {
0x00,0x00,0x20,0x00,0x50,0x00,0xC8,0x10,0x0C,0x29,0x04,0x46,0x26,0x40,0x72,0x48,
0xF2,0x5C,0xFA,0x5F,0xFA,0x5F,0xF6,0x6F,0xEC,0x37,0x18,0x18,0xE0,0x07,0x00,0x00,/*"火焰图标"*/};
const uint8_t image_6[] = {
0x00,0x00,0x80,0x07,0xEE,0x0C,0x33,0x78,0x01,0xC0,0x01,0x80,0x01,0x80,0x03,0xC0,
0x0E,0x60,0xF8,0x18,0xC0,0x0F,0x00,0x00,0xFC,0xF7,0x00,0x00,0x3F,0xFF,0x00,0x00,/*"烟雾图标"*/};
const uint8_t image_7[] = {
0x00,0x00,0x38,0x1C,0x44,0x22,0x86,0x61,0x01,0x40,0x21,0x80,0x51,0x80,0x8F,0xE0,
0x01,0x91,0x02,0x4A,0x04,0x24,0x08,0x10,0x10,0x08,0x60,0x06,0x80,0x01,0x00,0x00,/*"心率图像"*/};


#endif