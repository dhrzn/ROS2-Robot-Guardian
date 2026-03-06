# Guardian Robot
A ROS2 + Arduino integration project that monitors physical space using an ultrasonic distance sensor and triggers proportional hardware alerts when an object gets too close.

## The Problem & Why I Built This
I wanted to get comfortable integrating ROS2 with real hardware rather than just writing nodes in simulation, but having software talk to physical components in the real world. So I built this project from scratch to teach myself exactly that.

## How It Works
The system bridges two worlds: the hardware side (Arduino) and the software side (ROS2).
An HC-SR04 ultrasonic sensor continuously measures distance and sends readings to the computer over serial communication. The arduino_bridge_node receives this data, publishes it to the /distance topic, and is also responsible for sending commands back to the Arduino whenever the threshold is passed. A monitor node subscribes to /distance and calls an alert service when the distance drops below a set threshold (default: 30cm). The alert server handles that service call and publishes a command to /alert_command, which the arduino_bridge_node picks up and forwards to the Arduino — activating the buzzer, LED, and servo.
The closer the object, the stronger the reaction. At 30cm the components barely respond. At 0cm they are at full intensity. This proportional response is achieved using PWM (Pulse Width Modulation).

# System Architecture


Ardunio (HC-SR04 sensor) --> serial /dev/ttyACM0 --> ardunio_bridge_node --> /distance topic --> monitor_node --> TriggerAlert service --> alert_server --> /alert_command topic --> ardunio_bridge_node


## Hardware components used:
HC-SR04 Trigger
HC-SR04 Echo
Active Buzzer
Red LED
Servo (signal)

## Software Stack

ROS2 Humble
Python 3 (ROS2 nodes)
Arduino / C++ (sensor + actuator control)
pyserial (Python ↔ Arduino serial communication)
Ubuntu 24 (VirtualBox on Windows)


## ROS2 Package Structure
my_robot_pkg/
├── my_robot_pkg/
│   ├── ardunio_bridge_node.py   # Reads serial data, publishes /distance, forwards commands to Arduino
│   ├── monitor_node.py          # Subscribes to /distance, calls TriggerAlert service
│   └── alert_server.py          # Serves TriggerAlert, publishes to /alert_command
├── launch/
│   └── robot_system.launch.py   # Launches all three nodes
└── package.xml

## How To Run
Prerequisites
- ROS2 Humble installed
- Arduino IDE with sketch uploaded to board
- Arduino connected via USB (/dev/ttyACM0)
- pyserial: pip install pyserial

## Steps: 
1. bash #Build the workspace
2. cd ~/my_ros2_ws
3. colcon build
4. source install/setup.bash
5. ros2 launch my_robot_pkg robot_system.launch.py # launch the full system

## Key Things I Learned
- ROS2 fundamentals in a real context. Publisher/subscriber pattern, custom service definitions (.srv), and client/server communication, all wired to actual hardware.
- Serial communication is sensitive. Early on I had two nodes both trying to read from /dev/ttyACM0 at the same time. This caused race conditions and corrupted data. The fix was giving exclusive serial ownership to arduino_bridge_node and having the other node communicate through a ROS2 topic instead.
- Case sensitivity can cost you hours. The Arduino was reading "Clear" instead of "CLEAR", which meant the components never reset to their zero position when an object moved away. After hours of debugging, that one character difference was the entire problem.
- PWM enables proportional hardware response. Instead of just turning components on/off, analogWrite() lets you control intensity. Combined with Arduino's map() and constrain() functions, the hardware response scales smoothly with distance.

## Demo
Photos and video coming soon
![IMG_3909](https://github.com/user-attachments/assets/ca9b78f2-19c1-430b-94a3-285ef22f81c0)
![IMG_3910](https://github.com/user-attachments/assets/957f9d50-4f6c-4ce5-9d85-5f84cd38afb7)
![IMG_3911](https://github.com/user-attachments/assets/129d65d3-6866-41a6-920a-d9eac72dddec)
![IMG_3915](https://github.com/user-attachments/assets/a8cfbb20-e634-496a-89b3-9e59f17e8b79)
![IMG_3914](https://github.com/user-attachments/assets/b8238541-dc33-4a82-b8c7-840811f909e6)

# About
Built as part of my robotics portfolio while studying at CSULB, working toward becoming a robotics software engineer.
