#include "parseInput.h"

namespace communication {
    ParseInput::ParseInput() {
        this->_headerLineCounter = 0;
    }

    ParseInput::~ParseInput() {

    }

    void ParseInput::setMainMfcObjectPointer(control::Main_MfcCtrl *main_mfcCtrl) {
        this->main_mfcCtrl = main_mfcCtrl;
    }

    void ParseInput::setMainValveObjectPointer(control::Main_ValveCtrl *main_valveCtrl) {
        this->main_valveCtrl = main_valveCtrl;
    }

    void ParseInput::setMainBoschObjectPointer(communication::Main_BoschCom *main_boschCom) {
        this->main_boschCom = main_boschCom;
    }

    void ParseInput::setMainDisplayObjectPointer(io::Main_Display *main_display) {
        this->main_display = main_display;
    }

    void ParseInput::setMainStringBuilderObjectPointer(communication::Main_StringBuilder *main_stringBuilder) {
        this->main_stringBuilder = main_stringBuilder;
    }

    void ParseInput::set_startFunction(void (*startFunction) ()) {
        this->startFunction = startFunction;
    }

    uint8_t ParseInput::get_headerLineCounter() {
        return this->_headerLineCounter;
    }

    void ParseInput::set_headerLineCounter(uint8_t headerLineCounter) {
        this->_headerLineCounter = headerLineCounter;
    }

