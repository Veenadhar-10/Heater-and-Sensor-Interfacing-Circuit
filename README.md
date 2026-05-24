# PID-Based Portable Heater and Resistive Sensor Interfacing System

## Overview
This project presents the design and implementation of a portable closed-loop heater and resistive sensor interfacing system using Arduino. The system is capable of heating a sensing platform from room temperature to high temperatures while maintaining stable temperature regulation using PID control.

An extruder hotend kit is used as the heating element, and the thermistor integrated inside the hotend is used for temperature sensing. The resistance of the thermistor is measured continuously and converted into temperature using the NTC thermistor beta equation.

The project also includes an analog interfacing circuit for measuring the resistance of an external resistive sensor placed on the heated platform. Signal conditioning is implemented using operational amplifier based inverting amplifier stages so that the Arduino ADC can accurately measure the sensor voltage.

The measured temperature, resistance, and setpoint values are displayed in real time on an OLED display. The user can modify the temperature setpoint using push buttons.

This project combines embedded systems, analog electronics, sensor interfacing, signal conditioning, and closed-loop control techniques.

---

## Features
- Closed-loop PID temperature control
- Adjustable temperature setpoint
- Real-time temperature monitoring
- OLED display interface
- Continuous resistive sensor monitoring
- Analog signal conditioning using op-amps
- PWM-based heater control
- Push-button user interface
- Debouncing implementation
- Real-time data acquisition

---

## Components Used
- Arduino Uno
- SSD1306 OLED Display
- Extruder Hotend Heater Kit
- NTC Thermistor
- Operational Amplifiers
- Push Buttons
- Resistors and Passive Components
- External Resistive Sensor

---

## Working Principle

The thermistor resistance is converted into temperature using the NTC beta equation:

\[
\frac{1}{T} = \frac{1}{T_0} + \frac{1}{\beta}\ln\left(\frac{R}{R_0}\right)
\]

The heater output is controlled using PID control:

\[
u(t)=K_p e(t)+K_i \int e(t)dt+K_d\frac{de(t)}{dt}
\]

The external resistive sensor is measured using an op-amp based inverting amplifier:

\[
V_{out}=-\frac{R_f}{R_{in}}V_{in}
\]

A second inverting amplifier stage restores the signal polarity for Arduino ADC compatibility.

---

## Software Implementation
The firmware was developed using Arduino IDE in Embedded C/C++.

Main functionalities include:
- Thermistor ADC acquisition
- Temperature computation
- PID control computation
- PWM heater control
- Sensor resistance calculation
- OLED display updates
- Push button handling
- Debouncing logic

---

## Project Structure

```text
Portable-Heater-System/
│
├── README.md
├── heater_pid_system.ino
```

---

## Results
- Stable PID-based temperature control achieved
- Real-time resistance monitoring implemented
- Continuous OLED display updates
- Successful analog signal conditioning
- Stable operation near desired setpoint
- Accurate sensor interfacing using op-amp circuits

---

## Skills Demonstrated
- Embedded Systems
- PID Control
- Sensor Interfacing
- Analog Circuit Design
- Operational Amplifier Circuits
- Signal Conditioning
- Real-Time Monitoring
- Hardware Debugging
- Arduino Programming
- Data Acquisition Systems

---

## Applications
- Sensor characterization systems
- Thermal testing platforms
- Embedded instrumentation systems
- Portable heating applications
- Resistive sensor analysis

---

## Future Improvements
- Wireless monitoring using Bluetooth/WiFi
- Data logging support
- PCB implementation
- Improved resistance calibration
- Higher precision ADC integration

---

## Author
Veenadhar
## Team Members
- Phanidhar
- Sharandeep
