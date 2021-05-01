/*********************************************************************
SSD1306 I2C C++ Class Library for Raspberry Pi
Based on Adafruit SSD1306 Arduino library. Some functions came from Adafruit GFX lib

Modified by Antoine MATTEI
Tested on Raspberry Pi 4 with 0.96" Blue OLED
https://www.gotronic.fr/art-afficheur-oled-0-96-i2c-tf052-28511.htm192
*********************************************************************/

/*********************************************************************
This is a class for the Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen below must be included in any redistribution
*********************************************************************/
#include <cstdio>
#include <cstring>
#include <wiringPiI2C.h>

#include "oled_fonts.h"
#include "ssd1306.h"

#include "capteurs.h"

#define rotation 0

#define pgm_read_byte(addr) (*const_cast<unsigned char *>(addr))

#define ssd1306_swap(a, b) { int t = a; a = b; b = t; }

ssd1306::ssd1306()
{
    ssd1306::cursor_y = 0;
    ssd1306::cursor_x = 0;

    ssd1306::textsize = 1;
    ssd1306::wrap = 1;
}

void ssd1306::begin(unsigned int vccstate, unsigned int i2caddr)
{
    _vccstate = static_cast<int>(vccstate);

    i2cd = wiringPiI2CSetup(static_cast<int>(i2caddr));
    if (i2cd < 0)
    {
        fprintf(stderr, "ssd1306_i2c : unable to initialize I2C");
        return;
    }
    // Init sequence
    ssd1306::command(SSD1306_DISPLAYOFF);	// 0xAE
    ssd1306::command(SSD1306_SETDISPLAYCLOCKDIV);	// 0xD5
    ssd1306::command(0x80);	// the suggested ratio 0x80

    ssd1306::command(SSD1306_SETMULTIPLEX);	// 0xA8
    ssd1306::command(SSD1306_LCDHEIGHT - 1);

    ssd1306::command(SSD1306_SETDISPLAYOFFSET);	// 0xD3
    ssd1306::command(0x0);	// no offset
    ssd1306::command(SSD1306_SETSTARTLINE | 0x0);	// line #0
    ssd1306::command(SSD1306_CHARGEPUMP);	// 0x8D
    if (vccstate == SSD1306_EXTERNALVCC) {
        ssd1306::command(0x10);
    }
    else {
        ssd1306::command(0x14);
    }
    ssd1306::command(SSD1306_MEMORYMODE);	// 0x20
    ssd1306::command(0x00);	// 0x0 act like ks0108
    ssd1306::command(SSD1306_SEGREMAP | 0x1);
    ssd1306::command(SSD1306_COMSCANDEC);

#if defined SSD1306_128_32
    ssd1306::command(SSD1306_SETCOMPINS);	// 0xDA
    ssd1306::command(0x02);
    ssd1306::command(SSD1306_SETCONTRAST);	// 0x81
    ssd1306::command(0x8F);

#elif defined SSD1306_128_64
    ssd1306::command(SSD1306_SETCOMPINS);	// 0xDA
    ssd1306::command(0x12);
    ssd1306::command(SSD1306_SETCONTRAST);	// 0x81
    if (vccstate == SSD1306_EXTERNALVCC) {
        ssd1306::command(0x9F);
    }
    else {
        ssd1306::command(0xCF);
    }

#elif defined SSD1306_96_16
    ssd1306::command(SSD1306_SETCOMPINS);	// 0xDA
    ssd1306::command(0x2);	// ada x12
    ssd1306::command(SSD1306_SETCONTRAST);	// 0x81
    if (vccstate == SSD1306_EXTERNALVCC) {
        ssd1306::command(0x10);
    }
    else {
        ssd1306::command(0xAF);
    }

#endif
    ssd1306::command(SSD1306_SETPRECHARGE);	// 0xd9
    if (vccstate == SSD1306_EXTERNALVCC) {
        ssd1306::command(0x22);
    }
    else {
        ssd1306::command(0xF1);
    }
    ssd1306::command(SSD1306_SETVCOMDETECT);	// 0xDB
    ssd1306::command(0x40);
    ssd1306::command(SSD1306_DISPLAYALLON_RESUME);	// 0xA4
    ssd1306::command(SSD1306_NORMALDISPLAY);	// 0xA6

    ssd1306::command(SSD1306_DEACTIVATE_SCROLL);

    ssd1306::command(SSD1306_DISPLAYON);	// --turn on oled panel
}