    uint16_t ParseInput::parseNewLine(char newLine[]) {
        //hier werden zerlegte Eingabestrings gespeichert
        char newLineArray[SERIAL_READ_MAX_BLOCK_AMOUNT][SERIAL_READ_MAX_BLOCK_SIZE];

        //zerteile String an ','
        uint8_t newLineArray_size = cmn::split(newLineArray, newLine, ',');
        //entferne Leerzeichen an Anfang und Ende
        for (uint8_t i = 0; i < newLineArray_size; i++)
            cmn::trim(newLineArray[i]);

        //Sonderfall: Softwarereset
        if (strcmp(newLineArray[0], "reset") == 0) {
            digitalWrite(PIN_SOFTWARE_RESET, HIGH);
            return 0;
        }

        //verarbeite Input
        switch (this->_headerLineCounter) {
            case 0: //ZEILE 0: MFC+Ventilanzahl
                if (newLineArray_size != 2) {
                    srl->println('D', "Zeile 0: Falsche Anzahl an Eintraegen.");
                    srl->println('L', ERR_SERIAL_COMMANDS_WRONG_AMOUNT);
                    return ERR_SERIAL_COMMANDS_WRONG_AMOUNT;
                }
                this->amount_MFC   = atoi(newLineArray[0]);
                this->amount_valve = atoi(newLineArray[1]);

                //erstelle MFC-Objekte in der main_mfcCtrl
                this->main_mfcCtrl->createMFC(this->amount_MFC);

                //erstelle Ventil-Objekte in der main_valveCtrl
                this->main_valveCtrl->createValve(this->amount_valve);

                //sage Display, dass Uebertragung gestartet wurde
                this->main_display->header_started(this->amount_MFC, this->amount_valve);

                this->_headerLineCounter = 1;
                srl->println('L', "ok"); //Sende 'Befehl ok' an LabView
                break;
            case 1: //ZEILE 1: MFC-Adressen
                if (this->amount_MFC > 0 && newLineArray_size != this->amount_MFC) {
                    srl->println('D', "Zeile 1: Anzahl der gegebenen Adressen stimmt nicht mit Anzahl der MFCs ueberein.");
                    srl->println('L', ERR_SERIAL_COMMANDS_WRONG_AMOUNT);
                    return ERR_SERIAL_COMMANDS_WRONG_AMOUNT;
                }
                this->main_mfcCtrl->setAddresses(newLineArray);

                this->_headerLineCounter = 2;
                srl->println('L', "ok"); //Sende 'Befehl ok' an LabView
                break;
            case 2: //ZEILE 2: MFC-Typen
                if (this->amount_MFC > 0 && newLineArray_size != this->amount_MFC) {
                    srl->println('D', "Zeile 2: Anzahl der gegebenen Typen stimmt nicht mit Anzahl der MFCs ueberein.");
                    srl->println('L', ERR_SERIAL_COMMANDS_WRONG_AMOUNT);
                    return ERR_SERIAL_COMMANDS_WRONG_AMOUNT;
                }
                this->main_mfcCtrl->setTypes(newLineArray);
                this->main_stringBuilder->setTypes(newLineArray);

                this->_headerLineCounter = 3;
                srl->println('L', "ok"); //Sende 'Befehl ok' an LabView
                break;
            case 3: //ZEILE 3: Ventil PCB Adressen
                if (newLineArray_size == 0 || newLineArray_size > MAX_AMOUNT_VALVE_PCB) {
                    srl->println('D', "Zeile 3: Anzahl der Adressen ist nicht korrekt.");
                    srl->println('L', ERR_SERIAL_COMMANDS_WRONG_AMOUNT);
                    return ERR_SERIAL_COMMANDS_WRONG_AMOUNT;
                }
                this->main_valveCtrl->setPcbAddresses(newLineArray_size, newLineArray);

                this->_headerLineCounter = 4;
                srl->println('L', "ok"); //Sende 'Befehl ok' an LabView
                break;
            case 4: //ZEILE 4: Ventil-Pins
                if (this->amount_valve > 0 && newLineArray_size != this->amount_valve) {
                    srl->println('D', "Zeile 4: Anzahl der gegebenen Pin-Nummern stimmt nicht mit Anzahl der Ventile ueberein.");
                    srl->println('L', ERR_SERIAL_COMMANDS_WRONG_AMOUNT);
                    return ERR_SERIAL_COMMANDS_WRONG_AMOUNT;
                }
                this->main_valveCtrl->setPins(newLineArray);

                this->_headerLineCounter = 5;
                srl->println('L', "ok"); //Sende 'Befehl ok' an LabView
                break;
            case 5: //ZEILE 5: Messaufloesung wird gesetzt
                if (newLineArray_size != 1) {
                    srl->println('D', "Zeile 5: Intervallgroesse besteht aus einem Wert.");
                    srl->println('L', ERR_SERIAL_COMMANDS_WRONG_AMOUNT);
                    return ERR_SERIAL_COMMANDS_WRONG_AMOUNT;
                }
                //StringBuilder, sowie BoschCom arbeiten mit dem selben Intervall
                //Ersterer ist jedoch um eine halbe Periode in der Zeit verschoben
                this->main_boschCom->setIntervall(atoi(newLineArray[0]));
                this->main_stringBuilder->setIntervall(atoi(newLineArray[0]));

                this->_headerLineCounter = 6;
                srl->println('L', "ok"); //Sende 'Befehl ok' an LabView
                break;
            case 6: //ZEILE 6: DateString wird gesetzt
                if (newLineArray_size != 1) {
                    srl->println('D', "Zeile 6: Datum besteht aus einem Wert.");
                    srl->println('L', ERR_SERIAL_COMMANDS_WRONG_AMOUNT);
                    return ERR_SERIAL_COMMANDS_WRONG_AMOUNT;
                }
                this->main_stringBuilder->setDateString(newLineArray[0]);

                this->_headerLineCounter = 7;
                srl->println('L', "ok"); //Sende 'Befehl ok' an LabView
                break;
            case 7: //ZEILE 7: Letzte Zeile, hier wird ein 'begin' erwartet
                if (strcmp(newLineArray[0], "begin") == 0) {
                    srl->println('D', "Header vollstaendig.");

                    //Sage Display, dass Header vollstaendig und Events beginnen
                    this->main_display->loading_data();

                    srl->println('L', "ok"); //Sende 'Befehl ok' an LabView
                    this->_headerLineCounter = 8;
                } else {
                    srl->println('D', "Zeile 7: Es wird ein 'begin' erwartet");
                    srl->println('L', ERR_SERIAL_COMMANDS_WRONG_AMOUNT);
                    return ERR_SERIAL_COMMANDS_WRONG_AMOUNT;
                }
                break;
            case 8: //ZEILE 8: Eventliste
                if (newLineArray_size != 4 && strcmp(newLineArray[0], "end") != 0) {
                    srl->println('D', "Zeile 8: Events bestehen aus 4 Elementen, ansonsten wird ein 'end' erwartet.");
                    srl->println('L', ERR_SERIAL_COMMANDS_WRONG_AMOUNT);
                    return ERR_SERIAL_COMMANDS_WRONG_AMOUNT;
                }
                if (strcmp(newLineArray[0], "M") == 0) { //MFC
                    if (atoi(newLineArray[1]) < this->amount_MFC) {
                        this->main_mfcCtrl->setEvent(
                            atoi(newLineArray[1]), //MFC-ID
                            cmn::charArrayToFloat(newLineArray[2]), //value
                            strtoul(newLineArray[3], NULL, 0) //time (uint32_t)
                        );
                    } else {
                        this->main_display->throwError(ERR_SERIAL_UNDEFINED_INDEX);
                        srl->println('L', ERR_SERIAL_UNDEFINED_INDEX); //Sende Errorcode an LabView
                    }
                } else if (strcmp(newLineArray[0], "V") == 0) { //Ventil
                    if (atoi(newLineArray[1]) < this->amount_valve) {
                        this->main_valveCtrl->setEvent(
                            atoi(newLineArray[1]), //Ventil-ID
                            atoi(newLineArray[2]), //value
                            strtoul(newLineArray[3], NULL, 0) //time (uint32_t)
                        );
                    } else {
                        this->main_display->throwError(ERR_SERIAL_UNDEFINED_INDEX);
                        srl->println('L', ERR_SERIAL_UNDEFINED_INDEX); //Sende Errorcode an LabView
                    }
                }

                else if (strcmp(newLineArray[0], "end") == 0) { //Am ende wechselt labCom in den Sende-Modus
                    srl->println('D', "Uebertragung abgeschlossen.");

                    //Sage Display, dass Event-Uebertragung abgeschlossen ist
                    this->main_display->loading_finished();

                    this->_headerLineCounter = 9;
                }
                srl->println('L', "ok"); //Sende 'Befehl ok' an LabView
                break;
            case 9: //ZEILE 9: Warte auf Start (kann auch durch Button aufgerufen werden)
                if (newLineArray_size != 1) {
                    srl->println('D', "Zeile 9: Es wird ein 'start' erwartet.");
                    srl->println('L', ERR_SERIAL_COMMANDS_WRONG_AMOUNT);
                    return ERR_SERIAL_COMMANDS_WRONG_AMOUNT;
                }
                if (strcmp(newLineArray[0], "start") == 0) {
                    this->startFunction(); //Teile LabCom mit, dass die Messung zu starten ist
                    srl->println('L', "ok"); //Sende 'Befehl ok' an LabView
                    return 0;
                }
                break;
        }
        return 0;
    }
}
