

int chargePin = 9, consignePin = A5;
int blueLedPin = 7, orangeLedPin = 10, papp = 0;
int conso_consigne = 800;
float charge = 200, chargeMax = 250;


// --- VARIABLES GLOBALES PWR_Mesure --- //
int16_t cur_pwr;
bool new_cur_pwr_flag;
int16_t u_tab[125];
float i_tab[125];

// ------------------------------------- //
#define DEBUG_PWR
//#define DEBUG_PID
#define USE_LINKY


#ifdef USE_LINKY
#include <LibTeleinfo.h>

TInfo          tinfo; // Teleinfo object

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

    //papp = atol(me->value);

  }
  else {
    //Serial.println("#     .. can't find PAPP ");
  }
}
#endif


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

  Serial.begin(9600);

#ifdef USE_LINKY
  // Init teleinfo
  tinfo.init();

  // Attacher les callback dont nous avons besoin
  // pour cette demo, ici attach data
  tinfo.attachData(DataCallback);

  //printUptime();
  Serial.println(F("# Teleinfo started"));
#endif
  Serial.println(F("# Regul started"));
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

  /*
    // Teleinformation processing
    if (Serial.available() ) {
    tinfo.process(Serial.read());
    }
  */
  pwr_cadenced_task();

  if (new_cur_pwr_flag) {
    papp = cur_pwr;
#ifdef DEBUG_PWR
    Serial.println(String(cur_pwr) + "VA");
#endif
    new_cur_pwr_flag = false;
  }

  regul();

}
