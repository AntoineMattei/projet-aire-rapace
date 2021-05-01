/*********************************************************************
SSD1306 I2C C++ Class Library for Raspberry Pi.
Based on Adafruit SSD1306 Arduino library. Some functions came from Adafruit GFX lib.

Modified by Antoine MATTEI
Tested on Raspberry Pi 4 with 0.96 Yellow/Blue OLED
*********************************************************************/

/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/
#ifndef SSD1306_H
#define SSD1306_H

#define BLACK 0
#define WHITE 1
#define INVERSE 2

#define SSD1306_I2C_ADDRESS   0x3c	// 011110+SA0+RW - 0x3C or 0x3D
// Address for 128x32 is 0x3C
// Address for 128x64 is 0x3D (default) or 0x3C (if SA0 is grounded)

/*=========================================================================
    SSD1306 Displays
    -----------------------------------------------------------------------
    The driver is used in multiple displays (128x64, 128x32, etc.).
    Select the appropriate display below to create an appropriately
    sized framebuffer, etc.

    SSD1306_128_64  128x64 pixel display

    SSD1306_128_32  128x32 pixel display

    SSD1306_96_16

    -----------------------------------------------------------------------*/
#define SSD1306_128_64
    //   #define SSD1306_128_32
    //   #define SSD1306_96_16
    /*=========================================================================*/

#if defined SSD1306_128_32
#define WIDTH 128
#define HEIGHT 32
#endif

#if defined SSD1306_128_64
#define WIDTH 128
#define HEIGHT 64
#endif

#if defined SSD1306_96_16
#define WIDTH 96
#define HEIGHT 16
#endif

#if defined SSD1306_128_64 && defined SSD1306_128_32
#error "Only one SSD1306 display can be specified at once in SSD1306.h"
#endif
#if !defined SSD1306_128_64 && !defined SSD1306_128_32 && !defined SSD1306_96_16
#error "At least one SSD1306 display must be specified in SSD1306.h"
#endif

#if defined SSD1306_128_64
#define SSD1306_LCDWIDTH                  128
#define SSD1306_LCDHEIGHT                 64
#endif
#if defined SSD1306_128_32
#define SSD1306_LCDWIDTH                  128
#define SSD1306_LCDHEIGHT                 32
#endif
#if defined SSD1306_96_16
#define SSD1306_LCDWIDTH                  96
#define SSD1306_LCDHEIGHT                 16
#endif

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

#include "capteurs.h"

/*!
 * \file ssd1306.cpp ssd1306.h
 *
 * \mainpage C++ library for monochrome OLEDs based on SSD1306 drivers.
 *
 * \class ssd1306 ssd1306.h "ssd1306.h"
 * \brief The ssd1306 class
 *
 * \section intro_sec Introduction
 *
 * This is documentation for Adafruit's SSD1306 library for monochrome
 * OLED displays: http://www.adafruit.com/category/63_98
 *
 * These displays use I2C or SPI to communicate. I2C requires 2 pins
 * (SCL+SDA) and optionally a RESET pin. SPI requires 4 pins (MOSI, SCK,
 * select, data/command) and optionally a reset pin. Hardware SPI or
 * 'bitbang' software SPI are both supported.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * \section author Author
 *
 * \author Limor Fried    : Arduino lib for Adafruit Industries
 * \author Ladyada        : Arduino lib for Adafruit Industries
 * \author Ilia Penev     : C lib
 * \author Antoine Mattei : C++ lib
 * \version 3.0
 * \date 2021
 *
 * \section license License
 *
 * \copyright BSD license, all text above, and the splash screen included below, must be included in any redistribution.
 *
 */
class ssd1306
{
private:
    int _vccstate;
    int i2cd;

    int cursor_y;
    int cursor_x;

    int textsize;
    int wrap;

