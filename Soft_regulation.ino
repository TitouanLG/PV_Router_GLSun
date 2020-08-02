// **********************************************************************************
// Arduino Teleinfo sample, display information on teleinfo values received
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// http://creativecommons.org/licenses/by-sa/4.0/
//
// for detailled explanation of this library see dedicated article
// https://hallard.me/libteleinfo/
//
// For any explanation about teleinfo or use, see my blog
// https://hallard.me/category/tinfo
//
// connect Teleinfo RXD pin To Arduin D3
// see schematic here https://hallard.me/demystifier-la-teleinfo/
// and dedicated article here
//
// Written by Charles-Henri Hallard (https://hallard.me)
//
// History : V1.00 2015-06-14 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#include <LibTeleinfo.h>

TInfo          tinfo; // Teleinfo object

int chargePin = 9, consignePin = A5;
int blueLedPin = 7, orangeLedPin = 10, papp = 0;
int conso_consigne = 800;
float charge = 200, chargeMax = 250;


/* ======================================================================
  Function: printUptime
  Purpose : print pseudo uptime value
  Input   : -
  Output  : -
  Comments: compteur de secondes basique sans controle de dépassement
          En plus SoftwareSerial rend le compteur de millis() totalement
          A la rue, donc la precision de ce compteur de seconde n'est
          pas fiable du tout, dont acte !!!
  ====================================================================== */
void printUptime(void)
{
  Serial.print(millis() / 1000);
  Serial.print(F("s\t"));
}

/* ======================================================================
  Function: DataCallback
  Purpose : callback when we detected new or modified data received
  Input   : linked list pointer on the concerned data
          current flags value
  Output  : -
  Comments: -
  ====================================================================== */
void DataCallback(ValueList * me, uint8_t  flags)
{
  // Show our not accurate second counter
  //printUptime();

  if (flags & TINFO_FLAGS_ADDED)
    Serial.print(F("NEW_"));

  if (flags & TINFO_FLAGS_UPDATED)
    Serial.print(F("MAJ_"));

  // Display values
  Serial.print(me->name);
  Serial.print("=");
  Serial.println(me->value);

  if (String(me->name) == "PAPP") {
    //Serial.println("#     .. finding PAPP !");
    papp = atol(me->value);
  }
  else {
    //Serial.println("#     .. can't find PAPP ");
  }
}

/* ======================================================================
  Function: setup
  Purpose : Setup I/O and other one time startup stuff
  Input   : -
  Output  : -
  Comments: -
  ====================================================================== */


void setup()
{
  pinMode(blueLedPin, OUTPUT);
  pinMode(orangeLedPin, OUTPUT);
  for (int i = 0; i < 3; i++) {
    ledFlash(blueLedPin);
    delay(200);
  }
  delay(1000);
  for (int i = 0; i < 3; i++) {
    ledFlash(orangeLedPin);
    delay(200);
  }

  // Configure Teleinfo Soft serial
  // La téléinfo est connectee sur D3
  // ceci permet d'eviter les conflits avec la
  // vraie serial lors des uploads
  Serial.begin(1200);

  // Init teleinfo
  tinfo.init();

  // Attacher les callback dont nous avons besoin
  // pour cette demo, ici attach data
  tinfo.attachData(DataCallback);

  //printUptime();
  Serial.println(F("# Teleinfo started"));
}

/* ======================================================================
  Function: loop
  Purpose : infinite loop main code
  Input   : -
  Output  : -
  Comments: -
  ====================================================================== */
void loop()
{
  conso_consigne = analogRead(consignePin);

  // Teleinformation processing
  if (Serial.available() ) {
    tinfo.process(Serial.read());
  }
  if (timeUp()) {
    regul();
  }
}
