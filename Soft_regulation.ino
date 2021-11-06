
//Mise à jour des pins pour nouvelle version H2 le 2021_11_06
//PINNING
#define VOLTAGE_PIN A0
#define SOLAR_CURRENT_PIN A1  //Courant lié au panneaux TODO
#define CURRENT_PIN A2       //Courant lié au compteur
#define AUX_CURRENT_PIN A3  //Courant lié a ... TODO
#define CONSIGNE_PIN A4          //Entrée analogique indiquant la conso EDF idéal (proche de 0)
#define CONSIGNE_AUX1_PIN A5     //Entrée analogique indiquant ... TODO
#define CONSIGNE_AUX2_PIN A6    //Entrée analogique sur bornier ... TODO
#define VOLTAGE_OFFSET_PIN A7 //Entrée de la tension moyenne VGRID = voltageOffset

#define LOAD1_PIN 5      //Pin pour le délestage résitif en PWM [Sortie 1]
#define LOAD2_PIN 6     //Pin pour le délestage résitif en PWM  [Sortie 2]
#define RELAY1_PIN 7   //Pin pour le délestage résitif en PWM   [Sortie 3]
#define RELAY2_PIN 8  //Pin pour le délestage résitif en PWM    [Sortie 4]

int infoLedPin = 10, gridZeroLedPin = 13; //Voyant d'info et d'équilibre EDF
int pidPLedPin = 12, pidMLedPin = 11;    //Voyant du PID

//CONFIG
#define START_UP_TIME 3000
#define CURRENT_OFFSET 1024    // =VREF/2
#define ADC_COEF 2            // =2mV /pt car 2048mV / 1024pt
#define VOLTAGE_COEF 0.73    // Conversion mV sur input vers tension réseau  //1000mV ~= 600V
#define CURRENT_COEF 0      //TODO Il faut regler ça

int papp = 0;         //Variable de puissance apparente
int voltageOffset = 0;
int conso_consigne = 800;
float charge = 128, chargeMax = 255;

// --- VARIABLES GLOBALES PWR_Mesure --- //
int16_t cur_pwr;
bool new_cur_pwr_flag;
int16_t u_tab[125];
float i_tab[125];

// ------------------------------------- //
//#define DEBUG_PWR  //Envoi des courbes de U,I et S sur l'uart
#define DEBUG_PID
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
  analogReference(EXTERNAL);
  pinMode(infoLedPin, OUTPUT);
  pinMode(gridZeroLedPin, OUTPUT);
  pinMode(pidPLedPin, OUTPUT);
  pinMode(pidMLedPin, OUTPUT);

  pinMode(LOAD1_PIN, OUTPUT);
  pinMode(LOAD2_PIN, OUTPUT);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  
  for (int i = 0; i < 3; i++) {
    ledFlash(infoLedPin);
    delay(200);
  }
  delay(1000);
  for (int i = 0; i < 3; i++) {
    ledFlash(gridZeroLedPin);
    delay(200);
  }
  
  Serial.begin(57600); //9600
  

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
  conso_consigne = analogRead(CONSIGNE_PIN);

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
