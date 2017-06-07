#ifndef COMMON_H
#define COMMON_H

#include "Arduino.h"

namespace cmn {
    //Entfernt Leerzeichen am Anfang und Ende des Strings
    void trim (char string[]);
    //Gibt eine gegebene Zeit (millisekunden) als DD:HH:MM:SS char[] zurueck
    void getTimeString(uint32_t time, char timeString_out[]);
    //teilt gegeben String an bestimmten Zeichen, speichert diesen in Array
    uint8_t split(char destination[][16], char source[], char symbol);
    //bekommt ein x <= 256^n und gibt n Byte großen String zurueck
    void integerToByte(unsigned long value, int bytesize, char output[]);
};

#endif
