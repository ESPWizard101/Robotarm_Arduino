# Robotic Arm Control with Joystick (Robotarm.ino)
### Overview
This project controls a 3-servo robotic arm (base rotation, arm extension, gripper) using a joystick for X/Y axes and a button for gripper toggling. It includes joystick smoothing, debouncing, and deadzone to reduce jitter. 

### Hardware Requirements

ARDUINO UNO
Joystick module (analog X/Y on A0/A1, button on D2).
3x Servo motors: Base (D3), Arm (D5), Gripper (D10).

### Setup and Usage

Upload Robotarm.ino to your ESP32.
Open Serial Monitor (9600 baud) to see initialization and gripper state.
Move joystick: X-axis controls base servo (0-180°), Y-axis controls arm servo (0-180°).
Press button to toggle gripper .
The loop runs with a 15ms delay for smoothness.
