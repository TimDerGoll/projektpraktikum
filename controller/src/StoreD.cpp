#include "StoreD.h"

namespace storage {
    StoreD::StoreD() {
        //WICHTIG: "BUILTIN_SDCARD", da dieser SD-Leser 4-bit parallel liesst
        this->sd_available = SD.begin(BUILTIN_SDCARD);
        if (this->sd_available) {
            srl->println('D', "SD Karte gefunden.");
        } else {
            srl->println('D', "SD Karte nicht gefunden.");
        }
    }

    StoreD::~StoreD() {

    }

    void StoreD::setDate(char dateString[]){
        strcpy(this->dateString, dateString);

        this->setFileName();
    }
    void StoreD::setIntervall(int intervall){
        this->intervall = intervall;
    }
    void StoreD::setAmountMFC(int amount_MFC){
        this->amount_MFC = amount_MFC;
    }
    void StoreD::setAmountValve(int amount_valve){
        this->amount_valve = amount_valve;
    }

    void StoreD::setParseInputNewLineFunction(uint16_t (*parseInputNewLine) (char[])) {
        this->parseInputNewLine=parseInputNewLine;
    }

    void StoreD::setFileName() {
        //setze sechstelliges Datum
        this->fileName[0] = this->dateString[8];
        this->fileName[1] = this->dateString[9];
        this->fileName[2] = this->dateString[3];
        this->fileName[3] = this->dateString[4];
        this->fileName[4] = this->dateString[0];
        this->fileName[5] = this->dateString[1];
        this->fileName[6] = '\0';

        //Erzeuge Dateinummer
        char tmp_path[32];
        char num_buf[4];
        uint8_t number = 0;
        while(true) {
            sprintf(num_buf, "%02d", number);
            strcpy(tmp_path, "/results/");
            strcat(tmp_path, this->fileName);
            strcat(tmp_path, num_buf);
            strcat(tmp_path, ".txt");
            if (SD.exists(tmp_path)) {
                number++;
            } else {
                strcat(this->fileName, num_buf);
                strcat(this->fileName, ".txt");
                break;
            }
        }
    }

    uint8_t StoreD::listsource(char list[][16]) {
        if (this->sd_available) {
            File dir = SD.open("/programs/");
            uint8_t counter = 0;
            while (true) {
                File entry = dir.openNextFile();
                if (!entry) { // keine Dateien mehr
                    break;
                }
                if (!entry.isDirectory()) {
                    srl->println('D', entry.name());
                    strcpy(list[counter], entry.name());
                }
                entry.close();
                counter++;
            }
            dir.close();
            return counter;
        }
        return 0; //keine SD Karte gefunden
    }

    void StoreD::readFile(char name[]) {
        if (this->sd_available) {
            char filepath[32] = "/programs/";
            strcat(filepath, name);
            File file = SD.open(filepath, FILE_READ);
            while (file.available() > 0){
                uint16_t counter = 0;
                char line[SERIAL_READ_MAX_LINE_SIZE];
                while (true){
                    char newchar = file.read();
                    if (newchar == '\n') {
                        line[counter] = '\0';
                        break;
                    }
                    line[counter] = newchar;
                    counter++;
                }
                this->parseInputNewLine(line);
            }
            file.close();
        }
    }


    void StoreD::openFile(){
        if (this->sd_available) {
            char filepath[32] = "/results/";
            strcat(filepath, this->fileName);
            this->fileStream = SD.open(filepath, FILE_WRITE);

            srl->print('D', filepath);
            if (this->fileStream) {
                srl->println('D', " - Datei erstellt");
            } else {
                srl->println('D', " - Fehler beim Datei erstellen!");
                return;
            }

            //HEADER schreiben
            this->fileStream.print("Messung am:     ");   this->fileStream.println(this->dateString);
            this->fileStream.print("Messintervall:  ");   this->fileStream.println(this->intervall);
            this->fileStream.print("Anzahl MFCs:    ");   this->fileStream.println(this->amount_MFC);
            this->fileStream.print("Anzahl Ventile: ");   this->fileStream.println(this->amount_valve);
            this->fileStream.println("");

            this->fileStream.print("Bytesortierung: ");   this->fileStream.println("Time, MFC Soll, MFC Ist, Ventil Soll, Bosch");
            this->fileStream.print("Bytecode:       ");
            this->fileStream.print("4");
            for (uint8_t i = 0; i < this->amount_MFC; i++)
                this->fileStream.print("4");
            for (uint8_t i = 0; i < this->amount_MFC; i++)
                this->fileStream.print("4");
            for (uint8_t i = 0; i < this->amount_valve; i++)
                this->fileStream.print("1");
            this->fileStream.print("2");this->fileStream.print("2");this->fileStream.println("2");
        }
    }

    void StoreD::closeFile(){
        if (this->sd_available && this->fileStream) {
            this->fileStream.close();
        }
    }

    void StoreD::writeNewLine(char newLine[]){
        if (this->sd_available && this->fileStream) {
            this->fileStream.println(newLine);
        }
    }
}
