// #define led 13 
// String inputString = "";

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(9600);
//   pinMode(led, OUTPUT);
//   digitalWrite(led, 0);
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   // digitalWrite(led, 1);
//   // delay(2000);
//   // digitalWrite(led, 0);
//   // delay(2000);
//   while (Serial.available()) {
//     char inChar = (char)Serial.read();
//     if (inChar == '\n') { 
//       processCommand(inputString);
//       inputString = "";
//     } else {
//       inputString += inChar;
//     }
//   }

// }

// void processCommand(String cmd) {
//   cmd.trim();
//   if (cmd == "1") {
//     digitalWrite(led, 1);
//   } else if (cmd == "0") {
//     digitalWrite(led, 0);
//   }
// }

#define led 13 
char data = "";

void setup() {
  Serial.begin(9600); // For serial monitor
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
}

void loop() {
  while (Serial.available()) {
    char inChar = Serial.read();
    if (inChar == '1') { 
      digitalWrite(led, HIGH);
      Serial.println(inChar);
    } else if (inChar == '2') {
      digitalWrite(led, LOW);
      Serial.println(inChar);
    }
    else{
      Serial.println(inChar);
    }
  }
}