    unsigned int buffer[SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x80,
        0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x80, 0x80, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xF8, 0xE0,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x00, 0xFF,
    #if (SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH > 96*16)
        0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00,
        0x80, 0x80, 0x00, 0x00,
        0x80, 0xFF, 0xFF, 0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80,
        0x80, 0x00, 0x80, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x8C, 0x8E, 0x84,
        0x00, 0x00, 0x80, 0xF8,
        0xF8, 0xF8, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
        0xE0, 0xE0, 0xC0, 0x80,
        0x00, 0xE0, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE,
        0xFF, 0xC7, 0x01, 0x01,
        0x01, 0x01, 0x83, 0xFF, 0xFF, 0x00, 0x00, 0x7C, 0xFE, 0xC7, 0x01, 0x01,
        0x01, 0x01, 0x83, 0xFF,
        0xFF, 0xFF, 0x00, 0x38, 0xFE, 0xC7, 0x83, 0x01, 0x01, 0x01, 0x83, 0xC7,
        0xFF, 0xFF, 0x00, 0x00,
        0x01, 0xFF, 0xFF, 0x01, 0x01, 0x00, 0xFF, 0xFF, 0x07, 0x01, 0x01, 0x01,
        0x00, 0x00, 0x7F, 0xFF,
        0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
        0x00, 0x00, 0x01, 0xFF,
        0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x03, 0x0F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xE7, 0xC7, 0xC7, 0x8F,
        0x8F, 0x9F, 0xBF, 0xFF, 0xFF, 0xC3, 0xC0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFC, 0xFC, 0xFC,
        0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x00,
        0x01, 0x03, 0x03, 0x03,
        0x03, 0x03, 0x01, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03,
        0x03, 0x03, 0x01, 0x01,
        0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01,
        0x03, 0x03, 0x00, 0x00,
        0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x01,
        0x00, 0x00, 0x00, 0x03,
        0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    #if (SSD1306_LCDHEIGHT == 64)
        0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x3F, 0x1F, 0x0F,
        0x87, 0xC7, 0xF7, 0xFF, 0xFF, 0x1F, 0x1F, 0x3D, 0xFC, 0xF8, 0xF8, 0xF8,
        0xF8, 0x7C, 0x7D, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x0F, 0x07, 0x00,
        0x30, 0x30, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xE0, 0xC0, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0xC0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x7F, 0x7F, 0x3F, 0x1F,
        0x0F, 0x07, 0x1F, 0x7F, 0xFF, 0xFF, 0xF8, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFE, 0xF8, 0xE0,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xFE, 0xFE, 0x00, 0x00,
        0x00, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x0E, 0xFC, 0xF8, 0x00, 0x00,
        0xF0, 0xF8, 0x1C, 0x0E,
        0x06, 0x06, 0x06, 0x0C, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFE, 0xFE, 0x00,
        0x00, 0x00, 0x00, 0xFC,
        0xFE, 0xFC, 0x00, 0x18, 0x3C, 0x7E, 0x66, 0xE6, 0xCE, 0x84, 0x00, 0x00,
        0x06, 0xFF, 0xFF, 0x06,
        0x06, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x06, 0x00, 0x00, 0xFE, 0xFE,
        0x00, 0x00, 0xC0, 0xF8,
        0xFC, 0x4E, 0x46, 0x46, 0x46, 0x4E, 0x7C, 0x78, 0x40, 0x18, 0x3C, 0x76,
        0xE6, 0xCE, 0xCC, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F,
        0x3F, 0x1F, 0x0F, 0x03,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0F, 0x0F, 0x00, 0x00,
        0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00,
        0x03, 0x07, 0x0E, 0x0C,
        0x18, 0x18, 0x0C, 0x06, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x01, 0x0F, 0x0E,
        0x0C, 0x18, 0x0C, 0x0F,
        0x07, 0x01, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07, 0x00, 0x00,
        0x00, 0x0F, 0x0F, 0x00,
        0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F,
        0x00, 0x00, 0x00, 0x07,
        0x07, 0x0C, 0x0C, 0x18, 0x1C, 0x0C, 0x06, 0x06, 0x00, 0x04, 0x0E, 0x0C,
        0x18, 0x0C, 0x0F, 0x07,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    #endif
    #endif
    };

public:
    /*!
     * \brief ssd1306
     */
    ssd1306();

    /*!
        @brief  Allocate RAM for image buffer, initialize peripherals and pins.
        @param  vccstate
                VCC selection. Pass SSD1306_SWITCHCAPVCC to generate the display
                voltage (step up) from the 3.3V source, or SSD1306_EXTERNALVCC
                otherwise. Most situations with Adafruit SSD1306 breakouts will
                want SSD1306_SWITCHCAPVCC.
        @param  i2caddr
                I2C address of corresponding SSD1306 display (or pass 0 to use
                default of 0x3C for 128x32 display, 0x3D for all others).
                SPI displays (hardware or software) do not use addresses, but
                this argument is still required (pass 0 or any value really,
                it will simply be ignored). Default if unspecified is 0.

        @return None (void).

        @details See the following example:
        @code{.cpp}
                ssd1306 ecran;
                ecran.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS); // switchvcc should be SSD1306_SWITCHCAPVCC(0x2)
        @endcode

        @note   MUST call this function before any drawing or updates!
    */
    void begin(unsigned int vccstate, unsigned int i2caddr);

