// ======================================================
// Pin definitions
// ======================================================

// Motor driver (L293D)
const int MOTOR_IN1 = 5;   // PWM
const int MOTOR_IN2 = 6;   // PWM

// Ultrasonic sensor (HC-SR04)
const int TRIG_PIN  = 9;
const int ECHO_PIN  = 10;

// RGB LED (common cathode)
const int LED_R_PIN = 3;   // PWM
const int LED_G_PIN = 11;  // PWM
const int LED_B_PIN = 2;   // digital (on/off)

// ======================================================
// Control parameters
// ======================================================

const unsigned long LOOP_INTERVAL_MS = 10;   // main loop tick

// Ultrasonic timing
const unsigned long ECHO_TIMEOUT_US  = 30000UL; // 30 ms timeout
const unsigned long US_MIN_INTERVAL_MS = 50UL;  // >= 50ms between real pings

// Distance thresholds (cm)
const int MIN_DIST_CM = 5;    // closer than this -> 0% speed
const int MAX_DIST_CM = 60;   // farther than this -> maneuver

// Motor deadband (%)
const int MOTOR_DEADBAND_PERCENT = 3;

// Maneuver timing
const unsigned long STOP_TIME_MS    = 500;   // ms stop before reversing
const unsigned long REVERSE_TIME_MS = 200;   // ms reverse duration

// ======================================================
// State variables
// ======================================================

// For ultrasonic
unsigned long lastUSReadMs   = 0;
int           lastUSDistance = 0;

// For main loop pacing
unsigned long lastLoopMs     = 0;

// For motor speed tracking
int lastMotorPercent = 100;  // start at full forward

// Motor mode state machine
enum MotorMode {
  MODE_NORMAL,
  MODE_STOPPING,
  MODE_REVERSING,
  MODE_SLOW_FORWARD
};

MotorMode     motorMode    = MODE_NORMAL;
unsigned long modeStartMs  = 0;

// ======================================================
// Helper: set motor speed (L293D, bidirectional, PWM)
//   percent: -100 (full reverse) to 100 (full forward)
// ======================================================
void setMotorSpeed(int percent)
{
  // Constrain and apply deadband
  percent = constrain(percent, -100, 100);
  if (abs(percent) < MOTOR_DEADBAND_PERCENT) {
    percent = 0;
  }

  // Avoid redundant writes
  if (percent == lastMotorPercent) {
    return;
  }
  lastMotorPercent = percent;

  if (percent == 0) {
    // Stop / coast
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW);
    return;
  }

  bool forward = (percent > 0);
  int magnitude = abs(percent);

  // Map 1..100% -> 1..255 PWM
  int pwmValue = map(magnitude, 1, 100, 1, 255);

  if (forward) {
    // Forward: IN1 = PWM, IN2 = LOW
    analogWrite(MOTOR_IN1, pwmValue);
    digitalWrite(MOTOR_IN2, LOW);
  } else {
    // Reverse: IN1 = LOW, IN2 = PWM
    digitalWrite(MOTOR_IN1, LOW);
    analogWrite(MOTOR_IN2, pwmValue);
  }
}

// ======================================================
// Helper: read distance from HC-SR04 in cm
//   Returns 2..400 for valid distance, 0 for timeout/invalid
// ======================================================
int readDistanceCm()
{
  unsigned long now = millis();

  // Rate limit: if polled too fast, return last stable value
  if (now - lastUSReadMs < US_MIN_INTERVAL_MS) {
    return lastUSDistance;
  }
  lastUSReadMs = now;

  // Trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Echo measurement with timeout
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, ECHO_TIMEOUT_US);

  if (duration == 0) {
    lastUSDistance = 0;
    return 0;
  }

  // Convert microseconds to cm (integer friendly)
  int dist = (int)(duration / 58UL);

  if (dist < 2 || dist > 400) {
    lastUSDistance = 0;
    return 0;
  }

  lastUSDistance = dist;
  return dist;
}

