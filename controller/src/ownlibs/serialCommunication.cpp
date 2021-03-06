#include "serialCommunication.h"

SerialCommunication::SerialCommunication() {
    this->debug_activated = false;
}
SerialCommunication::~SerialCommunication() {

}

void SerialCommunication::setSerial(HardwareSerial *serial_labView, HardwareSerial *serial_debug, HardwareSerial *serial_mks, HardwareSerial *serial_buerkert) {
    this->serial_labView  = serial_labView;
    this->serial_debug    = serial_debug;
    this->serial_mks      = serial_mks;
    this->serial_buerkert = serial_buerkert;

    this->serial_labView->begin(SERIAL_LABVIEW_BAUDRATE);
    this->serial_debug->begin(SERIAL_DEBUG_BAUDRATE);
    this->serial_mks->begin(SERIAL_MKS_BAUDRATE);
    this->serial_buerkert->begin(SERIAL_BUERKERT_BAUDRATE);
}

void SerialCommunication::activateDebug(bool activate) {
    this->debug_activated = activate;
}

HardwareSerial *SerialCommunication::getType(const char type) {
    HardwareSerial *this_serial;

    if (type == 'L') {
        this_serial = this->serial_labView;
    } else if (type == 'D') {
        this_serial = this->serial_debug;
    } else if (type == 'M') {
        this_serial = this->serial_mks;
    } else if (type == 'B') {
        this_serial = this->serial_buerkert;
    } else {
        this_serial = this->serial_debug;
        this->serial_debug->println("ERROR - Ungueltigen Typ gewaehlt");
    }

    return this_serial;
}

int SerialCommunication::available(const char type) {
    return this->getType(type)->available();
}

int SerialCommunication::read(const char type) {
    return this->getType(type)->read();
}

void SerialCommunication::flush(const char type) {
    this->getType(type)->flush();
}



void SerialCommunication::print(const char type, String &input) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->print(input);
}

void SerialCommunication::print(const char type, const char input[]) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->print(input);
}

void SerialCommunication::print(const char type, const char input) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->print(input);
}

void SerialCommunication::print(const char type, unsigned char input) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->print(input);
}

void SerialCommunication::print(const char type, int input) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->print(input);
}

void SerialCommunication::print(const char type, unsigned int input) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->print(input);
}

void SerialCommunication::print(const char type, long input) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->print(input);
}

void SerialCommunication::print(const char type, unsigned long input) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->print(input);
}

void SerialCommunication::print(const char type, double input) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->print(input);
}




void SerialCommunication::println(const char type, String &input) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->println(input);
}

void SerialCommunication::println(const char type, const char input[]) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->println(input);
}

void SerialCommunication::println(const char type, const char input) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->println(input);
}

void SerialCommunication::println(const char type, unsigned char input) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->println(input);
}

void SerialCommunication::println(const char type, int input) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->println(input);
}

void SerialCommunication::println(const char type, unsigned int input) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->println(input);
}

void SerialCommunication::println(const char type, long input) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->println(input);
}

void SerialCommunication::println(const char type, unsigned long input) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->println(input);
}

void SerialCommunication::println(const char type, double input) {
    if (type != 'D' || ( type == 'D' && this->debug_activated ))
        this->getType(type)->println(input);
}

// Initialisiere die Serielle Kommunikation
SerialCommunication *srl = new SerialCommunication();
