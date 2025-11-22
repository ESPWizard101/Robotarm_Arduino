#include <Servo.h>

// Pin definitions
const int JOYSTICK_X_PIN = A0;     // Joystick X-axis
const int JOYSTICK_Y_PIN = A1;     // Joystick Y-axis
const int BUTTON_PIN = 2;          // Joystick button 
const int SERVO1_PIN = 3;          // Base servo
const int SERVO2_PIN = 5;         // Arm servo
const int SERVO3_PIN = 10;         // Catch servo (gripper)




// Servo position limits and initials
const int MIN_ANGLE = 0;           // Minimum servo angle
const int MAX_ANGLE = 180;         // Maximum servo angle
const int NEUTRAL_ANGLE = 90;      // Center position
const int CLOSED_ANGLE = 0;        // Gripper closed
const int OPEN_ANGLE = 180;        // Gripper open

// Joystick settings in order to be more stable
const unsigned long DEBOUNCE_DELAY = 50;  // ms to ignore button chatter
const int DEADZONE = 50;
const int JOYSTICK_CENTER = 512;

// Servo objects
Servo servo1;  
Servo servo2;  
Servo servo3;  

int servo1Pos = NEUTRAL_ANGLE;     // Current position servo1
int servo2Pos = NEUTRAL_ANGLE;     // Current position servo2
bool gripperOpen = false;          // Gripper state
int lastButtonState = HIGH;        // Previous button state
unsigned long lastDebounceTime = 0; // Timestamp of last button state change

// For joystick smoothing 
const int SMOOTH_SAMPLES = 5;      // Number of samples for averaging
int xSamples[SMOOTH_SAMPLES];      // Array for X readings
int ySamples[SMOOTH_SAMPLES];      // Array for Y readings
int sampleIndex = 0;               // Current sample index

void setup() {
  // Attach servos
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);
  
  // Initialize gripper to closed
  servo3.write(CLOSED_ANGLE);
  
  // Initialize Serial Monitor
  Serial.begin(9600);
  Serial.println("Robot Arm Initialized");
  
  // Initialize sample arrays to neutral
  for (int i = 0; i < SMOOTH_SAMPLES; i++) {
    xSamples[i] = 512;  // Joystick center raw value
    ySamples[i] = 512;
  }
}

void loop() {
  // Read and smooth joystick values
  readAndSmoothJoystick();
  // Get averaged values
  int xAvg = getAverage(xSamples);
  int yAvg = getAverage(ySamples);
  
  // Apply deadzone: Snap to center if within threshold
  if (abs(xAvg - JOYSTICK_CENTER) < DEADZONE) {
    xAvg = JOYSTICK_CENTER;
  }
  if (abs(yAvg - JOYSTICK_CENTER) < DEADZONE) {
    yAvg = JOYSTICK_CENTER;
  }
  // Map smoothed values to servo positions
  servo1Pos = map(xAvg, 0, 1023, MIN_ANGLE, MAX_ANGLE);
  servo2Pos = map(yAvg, 0, 1023, MIN_ANGLE, MAX_ANGLE);
  
  // Clamp positions to safe range 
  servo1Pos = constrain(servo1Pos, MIN_ANGLE, MAX_ANGLE);
  servo2Pos = constrain(servo2Pos, MIN_ANGLE, MAX_ANGLE);
  
  // Update main servos
  servo1.write(servo1Pos);
  servo2.write(servo2Pos);
  
  // Handle button for gripper toggle
  handleButton();


  // delay for smooth
  delay(15);
}

void readAndSmoothJoystick() {
  int xRaw = analogRead(JOYSTICK_X_PIN);
  int yRaw = analogRead(JOYSTICK_Y_PIN);
  
  // Store in circular buffer
  xSamples[sampleIndex] = xRaw;
  ySamples[sampleIndex] = yRaw;
  
  // Increment index, wrap around
  sampleIndex = (sampleIndex + 1) % SMOOTH_SAMPLES;
}

// calculate average
int getAverage(int samples[]) {
  long sum = 0;
  for (int i = 0; i < SMOOTH_SAMPLES; i++) {
    sum += samples[i];
  }
  return sum / SMOOTH_SAMPLES;
}

void handleButton() {
  int currentButtonState = digitalRead(BUTTON_PIN);
  
  // Check if state changed
  if (currentButtonState != lastButtonState) {
    lastDebounceTime = millis();  // Reset debounce timer
  }
  
  // If stable for DEBOUNCE_DELAY, process
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (currentButtonState == LOW) {  // Button pressed 
      gripperOpen = !gripperOpen;    // Toggle state
      int targetAngle = gripperOpen ? OPEN_ANGLE : CLOSED_ANGLE;
      servo3.write(targetAngle);
      Serial.print("Gripper toggled to: ");
      Serial.println(gripperOpen ? "Open" : "Closed");
    }
  }
  
  // Update last state
  lastButtonState = currentButtonState;
}

