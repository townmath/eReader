/*****************************************************************************
* | File      	:   EPD_4in2.c
* | Author      :   Waveshare team
* | Function    :   4.2inch e-paper
* | Info        :
*----------------
* |	This version:   V3.0
* | Date        :   2019-06-13
* | Info        :
* -----------------------------------------------------------------------------

******************************************************************************/
#include "OpenBook_4in2.h"

PAINT_IMAGE Paint_Image;

static const unsigned char EPD_4IN2_lut_vcom0[] = {
    0x00, 0x17, 0x00, 0x00, 0x00, 0x02,
    0x00, 0x17, 0x17, 0x00, 0x00, 0x02,
    0x00, 0x0A, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x0E, 0x0E, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const unsigned char EPD_4IN2_lut_ww[] = {
    0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
    0x40, 0x0A, 0x01, 0x00, 0x00, 0x01,
    0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const unsigned char EPD_4IN2_lut_bw[] = {
    0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
    0x40, 0x0A, 0x01, 0x00, 0x00, 0x01,
    0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const unsigned char EPD_4IN2_lut_wb[] = {
    0x80, 0x17, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
    0x80, 0x0A, 0x01, 0x00, 0x00, 0x01,
    0x50, 0x0E, 0x0E, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const unsigned char EPD_4IN2_lut_bb[] = {
    0x80, 0x17, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
    0x80, 0x0A, 0x01, 0x00, 0x00, 0x01,
    0x50, 0x0E, 0x0E, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
/******************************partial screen update LUT*********************************/

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
static void EPD_4IN2_Reset(void)
{
    DEV_Digital_Write(OPENBOOK_ERST, 1);
    DEV_Delay_ms(200);
    DEV_Digital_Write(OPENBOOK_ERST, 0);
    DEV_Delay_ms(200);
    DEV_Digital_Write(OPENBOOK_ERST, 1);
    DEV_Delay_ms(200);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void EPD_4IN2_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(OPENBOOK_EDC, 0);
    DEV_Digital_Write(OPENBOOK_ECS, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(OPENBOOK_ECS, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void EPD_4IN2_SendData(UBYTE Data)
{
    DEV_Digital_Write(OPENBOOK_EDC, 1);
    DEV_Digital_Write(OPENBOOK_ECS, 0);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(OPENBOOK_ECS, 1);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
void EPD_4IN2_ReadBusy(void)
{
    //DEBUG("e-Paper busy\r\n");
    while(DEV_Digital_Read(OPENBOOK_EBSY) == 0) {      //LOW: idle, HIGH: busy
        DEV_Delay_ms(100);
    }
    //DEBUG("e-Paper busy release\r\n");
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
static void EPD_4IN2_TurnOnDisplay(void)
{
    EPD_4IN2_SendCommand(0x12);
    DEV_Delay_ms(100);
    EPD_4IN2_ReadBusy();
}

/******************************************************************************
function :	set the look-up tables
parameter:
******************************************************************************/
static void EPD_4IN2_SetLut(void)
{
    UWORD count;
    EPD_4IN2_SendCommand(0x20);         //g vcom
    for(count=0; count<44; count++) {
        EPD_4IN2_SendData(EPD_4IN2_lut_vcom0[count]);
    }

    EPD_4IN2_SendCommand(0x21);
    for(count=0; count<42; count++) {
        EPD_4IN2_SendData(EPD_4IN2_lut_ww[count]);
    }

    EPD_4IN2_SendCommand(0x22);
    for(count=0; count<42; count++) {
        EPD_4IN2_SendData(EPD_4IN2_lut_bw[count]);
    }

    EPD_4IN2_SendCommand(0x23);
    for(count=0; count<42; count++) {
        EPD_4IN2_SendData(EPD_4IN2_lut_wb[count]);
    }

    EPD_4IN2_SendCommand(0x24);
    for(count=0; count<42; count++) {
        EPD_4IN2_SendData(EPD_4IN2_lut_bb[count]);
    }
}

/******************************************************************************/
void EPD_4IN2_Init(void)
{
    EPD_4IN2_Reset();

    EPD_4IN2_SendCommand(0x01); // POWER SETTING
    EPD_4IN2_SendData(0x03);
    EPD_4IN2_SendData(0x00);
    EPD_4IN2_SendData(0x2b);
    EPD_4IN2_SendData(0x2b);

    EPD_4IN2_SendCommand(0x06); // boost soft start
    EPD_4IN2_SendData(0x17);		//A
    EPD_4IN2_SendData(0x17);		//B
    EPD_4IN2_SendData(0x17);		//C

    EPD_4IN2_SendCommand(0x04); // POWER_ON
    EPD_4IN2_ReadBusy();

    EPD_4IN2_SendCommand(0x00); // panel setting
    EPD_4IN2_SendData(0xbf); // KW-BF   KWR-AF	BWROTP 0f	BWOTP 1f
    EPD_4IN2_SendData(0x0d);

    EPD_4IN2_SendCommand(0x30); // PLL setting
    EPD_4IN2_SendData(0x3C); // 3A 100HZ   29 150Hz 39 200HZ	31 171HZ

    EPD_4IN2_SendCommand(0x61); // resolution setting
    EPD_4IN2_SendData(0x01);
    EPD_4IN2_SendData(0x90); //128
    EPD_4IN2_SendData(0x01); //
    EPD_4IN2_SendData(0x2c);

    EPD_4IN2_SendCommand(0x82); // vcom_DC setting
    EPD_4IN2_SendData(0x28);

    EPD_4IN2_SendCommand(0X50); // VCOM AND DATA INTERVAL SETTING
    EPD_4IN2_SendData(0x97); // 97white border 77black border		VBDF 17|D7 VBDW 97 VBDB 57		VBDF F7 VBDW 77 VBDB 37  VBDR B7

    EPD_4IN2_SetLut();
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_4IN2_Clear(void)
{
    UWORD Width, Height;
    Width = (EPD_4IN2_WIDTH % 8 == 0)? (EPD_4IN2_WIDTH / 8 ): (EPD_4IN2_WIDTH / 8 + 1);
    Height = EPD_4IN2_HEIGHT;

    EPD_4IN2_SendCommand(0x10);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_4IN2_SendData(0xFF);
        }
    }

    EPD_4IN2_SendCommand(0x13);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_4IN2_SendData(0xFF);
        }
    }

    EPD_4IN2_TurnOnDisplay();
}

UBYTE  SPIRAM_RD_Byte(UDOUBLE Addr)
{
    UBYTE RD_Byte;

    SPIRAM_CS_0;
    DEV_SPI_WriteByte(CMD_READ);

    DEV_SPI_WriteByte((UBYTE)(Addr >> 16));
    DEV_SPI_WriteByte((UBYTE)(Addr >> 8));
    DEV_SPI_WriteByte((UBYTE)Addr);

    RD_Byte = DEV_SPI_ReadByte(0x00);
    SPIRAM_CS_1;

    return RD_Byte;
}

void  SPIRAM_WR_Byte(UDOUBLE Addr, const UBYTE Data)
{
    // Write Addr,data
    SPIRAM_CS_0;

    DEV_SPI_WriteByte(CMD_WRITE);

    DEV_SPI_WriteByte((UBYTE)(Addr >> 16));
    DEV_SPI_WriteByte((UBYTE)(Addr >> 8));
    DEV_SPI_WriteByte((UBYTE)Addr);

    DEV_SPI_WriteByte(Data);
    SPIRAM_CS_1;
}

void  SPIRAM_Set_Mode(UBYTE mode)
{
    SPIRAM_CS_0;

    DEV_SPI_WriteByte(CMD_WRSR);
    DEV_SPI_WriteByte(mode);

    SPIRAM_CS_1;
}

void DEV_Delay_ms(UWORD xms)
{
    delay(xms);
}

UBYTE DEV_Module_Init(void)
{
    //set pin
    pinMode(SPIRAM_CS, OUTPUT);
    pinMode(SD_CS, OUTPUT);
    
    pinMode(OPENBOOK_ECS, OUTPUT);
    pinMode(OPENBOOK_EDC, OUTPUT);
    pinMode(OPENBOOK_ERST, OUTPUT);
    pinMode(OPENBOOK_EBSY, INPUT);

    EPD_CS_1;
    SD_CS_1;
    SPIRAM_CS_1;
    
    //set Serial
    //Serial.begin(115200);
      
    //set OLED SPI
    // SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV4);
    SPI.begin();

    return 0;
}

void DEV_Module_Exit(void)
{
    EPD_CS_1;
    SD_CS_1;
    SPIRAM_CS_1;
    EPD_RST_0;
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EPD_4IN2_Display(void)
{
    UWORD Width, Height;
    Width = (EPD_4IN2_WIDTH % 8 == 0)? (EPD_4IN2_WIDTH / 8 ): (EPD_4IN2_WIDTH / 8 + 1);
    Height = EPD_4IN2_HEIGHT;

    EPD_4IN2_SendCommand(0x13);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_4IN2_SendData(SPIRAM_RD_Byte(i + j * Width));
        }
    }
    EPD_4IN2_TurnOnDisplay();
}

void Paint_Clear(UWORD Color)
{
    UWORD Height = Paint_Image.Memory_Height;
		UWORD Width = Paint_Image.Memory_Width; 
		if(Paint_Image.Image_Color == IMAGE_COLOR_INVERTED) {
						Color = ~Color;
		}
		
		if(Paint_Image.Scale == 2){
				Width = (Paint_Image.Memory_Width % 8 == 0)? (Paint_Image.Memory_Width / 8 ): (Paint_Image.Memory_Width / 8 + 1);
				UDOUBLE Offset = Paint_Image.Image_Offset;
				
				for (UWORD Y = 0; Y < Height; Y++) {
						for (UWORD X = 0; X < Width; X++ ) {//8 pixel =  1 byte
								SPIRAM_WR_Byte((X + Y * Width) + Offset, Color);
						}
				}
		}else if(Paint_Image.Scale == 4){
				Width = (Paint_Image.Memory_Width % 4 == 0)? (Paint_Image.Memory_Width / 4 ): (Paint_Image.Memory_Width / 4 + 1);
				UDOUBLE Offset = Paint_Image.Image_Offset;
				if(Color%4 == 0x00){
						Color = 0x00;
				}else if(Color%4 == 0x01){
						Color = 0x55;
				}else if(Color%4 == 0x02){
						Color = 0xAA;
				}else if(Color%4 == 0x03){
						Color = 0xff;
				}
				for (UWORD Y = 0; Y < Height; Y++) {
						for (UWORD X = 0; X < Width; X++ ) {//8 pixel =  1 byte
								SPIRAM_WR_Byte((X + Y * Width) + Offset, Color);
						}
				}
		}
}

void Paint_NewImage(UWORD Image_Name, UWORD Width, UWORD Height, UWORD Rotate, UWORD Color)
{
    SPIRAM_Set_Mode(BYTE_MODE);

    if(Rotate == IMAGE_ROTATE_0){// || Rotate == IMAGE_ROTATE_180) {
        Paint_Image.Image_Width = Width;
        Paint_Image.Image_Height = Height;
    } else {
        Paint_Image.Image_Width = Height;
        Paint_Image.Image_Height = Width;
    }
    Paint_Image.Image_Name = Image_Name;//At least one picture
    Paint_Image.Image_Rotate = Rotate;
    Paint_Image.Memory_Width = Width;
    Paint_Image.Memory_Height = Height;
    Paint_Image.Image_Color = Color;
	Paint_Image.Scale = 2;
	Paint_Image.Mirror = MIRROR_NONE;
		
    UWORD BYTE_Height = Paint_Image.Memory_Height;
    UWORD BYTE_Width = (Paint_Image.Memory_Width % 8 == 0)? (Paint_Image.Memory_Width / 8 ): (Paint_Image.Memory_Width / 8 + 1);
    Paint_Image.Image_Offset =  Paint_Image.Image_Name * (BYTE_Height * BYTE_Width);
}

static void Paint_DrawPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color)
{
    // UWORD Height = Paint_Image.Memory_Height;
    UWORD Width = (Paint_Image.Memory_Width % 8 == 0)? (Paint_Image.Memory_Width / 8 ): (Paint_Image.Memory_Width / 8 + 1);
    UDOUBLE Offset = Paint_Image.Image_Offset;
  
    switch(Paint_Image.Mirror) {
    case MIRROR_NONE:
        break;
    case MIRROR_HORIZONTAL:
        Xpoint = Paint_Image.Memory_Width - Xpoint - 1;
        break;
    case MIRROR_VERTICAL:
        Ypoint = Paint_Image.Memory_Height - Ypoint - 1;
        break;
    case MIRROR_ORIGIN:
        Xpoint = Paint_Image.Memory_Width - Xpoint - 1;
        Ypoint = Paint_Image.Memory_Height - Ypoint - 1;
        break;
    default:
        return;
    }
  
    UDOUBLE Addr = Xpoint / 8 + Ypoint * Width + Offset;
    UBYTE Rdata = SPIRAM_RD_Byte(Addr);

  if(Paint_Image.Scale == 2){
      UWORD Width = (Paint_Image.Memory_Width % 8 == 0)? (Paint_Image.Memory_Width / 8 ): (Paint_Image.Memory_Width / 8 + 1);
      UDOUBLE Addr = Xpoint / 8 + Ypoint * Width + Offset;
      UBYTE Rdata = SPIRAM_RD_Byte(Addr);
      if(Paint_Image.Image_Color == IMAGE_COLOR_POSITIVE) {
          if(Color == BLACK)
              SPIRAM_WR_Byte(Addr, Rdata & ~(0x80 >> (Xpoint % 8)));
          else
              SPIRAM_WR_Byte(Addr, Rdata | (0x80 >> (Xpoint % 8)));
      } else {
          if(Color == WHITE)
              SPIRAM_WR_Byte(Addr, Rdata & ~(0x80 >> (Xpoint % 8)));
          else
              SPIRAM_WR_Byte(Addr, Rdata | (0x80 >> (Xpoint % 8)));
      }
  }else if(Paint_Image.Scale == 4){
      UWORD Width = (Paint_Image.Memory_Width % 4 == 0)? (Paint_Image.Memory_Width / 4 ): (Paint_Image.Memory_Width / 4 + 1);
      UDOUBLE Addr = Xpoint / 4 + Ypoint * Width + Offset;
  Color = Color % 4;//Guaranteed color scale is 4  --- 0~3
  UBYTE Rdata = SPIRAM_RD_Byte(Addr);
  
  Rdata = Rdata & (~(0xC0 >> ((Xpoint % 4)*2)));
  SPIRAM_WR_Byte(Addr, Rdata | ((Color << 6) >> ((Xpoint % 4)*2)));
  }
}

static void Paint_SetPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color)
{
    UWORD X, Y;
    switch(Paint_Image.Image_Rotate) {
    case IMAGE_ROTATE_0:
        X = Xpoint;
        Y = Ypoint;
        Paint_DrawPixel(X, Y, Color);
        break;
    case IMAGE_ROTATE_90:
        X = Ypoint;
        Y = Paint_Image.Image_Width - Xpoint;
        Paint_DrawPixel(X, Y, Color);
        break;
/*    case IMAGE_ROTATE_180:
        X = Paint_Image.Image_Width - Xpoint;
        Y = Paint_Image.Image_Height - Ypoint;
        Paint_DrawPixel(X, Y, Color);
        break;
    case IMAGE_ROTATE_270:
        X = Paint_Image.Image_Height - Ypoint;
        Y = Xpoint;
        Paint_DrawPixel(X, Y, Color);
        break;*/
    }
}

void Paint_DrawPoint(UWORD Xpoint, UWORD Ypoint, UWORD Color,
                     DOT_PIXEL Dot_Pixel, DOT_STYLE DOT_STYLE)
{
    if (Xpoint > Paint_Image.Image_Width || Ypoint > Paint_Image.Image_Height) {
        Serial.println("Paint_DrawPoint Input exceeds the normal display range\r\n");
        return;
    }

    int16_t XDir_Num , YDir_Num;
    if (DOT_STYLE == DOT_STYLE_DFT) {
        for (XDir_Num = 0; XDir_Num < 2 * Dot_Pixel - 1; XDir_Num++) {
            for (YDir_Num = 0; YDir_Num < 2 * Dot_Pixel - 1; YDir_Num++) {
                Paint_SetPixel(Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel, Color);
            }
        }
    } else {
        for (XDir_Num = 0; XDir_Num <  Dot_Pixel; XDir_Num++) {
            for (YDir_Num = 0; YDir_Num <  Dot_Pixel; YDir_Num++) {
                Paint_SetPixel(Xpoint + XDir_Num - 1, Ypoint + YDir_Num - 1, Color);
            }
        }
    }
}
/*
void Paint_DrawBitMap(const unsigned char* image_buffer)
{
    UWORD Xpoint, Ypoint, Height, Width;
    Height = Paint_Image.Memory_Height;
    Width = (Paint_Image.Memory_Width % 8 == 0)? (Paint_Image.Memory_Width / 8 ): (Paint_Image.Memory_Width / 8 + 1);
    UDOUBLE Offset = Paint_Image.Image_Offset;
    UDOUBLE Addr = 0;

    for (Ypoint = 0; Ypoint < Height; Ypoint++) {
        for (Xpoint = 0; Xpoint < Width; Xpoint++) {//8 pixel =  1 byte
            Addr = Xpoint + Ypoint * Width + Offset;					
            SPIRAM_WR_Byte(Addr, pgm_read_byte(image_buffer+Xpoint + Ypoint * Width));
        }
    }
}*/

void Paint_DrawChar(UWORD Xpoint, UWORD Ypoint, const char Acsii_Char,
                    sFONT* Font, UWORD Color_Background, UWORD Color_Foreground)
{
    UWORD Page, Column;

    if (Xpoint > Paint_Image.Image_Width || Ypoint > Paint_Image.Image_Height) {
        Serial.println("Paint_DrawChar Input exceeds the normal display range\r\n");
        return;
    }

    uint32_t Char_Offset = (Acsii_Char - ' ') * Font->Height * (Font->Width / 8 + (Font->Width % 8 ? 1 : 0));
    //Serial.println(Acsii_Char);
    //Serial.println(Char_Offset);
    const unsigned char *ptr = &Font->table[Char_Offset];

    for (Page = 0; Page < Font->Height; Page ++ ) {
        for (Column = 0; Column < Font->Width; Column ++ ) {

            //To determine whether the font background color and screen background color is consistent
            if (FONT_BACKGROUND == Color_Background) { //this process is to speed up the scan
                if (pgm_read_byte(ptr) & (0x80 >> (Column % 8)))
                    Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
            } else {
                if (pgm_read_byte(ptr) & (0x80 >> (Column % 8))) {
                    Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                } else {
                    Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                }
            }
            //One pixel is 8 bits
            if (Column % 8 == 7)
                ptr++;
        }// Write a line
        if (Font->Width % 8 != 0)
            ptr++;
    }// Write all
}

void Paint_DrawString_EN(UWORD Xstart, UWORD Ystart, const char * pString,
                         sFONT* Font, UWORD Color_Background, UWORD Color_Foreground )
{
    UWORD Xpoint = Xstart;
    UWORD Ypoint = Ystart;

    if (Xstart > Paint_Image.Image_Width || Ystart > Paint_Image.Image_Height) {
        Serial.println("Paint_DrawString_EN Input exceeds the normal display range\r\n");
        return;
    }

    while (* pString != '\0') {
        //if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the Height of the character
        if ((Xpoint + Font->Width ) > Paint_Image.Image_Width ) {
            Xpoint = Xstart;
            Ypoint += Font->Height;
        }

        // If the Y direction is full, reposition to(Xstart, Ystart)
        if ((Ypoint  + Font->Height ) > Paint_Image.Image_Height ) {
            Xpoint = Xstart;
            Ypoint = Ystart;
        }
        Paint_DrawChar(Xpoint, Ypoint, * pString, Font, Color_Background, Color_Foreground);

        //The next character of the address
        pString ++;

        //The next word of the abscissa increases the font of the broadband
        Xpoint += Font->Width;
    }
}

/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void EPD_4IN2_Sleep(void)
{
		//DEBUG("Enter sleep mode\r\n");
    EPD_4IN2_SendCommand(0x02); // POWER_OFF
    EPD_4IN2_ReadBusy();
    EPD_4IN2_SendCommand(0x07); // DEEP_SLEEP
    EPD_4IN2_SendData(0XA5);
}
