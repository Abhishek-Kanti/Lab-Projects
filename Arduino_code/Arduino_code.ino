#define ledPin 13
#define in1 6
#define in2 7
#define enA 5

String inputString = "";
int currentSpeed = 200; // Default speed (0–255 scale)

void setup() {
  Serial.begin(9600); 
  pinMode(ledPin, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enA, OUTPUT);
  digitalWrite(ledPin, LOW);
  analogWrite(enA, currentSpeed); // Initialize speed
}

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      processCommand(inputString);
      inputString = "";
    } else {
      inputString += inChar;
    }
  }
}

void processCommand(String cmd) {
  cmd.trim();

  if (cmd == "1") { // Start forward
    digitalWrite(ledPin, HIGH);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(enA, currentSpeed); // use saved speed
    Serial.println("LED ON - Motor START (Forward)");
  } 
  else if (cmd == "0") { // Stop
    digitalWrite(ledPin, LOW);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(enA, 0); // Stop the motor completely
    Serial.println("LED OFF - Motor STOP");
  }
  else if (cmd == "2") { // Reverse
    digitalWrite(ledPin, HIGH);
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(enA, currentSpeed); // use saved speed
    Serial.println("LED ON - Motor REVERSE");
  }
  else if (cmd.startsWith("s:")) { // Speed command
    digitalWrite(ledPin, HIGH);
    int speedValue = cmd.substring(2).toInt(); // Extract number after 's:'
    currentSpeed = map(speedValue, 0, 100, 0, 255); // Save new speed
    analogWrite(enA, currentSpeed);
    Serial.print("LED ON - Motor speed set to ");
    Serial.println(speedValue); // just prints user input speed
  }
}

