/* CONSIDERAR SINAL 0 QUANDO APERTADO */
const int buttonPin1 = 2; 
const int buttonPin2 = 3;
const int buttonPin3 = 5;
const int buttonPin4 = 4;
int val = 0;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buttonPin4, INPUT_PULLUP); 
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println("botao 1");
  Serial.println(digitalRead(buttonPin1));

  Serial.println("botao 2");
  Serial.println(digitalRead(buttonPin2));

  Serial.println("botao 3");
  Serial.println(digitalRead(buttonPin3));

  Serial.println("botao 4");
  Serial.println(digitalRead(buttonPin4));

  delay(1000);
}
