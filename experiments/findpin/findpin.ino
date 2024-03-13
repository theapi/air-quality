


//#define DATA_PIN 14 // GPIO14 == D5
#define DATA_PIN D8 // GPIO15 == D8

void setup() {
    Serial.begin(115200);
  Serial.println("\nStarting...\n");

pinMode(DATA_PIN, OUTPUT);
}

void loop() {
  digitalWrite(DATA_PIN, HIGH); 
  delay(50);
  digitalWrite(DATA_PIN, LOW);
  delay(50);
}
