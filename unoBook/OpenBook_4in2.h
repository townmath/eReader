/*****************************************************************************
* | File      	:   EPD_4in2.h
* | Author      :   Waveshare team
* | Function    :   4.2inch e-paper
* | Info        :
*----------------
* |	This version:   V3.0
* | Date        :   2019-06-13
* | Info        :
* -----------------------------------------------------------------------------
******************************************************************************/
#ifndef _EPD_4IN2_H_
#define _EPD_4IN2_H_

#include <stdint.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "fonts.h"

// Display resolution
#define EPD_4IN2_WIDTH       400
#define EPD_4IN2_HEIGHT      300
#define OPENBOOK_ECS (10)
#define OPENBOOK_EDC (9)
#define OPENBOOK_ERST (8)
#define OPENBOOK_EBSY (7)
#define OPENBOOK_SDCS (6)
#define OPENBOOK_SRCS (5)

/**
 * SPI RAM
**/
#define SPIRAM_CS OPENBOOK_SRCS
#define SPIRAM_CS_0     digitalWrite(SPIRAM_CS, LOW)
#define SPIRAM_CS_1     digitalWrite(SPIRAM_CS, HIGH)
#define CMD_READ  0x03
#define CMD_WRITE 0x02
#define CMD_WRSR  0x01
/**
 * SD Card
**/
#define SD_CS OPENBOOK_SDCS
#define SD_CS_0     digitalWrite(SD_CS, LOW)
#define SD_CS_1     digitalWrite(SD_CS, HIGH)
/**
 * e-Paper GPIO
**/
#define EPD_CS OPENBOOK_ECS
#define EPD_CS_0     digitalWrite(EPD_CS, LOW)
#define EPD_CS_1     digitalWrite(EPD_CS, HIGH)

#define EPD_DC OPENBOOK_EDC
#define EPD_DC_0     digitalWrite(EPD_DC, LOW)
#define EPD_DC_1     digitalWrite(EPD_DC, HIGH)

#define EPD_RST OPENBOOK_ERST
#define EPD_RST_0     digitalWrite(EPD_RST, LOW)
#define EPD_RST_1     digitalWrite(EPD_RST, HIGH)

#define EPD_BUSY OPENBOOK_EBSY
#define EPD_BUSY_RD   digitalRead(EPD_BUSY)

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

#define DEV_Digital_Write(_pin, _value) digitalWrite(_pin, _value == 0? LOW:HIGH)
#define DEV_Digital_Read(_pin) digitalRead(_pin)

#define DEV_SPI_WriteByte(_dat)   SPI.transfer(_dat)
#define DEV_SPI_ReadByte(_dat) SPI.transfer(_dat)

#define BYTE_MODE   0x00
#define PAGE_MODE   0x80
#define STREAM_MODE 0x40

UBYTE SPIRAM_RD_Byte(UDOUBLE Addr);
void SPIRAM_WR_Byte(UDOUBLE Addr, UBYTE Data);
void SPIRAM_Set_Mode(UBYTE mode);
void DEV_Delay_ms(UWORD xms);
UBYTE DEV_Module_Init(void);
void DEV_Module_Exit(void);
void EPD_4IN2_Init(void);
void EPD_4IN2_Clear(void);
void EPD_4IN2_Display(void);
void EPD_4IN2_Sleep(void);


//void EPD_4IN2_PartialDisplay(UWORD X_start,UWORD Y_start,UWORD X_end,UWORD Y_end);

//void EPD_4IN2_Init_4Gray(void);
//void EPD_4IN2_4GrayDisplay(void);

static void EPD_4IN2_SendData(UBYTE Data);
static void EPD_4IN2_SendCommand(UBYTE Reg);
static void EPD_4IN2_SetLut(void);
static void EPD_4IN2_TurnOnDisplay(void);

#define WHITE          0xFF
#define BLACK          0x00
#define RED            BLACK
#define IMAGE_BACKGROUND    WHITE
#define FONT_FOREGROUND     BLACK
#define FONT_BACKGROUND     WHITE
#define IMAGE_BW 0
#define IMAGE_COLOR_POSITIVE 0X01
#define IMAGE_COLOR_INVERTED 0X02
#define IMAGE_ROTATE_0            0
#define IMAGE_ROTATE_90           1
//#define IMAGE_ROTATE_180          2
//#define IMAGE_ROTATE_270          3

typedef enum {
    DOT_PIXEL_1X1  = 1,    // 1 x 1
    DOT_PIXEL_2X2  ,    // 2 X 2
    DOT_PIXEL_3X3  ,    // 3 X 3
    DOT_PIXEL_4X4  ,    // 4 X 4
    DOT_PIXEL_5X5  ,    // 5 X 5
    DOT_PIXEL_6X6  ,    // 6 X 6
    DOT_PIXEL_7X7  ,    // 7 X 7
    DOT_PIXEL_8X8  ,    // 8 X 8
} DOT_PIXEL;
#define DOT_PIXEL_DFT  DOT_PIXEL_1X1  //Default dot pilex

typedef enum {
    DOT_FILL_AROUND  = 1,    // dot pixel 1 x 1
    DOT_FILL_RIGHTUP  ,     // dot pixel 2 X 2
} DOT_STYLE;
#define DOT_STYLE_DFT  DOT_FILL_AROUND  //Default dot pilex

void Paint_NewImage(UWORD Image_Name, UWORD Width, UWORD Height, UWORD Rotate, UWORD Color);
void Paint_Clear(UWORD Color);
void Paint_DrawPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color);
void Paint_DrawPoint(UWORD Xpoint, UWORD Ypoint, UWORD Color, DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_FillWay);
//void Paint_DrawBitMap(const unsigned char* image_buffer);
void Paint_DrawChar(UWORD Xstart, UWORD Ystart, const char Acsii_Char, sFONT* Font, UWORD Color_Background, UWORD Color_Foreground);
void Paint_DrawString_EN(UWORD Xstart, UWORD Ystart, const char * pString, sFONT* Font, UWORD Color_Background, UWORD Color_Foreground);

typedef struct {
    UWORD Image_Name; //max = 128K / (Image_Width/8 * Image_Height)
    UWORD Image_Offset;
    UWORD Image_Width;
    UWORD Image_Height;
    UWORD Image_Rotate;
    UWORD Image_Color;
    UWORD Memory_Width;
    UWORD Memory_Height;
		UWORD Mirror;
		UWORD Scale;
} PAINT_IMAGE;

typedef enum {
    MIRROR_NONE  = 0x00,
    MIRROR_HORIZONTAL = 0x01,
    MIRROR_VERTICAL = 0x02,
    MIRROR_ORIGIN = 0x03,
} MIRROR_IMAGE;


#endif
