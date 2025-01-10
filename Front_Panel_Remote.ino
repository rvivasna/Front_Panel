#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <MCUFRIEND_kbv.h>

// Assign human-readable names to some common 16-bit color values:

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

// Define commands

#define DRAW_LINE           48
#define DRAW_FAST_VLINE     49
#define DRAW_FAST_HLINE     50
#define DRAW_RECT           51
#define FILL_RECT           52
#define FILL_SCREEN         53
#define INVERT_DISPLAY      54
#define WRITE_PIXEL         55
#define SET_ROTATION        56
#define PRINT               57
#define SET_CURSOR          58
#define SET_TEXT_COLOR      59
#define SET_TEXT_SIZE       60
#define SET_TEXT_WRAP       61

#define DRAW_PIXEL          62
#define DRAW_CIRCLE         63
#define FILL_CIRCLE         64
#define DRAW_ROUND_RECT     65
#define FILL_ROUND_RECT     66
#define DRAW_TRIANGLE       67
#define FILL_TRIANGLE       68

#define STATUS              90 // Z

#define MAX_COMMAND_LEN 255

int    index_buffer  = 0;
char   data_buffer[MAX_COMMAND_LEN];
int    cmd_to_exec;

#define MAX_INT_REGISTERS 7
#define MAX_TEXT_REGISTERS 2
#define MAX_LEN_TEXT_REGISTER 100

int     int_registers[MAX_INT_REGISTERS];
char    text_registers[MAX_TEXT_REGISTERS][MAX_LEN_TEXT_REGISTER+1];

MCUFRIEND_kbv tft;

    int frameHeight = 53;
    int frameWithd = 476;
    int frameLeft = 3;

    int titleHeigth = 35;
    int titleWithd = 477;
// ======================================================================
// Execute command
// ======================================================================

void status()
{

}

void exec_cmd(const byte cmd,const int p1,const int p2,const int p3,const int p4,const int p5, const int p6, const int p7,
              const char *t1, const char *t2){
    switch (cmd){
        case PRINT:           tft.print(t1);break;
        case DRAW_LINE:       tft.drawLine(p1,p2,p3,p4,p5);break;
        case DRAW_FAST_VLINE: tft.drawFastVLine(p1,p2,p3,p4);break;
        case DRAW_FAST_HLINE: tft.drawFastHLine(p1,p2,p3,p4);break;
        case DRAW_RECT:       tft.drawRect(p1,p2,p3,p4,p5);break;
        case FILL_RECT:       tft.fillRect(p1,p2,p3,p4,p5);break;
        case FILL_SCREEN:     tft.fillScreen(p1);break;
        case INVERT_DISPLAY:  tft.invertDisplay(p1);break;
        case SET_ROTATION:    tft.setRotation(p1);break;

        case SET_CURSOR:      tft.setCursor(p1,p2);break;
        case SET_TEXT_COLOR:  tft.setTextColor(p1,p2);break;
        case SET_TEXT_SIZE:   tft.setTextSize(p1);break;
        case SET_TEXT_WRAP:   tft.setTextWrap(p1);break;

        case DRAW_PIXEL:      tft.drawPixel(p1,p2,p3);break;
        case DRAW_CIRCLE:     tft.drawCircle(p1,p2,p3,p4);break;
        case FILL_CIRCLE:     tft.fillCircle(p1,p2,p3,p4);break;
        case DRAW_ROUND_RECT: tft.drawRoundRect(p1,p2,p3,p4,p5,p6);break;
        case FILL_ROUND_RECT: tft.fillRoundRect(p1,p2,p3,p4,p5,p6);break;
        case DRAW_TRIANGLE:   tft.drawTriangle(p1,p2,p3,p4,p5,p6,p7);break;
        case FILL_TRIANGLE:   tft.fillTriangle(p1,p2,p3,p4,p5,p6,p7);break;

        case STATUS:          status();break;
    }
}

// ======================================================================
// Convert 4 hex digit to int
// ======================================================================

