# 🦯 Smart Blind Stick using STM32

## 📌 Project Overview
The **Smart Blind Stick** is an assistive embedded system designed to help visually impaired people navigate safely and confidently.  
This project integrates **ultrasonic sensing**, **light detection**, **visual alerts**, and an **emergency alarm system** using an **STM32 microcontroller**.

The system detects obstacles in front of the user, alerts surrounding people when the user is walking in dark environments, and allows the user to request help using an emergency button.

---

## 🎯 Objectives
- Detect obstacles in front of the user and alert them using sound
- Automatically turn on a light indicator in dark environments
- Provide an emergency alert system to ask for help
- Build a low-cost, reliable, and portable assistive device

---

## 🛠️ Hardware Components Used
- STM32F4 Series Microcontroller
- Ultrasonic Distance Sensor (HC-SR04 or equivalent)
- LDR (Light Dependent Resistor)
- LED (Indicator Light)
- Push Button (Emergency Button)
- Buzzer
- Resistors & Connecting Wires
- Power Supply (Battery / USB)

---

## 🔌 Pin Configuration

| Component            | STM32 Pin | Description                  |
|---------------------|-----------|------------------------------|
| Emergency Button     | PA0       | Input with pull-up           |
| Ultrasonic Echo      | PA1       | Input                        |
| Emergency LED        | PA2       | Output                       |
| LDR Indicator LED    | PA3       | Output                       |
| LDR Sensor           | PA4       | ADC Channel 4                |
| Buzzer               | PA5       | Output                       |
| Ultrasonic Trigger   | PA6       | Output                       |

---

## ⚙️ System Functionality

### 1️⃣ Obstacle Detection (Ultrasonic Sensor)
- The ultrasonic sensor continuously measures the distance in front of the user.
- If an obstacle is detected within a short range, the **buzzer beeps** to warn the user.
- Helps avoid collisions with objects such as walls, people, or furniture.

### 2️⃣ Light Detection (LDR Module)
- The LDR measures ambient light intensity.
- In **dark environments**, the LED automatically turns ON.
- This helps **surrounding people notice the blind person** during nighttime or low-light conditions.

### 3️⃣ Emergency Alert System (Button + Buzzer)
- When the emergency button is pressed:
  - An **emergency LED** turns ON
  - A **high-frequency buzzer alarm** sounds repeatedly
- This feature allows the user to **ask for help** in critical situations.

---

##  Software Features
- Bare-metal programming using CMSIS registers
- ADC used for LDR light sensing
- Microsecond & millisecond delay functions
- Modular code structure for easy understanding
- Efficient polling-based design (no RTOS)

---

##  Working Principle
1. STM32 initializes GPIOs, ADC, and peripherals.
2. ADC continuously reads LDR sensor values.
3. Ultrasonic sensor measures distance using trigger and echo timing.
4. Based on sensor readings:
   - LED turns ON in darkness
   - Buzzer alerts for nearby obstacles
   - Emergency alarm activates on button press

---

##  Application Areas
- Assistive devices for visually impaired people
- Smart wearable safety systems
- Embedded systems learning projects
- Social welfare and accessibility solutions

---

## 🚀 Future Improvements
- Add vibration motor for silent alerts
- GPS module for location tracking
- GSM module for automatic emergency calls
- Battery level monitoring
- Weather-proof casing

