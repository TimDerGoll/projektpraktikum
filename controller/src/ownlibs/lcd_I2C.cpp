// LiquidCrystal_I2C V2.0

#include "lcd_I2C.h"

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set:
//    DL = 1; 8-bit interface data
//    N = 0; 1-line display
//    F = 0; 5x8 dot character font
// 3. Display on/off control:
//    D = 0; Display off
//    C = 0; Cursor off
//    B = 0; Blinking off
// 4. Entry mode set:
//    I/D = 1; Increment by 1
//    S = 0; No shift
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

LiquidCrystal_I2C::LiquidCrystal_I2C(uint8_t lcd_Addr, uint8_t i_red_pin, uint8_t i_green_pin, uint8_t i_blue_pin, uint8_t lcd_cols, uint8_t lcd_rows) {
    _Addr = lcd_Addr;
    _cols = lcd_cols;
    _rows = lcd_rows;
    _backlightval = LCD_NOBACKLIGHT;

    _red_pin = i_red_pin;
    _green_pin = i_green_pin;
    _blue_pin = i_blue_pin;

    _brightness = 255;
    _backlight = true;

    _r = 255;
    _g = 255;
    _b = 255;

    pinMode(_red_pin, OUTPUT);
    pinMode(_green_pin, OUTPUT);
    pinMode(_blue_pin, OUTPUT);

    strcpy(last_dm0, "                    ");
    strcpy(last_dm1, "                    ");
    strcpy(last_dm2, "                    ");
    strcpy(last_dm3, "                    ");
}

void LiquidCrystal_I2C::init(){
	init_priv();
}

void LiquidCrystal_I2C::init_priv()
{
	Wire.begin();
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	begin(_cols, _rows);
}

void LiquidCrystal_I2C::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delayMicroseconds(50000);

	// Now we pull both RS and R/W low to begin commands
	expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	delay(1000);

  	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46

	// we start in 8bit mode, try to set 4 bit mode
	write4bits(0x03);
	delayMicroseconds(4500); // wait min 4.1ms

	// second try
	write4bits(0x03);
	delayMicroseconds(4500); // wait min 4.1ms

	// third go!
	write4bits(0x03);
	delayMicroseconds(150);

	// finally, set to 4-bit interface
	write4bits(0x02);


	// set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);

	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();

	// clear it off
	clear();

	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);

	home();

}

// Ergaenzungen

void LiquidCrystal_I2C::backlight_brightness(uint8_t brightness) {
	this->_brightness = brightness;
}

void LiquidCrystal_I2C::backlight_off() {
	this->_backlight = false;
	this->backlight_drawColor(0, 0, 0);
}

void LiquidCrystal_I2C::backlight_on() {
	this->_backlight = true;
	this->backlight_drawColor(this->_r, this->_g, this->_b);
}

void LiquidCrystal_I2C::backlight_setColor(uint8_t r, uint8_t g, uint8_t b) {
    this->_r = r;
    this->_g = g;
    this->_b = b;

	if (!this->_backlight)
        this->backlight_drawColor(0, 0, 0);
    else
        this->backlight_drawColor(this->_r, this->_g, this->_b);
}

void LiquidCrystal_I2C::backlight_drawColor(uint8_t r, uint8_t g, uint8_t b) {
    //mische Farben zueinander ab
    r = map(r, 0, 255, 0, 255);
    g = map(g, 0, 255, 0, 110);
    b = map(b, 0, 255, 0, 115);

    r = map(r, 0, 255, 0, _brightness);
    g = map(g, 0, 255, 0, _brightness);
    b = map(b, 0, 255, 0, _brightness);

    //Input ist Anode --> Wert invertieren
    r = map(r, 0, 255, 255, 0);
    g = map(g, 0, 255, 255, 0);
    b = map(b, 0, 255, 255, 0);

    analogWrite(_red_pin, r);
    analogWrite(_green_pin, g);
    analogWrite(_blue_pin, b);
}

/*
 * Neue Methode zum Update der gesamten Bildmatrix. Diese vergleicht ausserdem den neuen Text mit
 * dem Alten, um nur die Zeichen zu uebertragen, die sich geaendert haben.
 */
void LiquidCrystal_I2C::updateDisplayMatrix(const char dm0[21], const char dm1[21], const char dm2[21], const char dm3[21]) {
    changeSingleChars(dm0, last_dm0, 0);
    changeSingleChars(dm1, last_dm1, 1);
    changeSingleChars(dm2, last_dm2, 2);
    changeSingleChars(dm3, last_dm3, 3);
}

