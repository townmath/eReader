/*
 * MVBook for the Open Book
 * The Minimum Viable Book Reader
 * by Joey Castillo
 * updated to Arduino with E-Paper Shield by Jim Town
 *
 * MIT License
 */

#include <SD.h>
#include <SPI.h>
#include "OpenBook_4in2.h"

#define COLORED     0
#define UNCOLORED   1
//#define SD_CS 6
//maybe need these?
//#define EPD_CS 10
//#define SPIRAM_CS 5
// global state
//Epd epd;

//File entry;
File file;
bool fullRefresh = true;
char currentProgressFile[13];
char book[13];
const unsigned char linesPerPage=24;
size_t currentLine = 0;
bool atEnd = false;
bool bookNeedsRefresh = false;
const int selectButton=A1;
const int nextButton=A0;
const int prevButton=A3;



void open_file(char* currentBook) {
    //currentBook = (char *)menu_item->get_name();

    // create or read progress file, just an int for what line the reader is on
    strcpy(currentProgressFile, currentBook);
    currentProgressFile[strlen(currentProgressFile) - 1] = 'P';
    if (SD.exists(currentProgressFile)) {
        File f = SD.open(currentProgressFile, FILE_READ);
        f.read(&currentLine, sizeof(size_t));
        f.close();
    } else {
        currentLine = 2; // start of lines, after title and author
        File f = SD.open(currentProgressFile, FILE_WRITE);
        f.write((byte *)&currentLine, sizeof(size_t));
        f.close();
    }
    bookNeedsRefresh = true;

    //Serial.print("Opening ");
    //Serial.println(currentBook);
}

void setup() {
    //Serial.begin(115200);
    //while (!Serial) {
    //  ; // wait for serial port to connect. Needed for native USB port only
    //}
    DEV_Module_Init();
    EPD_4IN2_Init();//initialize screen
    EPD_4IN2_Clear();//clear screen buffer
    DEV_Delay_ms(500);//possibly superfluous

    //Serial.println("MVBook Demo!");
    //initialize sd card

    //Serial.print("Initializing SD card...");

    if (!SD.begin(SD_CS)) {
    //  Serial.println("initialization failed!");
      while (1);
    }

    //TODO configure buttons
    pinMode(selectButton,INPUT_PULLUP);
    pinMode(prevButton,INPUT_PULLUP);
    pinMode(nextButton,INPUT_PULLUP);

    File root = SD.open("/");

    int i = 0;
    File entry = root.openNextFile();
    //Serial.println("Opening book...");
   // Serial.println(entry.name());


    while (entry){
      //Serial.println(entry.name());
        if (!entry.isDirectory()) {
            uint64_t magic = 0;
            uint8_t terminator = 0;
            entry.read((void *)&magic, 8);
            if (magic == 5426643222204338255) { // the string "OPENBOOK"
              //Serial.println(entry.name());
              char *filename = (char *)malloc(13);
              strcpy(filename, entry.name());

              //Serial.println(filename);//
             // ms.get_root_menu().add_item(item);
              strcpy(book,filename);
              open_file(book);
              break;
            }
        }
        entry.close();
        entry = root.openNextFile();
    }
    entry.close();
    root.close();
    file = SD.open(book);
    //open_file(book);
}

void loop() {

    if (!digitalRead(prevButton)){
        delay(200);//debounce
        Serial.println("prev pressed");
        if (currentLine > linesPerPage) {
            currentLine -= linesPerPage;
            atEnd = false;
            bookNeedsRefresh = true;
        }
    }
    if (!digitalRead(nextButton)) {
        delay(200);//debounce
        Serial.println("next pressed");
        if (!atEnd) {
            currentLine += linesPerPage;
            bookNeedsRefresh = true;
        }
    }
    if (!digitalRead(selectButton)) {
        delay(200);//debounce
        Serial.println("select pressed");
        // update user's stored location
        //Serial.println(currentProgressFile);
        //Serial.println(currentLine);
        File f = SD.open(currentProgressFile, O_WRITE | O_CREAT);
        f.write((byte *)&currentLine, sizeof(size_t));
        f.close();
        //clear and turn off screen
        EPD_4IN2_Clear();
        EPD_4IN2_Sleep();
        DEV_Module_Exit();
        return;
    }

    if (bookNeedsRefresh) {
        //Serial.println("test");

        // refresh the screen
        bookNeedsRefresh = false;
        
        int linePos=5;//16?
        Paint_NewImage(IMAGE_BW, EPD_4IN2_WIDTH, EPD_4IN2_HEIGHT, IMAGE_ROTATE_90, IMAGE_COLOR_POSITIVE);
        Paint_Clear(WHITE);
        //delay(500);
        //Serial.print("writing lines of ");
        //Serial.println(book);
        for(int i = 0; i < linesPerPage; i++) {
            uint64_t loc;
            uint32_t len;

            file.seek(8 + (i + currentLine) * 12);
            file.read(&loc, 8);
            file.read(&len, 4);

            // handle flags (high bit of len indicates last line)
            atEnd = len >> 15;
            len &= 0x7FFF;

            file.seek(loc);
            char *line = (char *)malloc(len + 1);
            file.read(line, len);
            line[len] = 0;
            //Serial.println(line);

            Paint_DrawString_EN(5, linePos, line, &Font16, WHITE, BLACK);
            free(line);
            linePos+=16;
            if (atEnd) {
                Serial.println("break");
                break;
            }
        }

        EPD_4IN2_Display();

    }

}
