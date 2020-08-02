bool timeUp(void) {
  static uint32_t timeIn = millis();
  const uint32_t tickPeriode = 500;
  bool timeIsUp = false;
  if (millis() > timeIn + tickPeriode) {
    //Time is up !
    timeIsUp = true;
    timeIn = millis();
  }
  return timeIsUp;
}


void regul(void) {

  //Regul
  const float P = 0.01;
  const float I = 0;
  const float D = 0.01;
  static int delta = 0, deltaPast = 0;

  // Si delta > 0 alors on consomme plus que l'idéal mais
  // sinon on risque de réinjecter si delta < 0 !
  delta = papp - conso_consigne;
  float correction = 0;

#ifdef DEBUG_PID
  Serial.print("[");
  Serial.print(delta);
#endif
  /*
    if (abs(delta) > 300) { //W
    correction = P*10;
    }
    else if (abs(delta) > 80) { //W
    correction = P*3;
    }
    else { //Il ne faut faire qu'une petit regul donc on passe en ++ ou --
    correction = P*1;
    }
  */

  correction = P * delta + D * (delta - deltaPast);
  deltaPast = delta;

  if (papp <= conso_consigne) {
    charge -= correction;
    ledOn(orangeLedPin);
  }
  else {
    charge -= correction;
    ledOff(orangeLedPin);
  }

  if (charge > chargeMax) {
    charge = chargeMax;
  }
  else if (charge < 0) {
    charge = 0;
  }

  //Affichage
  if (papp < (conso_consigne - 50))
    ledOn(blueLedPin);
  else
    ledOff(blueLedPin);
    
#ifdef DEBUG_PID
  Serial.print("/");
  Serial.print(papp);
  Serial.print("/");
  Serial.print(charge);
  Serial.print("/");
  Serial.print(conso_consigne);
  Serial.println("]");
#endif

  analogWrite(chargePin, int(charge));
}
