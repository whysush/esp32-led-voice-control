# ESP32 Voice-Controlled LED (TinyML + Edge Impulse)

Offline voice control using **ESP32**, **INMP441 microphone**, and **Edge Impulse TinyML**.  
Say **“on”** or **“off”** to control an LED.
---

## Project Overview

<img width="1600" height="1200" alt="image" src="https://github.com/user-attachments/assets/ddab8a07-1337-4488-9b9a-60efb9f3d766" />


This project demonstrates **keyword spotting** on an ESP32 using a trained **quantized (int8) neural network** deployed via **TensorFlow Lite Micro**.

### Features
- Offline voice recognition
- Real-time inference on ESP32
- INMP441 I2S microphone support
- Edge Impulse ML pipeline
- Extendable to relays, fans, and appliances

---

## How It Works
```
Voice ("on"/"off")
↓
INMP441 Microphone (I2S)
↓
ESP32 (16 kHz audio)
↓
MFCC feature extraction
↓
TinyML Neural Network (int8)
↓
ON / OFF classification
↓
GPIO → LED / Relay
```

---

## Hardware Required

| Component | Description |
|--------|------------|
| ESP32 | ESP32 Dev Module / WROOM |
| INMP441 | I2S MEMS microphone |
| LED or Relay | Output device |
| Resistor | 220Ω for LED |
| Breadboard + Wires | Connections |

---

## Wiring

### INMP441 → ESP32

| INMP441 | ESP32 |
|--------|------|
| VCC | 3.3V |
| GND | GND |
| SCK | GPIO 14 |
| WS | GPIO 15 |
| SD | GPIO 32 |
| L/R | GND |

### LED → ESP32

| LED | ESP32 |
|----|------|
| Anode | GPIO 25 (via resistor) |
| Cathode | GND |

Replace the LED with a relay module for real appliances.

---

## Model Training (Edge Impulse)

### Dataset
- Labels: `on`, `off`, `noise`
- Sample length: `1 second`
- Frequency: `16,000 Hz`
- Samples:
  - `on`: 20
  - `off`: 20
  - `noise`: 20

### DSP
- MFCC
  - Frame length: `0.02` seconds
  - Frame stride: `0.01` seconds
  - Coefficients: `13`

### Classifier
- Neural Network (1D CNN)
- Quantized **int8**
- Achieved accuracy: ~89%

---

## Deployment

- Deployment type: **Arduino Library**
- Optimization: **Quantized (int8)**
- Installed via:
```Sketch → Include Library → Add .ZIP Library```
