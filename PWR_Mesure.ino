
int readInstVoltage() {  //In V
  int32_t digValue = 0;
  float u = 0;
  digValue = analogRead(VOLTAGE_PIN) * ADC_COEF; //digValue in mV relative
  digValue = digValue - voltageOffset;          //digValue in mV absolute
  u = float(float(digValue) * float(VOLTAGE_COEF));          //u in Volts
  u += 8;  //Correction d'usine de la valeur moyenne
  return int(u);
}

double readInstCurrent() {  //In mA
  int32_t digValue = 0;
  double i = 0;
  digValue = analogRead(CURRENT_PIN) * ADC_COEF; //digValue in mV relative
  digValue = digValue - CURRENT_OFFSET;         //digValue in mV absolute
  i = double(digValue) * double(CURRENT_COEF); //i in milli-Amps
  return i;
}

/*
double readInstPower() {
  return (readInstVoltage() * readInstCurrent());
}
*/
double readInstPower(){
  return (analogRead(CONSIGNE_AUX1_PIN)/2);
}


float readRMSVoltage() {    //TODO
  if (millis() > START_UP_TIME) {
    float U = 0;
    U = VOLTAGE_COEF * (analogRead(VOLTAGE_PIN)) * ADC_COEF;
    return U;
  }
}

float readRMSCurrent() {    //TODO
  if (millis() > START_UP_TIME) {
    float I = 0;
    I = CURRENT_COEF * (analogRead(CURRENT_PIN)) * ADC_COEF;
    return I;
  }
}

float readRMSPower() {
  if (millis() > START_UP_TIME) {
    return (readRMSVoltage() * readRMSCurrent());
  }
}

void pwr_cadenced_task() {
  const uint32_t pwr_cadenced_task_period = 100;    //200; //5fois par secondes //10motifs
  static uint32_t timeIn = millis();
  int32_t sigma = 0, brut_pwr = 0;;
  int16_t u_past = 100;
  const int nb_average = 10;
  static int past_pwr[nb_average];

  if (millis() > timeIn + pwr_cadenced_task_period) {
    //Il est l'heure de faire la mesure de puissance !
    int nb = 0;
    bool end_of_per_flag = false;

    // On attends un front montant sur le 230V ..
    do {
      u_past = readInstVoltage();
    } while ( !(readInstVoltage() > 0 || u_past < 0) );

    timeIn = millis();
    digitalWrite(infoLedPin, HIGH);

    while (!end_of_per_flag) {
      //On récupère la valeur moyenne de la tension réseau
      voltageOffset = analogRead(VOLTAGE_OFFSET_PIN) * ADC_COEF;
      //puis on lance les vraies mesures
      sigma += readInstPower();
      nb++;
      u_tab[nb] = readInstVoltage();
      i_tab[nb] = readInstCurrent();
      if (millis() > timeIn + 20 * 2)
        end_of_per_flag = true;
    }
    brut_pwr = sigma / nb;

    //On fait la mise à jour du tableau pour la moyenne glissante
    for (int rang = 0; rang < nb_average - 1; rang++)
      past_pwr[rang] = past_pwr[rang + 1];
    past_pwr[nb_average - 1] = brut_pwr;

    sigma = 0;
    for (int rang = 0; rang < nb_average; rang++) {
      sigma += past_pwr[rang];
    }

    cur_pwr = sigma / nb_average;
    new_cur_pwr_flag = true;
    digitalWrite(infoLedPin, LOW);

#ifdef DEBUG_PWR
    Serial.println(nb);
    for (int i = 0; i < nb; i++) {
      Serial.println(String(u_tab[i]) +"," +String(i_tab[i]) +"," +String(u_tab[i]*i_tab[i]));
    }
    delay(5000);
#endif


  }
}
