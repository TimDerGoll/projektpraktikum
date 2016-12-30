#include "main_StoreD.h"

namespace storage {
    Main_StoreD::Main_StoreD() {

    }
    Main_StoreD::~Main_StoreD() {

    }

    filenumber = 1;
    SD.begin();
    filename[10 + (filenumber % 10)] = "file" + filenumber + ".txt";

    //TODO: soll nur zu Beginn des Speicherns einmal ausgeführt werden -> in welche loop soll ich das packen?
    //falls neue Messung gestartet wird und Dateien von voriger Messung auf SD, starte bei chronologisch logischer filenumber
    if (filenumber != 1) {
      while (SD.exists(filename)) {
        filenumber++;
      }
    }

    bool Main_StoreD::loop() {
        //Gebe false zurueck um den Thread zu beenden. True bedeutet, dass der Thread weiter läuft
        if (kill_flag)
            return false;

        if (button) { //TODO: setzen von button durch Tastendruck
          //TODO: auf Display anzeigen, dass gespeichert wird
          if (!SD.exists(filename)) {
            myFile = SD.open(filename, FILE_WRITE);
          }
          if ((MAX_SD_FILE_SIZE - myFile.size() > SERIAL_READ_MAX_LINE_SIZE)) {
            myFile.println(/*hier Daten*/) ;
          }
          else {
            myFile.close();
            filenumber++;
            filename[] = "file" + filenumber + ".txt";
            myFile = SD.open(filename, FILE_WRITE);
            myFile.println(/*hier Daten*/);
          }
        }
        else {
          if (SD.exists(filename)) { //wenn gespeichert wurde und jetzt nicht mehr, soll die geöffnete Datei geschlossen werden
            myFile.close();
            filenumber++; //auch wenn MAX_SD_FILE_SIZE noch nicht erreicht, beginne neue Datei beim erneuten Starten
          }
          //else: keine laufende Messung && keine geöffnete Datei -> mache nichts
        }

        //Serial.println("loop called");
        return true;
    }
}