// ======================================================
// Helper: RGB LED control (common cathode)
//   r, g in 0..255 (PWM), b = 0/255 (digital)
// ======================================================
void setRGB(byte r, byte g, byte b)
{
  // Common cathode: 0 = off, 255 = full on
  analogWrite(LED_R_PIN, r);
  analogWrite(LED_G_PIN, g);
  digitalWrite(LED_B_PIN, (b > 0) ? HIGH : LOW);
}

// ======================================================
// Helper: update RGB based on motor speed
//   -100..100 -> magnitude 0..100
//   0%   -> red
//   50%  -> yellow
//   100% -> green
// ======================================================
void updateStatusLED(int speedPercent)
{
  int mag = abs(speedPercent);
  if (mag > 100) mag = 100;

  byte r, g, b;
  b = 0;

  const int MID = 50;

  if (mag <= MID) {
    // 0 -> 50 : red (255,0) to yellow (255,255)
    float t = mag / (float)MID;   // 0..1
    r = 255;
    g = (byte)(255.0 * t);
  } else {
    // 50 -> 100: yellow (255,255) to green (0,255)
    float t = (mag - MID) / (float)MID;
    r = (byte)(255.0 * (1.0 - t));
    g = 255;
  }

  setRGB(r, g, b);
}

// ======================================================
// Arduino setup()
// ======================================================
void setup()
{
  Serial.begin(9600);

  // Motor pins
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);

  // Ultrasonic pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  // RGB LED pins
  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  setRGB(0, 0, 0);

  lastLoopMs = millis();
  modeStartMs = millis();

  // Start full forward
  lastMotorPercent = 100;
  setMotorSpeed(lastMotorPercent);
  updateStatusLED(lastMotorPercent);

  Serial.println("Motor + Ultrasonic + RGB Monolith (Tinkercad)");
}

// ======================================================
// Arduino loop()
// ======================================================
void loop()
{
  unsigned long now = millis();

  // Pace main control loop
  if (now - lastLoopMs < LOOP_INTERVAL_MS) {
    return;
  }
  lastLoopMs = now;

  // --- Read distance ---
  int distance = readDistanceCm();
  int speedPct = lastMotorPercent;

if (distance > 0 && distance <= MIN_DIST_CM) {
  switch (motorMode) {
    case MODE_NORMAL:
      speedPct = 0;
      motorMode = MODE_STOPPING;
      modeStartMs = now;
      break;

    case MODE_STOPPING:
      speedPct = 0;
      if (now - modeStartMs >= STOP_TIME_MS) {
        motorMode = MODE_REVERSING;
        modeStartMs = now;
      }
      break;

    case MODE_REVERSING:
      speedPct = -20;
      if (now - modeStartMs >= REVERSE_TIME_MS) {
        motorMode = MODE_SLOW_FORWARD;
        modeStartMs = now;
      }
      break;

    case MODE_SLOW_FORWARD:
      speedPct = 20;
      if (distance > MAX_DIST_CM || distance == 0) {
        motorMode = MODE_NORMAL;
      }
      break;
  }
} else {
  motorMode = MODE_NORMAL;
  modeStartMs = now;

  if (distance == 0) {
    speedPct = 100;
  } else if (distance >= MAX_DIST_CM) {
    speedPct = 100;
  } else {
    speedPct = map(distance, MIN_DIST_CM, MAX_DIST_CM, 0, 100);
    speedPct = (speedPct / 2) * 2;
    speedPct = constrain(speedPct, 0, 100);
  }
}

  // --- Apply outputs ---
  setMotorSpeed(speedPct);
  updateStatusLED(speedPct);

  // --- Debug output ---
  lastMotorPercent = speedPct;
  Serial.print("Dist cm: ");
  Serial.print(distance);
  Serial.print(" | Mode: ");
  switch (motorMode) {
    case MODE_NORMAL:       Serial.print("NORMAL"); break;
    case MODE_STOPPING:     Serial.print("STOPPING"); break;
    case MODE_REVERSING:    Serial.print("REVERSING"); break;
    case MODE_SLOW_FORWARD: Serial.print("SLOW_FWD"); break;
  }
  Serial.print(" | Speed %: ");
  Serial.println(speedPct);
}