int hexToInt(const char dig1, const char dig2, const char dig3, const char dig4 ){
    int val = 0;
    char hex_number[5];
    char *hex = hex_number;
    hex_number[0]=dig1;
    hex_number[1]=dig2;
    hex_number[2]=dig3;
    hex_number[3]=dig4;
    hex_number[4]='\0';

    while (*hex) {
        // get current character then increment
        uint8_t byte = *hex++;
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}

// ======================================================================
// Parse data read from serial
// ======================================================================

void parseCmd(const char *cmd){
    uint8_t index;
    uint8_t reg_index;
    uint8_t num_params_received;

    cmd_to_exec = cmd[0];
    num_params_received = cmd[1];

    reg_index=2;

#ifdef DEBUG_MODE
    for(index=0;index<MAX_INT_REGISTERS;index++)
        int_registers[index]=0;
#endif

    for(index=0;index<num_params_received;index++) {
        int_registers[index]=hexToInt(cmd[reg_index],cmd[reg_index+1],cmd[reg_index+2],cmd[reg_index+3]);
        reg_index+=4;
    }

    strcpy(text_registers[0],&cmd[reg_index]);
    *text_registers[1]='\0';
}


// =================================================================
// exec command
// =================================================================

void execCmd() {

#ifdef DEBUG_MODE
    char test_buffer[1000];
    tft.fillScreen(BLACK);
    tft.setCursor(0,0);
    tft.setTextSize(2);
    tft.println("Debug Mode ---------------");
    sprintf(test_buffer, "Cdm    %i",cmd_to_exec);
    tft.println(test_buffer);
    sprintf(test_buffer, "Reg 01 %u",int_registers[0]);
    tft.println(test_buffer);
    sprintf(test_buffer, "Reg 02 %u",int_registers[1]);
    tft.println(test_buffer);
    sprintf(test_buffer, "Reg 03 %u",int_registers[2]);
    tft.println(test_buffer);
    sprintf(test_buffer, "Reg 04 %u",int_registers[3]);
    tft.println(test_buffer);
    sprintf(test_buffer, "Reg 05 %u",int_registers[4]);
    tft.println(test_buffer);
    sprintf(test_buffer, "Reg 06 %u",int_registers[5]);
    tft.println(test_buffer);
    sprintf(test_buffer, "Reg 07 %u",int_registers[6]);
    tft.println(test_buffer);


    sprintf(test_buffer, "Txt 01 %s",text_registers[0]);
    tft.println(test_buffer);
    sprintf(test_buffer, "Txt 02 %s",text_registers[1]);
    tft.println(test_buffer);

    delay(2000);
#else
    exec_cmd(cmd_to_exec, int_registers[0], int_registers[1], int_registers[2], int_registers[3],
             int_registers[4],
             int_registers[5], int_registers[6],
             text_registers[0], text_registers[1]);
#endif
}


// ======================================================================
// Setup board
// ======================================================================

void setup()
{
    Serial.begin(19200);

    while (!Serial); // Wait for serial port to connect. Needed for native USB port only

    tft.reset();
    tft.begin(tft.readID());
    tft.fillScreen(BLUE); // Show welcome message
    tft.setRotation(1);

#ifdef DEBUG_MODE_01
    return;
#else
  #ifdef DEBUG_STAN
    frameHeight = 53;
    frameWithd = 476;
    frameLeft = 3;

    titleHeigth = 35;
    titleWithd = 477;

    tft.fillScreen(BLACK);
    tft.setTextSize(3);

    tft.fillRect(frameLeft, 0, 477, 35, BLUE);
    tft.setCursor(5, 5);
    tft.print("Temperatura (Celsius)");

    tft.drawRect(frameLeft, 37, frameWithd, frameHeight, WHITE);
    tft.setCursor(12, 53);
    tft.print("Core 1 = 00C");
    tft.setCursor(252, 53);
    tft.print("Core 2 = 00C");

    tft.drawRect(frameLeft, 88, frameWithd, frameHeight, WHITE);
    tft.setCursor(12, 105);
    tft.print("Placa  = 00C");
    tft.setCursor(252, 105);
    tft.print("Chipset= 00C");

    tft.fillRect(3, 140, 477, 35, BLUE);
    tft.setCursor(5, 145);
    tft.print("Ventilacion (RPM)");

    tft.drawRect(frameLeft, 177, frameWithd, frameHeight, WHITE);
    tft.setCursor(12, 193);
    tft.print("CPU = 0.000");
    tft.setCursor(252, 193);
    tft.print("SIS = 0.000");

    tft.fillRect(3, 230, frameWithd, 35, BLUE);
    tft.setCursor(5, 236);
    tft.print("Carga Procesadores (%)");
    tft.drawRect(frameLeft, 265, frameWithd, frameHeight,WHITE);
    tft.setCursor(12, 281);
    tft.print("1=XXX 2=XXX 3=XXX 4=XXX");

    delay(5000);
    #endif
    
    tft.setCursor(10,10);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.print("Front Panel v1");
    tft.setTextSize(2);
    tft.setCursor(10,40);
    tft.print("GNU 2019 Ramon Vivas Navarro");
    tft.setCursor(10,60);
    tft.print("Serial port set:");
    tft.setCursor(10,80);
    tft.print("  9600 bps, 8 bits,No parity, 1 Stop");
    tft.setCursor(10,100);
    tft.print("Waiting for commands......\n\n\n");
#endif
}

// ======================================================================
// Loop
// ======================================================================

void loop()
{

    // =================================================================
    // Wait for incomming data
    // =================================================================

    if (Serial.available() > 0) {
        data_buffer[index_buffer] = Serial.read();

        if (data_buffer[index_buffer] == '\n') {
            data_buffer[index_buffer] = '\0';

            parseCmd(data_buffer);
            execCmd();

            //Serial.write('ready\n');

            index_buffer = 0;
        }
        else
            index_buffer++;

    }
}