void ssd1306::invertDisplay(bool i)
{
    if (i == true) {
        ssd1306::command(SSD1306_INVERTDISPLAY);
    }
    else {
        ssd1306::command(SSD1306_NORMALDISPLAY);
    }
}

void ssd1306::command(unsigned int c)
{
    // I2C
    unsigned int control = 0x00;	// Co = 0, D/C = 0
    wiringPiI2CWriteReg8(i2cd, static_cast<int>(control), static_cast<int>(c));
}

void ssd1306::drawPixel(int x, int y, unsigned int color)
{
    if ((x < 0) || (x >= WIDTH) || (y < 0) || (y >= HEIGHT))
        return;

    // check rotation, move pixel around if necessary
    switch (rotation) {
    case 0:
        break;
    case 1:
        ssd1306_swap(x, y);
        x = WIDTH - x - 1;
        break;
    case 2:
        x = WIDTH - x - 1;
        y = HEIGHT - y - 1;
        break;
    case 3:
        ssd1306_swap(x, y);
        y = HEIGHT - y - 1;
        break;
    }

    // x is which column
    switch (color) {
    case WHITE:
        buffer[x + (y / 8) * SSD1306_LCDWIDTH] |= (1 << (y & 7));
        break;
    case BLACK:
        buffer[x + (y / 8) * SSD1306_LCDWIDTH] &= ~(1 << (y & 7));
        break;
    case INVERSE:
        buffer[x + (y / 8) * SSD1306_LCDWIDTH] ^= (1 << (y & 7));
        break;
    }
}

