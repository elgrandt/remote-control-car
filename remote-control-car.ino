#include <PS2X_lib.h>  //for v1.6
#include <Servo.h>
#define LEDS_ADELANTE 8
#define LEDS_ATRAS 5
#define LEDS_MARCHA_ATRAS 9
#define LED_GIRO_IZQUIERDO 1
#define LED_GIRO_DERECHO 7
#define SERVO 6
#define ACELERAR 2
#define FRENAR 4
#define MOTOR_PWM 3
#define ROTATION_SPEED 22
#define ACCELERATION 22

PS2X ps2x; // PS2 Controller Manager
Servo LXservo;

double objectiveRotation = 128;
double currentRotation = 128;
double objectiveSpeed = 128;
double currentSpeed = 128;

void setup(){
  LXservo.attach(SERVO);
  ps2x.config_gamepad(13,11,10,12, true, true); //Ajuste del pin: (clock, command, attention, data, true, true)
  pinMode(ACELERAR, OUTPUT);
  pinMode(FRENAR, OUTPUT);
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(LEDS_ADELANTE, OUTPUT);
  pinMode(LEDS_MARCHA_ATRAS, OUTPUT);
  pinMode(LEDS_ATRAS, OUTPUT);
  pinMode(LED_GIRO_IZQUIERDO, OUTPUT);
  pinMode(LED_GIRO_DERECHO, OUTPUT);
  TCCR2B = TCCR2B & B11111000 | B00000001; // Motor frequency change
}

void loop(){
  ps2x.read_gamepad();

  actualizarLedsPosicion();

  actualizarVelocidad();

  actualizarRotacion();

  actualizarLedsGiro();
  
  delay(50);
}

void actualizarLedsGiro() {
  if (currentRotation < 120){
    digitalWrite(LED_GIRO_IZQUIERDO, HIGH);
    digitalWrite(LED_GIRO_DERECHO, LOW);
  }
  
  if (currentRotation > 150){
    digitalWrite(LED_GIRO_DERECHO, HIGH);
    digitalWrite(LED_GIRO_IZQUIERDO, LOW);
  }  
  
  if (currentRotation > 120 && currentRotation < 150){
    digitalWrite(LED_GIRO_IZQUIERDO, LOW);
    digitalWrite(LED_GIRO_DERECHO, LOW);
  }
}

void calcularRotacionActual() {
  if (currentRotation < objectiveRotation - (ROTATION_SPEED - 1)) {
    currentRotation += ROTATION_SPEED;
  } else if (currentRotation > objectiveRotation + ROTATION_SPEED - 1) {
    currentRotation -= ROTATION_SPEED;
  } else {
    currentRotation = objectiveRotation;
  }
  // La vuelta de las ruedas a su lugar inicial es automatica
  if ((objectiveRotation <= 127 && currentRotation > 127) || (objectiveRotation >= 127 && currentRotation < 127)) {
    currentRotation = 127;
  }
}

void actualizarRotacion() {
  objectiveRotation = ps2x.Analog(PSS_RX);
  calcularRotacionActual();

  double currentRotationMappedToServo = map(currentRotation, 0, 255, 15, 145);
  LXservo.write(currentRotationMappedToServo);
}

void actualizarLedsPosicion() {
  if(ps2x.ButtonPressed(PSB_CIRCLE)) digitalWrite(LEDS_ADELANTE, HIGH);
  if(ps2x.ButtonPressed(PSB_TRIANGLE)) digitalWrite(LEDS_ADELANTE, LOW);
  
  
  if(ps2x.ButtonPressed(PSB_SQUARE)) digitalWrite(LEDS_ATRAS, HIGH);
  if(ps2x.ButtonPressed(PSB_CROSS)) digitalWrite(LEDS_ATRAS, LOW);
}

void calcularVelocidadObjetivo() {
  objectiveSpeed = ps2x.Analog(PSS_LY);
  if (objectiveSpeed > 128 && objectiveSpeed < 160) {
    objectiveSpeed = 180;
  }
  if (objectiveSpeed < 128 && objectiveSpeed > 65) {
    objectiveSpeed = 40;
  }
}

void calcularVelocidadActual() {
  if (currentSpeed < objectiveSpeed - (ACCELERATION - 1)) {
    currentSpeed += ACCELERATION;
  } else if (currentSpeed > objectiveSpeed + ACCELERATION - 1) {
    currentSpeed -= ACCELERATION;
  } else if (currentSpeed < objectiveSpeed) {
    currentSpeed += 1;
  } else if (currentSpeed > objectiveSpeed) {
    currentSpeed -= 1;
  }
  if (currentSpeed > 128 && currentSpeed < 160) {
    currentSpeed = 180;
  }
  if (currentSpeed < 128 && currentSpeed > 65) {
    currentSpeed = 40;
  }
  if ((objectiveSpeed <= 128 && currentSpeed > 128) || (objectiveSpeed >= 128 && currentSpeed < 128)) {
    currentSpeed = 128;
  }
}

void actualizarVelocidad() {
  int fspeed;
  
  calcularVelocidadObjetivo();
  
  calcularVelocidadActual();
  
  if (currentSpeed == 128) analogWrite(MOTOR_PWM, 0);

  if (currentSpeed > 128){
    fspeed = map(currentSpeed, 129, 255, 0, 255);
    digitalWrite(ACELERAR, LOW);
    digitalWrite(FRENAR, HIGH);
    analogWrite(MOTOR_PWM, fspeed);
    digitalWrite(LEDS_MARCHA_ATRAS, HIGH);
  }
  
  if (currentSpeed < 128){
    fspeed = map(currentSpeed, 0, 127, 255, 0);
    digitalWrite(ACELERAR, HIGH);
    digitalWrite(FRENAR, LOW);
    analogWrite(MOTOR_PWM, fspeed);
    digitalWrite(LEDS_MARCHA_ATRAS, LOW);
  }
}
