void ledFlash(int ledPin){
  digitalWrite(ledPin, HIGH);
  delay(50);
  digitalWrite(ledPin, LOW);
}

void ledOn(int ledPin){
  digitalWrite(ledPin, HIGH);
}

void ledOff(int ledPin){
  digitalWrite(ledPin, LOW);
}
