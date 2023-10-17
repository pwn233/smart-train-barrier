#include <Servo.h>


Servo TRAIN_BARRIER_SERVO;

const int GREEN_BUTTON_PIN = 2;
const int RED_BUTTON_PIN = 3;

const int LED_BUILTIN_PIN = 13;
const int LED_RED_PIN = 8;
const int LED_GREEN_PIN = 9;
const int LED_YELLOW_PIN = 10;

const int SERVO_PIN = 7;

const int OPEN_POSITION = 90;
const int CLOSE_POSITION = 180;

const int LEFT_IR_PIN = 4;
const int RIGHT_IR_PIN = 5;

const int BUZZER_PIN = 11;

const int TRIG_PIN = 12;
const int ECHO_PIN = 13;

int GREEN_BUTTON_STATE = 0;
int RED_BUTTON_STATE = 0;

// ir sensor damned, need new ones
int LEFT_IR_STATE = 0;
int RIGHT_IR_STATE = 0;

bool IS_OPENED = false;
bool IS_WARNING = 0;
bool IS_DETECTED_ALREADY = false;

long DURATION, DISTANCE;

void setup() {
  pinMode(LED_BUILTIN_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  digitalWrite(LED_RED_PIN, HIGH);
  digitalWrite(LED_GREEN_PIN, LOW);
  digitalWrite(LED_YELLOW_PIN, LOW);

  pinMode(GREEN_BUTTON_PIN, INPUT);
  pinMode(RED_BUTTON_PIN, INPUT);

  pinMode(LEFT_IR_PIN, INPUT);
  pinMode(RIGHT_IR_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  TRAIN_BARRIER_SERVO.attach(SERVO_PIN);
  TRAIN_BARRIER_SERVO.write(CLOSE_POSITION);

  Serial.begin(9600);
}

void TRAIN_BARRIER_FUNCTION() {
  GREEN_BUTTON_STATE = digitalRead(GREEN_BUTTON_PIN);
  RED_BUTTON_STATE = digitalRead(RED_BUTTON_PIN);
  if (!IS_OPENED) {
    if (GREEN_BUTTON_STATE == HIGH) {
      digitalWrite(LED_RED_PIN, LOW);
      digitalWrite(LED_BUILTIN_PIN, HIGH);
      IS_OPENED = true;
      // fast version
      // trainBarrierServo.write(90);
      // smooth version
      for (int POS = CLOSE_POSITION; POS >= OPEN_POSITION; POS -= 1) {
        if (POS % 10 == 0 && POS != OPEN_POSITION) {
          digitalWrite(LED_YELLOW_PIN, HIGH);
        } else if (POS & 10 != 0 || POS == OPEN_POSITION) {
          digitalWrite(LED_YELLOW_PIN, LOW);
        }
        TRAIN_BARRIER_SERVO.write(POS);
        delay(25);
      }
      digitalWrite(LED_GREEN_PIN, HIGH);
    }
  } else if (IS_OPENED) {
    if (RED_BUTTON_STATE == HIGH) {
      digitalWrite(LED_GREEN_PIN, LOW);
      digitalWrite(LED_BUILTIN_PIN, LOW);
      IS_OPENED = false;
      // fast version
      // trainBarrierServo.write(180);
      // smooth version
      for (int POS = OPEN_POSITION; POS <= CLOSE_POSITION; POS += 1) {
        if (POS % 10 == 0 && POS != CLOSE_POSITION) {
          digitalWrite(LED_YELLOW_PIN, HIGH);
        } else if (POS & 10 != 0 || POS == CLOSE_POSITION) {
          digitalWrite(LED_YELLOW_PIN, LOW);
        }
        TRAIN_BARRIER_SERVO.write(POS);
        delay(25);
      }
      digitalWrite(LED_RED_PIN, HIGH);
    }
  }
}

void loop() {
  if (IS_OPENED == false) {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    DURATION = pulseIn(ECHO_PIN, HIGH);
    DISTANCE = microsecondsToCentimeters(DURATION);
    // Serial.println(DISTANCE);
    if (DISTANCE <= 8) {
      if (digitalRead(LED_YELLOW_PIN) == LOW && IS_DETECTED_ALREADY == false) {
        digitalWrite(LED_YELLOW_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);
        IS_DETECTED_ALREADY = true;
        Serial.println(DISTANCE);
      }
    } else {
      digitalWrite(LED_YELLOW_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
      IS_DETECTED_ALREADY = false;
    }
  }
  TRAIN_BARRIER_FUNCTION();
  delay(200);
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}
