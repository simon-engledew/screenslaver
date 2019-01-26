/**
    @filename   :   epd5in83.h
    @brief      :   Header file for e-paper library epd5in83.cpp
    @author     :   MyMX from Waveshare

    Copyright (C) Waveshare     April 6 2018

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documnetation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to  whom the Software is
   furished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include <stdlib.h>
#include "epd5in83.h"
#include <spi.h>

Epd::~Epd() {
};

Epd::Epd() {
  reset_pin = RST_PIN;
  dc_pin = DC_PIN;
  cs_pin = CS_PIN;
  busy_pin = BUSY_PIN;
  width = EPD_WIDTH;
  height = EPD_HEIGHT;
};

int Epd::Init(void) {
  pinMode(cs_pin, OUTPUT);
  pinMode(reset_pin, OUTPUT);
  pinMode(dc_pin, OUTPUT);
  pinMode(busy_pin, INPUT);

  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  SPI.begin();

  Reset();

  SendCommand(POWER_SETTING);
  SendData(0x37);
  SendData(0x00);

  SendCommand(PANEL_SETTING);
  SendData(0xCF);
  SendData(0x08);

  SendCommand(BOOSTER_SOFT_START);
  SendData(0xc7);
  SendData(0xcc);
  SendData(0x28);

  SendCommand(POWER_ON);
  WaitUntilIdle();

  SendCommand(PLL_CONTROL);
  SendData(0x3c);

  SendCommand(TEMPERATURE_CALIBRATION);
  SendData(0x00);

  SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
  SendData(0x77);

  SendCommand(TCON_SETTING);
  SendData(0x22);

  SendCommand(TCON_RESOLUTION);
  SendData(width >> 8);     //source 640
  SendData(width & 0xff);
  SendData(height >> 8);     //gate 384
  SendData(height & 0xff);

  SendCommand(VCM_DC_SETTING);
  SendData(0x1E);      //decide by LUT file

  SendCommand(0xe5);           //FLASH MODE
  SendData(0x03);

  return 0;
}

void Epd::DigitalWrite(int pin, int value) {
    digitalWrite(pin, value);
}

int Epd::DigitalRead(int pin) {
    return digitalRead(pin);
}

void Epd::SpiTransfer(unsigned char data) {
    digitalWrite(CS_PIN, LOW);
    SPI.transfer(data);
    digitalWrite(CS_PIN, HIGH);
}

void Epd::SendCommand(unsigned char command) {
  DigitalWrite(dc_pin, LOW);
  SpiTransfer(command);
}

void Epd::SendData(unsigned char data) {
  DigitalWrite(dc_pin, HIGH);
  SpiTransfer(data);
}

void Epd::WaitUntilIdle(void) {
  while (DigitalRead(busy_pin) == 0) {     //0: busy, 1: idle
    delay(100);
  }
}

void Epd::Reset(void) {
  DigitalWrite(reset_pin, HIGH);
  delay(200);
  DigitalWrite(reset_pin, LOW);                //module reset
  delay(200);
  DigitalWrite(reset_pin, HIGH);
  delay(200);
}

void Epd::Clear(void)
{
  SendCommand(DATA_START_TRANSMISSION_1);

  int remaining = Size();

  while (remaining > 0) {
    SendData(0x33);
    SendData(0x33);
    SendData(0x33);
    SendData(0x33);

    remaining--;
  }

  SendCommand(DISPLAY_REFRESH);
  delay(100);
  WaitUntilIdle();
}

int Epd::Size(void) {
  return height * (width / 8);
}

#define hasBit(v, idx) (v & (1 << idx)) > 0
#define pairOfPixels(v, idx) hasBit(v, idx) ? (hasBit(v, idx+1) ? 0x33 : 0x30) : (hasBit(v, idx+1) ? 0x03 : 0x00)

void Epd::DisplayStream(Stream* stream) {
  unsigned char imageBuffer[width/8];

  SendCommand(DATA_START_TRANSMISSION_1);

  int remaining = Size();

  unsigned char pixels;

  while (remaining > 0) {
    const int count = stream->readBytes(imageBuffer, sizeof(imageBuffer));

    if (count > 0) {
      remaining -= count;

      for (int x = 0; x < count; x++) {
        pixels = imageBuffer[x];

        SendData(pairOfPixels(pixels, 0));
        SendData(pairOfPixels(pixels, 2));
        SendData(pairOfPixels(pixels, 4));
        SendData(pairOfPixels(pixels, 6));
      }
    } else {
      break;
    }
  }

  while (remaining > 0) {
    SendData(0x33);
    SendData(0x33);
    SendData(0x33);
    SendData(0x33);

    remaining--;
  }

  SendCommand(DISPLAY_REFRESH);
  delay(100);
  WaitUntilIdle();
}

void Epd::Sleep(void) {
  SendCommand(POWER_OFF);
  WaitUntilIdle();
  SendCommand(DEEP_SLEEP);
  SendData(0xa5);
}
