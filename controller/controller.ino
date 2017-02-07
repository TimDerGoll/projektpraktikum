// Dies ist die Hauptdatei unseres Prpgramms. Der Name muss identisch zum Programmverzeichnis sein.
// Hier werden alle Bibliotheken importiert, Threads gestartet und verwaltet

// INCLUDES
#include <newdel.h> //fügt new und delete hinzu, wird für "mthread" benötigt
#include <mthread.h>
#include <QueueList.h>
#include <Wire.h>



//Klassen aus Includes mit einem "main" im Namen werden in dieser Datei erstellt
//und haben einen Thread
#include "src/main_labCom.h"
#include "src/main_mfcCtrl.h"
#include "src/main_valveCtrl.h"
#include "src/main_boschCom.h"
#include "src/main_display.h"
#include "src/main_stringBuilder.h"
#include "src/StoreD.h"
#include "src/mfcCtrl.h"
#include "src/valveCtrl.h"

#include "src/ownlibs/serialCommunication.h"

void setup() {
    // ERSTELLE SERIELLE VERBINDUNGEN
    srl->setSerial(&Serial1, &Serial, &Serial2); //labview / debug / uart

    // ERSTELLE GEBRAUCHTE OBJEKTE
    io::Main_Display *main_display                        = new io::Main_Display();
    communication::Main_LabCom *main_labCom               = new communication::Main_LabCom();
    communication::Main_BoschCom *main_boschCom           = new communication::Main_BoschCom();
    communication::Main_StringBuilder *main_stringBuilder = new communication::Main_StringBuilder();
    control::Main_MfcCtrl *main_mfcCtrl                   = new control::Main_MfcCtrl();
    control::Main_ValveCtrl *main_valveCtrl               = new control::Main_ValveCtrl();

    // TAUSCHE OBJEKTPOINTER ZWISCHEN OBJEKTEN AUS
    main_labCom->setMainMfcObjectPointer(main_mfcCtrl);
    main_labCom->setMainValveObjectPointer(main_valveCtrl);
    main_labCom->setMainBoschObjectPointer(main_boschCom);
    main_labCom->setMainDisplayObjectPointer(main_display);
    main_labCom->setMainStringBuilderObjectPointer(main_stringBuilder);
    
    main_mfcCtrl->setMainDisplayObjectPointer(main_display);
    
    main_valveCtrl->setMainDisplayObjectPointer(main_display);
    
    main_stringBuilder->setMainValveObjectPointer(main_valveCtrl);
    main_stringBuilder->setMainMfcObjectPointer(main_mfcCtrl);
    main_stringBuilder->setMainBoschObjectPointer(main_boschCom);

    // STARTE PSEUDOTHREADS
    main_thread_list -> add_thread(main_display);
    main_thread_list -> add_thread(main_labCom);
    main_thread_list -> add_thread(main_boschCom);
    main_thread_list -> add_thread(main_stringBuilder);
    main_thread_list -> add_thread(main_mfcCtrl);
    main_thread_list -> add_thread(main_valveCtrl);

    // ERSTELLE INTERRUPTS FUER TASTER

    //Taster start
    //Schalter Debug
}