    /*!
        \brief  Issue a single low-level command directly to the SSD1306
                display, bypassing the library.
        \param  c
                Command to issue (0x00 to 0xFF, see datasheet).
        \return None (void).
    */
    void command(unsigned int c);

    /*!
        @brief  Clear contents of display buffer (set all pixels to off).
        @return None (void).
        @note   Changes buffer contents only, no immediate effect on display.
                Follow up with a call to display(), or with other graphics
                commands as needed by one's own application.
    */
    void clearDisplay(void);
    /*!
        @brief  Enable or disable display invert mode (white-on-black vs
                black-on-white).
        @param  i
                If true, switch to invert mode (black-on-white), else normal
                mode (white-on-black).
        @return None (void).
        @note   This has an immediate effect on the display, no need to call the
                display() function -- buffer contents are not changed, rather a
                different pixel mode of the display hardware is used. When
                enabled, drawing SSD1306_BLACK (value 0) pixels will actually draw
                white, SSD1306_WHITE (value 1) will draw black.
    */
    void invertDisplay(bool i);
    /*!
     * @brief  Push data currently in RAM to SSD1306 display.
       @return None (void).
       @note   Drawing operations are not visible until this function is
               called. Call after each graphics command, or after a whole set
               of graphics commands, as best needed by one's own application.
     * \code{.cpp}
     * ssd1306 ecran;
     * ecran.ecrireValeurEcran(Capteurs::Temperature, capteur);
     * ecran.display();
     * \endcode
     */
    void display();

    /*!
        @brief  Activate a right-handed scroll for all or part of the display.
        @param  start
                First row.
        @param  stop
                Last row.
        @return None (void).

        \details Hint, the display is 16 rows tall. To scroll the whole display, run:
        \code{.cpp}
        ssd1306 ecran;
        ecran.startscrollright(0x00, 0x0F);
        \endcode
     */
    void startscrollright(unsigned int start, unsigned int stop);
    /*!
        @brief  Activate a left-handed scroll for all or part of the display.
        @param  start
                First row.
        @param  stop
                Last row.
        @return None (void).
        @details Hint, the display is 16 rows tall. To scroll the whole display, run:
        @code{.cpp}
        ssd1306 ecran;
        ecran.startscrollleft(0x00, 0x0F)
        @endcode
    */
    void startscrollleft(unsigned int start, unsigned int stop);

    /*!
        @brief  Activate a diagonal scroll for all or part of the display.
        @param  start
                First row.
        @param  stop
                Last row.
        @return None (void).
        @details Hint, the display is 16 rows tall. To scroll the whole display, run:
        @code{.cpp}
        ssd1306 ecran;
        ecran.startscrolldiagright(0x00, 0x0F);
        @endcode
    */
    void startscrolldiagright(unsigned int start, unsigned int stop);
    /*!
        @brief  Activate alternate diagonal scroll for all or part of the display.
        @param  start
                First row.
        @param  stop
                Last row.
        @return None (void).
        @details Hint, the display is 16 rows tall. To scroll the whole display, run:
        @code{.cpp}
        ssd1306 ecran;
        ecran.startscrolldiagleft(0x00, 0x0F);
        @endcode
    */
    void startscrolldiagleft(unsigned int start, unsigned int stop);
    /*!
        @brief  Cease a previously-begun scrolling action.
        @return None (void).
    */
    void stopscroll(void);

    /*!
     * \brief write
     * \param c int
     *
     * Ecrit le caractère c sur l'écran.
     */
    void write(int c);

    /*!
        @brief  Dim the display.
        @param  dim
                true to enable lower brightness mode, false for full brightness.
        @return None (void).
        @note   This has an immediate effect on the display, no need to call the
                display() function -- buffer contents are not changed.
    */
    void dim(bool dim);

