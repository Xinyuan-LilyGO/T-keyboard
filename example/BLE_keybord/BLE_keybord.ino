#define TFT_HIGH  40
#define TFT_WIDE 160
#define GAP 8
#define keyborad_BL_PIN  9


#include "Arduino.h"
#include <SPI.h>
#include "img.h"
#include<TFT_GC9D01N.h>
TFT_GC9D01N_Class TFT_099;

#include <BleKeyboard.h>
BleKeyboard bleKeyboard("T-Keyboard", "ESPRESSIF", 100);

byte rows[] = {0, 3, 18, 12, 11, 6, 7};
const int rowCount = sizeof(rows) / sizeof(rows[0]);


byte cols[] = {1, 4, 5, 19, 13};
const int colCount = sizeof(cols) / sizeof(cols[0]);

bool keys[colCount][rowCount];
bool lastValue[colCount][rowCount];
bool changedValue[colCount][rowCount];

char keyboard[colCount][rowCount];
char keyboard_symbol[colCount][rowCount];


bool symbolSelected;
int OffsetX = 0;
uint16_t flow_i = 0;
bool keyborad_BL_state = true;
bool display_connected = true;  //The bluetooth connection is displayed on the screen
bool case_locking = false;
bool alt_active = false;
unsigned long previousMillis_1 = 0; //Millisecond time record
unsigned long previousMillis_2 = 0; //Millisecond time record
const long backlight_off_time = 20000;       //Turn off the screen backlight
const long display_Wait_blue_time = 2000;       //The screen shows waiting for bluetooth connection

void readMatrix();
bool keyPressed(int colIndex, int rowIndex);
bool keyActive(int colIndex, int rowIndex);
bool isPrintableKey(int colIndex, int rowIndex);
void printMatrix();
void set_keyborad_BL(bool state);
void clear_sccreen();

