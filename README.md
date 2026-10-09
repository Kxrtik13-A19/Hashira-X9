<div align="center">

# █ HASHIRA X9 █
### Decentralized Edge-IoT Security & Disaster Mitigation Grid

[![OS Kernel](https://img.shields.io/badge/OS_Kernel-v8.1.0_Dark--Tech-00FFFF?style=for-the-badge&logo=linux)](#)
[![Hardware](https://img.shields.io/badge/Hardware-ATMega328P-000080?style=for-the-badge&logo=arduino)](#)
[![Status](https://img.shields.io/badge/Status-RESTRICTED_%7C_OPERATIONAL-FF0000?style=for-the-badge)](#)
[![Architect](https://img.shields.io/badge/Lead_Architect-Kartik_Kumar-00FF00?style=for-the-badge)](#)

> *"Engineering is a core biological drive. I cannot live without innovation."*

</div>

---

## ▰ THE SECURITY DEFICIT
Modern security infrastructure is fundamentally flawed. It relies on passive, one-dimensional deterrents (like deadbolts and glass-break alarms) that only react *after* a breach has occurred. Furthermore, traditional reinforced vaults protect against physical theft but act as fatal furnaces during undetected electrical fires or invisible hydrocarbon gas leaks.

The **Hashira X9** was engineered to obliterate this deficit. Named after the elite *Hashira* (Pillars) from Demon Slayer, this project represents the pinnacle of strength, style, and automated guardianship. It shifts security from passive property deterrence to **active, autonomous environmental interception.**

## ▰ SYSTEM ARCHITECTURE: THE HARDWARE MATRIX
The X9 operates on a highly decentralized, modular hardware array. Rather than relying on a single point of failure, it utilizes sensor fusion to aggregate physical, spatial, and ecological data in real-time.

| Component | Protocol | Kinematic / Ecological Function |
| :--- | :--- | :--- |
| **ATMega328P Core** | UART / I2C / SPI | The central logic processing unit handling FSM transitions. |
| **MPU6050 MEMS** | I2C (Fast-mode) | 6-Axis Gyro/Accelerometer. Tracks sub-millimeter structural shifts, tampering, or seismic anomalies (earthquakes). |
| **MQ-2 Chemiresistor** | Analog (ADC) | Detects volatile hydrocarbons (LPG, Hydrogen, Smoke) at the pre-ignition phase to prevent catastrophic events. |
| **DHT11 Sensor** | 1-Wire Digital | Persistent baseline microclimate tracking (Temperature & RH%). |
| **MFRC522 Scanner** | SPI Bus | 13.56MHz cryptographic RFID handshake. Handles secure system overrides and mode disarming. |
| **RCWL / Laser / LDR**| Digital GPIO | Microwave Doppler Shift and optical tripwires for absolute spatial denial. |

---

## ▰ KERNEL DEEP DIVE: NON-BLOCKING FSM
Amateur embedded systems rely on blocking wait states (`delay()`), which cause CPU stalls. If a fire starts while an amateur system is "waiting" for an LED to blink, the system is blind. 

The Hashira X9 utilizes a **Deterministic Non-Blocking C++ Kernel**. By leveraging hardware timers (`millis()`) and the `avr/wdt.h` watchdog integration, the OS executes continuous, concurrent telemetry threads. The execution loop strictly polls the entire multi-vector array every **250 milliseconds**—resulting in zero blind spots and lethal threat accuracy.

### Tri-State Quantum Logic (The Modes)
The system operates dynamically across three cryptographic states:

*   🟢 **WIND MODE [Baseline Posture]** 
    Relaxed operational state. The system focuses on ambient atmospheric tracking, standard access control, and capacitive doorbell signaling without generating false alarms.
*   🔵 **MIST MODE [Elevated Vigilance]**
    Environmental net is fully activated. Zero-tolerance polling for gas concentrations and infrared flame anomalies. Unauthorized motion triggers pre-lockdown acoustic warnings.
*   🔴 **SERPENT MODE [Absolute Denial]**
    Total perimeter lockdown. Microwave radar and optical laser tripwires lock the spatial dome. The MPU6050 sensitivity is maxed out. Can only be disarmed via a hardcoded hexadecimal RFID Master Key override.

### The "Abyssal Protocol" Acoustic Engine
Standard alarms are abrasive and annoying. The X9 utilizes a custom polyphonic acoustic synthesizer mathematically timed to a 170 BPM grid. It generates cinematic, sci-fi audio feedback and a heavy, dissonant synth sequence during lockdown events, elevating the aesthetic of the hardware.

---

### Phase II Expansion (Future Scope)
The X9 is currently a localized node. Phase II will introduce **Mesh Integration** (ESP32-driven cloud telemetry) and **Quantum Machine Learning**, allowing the system to use localized neural networks to filter out false positives (e.g., ignoring a pet walking by, but locking down for a human intruder).

---

## ▰ LEGAL RESTRICTIONS & LICENSING
**PROJECT IDENTIFIER:** HASHIRA X9 (X9-PROT-2026-NAT)

⚠️ **STRICTLY PROPRIETARY & CONFIDENTIAL**
This repository serves as a read-only portfolio exhibit. The source code, circuit schematics, FSM architecture, and sensor fusion algorithms contained within this project are strictly protected under dual-jurisdiction law (Republic of India & United States of America). 

Copyright © Kartik Kumar. All Right Reserved
*   **No Cloning:** You may not reproduce, compile, or manufacture this hardware.
*   **No Derivation:** Reverse-engineering or idea extraction for academic or commercial use is strictly prohibited. 
*   **All Rights Reserved © 2026 Kartik Kumar.**