    /*!
        @brief  Set/clear/invert a single pixel. This is also invoked by the
                Adafruit_GFX library in generating many higher-level graphics
                primitives.
        @param  x
                Column of display -- 0 at left to (screen width - 1) at right.
        @param  y
                Row of display -- 0 at top to (screen height -1) at bottom.
        @param  color
                Pixel color, one of: SSD1306_BLACK, SSD1306_WHITE or SSD1306_INVERT.
        @return None (void).
        @note   Changes buffer contents only, no immediate effect on display.
                Follow up with a call to display(), or with other graphics
                commands as needed by one's own application.
    */
    void drawPixel(int x, int y, unsigned int color);

    /*!
     * \brief drawFastVLineInternal
     * \param x int
     * \param __y int
     * \param __h int
     * \param color unsigned int
     *
     */
    void drawFastVLineInternal(int x, int __y, int __h, unsigned int color);
    /*!
     * \brief drawFastHLineInternal
     * \param x int
     * \param __y int
     * \param __h int
     * \param color unsigned int
     */
    void drawFastHLineInternal(int x, int __y, int __h, unsigned int color);

    /*!
        @brief  Draw a vertical line. This is also invoked by the Adafruit_GFX
                library in generating many higher-level graphics primitives.
        @param  x
                Column of display -- 0 at left to (screen width -1) at right.
        @param  y
                Topmost row -- 0 at top to (screen height - 1) at bottom.
        @param  h
                Height of line, in pixels.
        @param  color
                Line color, one of: SSD1306_BLACK, SSD1306_WHITE or SSD1306_INVERT.
        @return None (void).
        @note   Changes buffer contents only, no immediate effect on display.
                Follow up with a call to display(), or with other graphics
                commands as needed by one's own application.
    */
    void drawFastVLine(int x, int y, int h, unsigned int color);
    /*!
        @brief  Draw a horizontal line. This is also invoked by the Adafruit_GFX
                library in generating many higher-level graphics primitives.
        @param  x
                Leftmost column -- 0 at left to (screen width - 1) at right.
        @param  y
                Row of display -- 0 at top to (screen height -1) at bottom.
        @param  w
                Width of line, in pixels.
        @param  color
                Line color, one of: SSD1306_BLACK, SSD1306_WHITE or SSD1306_INVERT.
        @return None (void).
        @note   Changes buffer contents only, no immediate effect on display.
                Follow up with a call to display(), or with other graphics
                commands as needed by one's own application.
    */
    void drawFastHLine(int x, int y, int w, unsigned int color);

    /*!
     * \brief fillRect
     * \param x int
     * \param y int
     * \param w int
     * \param h int
     * \param fillcolor int
     */
    void fillRect(int x, int y, int w, int h, int fillcolor);

    /*!
     * \brief setTextSize
     * \param s int
     *
     * \details Permet de modifier la taille du texte.
     * \pre Méthode à executer avant d'executer une méthode d'écriture :
     * \code{.cpp}
     * ssd1306 ecran;
     * ecran.setTextSize(2);                // On dit que le texte a une taille de 2.
     * ecran.drawString("Hello World!");    // On écrit "Hello World"
     * ecran.display();                     // On affiche le texte à l'écran.
     * \endcode
     */
    void setTextSize(int s);
    /*!
     * \brief drawString
     * \param str char*
     *
     * Permet de dessiner une chaine de caractère sur l'écran.
     * \warning Utiliser la méthode ssd1306::display() pour afficher la chaine de caractère.
     */
    void drawString(char* str);
    /*!
     * \brief drawChar
     * \param x int
     * \param y int
     * \param c int
     * \param color int
     * \param size int
     * \details Permet de dessiner une chaine de caractère sur l'écran.
     * \warning Utiliser la méthode ssd1306::display() pour afficher le caractère.
     */
    void drawChar(int x, int y, unsigned char c, int color, int size);

    /*!
     * \brief ecrireValeurEcran
     * \param typeCapteur enum Capteurs::type_capteur
     * \param capteur     Capteur
     *
     * \code{.cpp}
     * #include "capteurs.h"
     *
     * ssd1306 ecran;
     * //Capteurs::capteur(double vitesseVent, double poidsAir, double luminosite, double temperature, double humidite);
     * Capteurs capteur(9.15, 6.3, 56.8, 28.00, 65.7);
     * ecran.ecrireValeurEcran(Capteurs::Luminosite, capteur);
     * ecran.display();
     *
     * // La valeur 56.8 (valeur de la luminosité) sera affichée sur l'écran.
     * \endcode
     */
    void ecrireValeurEcran(int typeCapteur, Capteurs capteur);
};

#endif  // SSD1306_H