void setup()
{
    Serial.begin(115200);
    Serial.printf("setup \n");
    keyboard[0][0] = 'q';
    keyboard[0][1] = 'w';
    keyboard[0][2] = NULL; // symbol
    keyboard[0][3] = 'a';
    keyboard[0][4] = NULL; // ALT
    keyboard[0][5] = ' ';
    keyboard[0][6] = NULL; // Mic

    keyboard[1][0] = 'e';
    keyboard[1][1] = 's';
    keyboard[1][2] = 'd';
    keyboard[1][3] = 'p';
    keyboard[1][4] = 'x';
    keyboard[1][5] = 'z';
    keyboard[1][6] = NULL; // Left Shift

    keyboard[2][0] = 'r';
    keyboard[2][1] = 'g';
    keyboard[2][2] = 't';
    keyboard[2][3] = NULL; // Right Shit
    keyboard[2][4] = 'v';
    keyboard[2][5] = 'c';
    keyboard[2][6] = 'f';

    keyboard[3][0] = 'u';
    keyboard[3][1] = 'h';
    keyboard[3][2] = 'y';
    keyboard[3][3] = NULL; // Enter
    keyboard[3][4] = 'b';
    keyboard[3][5] = 'n';
    keyboard[3][6] = 'j';

    keyboard[4][0] = 'o';
    keyboard[4][1] = 'l';
    keyboard[4][2] = 'i';
    keyboard[4][3] = NULL; // Backspace
    keyboard[4][4] = '$';
    keyboard[4][5] = 'm';
    keyboard[4][6] = 'k';

    keyboard_symbol[0][0] = '#';
    keyboard_symbol[0][1] = '1';
    keyboard_symbol[0][2] = NULL;
    keyboard_symbol[0][3] = '*';
    keyboard_symbol[0][4] = NULL;
    keyboard_symbol[0][5] = NULL;
    keyboard_symbol[0][6] = '0';

    keyboard_symbol[1][0] = '2';
    keyboard_symbol[1][1] = '4';
    keyboard_symbol[1][2] = '5';
    keyboard_symbol[1][3] = '@';
    keyboard_symbol[1][4] = '8';
    keyboard_symbol[1][5] = '7';
    keyboard_symbol[1][6] = NULL;

    keyboard_symbol[2][0] = '3';
    keyboard_symbol[2][1] = '/';
    keyboard_symbol[2][2] = '(';
    keyboard_symbol[2][3] = NULL;
    keyboard_symbol[2][4] = '?';
    keyboard_symbol[2][5] = '9';
    keyboard_symbol[2][6] = '6';

    keyboard_symbol[3][0] = '_';
    keyboard_symbol[3][1] = ':';
    keyboard_symbol[3][2] = ')';
    keyboard_symbol[3][3] = NULL;
    keyboard_symbol[3][4] = '!';
    keyboard_symbol[3][5] = ',';
    keyboard_symbol[3][6] = ';';

    keyboard_symbol[4][0] = '+';
    keyboard_symbol[4][1] = '"';
    keyboard_symbol[4][2] = '-';
    keyboard_symbol[4][3] = NULL;
    keyboard_symbol[4][4] = NULL;
    keyboard_symbol[4][5] = '.';
    keyboard_symbol[4][6] = '\'';

    delay(500);
    pinMode(keyborad_BL_PIN, OUTPUT);
    set_keyborad_BL(keyborad_BL_state);

    bleKeyboard.begin();

    for (int x = 0; x < rowCount; x++) {
        Serial.print(rows[x]); Serial.println(" as input");
        pinMode(rows[x], INPUT);
    }

    for (int x = 0; x < colCount; x++) {
        Serial.print(cols[x]); Serial.println(" as input-pullup");
        pinMode(cols[x], INPUT_PULLUP);
    }

    symbolSelected = false;


    TFT_099.begin();
    TFT_099.backlight(50);
    TFT_099.DispColor(0, 0, TFT_WIDTH, TFT_HEIGHT, BLACK);
    TFT_099.DrawImage(0, 0, 40, 160, liligo_logo);
    delay(2000);

    //Flow of the logo
    while (millis() < 6000) {
        for (int j = 0; j < 4; j++) {
            TFT_099.DrawImage(0, (160 - (flow_i + j * 55)), 40, 40, liligo_logo1);
        }
        flow_i++;
        if (flow_i == 55) {
            flow_i = 0;
        }
    }

        TFT_099.DispColor(0, 0, TFT_WIDTH, TFT_HEIGHT, BLACK);
    TFT_099.DispStr("version 1.0.0", 0, 2, WHITE, BLACK);
    delay(3000);

    TFT_099.DispColor(0, 0, TFT_WIDTH, TFT_HEIGHT, BLACK);
    TFT_099.DispStr("Wait bluetooth ......", 0, 2, WHITE, BLACK);
}


void loop()
{
    if (alt_active && keyPressed(3, 4)) {//alt+b   Change keyboard backlight status
        alt_active = false;
        TFT_099.DispColor(0, 0, TFT_HIGH, TFT_WIDE, BLACK);
        keyborad_BL_state = !keyborad_BL_state;
        set_keyborad_BL(keyborad_BL_state);
        clear_sccreen();
    }

    if (keyPressed(2, 3)) {  //Right Shit  ,Toggle case locking
        case_locking = !case_locking;
    }

    if (bleKeyboard.isConnected()) {
        if (millis() - previousMillis_1  > backlight_off_time) {//No keyboard for 20 seconds. Turn off the screen backlight
            TFT_099.backlight(0);
            previousMillis_1 = millis();;
        }

        if (display_connected) {
            TFT_099.backlight(50);
            TFT_099.DispStr("Bluetooth connected", 0, 2, WHITE, BLACK);
            display_connected = false;
        }

        readMatrix();
        printMatrix();

        // key 3,3 is the enter key
        if (keyPressed(3, 3)) {
            clear_sccreen();
            Serial.println();
            bleKeyboard.println();
        }
        //BACKSPACE
        if (keyPressed(4, 3)) {
            if (OffsetX < 8) {
                OffsetX = 0;
            } else {
                OffsetX = OffsetX - GAP;
            }

            TFT_099.DispColor(0, OffsetX, TFT_HIGH, TFT_WIDE, BLACK);
            bleKeyboard.press(KEY_BACKSPACE);
        }
        //SHIFT
        if (keyPressed(1, 6)) {
            bleKeyboard.press(KEY_RIGHT_SHIFT);
        }
        //alt+left shit, trigger ctrl+shift(Switch the input method)
        if (keyActive(0, 4) && keyPressed(1, 6)) {
            bleKeyboard.press(KEY_RIGHT_CTRL);
            bleKeyboard.press(KEY_RIGHT_SHIFT);
        }

        bleKeyboard.releaseAll();

    } else {
        if (millis() - previousMillis_2 > display_Wait_blue_time ) {
            TFT_099.DispColor(0, 0, TFT_WIDTH, TFT_HEIGHT, BLACK);
            TFT_099.DispStr("Wait bluetooth ......", 0, 2, WHITE, BLACK);
            display_connected = true;
            previousMillis_2 = millis();
        }
    }

}