void ssd1306::display(void)
{
    ssd1306::command(SSD1306_COLUMNADDR);
    ssd1306::command(0);	// Column start address (0 = reset)
    ssd1306::command(SSD1306_LCDWIDTH - 1);	// Column end address (127
    // = reset)

    ssd1306::command(SSD1306_PAGEADDR);
    ssd1306::command(0);	// Page start address (0 = reset)
#if SSD1306_LCDHEIGHT == 64
    ssd1306::command(7);	// Page end address
#endif
#if SSD1306_LCDHEIGHT == 32
    ssd1306::command(3);	// Page end address
#endif
#if SSD1306_LCDHEIGHT == 16
    ssd1306::command(1);	// Page end address
#endif

    // I2C
    int i;
    for (i = 0; i < (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8); i++) {
        wiringPiI2CWriteReg8(i2cd, 0x40, static_cast<int>(buffer[i]));
        //This sends byte by byte.
        //Better to send all buffer without 0x40 first
        //Should be optimized
    }
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// ssd1306_scrollright(0x00, 0x0F)
void ssd1306::startscrollright(unsigned int start, unsigned int stop)
{
    ssd1306::command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
    ssd1306::command(0X00);
    ssd1306::command(start);
    ssd1306::command(0X00);
    ssd1306::command(stop);
    ssd1306::command(0X00);
    ssd1306::command(0XFF);
    ssd1306::command(SSD1306_ACTIVATE_SCROLL);
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// ssd1306_scrollright(0x00, 0x0F)
void ssd1306::startscrollleft(unsigned int start, unsigned int stop)
{
    ssd1306::command(SSD1306_LEFT_HORIZONTAL_SCROLL);
    ssd1306::command(0X00);
    ssd1306::command(start);
    ssd1306::command(0X00);
    ssd1306::command(stop);
    ssd1306::command(0X00);
    ssd1306::command(0XFF);
    ssd1306::command(SSD1306_ACTIVATE_SCROLL);
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// ssd1306_scrollright(0x00, 0x0F)
void ssd1306::startscrolldiagright(unsigned int start, unsigned int stop)
{
    ssd1306::command(SSD1306_SET_VERTICAL_SCROLL_AREA);
    ssd1306::command(0X00);
    ssd1306::command(SSD1306_LCDHEIGHT);
    ssd1306::command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
    ssd1306::command(0X00);
    ssd1306::command(start);
    ssd1306::command(0X00);
    ssd1306::command(stop);
    ssd1306::command(0X01);
    ssd1306::command(SSD1306_ACTIVATE_SCROLL);
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// ssd1306_scrollright(0x00, 0x0F)
void ssd1306::startscrolldiagleft(unsigned int start, unsigned int stop)
{
    ssd1306::command(SSD1306_SET_VERTICAL_SCROLL_AREA);
    ssd1306::command(0X00);
    ssd1306::command(SSD1306_LCDHEIGHT);
    ssd1306::command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
    ssd1306::command(0X00);
    ssd1306::command(start);
    ssd1306::command(0X00);
    ssd1306::command(stop);
    ssd1306::command(0X01);
    ssd1306::command(SSD1306_ACTIVATE_SCROLL);
}

void ssd1306::stopscroll(void)
{
    ssd1306::command(SSD1306_DEACTIVATE_SCROLL);
}

// Dim the display
// dim = true: display is dimmed
// dim = false: display is normal
void ssd1306::dim(bool dim)
{
    unsigned int contrast;

    if (dim) {
        contrast = 0;	// Dimmed display
    }
    else {
        if (_vccstate == SSD1306_EXTERNALVCC) {
            contrast = 0x9F;
        }
        else {
            contrast = 0xCF;
        }
    }
    // the range of contrast to too small to be really useful
    // it is useful to dim the display
    ssd1306::command(SSD1306_SETCONTRAST);
    ssd1306::command(contrast);
}

// clear everything
void ssd1306::clearDisplay(void)
{
    memset(buffer, 0,
        (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8) * sizeof(int));
    cursor_y = 0;
    cursor_x = 0;
}

void ssd1306::drawFastHLineInternal(int x, int y, int w, unsigned int color)
{
    // Do bounds/limit checks
    if (y < 0 || y >= HEIGHT) {
        return;
    }
    // make sure we don't try to draw below 0
    if (x < 0) {
        w += x;
        x = 0;
    }
    // make sure we don't go off the edge of the display
    if ((x + w) > WIDTH) {
        w = (WIDTH - x);
    }
    // if our width is now negative, punt
    if (w <= 0) {
        return;
    }
    // set up the pointer for movement through the buffer
    unsigned int* pBuf = buffer;
    // adjust the buffer pointer for the current row
    pBuf += ((y / 8) * SSD1306_LCDWIDTH);
    // and offset x columns in
    pBuf += x;

    unsigned int mask = 1 << (y & 7);

    switch (color) {
    case WHITE:
        while (w--) {
            *pBuf++ |= mask;
        };
        break;
    case BLACK:
        mask = ~mask;
        while (w--) {
            *pBuf++ &= mask;
        };
        break;
    case INVERSE:
        while (w--) {
            *pBuf++ ^= mask;
        };
        break;
    }
}

void ssd1306::drawFastVLineInternal(int x, int __y, int __h, unsigned int color)
{

    // do nothing if we're off the left or right side of the screen
    if (x < 0 || x >= WIDTH) {
        return;
    }
    // make sure we don't try to draw below 0
    if (__y < 0) {
        // __y is negative, this will subtract enough from __h to account
        // for __y being 0
        __h += __y;
        __y = 0;

    }
    // make sure we don't go past the height of the display
    if ((__y + __h) > HEIGHT) {
        __h = (HEIGHT - __y);
    }
    // if our height is now negative, punt
    if (__h <= 0) {
        return;
    }
    // this display doesn't need ints for coordinates, use local byte
    // registers for faster juggling
    unsigned int y = static_cast<unsigned int>(__y);
    unsigned int h = static_cast<unsigned int>(__h);

    // set up the pointer for fast movement through the buffer
    unsigned int* pBuf = buffer;
    // adjust the buffer pointer for the current row
    pBuf += ((y / 8) * SSD1306_LCDWIDTH);
    // and offset x columns in
    pBuf += x;

    // do the first partial byte, if necessary - this requires some
    // masking
    unsigned int mod = (y & 7);
    if (mod) {
        // mask off the high n bits we want to set
        mod = 8 - mod;

        // note - lookup table results in a nearly 10% performance
        // improvement in fill* functions
        // register unsigned int mask = ~(0xFF >> (mod));
        static unsigned int premask[8] =
        { 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
        unsigned int mask = premask[mod];

        // adjust the mask if we're not going to reach the end of this
        // byte
        if (h < mod) {
            mask &= (0XFF >> (mod - h));
        }

        switch (color) {
        case WHITE:
            *pBuf |= mask;
            break;
        case BLACK:
            *pBuf &= ~mask;
            break;
        case INVERSE:
            *pBuf ^= mask;
            break;
        }

        // fast exit if we're done here!
        if (h < mod) {
            return;
        }

        h -= mod;

        pBuf += SSD1306_LCDWIDTH;
    }
    // write solid bytes while we can - effectively doing 8 rows at a time
    if (h >= 8) {
        if (color == INVERSE) {	// separate copy of the code so we don't
            // impact performance of the black/white
            // write version with an extra comparison
            // per loop
            do {
                *pBuf = ~(*pBuf);

                // adjust the buffer forward 8 rows worth of data
                pBuf += SSD1306_LCDWIDTH;

                // adjust h & y (there's got to be a faster way for me to
                // do this, but this should still help a fair bit for now)
                h -= 8;
            } while (h >= 8);
        }
        else {
            // store a local value to work with
            unsigned int val = (color == WHITE) ? 255 : 0;

            do {
                // write our value in
                *pBuf = val;

                // adjust the buffer forward 8 rows worth of data
                pBuf += SSD1306_LCDWIDTH;

                // adjust h & y (there's got to be a faster way for me to
                // do this, but this should still help a fair bit for now)
                h -= 8;
            } while (h >= 8);
        }
    }
    // now do the final partial byte, if necessary
    if (h) {
        mod = h & 7;
        // this time we want to mask the low bits of the byte, vs the high
        // bits we did above
        // register unsigned int mask = (1 << mod) - 1;
        // note - lookup table results in a nearly 10% performance
        // improvement in fill* functions
        static unsigned int postmask[8] =
        { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
        unsigned int mask = postmask[mod];
        switch (color) {
        case WHITE:
            *pBuf |= mask;
            break;
        case BLACK:
            *pBuf &= ~mask;
            break;
        case INVERSE:
            *pBuf ^= mask;
            break;
        }
    }
}

void ssd1306::drawFastHLine(int x, int y, int w, unsigned int color)
{
    unsigned int bSwap = false;
    switch (rotation) {
    case 0:
        // 0 degree rotation, do nothing
        break;
    case 1:
        // 90 degree rotation, swap x & y for rotation, then invert x
        bSwap = true;
        ssd1306_swap(x, y);
        x = WIDTH - x - 1;
        break;
    case 2:
        // 180 degree rotation, invert x and y - then shift y around for
        // height.
        x = WIDTH - x - 1;
        y = HEIGHT - y - 1;
        x -= (w - 1);
        break;
    case 3:
        // 270 degree rotation, swap x & y for rotation, then invert y and
        // adjust y for w (not to become h)
        bSwap = true;
        ssd1306_swap(x, y);
        y = HEIGHT - y - 1;
        y -= (w - 1);
        break;
    }

    if (bSwap) {
        ssd1306::drawFastVLineInternal(x, y, w, color);
    }
    else {
        ssd1306::drawFastHLineInternal(x, y, w, color);
    }
}

void ssd1306::drawFastVLine(int x, int y, int h, unsigned int color)
{
    unsigned int bSwap = false;
    switch (rotation) {
    case 0:
        break;
    case 1:
        // 90 degree rotation, swap x & y for rotation, then invert x and
        // adjust x for h (now to become w)
        bSwap = true;
        ssd1306_swap(x, y);
        x = WIDTH - x - 1;
        x -= (h - 1);
        break;
    case 2:
        // 180 degree rotation, invert x and y - then shift y around for
        // height.
        x = WIDTH - x - 1;
        y = HEIGHT - y - 1;
        y -= (h - 1);
        break;
    case 3:
        // 270 degree rotation, swap x & y for rotation, then invert y
        bSwap = true;
        ssd1306_swap(x, y);
        y = HEIGHT - y - 1;
        break;
    }

    if (bSwap) {
        ssd1306::drawFastHLineInternal(x, y, h, color);
    }
    else {
        ssd1306::drawFastVLineInternal(x, y, h, color);
    }
}

void ssd1306::fillRect(int x, int y, int w, int h, int fillcolor)
{

    // Bounds check
    if ((x >= WIDTH) || (y >= HEIGHT))
        return;

    // Y bounds check
    if (y + h > HEIGHT) {
        h = HEIGHT - y - 1;
    }
    // X bounds check
    if (x + w > WIDTH) {
        w = WIDTH - x - 1;
    }

    switch (rotation) {
    case 0:
        break;
    case 1:
        ssd1306_swap(x, y);
        x = WIDTH - x - 1;
        break;
    case 2:
        x = WIDTH - x - 1;
        y = HEIGHT - y - 1;
        break;
    case 3:
        ssd1306_swap(x, y);
        y = HEIGHT - y - 1;
        break;
    }
    int i;
    for (i = 0; i < h; i++)
        ssd1306::drawFastHLine(x, y + i, w, static_cast<unsigned int>(fillcolor));
}

void ssd1306::setTextSize(int s)
{
    textsize = (s > 0) ? s : 1;
}

void ssd1306::write(int c)
{

    if (c == '\n') {
        cursor_y += textsize * 8;
        cursor_x = 0;
    }
    else if (c == '\r') {
        // skip em
    }
    else {
        ssd1306::drawChar(cursor_x, cursor_y, static_cast<char>(c), WHITE, textsize);
        cursor_x += textsize * 6;
        if (wrap && (cursor_x > (WIDTH - textsize * 6))) {
            cursor_y += textsize * 8;
            cursor_x = 0;
        }
    }
}

void ssd1306::drawString(char* str)
{
    int i, end;
    end = static_cast<int>(strlen(str));
    for (i = 0; i < end; i++)
        ssd1306::write(str[i]);
}

void ssd1306::drawChar(int x, int y, unsigned char c, int color, int size)
{

    if ((x >= WIDTH) ||	// Clip right
        (y >= HEIGHT) ||	// Clip bottom
        ((x + 6 * size - 1) < 0) ||	// Clip left
        ((y + 8 * size - 1) < 0))	// Clip top
        return;
    int i;
    int j;
    for (i = 0; i < 6; i++) {
        int line;
        if (i == 5)
            line = 0x0;
        else
            line = pgm_read_byte(font + (c * 5) + i);
        for (j = 0; j < 8; j++) {
            if (line & 0x1) {
                if (size == 1)	// default size
                    ssd1306::drawPixel(x + i, y + j, static_cast<unsigned int>(color));
                else {	// big size
                    ssd1306::fillRect(x + (i * size),
                        y + (j * size), size,
                        size, color);
                }
            }
            line >>= 1;
        }
    }
}


void ssd1306::ecrireValeurEcran(int typeCapteur, Capteurs capteur){
    char new_line[] = {"\n"};

    switch (typeCapteur) {
    case Capteurs::VitesseVent:{
        double V = capteur.methodeReturn(Capteurs::VitesseVent);
        char vitesse[10];
        sprintf(vitesse,"%.1f", V);
        char vitesse_vent[15] = {0};
        strcat(vitesse_vent, "V=");
        strcat(vitesse_vent, vitesse);
        const char unit_vitesse[] = {"m/s"};
        strcat(vitesse_vent,unit_vitesse);
        ssd1306::drawString(vitesse_vent);
        ssd1306::drawString(new_line);
        break;
    }
    case Capteurs::PoidsAire:{
        double P = capteur.methodeReturn(Capteurs::PoidsAire);
        char poids[10];
        sprintf(poids,"%.1f", P);
        char poids_air[15] = {0};
        strcat(poids_air, "P=");
        strcat(poids_air, poids);
        const char unit_poids[] = {"kg"};
        strcat(poids_air,unit_poids);
        ssd1306::drawString(poids_air);
        ssd1306::drawString(new_line);
        break;
    }
    case Capteurs::Luminosite:{
        double L = capteur.methodeReturn(Capteurs::Luminosite);
        char lumi[10];
        sprintf(lumi,"%.1f", L);
        char luminosite[15] = {0};
        strcat(luminosite, "L=");
        strcat(luminosite, lumi);
        const char unit_lum[] = {"Lux"};
        strcat(luminosite,unit_lum);
        ssd1306::drawString(luminosite);
        ssd1306::drawString(new_line);
        break;
    }
    case Capteurs::Temperature:{
        #define DEGRE 248
        char buff[10];
        double T = capteur.methodeReturn(Capteurs::Temperature);
        sprintf(buff, "%.1f", T);
        char temperature[12] = {0};
        strcat(temperature, "T=");
        strcat(temperature, buff);
        char unit[] = {DEGRE, 'C', 0};
        strcat(temperature, unit);
        ssd1306::drawString(temperature);
        ssd1306::drawString(new_line);
        break;
    }
    case Capteurs::Humidite:{
        double H = capteur.methodeReturn(Capteurs::Humidite);
        char hum[10];
        sprintf(hum,"%.1f", H);
        char humidite[15] = {0};
        strcat(humidite, "Hu=");
        strcat(humidite, hum);
        const char unit_hum[] = {"RH%"};
        strcat(humidite,unit_hum);
        ssd1306::drawString(humidite);
        ssd1306::drawString(new_line);
        break;
    }
    default:
        break;
    }
}
