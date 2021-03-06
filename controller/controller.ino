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
#include "src/parseInput.h"

#include "src/ownlibs/serialCommunication.h"
#include "src/ownlibs/inputHandler.h"

//OBJEKTVARIABLEN ERSTELLEN
io::Main_Display *main_display;
io::InputHandler *inputHandler;
communication::Main_LabCom *main_labCom;
communication::Main_BoschCom *main_boschCom;
communication::Main_StringBuilder *main_stringBuilder;
communication::ParseInput *parseInput;
control::Main_MfcCtrl *main_mfcCtrl;
control::Main_ValveCtrl *main_valveCtrl;
storage::StoreD *storeD;

void debug(uint8_t value) {
  //set debug-status to srl
  srl->activateDebug(value);
  digitalWrite(PIN_DEBUGLED, value);
}

void ok_start(uint8_t value) {
  //die beiden Funktionen sind niemals zur gleichen Zeit "gueltig",
  //das ueberpruefen sie aber selber beim Aufruf.
  main_labCom->start();
  main_display->menu_controlMenu();
}

void button_up(uint8_t value) {
  main_display->menu_navigateMenu(0);
}

void button_down(uint8_t value) {
  main_display->menu_navigateMenu(1);
}

uint16_t parseNewLine_withoutClass(char newLine[]) {
  return parseInput->parseNewLine(newLine);
}

void readFile_withoutClass(char name[]) {
  storeD->readFile(name);
}

void start_withoutClass() {
  main_labCom->start();
}

void setLaodingProgress_withoutClass(int8_t loadingProgress) {
  main_display->setLaodingProgress(loadingProgress);
}

void setup() {
  // ERSTELLE SERIELLE VERBINDUNGEN
  srl->setSerial(&Serial1, &Serial3, &Serial2, &Serial4); //labview / debug / mks / buerkert

  // ERSTELLE GEBRAUCHTE OBJEKTE
  main_display       = new io::Main_Display();
  inputHandler       = new io::InputHandler();
  main_labCom        = new communication::Main_LabCom();
  main_boschCom      = new communication::Main_BoschCom();
  main_stringBuilder = new communication::Main_StringBuilder();
  parseInput         = new communication::ParseInput();
  main_mfcCtrl       = new control::Main_MfcCtrl();
  main_valveCtrl     = new control::Main_ValveCtrl();
  storeD             = new storage::StoreD(); //stringBuilder, this

  // TAUSCHE OBJEKTPOINTER ZWISCHEN OBJEKTEN AUS
  main_labCom->setMainMfcObjectPointer(main_mfcCtrl);
  main_labCom->setMainValveObjectPointer(main_valveCtrl);
  main_labCom->setMainBoschObjectPointer(main_boschCom);
  main_labCom->setMainDisplayObjectPointer(main_display);
  main_labCom->setMainStringBuilderObjectPointer(main_stringBuilder);
  main_labCom->setParseInputObjectPointer(parseInput);

  parseInput->setMainMfcObjectPointer(main_mfcCtrl);
  parseInput->setMainValveObjectPointer(main_valveCtrl);
  parseInput->setMainBoschObjectPointer(main_boschCom);
  parseInput->setMainDisplayObjectPointer(main_display);
  parseInput->setMainStringBuilderObjectPointer(main_stringBuilder);

  main_mfcCtrl->setMainDisplayObjectPointer(main_display);

  main_valveCtrl->setMainDisplayObjectPointer(main_display);

  main_stringBuilder->setMainValveObjectPointer(main_valveCtrl);
  main_stringBuilder->setMainMfcObjectPointer(main_mfcCtrl);
  main_stringBuilder->setMainBoschObjectPointer(main_boschCom);
  main_stringBuilder->setStoreDObjectPointer(storeD);

  storeD->setParseInputNewLineFunction(parseNewLine_withoutClass);
  storeD->setLoadingProgressFunction(setLaodingProgress_withoutClass);
  main_display->setReadFileFunction(readFile_withoutClass);
  parseInput->set_startFunction(start_withoutClass);

  // GENERIERE PROGRAMMLISTE
  char programs[32][16];
  int amount = storeD->listsource(programs);
  cmn::sort(programs, amount);
  main_display->menu_setMenuItems(programs, amount);

  // STARTE PSEUDOTHREADS
  main_thread_list -> add_thread(main_display);
  main_thread_list -> add_thread(inputHandler);
  main_thread_list -> add_thread(main_labCom);
  main_thread_list -> add_thread(main_boschCom);
  main_thread_list -> add_thread(main_stringBuilder);
  main_thread_list -> add_thread(main_mfcCtrl);
  main_thread_list -> add_thread(main_valveCtrl);

  // INIT LEDS
  pinMode(PIN_DEBUGLED, OUTPUT);
  pinMode(PIN_FINISHLED, OUTPUT);

  // ERSTELLE INTERRUPTS FUER TASTER
  pinMode(PIN_DEBUGSWITCH, INPUT_PULLDOWN);
  pinMode(PIN_OKBUTTON, INPUT_PULLDOWN);
  pinMode(PIN_UPBUTTON, INPUT_PULLDOWN);
  pinMode(PIN_DOWNBUTTON, INPUT_PULLDOWN);
  inputHandler->addInterrupt(PIN_DEBUGSWITCH, debug, CHANGE);
  inputHandler->addInterrupt(PIN_OKBUTTON, ok_start, RISING);
  inputHandler->addInterrupt(PIN_UPBUTTON, button_up, RISING, 350, 75);
  inputHandler->addInterrupt(PIN_DOWNBUTTON, button_down, RISING, 350, 75);

  // INIT SWITCH
  srl->activateDebug(digitalRead(PIN_DEBUGSWITCH));
  digitalWrite(PIN_DEBUGLED, digitalRead(PIN_DEBUGSWITCH));

  // SOFTWARE RESET INIT //TODO
  pinMode(PIN_SOFTWARE_RESET, OUTPUT);

  // INIT ANGESCHOSSEN
  main_display->foundSDcard(storeD->foundSDcard()); //setze SD-Card Flag
  main_display->boardIsReady(); //zeige "Board bereit" an
  srl->println('L', "ready"); //Sende Startbefehl an LabView
}
