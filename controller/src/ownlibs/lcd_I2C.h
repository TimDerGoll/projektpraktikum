// LiquidCrystal_I2C V2.0

#ifndef LiquidCrystal_I2C_h
#define LiquidCrystal_I2C_h

#include <inttypes.h>
#include "Print.h"
#include "Wire.h"
#include "Arduino.h"

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x00
#define LCD_NOBACKLIGHT 0x80

#define En B00010000  // Enable bit
#define Rw B00100000  // Read/Write bit
#define Rs B01000000  // Register select bit

class LiquidCrystal_I2C : public Print {
public:
    LiquidCrystal_I2C(uint8_t lcd_Addr, uint8_t i_red_pin, uint8_t i_green_pin, uint8_t i_blue_pin, uint8_t lcd_cols, uint8_t lcd_rows);
    void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS );
    void clear();
    void home();
    void noDisplay();
    void display();
    void noBlink();
    void blink();
    void noCursor();
    void cursor();
    void scrollDisplayLeft();
    void scrollDisplayRight();
    void printLeft();
    void printRight();
    void leftToRight();
    void rightToLeft();
    void shiftIncrement();
    void shiftDecrement();
    void autoscroll();
    void noAutoscroll();
    void createChar(uint8_t, uint8_t[]);
    void setCursor(uint8_t, uint8_t);
    virtual size_t write(uint8_t);
    void command(uint8_t);
    void init();

    // Ergaenzungen

	void backlight_brightness(uint8_t brightness);
	void backlight_off();
	void backlight_on();
	void backlight_setColor(uint8_t r, uint8_t g, uint8_t b);
    void updateDisplayMatrix(const char dm0[21], const char dm1[21], const char dm2[21], const char dm3[21]);
    void setSymbol(uint8_t id, uint8_t x, uint8_t y);

	// Ergaenzungen Ende

    ////compatibility API function aliases
    void blink_on();						// alias for blink()
    void blink_off();       					// alias for noBlink()
    void cursor_on();      	 					// alias for cursor()
    void cursor_off();      					// alias for noCursor()
    void load_custom_character(uint8_t char_num, uint8_t *rows);	// alias for createChar()
    void printstr(const char[]);

    ////Unsupported API functions (not implemented in this library)
    uint8_t status();
    void setContrast(uint8_t new_val);
    uint8_t keypad();
    void setDelay(int,int);
    void on();
    void off();
    uint8_t init_bargraph(uint8_t graphtype);
    void draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end);
    void draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end);


private:
    void init_priv();
    void send(uint8_t, uint8_t);
    void write4bits(uint8_t);
    void expanderWrite(uint8_t);
    void pulseEnable(uint8_t);

    void changeSingleChars(const char new_dm[21], char last_dm[21], int line);

    //Ergaenzungen
    void backlight_drawColor(uint8_t r, uint8_t g, uint8_t b);
    //Ergaenzungen Ende

    uint8_t _Addr;
    uint8_t _displayfunction;
    uint8_t _displaycontrol;
    uint8_t _displaymode;
    uint8_t _numlines;
    uint8_t _cols;
    uint8_t _rows;
    uint8_t _backlightval;

    //Ergaenzungen
    uint8_t _red_pin;
    uint8_t _green_pin;
    uint8_t _blue_pin;
    uint8_t _brightness;
    uint8_t _backlight;
    uint8_t _r;
    uint8_t _g;
    uint8_t _b;

    char last_dm0[21];
    char last_dm1[21];
    char last_dm2[21];
    char last_dm3[21];
    //Ergaenzungen Ende
};

#endif
