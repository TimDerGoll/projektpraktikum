#ifndef MAIN_DISPLAY_H
#define MAIN_DISPLAY_H

#include <Arduino.h>
#include <newdel.h> //fügt new und delete hinzu, wird für "mthread" benötigt
#include <mthread.h>

#include "ownlibs/serialCommunication.h"
#include "ownlibs/common.h"
#include "config.h"
#include "errors.h"

namespace io {
    class Main_Display : public Thread {
    public:
        //Defaultconstructor
        Main_Display();
        //Destructor
        ~Main_Display();
        //verarbeite Error Nummer und gebe Displayausgabe
        void throwError(int errorNumber);
        //zeige auf dem Display an, dass Uebertragung gestartet werden kann
        void boardIsReady();
        //Zeige an, dass Header-Uebertragung gestartet wurde
        void header_started(int amountMFC, int amountValve);
        //Zeige an, dass Header vollstanedig + Event-Uebertragung gestartet
        void event_started();
        //Zeige an, dass Event-Uebertragung dertig und Mess-Start erwartet wird
        void event_finished();
        //Messung gestartet, beginne Live-Ausgabe
        void start(unsigned long startTime);
        //setze letzt ausgefuehrtes Event zur Displayausgabe
        void setLastEvent(char type, int id, int value, unsigned int time);
    protected:
        //Die Loop wird kontinuierlich aufgerufen und vollstaendig ausgefuehrt
        bool loop();
    private:
        void Main_Display::setLastEvent_type (char type);
        void Main_Display::setLastEvent_id (int id);
        void Main_Display::setLastEvent_value (int value);
        void Main_Display::setLastEvent_time (unsigned int time);

        unsigned long afterErrorTime;
        unsigned long lastPrint;
        unsigned long startTime;
        int amountMFC;
        int amountValve;
        bool ready;

        //lastEvent-Variablen
        char lastEvent_type;
        int lastEvent_id;
        int lastEvent_value;
        unsigned long lastEvent_time;
    };
}

#endif