// Keyboard backlit status
void set_keyborad_BL(bool state)
{
    digitalWrite(keyborad_BL_PIN, state);
}

void clear_sccreen()
{
    OffsetX = 0;
    TFT_099.DispColor(0, 0, TFT_WIDTH, TFT_HEIGHT, BLACK);
}

void readMatrix()
{


    int delayTime = 0;
    // iterate the columns
    for (int colIndex = 0; colIndex < colCount; colIndex++) {
        // col: set to output to low
        byte curCol = cols[colIndex];
        pinMode(curCol, OUTPUT);
        digitalWrite(curCol, LOW);

        // row: interate through the rows
        for (int rowIndex = 0; rowIndex < rowCount; rowIndex++) {
            byte rowCol = rows[rowIndex];
            pinMode(rowCol, INPUT_PULLUP);
            delay(1); // arduino is not fast enought to switch input/output modes so wait 1 ms

            bool buttonPressed = (digitalRead(rowCol) == LOW);

            keys[colIndex][rowIndex] = buttonPressed;
            if ((lastValue[colIndex][rowIndex] != buttonPressed)) {
                changedValue[colIndex][rowIndex] = true;
            } else {
                changedValue[colIndex][rowIndex] = false;
            }

            lastValue[colIndex][rowIndex] = buttonPressed;
            pinMode(rowCol, INPUT);
        }
        // disable the column
        pinMode(curCol, INPUT);
    }

    if (keyPressed(0, 2)) {
        symbolSelected = true;
        // symbolSelected = !symbolSelected;
    }

}

bool keyPressed(int colIndex, int rowIndex)
{
    return changedValue[colIndex][rowIndex] && keys[colIndex][rowIndex] == true;
}

bool keyActive(int colIndex, int rowIndex)
{
    return keys[colIndex][rowIndex] == true;
}

bool isPrintableKey(int colIndex, int rowIndex)
{
    return keyboard_symbol[colIndex][rowIndex] != NULL || keyboard[colIndex][rowIndex] != NULL;
}


void printMatrix()
{

    for (int rowIndex = 0; rowIndex < rowCount; rowIndex++) {
        for (int colIndex = 0; colIndex < colCount; colIndex++) {
            // we only want to print if the key is pressed and it is a printable character
            if (keyPressed(colIndex, rowIndex) && isPrintableKey(colIndex, rowIndex)) {

                String toPrint;
                if (symbolSelected) {
                    symbolSelected = false;
                    toPrint = String(keyboard_symbol[colIndex][rowIndex]);
                } else {
                    toPrint = String(keyboard[colIndex][rowIndex]);
                }

                if (keyActive(0, 4)) {
                    alt_active = true;
                    keys[0][4] = false;
                    return;
                }
                // keys 1,6 and 2,3 are Shift keys, so we want to upper case
                if (case_locking || keyActive(1, 6)) {
                    toPrint.toUpperCase();
                }

                if (OffsetX >= TFT_WIDE) {
                    OffsetX = 0;
                    TFT_099.DispColor(0, 0, TFT_HIGH, TFT_WIDE, BLACK);
                }

                TFT_099.DispColor(0, OffsetX, TFT_HIGH, TFT_WIDE, BLACK);
                char c[2];
                strcpy(c, toPrint.c_str());
                TFT_099.DispStr(c, OffsetX, 2, WHITE, BLACK);
                Serial.println(c);
                Serial.print(toPrint);
                bleKeyboard.print(toPrint);
                OffsetX = OffsetX + GAP;

                TFT_099.backlight(50);
                previousMillis_1 = millis();

            }
        }
    }
}