void LiquidCrystal_I2C::setSymbol(uint8_t id, uint8_t x, uint8_t y) {
    setCursor(x,y);
    write(byte(id));
    if (y == 0)
        last_dm0[x] = '~'; //Platzhalter fuer changeSingleChars
    if (y == 1)
        last_dm1[x] = '~'; //Platzhalter fuer changeSingleChars
    if (y == 2)
        last_dm2[x] = '~'; //Platzhalter fuer changeSingleChars
    if (y == 3)
        last_dm3[x] = '~'; //Platzhalter fuer changeSingleChars
}

//privat
void LiquidCrystal_I2C::changeSingleChars(const char new_dm[21], char last_dm[21], int line) {
    bool end_reached = false;
    if (strcmp(new_dm, last_dm) != 0) {
        for (int i = 0; i < 21; i++) {
            if (new_dm[i] == '\0')
                end_reached = true;

            if (end_reached) {
                if (last_dm[i] == '\0')
                    break;

                if (last_dm[i] != ' ') {
                    setCursor(i, line);
                    print(' ');
                }
            } else {
                if (new_dm[i] != last_dm[i]) {
                    setCursor(i, line);
                    print(new_dm[i]);
                }
            }

        }
        strcpy(last_dm, new_dm);

        //garantiere, dass leerzeichen im last_dm sind, sodass alte Zeichen ueberschrieben werden
        int i;
        bool end_of_source = false;
        for (i = 0; i < 21; i++) {
            if (last_dm[i] == '\0')
                end_of_source = true;

            if (end_of_source) { //fuelle mit Leerzeichen bis zum Ende
                last_dm[i] = ' ';
                last_dm[i+1] = '\0';
            }
        }
    }
}

// Ergaenzungen Ende



/********** high level commands, for the user! */
void LiquidCrystal_I2C::clear(){
	command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal_I2C::home(){
	command(LCD_RETURNHOME);  // set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal_I2C::setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > _numlines ) {
		row = _numlines-1;    // we count rows starting w/0
	}
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystal_I2C::noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_I2C::display() {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal_I2C::noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_I2C::cursor() {
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidCrystal_I2C::noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_I2C::blink() {
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal_I2C::scrollDisplayLeft(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidCrystal_I2C::scrollDisplayRight(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal_I2C::leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystal_I2C::rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal_I2C::autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal_I2C::noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystal_I2C::createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		write(charmap[i]);
	}
}




/*********** mid level commands, for sending data/cmds */

inline void LiquidCrystal_I2C::command(uint8_t value) {
	send(value, 0);
}

inline size_t LiquidCrystal_I2C::write(uint8_t value) {
	send(value, Rs);
	return 1;
}



/************ low level data pushing commands **********/

// write either command or data
void LiquidCrystal_I2C::send(uint8_t value, uint8_t mode) {
	uint8_t highnib=value>>4;
	uint8_t lownib=value & 0x0F;
	write4bits((highnib)|mode);
	write4bits((lownib)|mode);
}

void LiquidCrystal_I2C::write4bits(uint8_t value) {
	expanderWrite(value);
	pulseEnable(value);
}

void LiquidCrystal_I2C::expanderWrite(uint8_t _data){
	Wire.beginTransmission(_Addr);
	Wire.write((int)(_data) | _backlightval);
	Wire.endTransmission();
}

void LiquidCrystal_I2C::pulseEnable(uint8_t _data){
	expanderWrite(_data | En);	// En high
	delayMicroseconds(1);		// enable pulse must be >450ns

	expanderWrite(_data & ~En);	// En low
	delayMicroseconds(50);		// commands need > 37us to settle
}


// Alias functions

void LiquidCrystal_I2C::cursor_on(){
	cursor();
}

void LiquidCrystal_I2C::cursor_off(){
	noCursor();
}

void LiquidCrystal_I2C::blink_on(){
	blink();
}

void LiquidCrystal_I2C::blink_off(){
	noBlink();
}

void LiquidCrystal_I2C::load_custom_character(uint8_t char_num, uint8_t *rows){
		createChar(char_num, rows);
}

void LiquidCrystal_I2C::printstr(const char c[]){
	//This function is not identical to the function used for "real" I2C displays
	//it's here so the user sketch doesn't have to be changed
	print(c);
}


// unsupported API functions
void LiquidCrystal_I2C::off(){}
void LiquidCrystal_I2C::on(){}
void LiquidCrystal_I2C::setDelay (int cmdDelay,int charDelay) {}
uint8_t LiquidCrystal_I2C::status(){return 0;}
uint8_t LiquidCrystal_I2C::keypad (){return 0;}
uint8_t LiquidCrystal_I2C::init_bargraph(uint8_t graphtype){return 0;}
void LiquidCrystal_I2C::draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end){}
void LiquidCrystal_I2C::draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_row_end){}
void LiquidCrystal_I2C::setContrast(uint8_t new_val){}
