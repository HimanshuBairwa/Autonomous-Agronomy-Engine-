# 🌾 Autonomous Agronomy Engine (Edge AI for Agriculture 4.0)

> **Status:** Architecture and firmware codebase open-sourced specifically for ESoC 2026 Batch 2 (Applied Project: Embedded AI for Predictive Sensor Systems in Agriculture 4.0).

This repository outlines the core architecture of a world-class, fault-tolerant edge-computing agriculture system. It transitions a standard IoT soil-moisture setup into a **predictive mathematical decision engine**. Built on the ESP32, it executes TinyML neural networks, mitigates field hardware failure via 5-layered health scoring, and directly computes thermodynamic spatial-temporal variables entirely on the edge.

---

## 🏗️ 1. The 4 Modes of Operation (State Machine)
The core architecture operates under four strict operational states to guarantee zero crop desiccation:
- **MANUAL MODE**: Standard fallback. The AI is detached, and the user controls the irrigation pump manually via the web dashboard.
- **BASIC AUTO**: The traditional "IoT" approach. Relies purely on the physical soil moisture sensor.
- **SMART AUTO (The Core AI)**: The primary engine. Combines physical sensor data with cloud climate data (temperature, pressure, humidity) using a dynamic mathematical weighting matrix.
- **EDGE AI PREDICTIVE (TinyML)**: Predicts the irrigation load by executing an embedded Neural Network—trained on historical spatial-temporal CSV data—directly on the ESP32 (via Edge Impulse).

---

## 🛡️ 2. The 3 Layers of Defense (Sensor Fault Tolerance)
Agricultural machinery operates in highly volatile physical environments. If a sensor degrades, the system does not fail; it enters **SAFE MODE**. Our Anomaly Detection algorithm scores the sensor from 1.0 (perfect) to 0.0 (dead) using 5 rules:
1. **Physical Disconnect:** Detects if the ADC hits 0 or max logic voltage.
2. **Stuck Sensor:** Scans the 30-second rolling history buffer to see if the chemical reading is frozen.
3. **Volatility Detection:** Flags unnatural jumps (>40% swing) indicating electrical noise.
4. **Climate Disagreement:** If the soil reads 100% wet, but weather telemetry reads 40°C with 10% humidity, trust scores degrade gracefully.
5. **Dual Failure Strategy:** Graceful collapse into historical extrapolation if both DHT and Soil break.

*If `HealthScore < 0.4`, the decision engine formally shifts trust 100% to meteorological data and the SPIFFS historical trend.*

---

## 🧠 3. The Intelligence Layer (Thermodynamic Math)
Unlike standard `if (temp > 30)` state machines, this system relies on rigorous scientific modeling:

*   **Vapor Pressure Deficit (VPD):** Evaporation is computed using the **Tetens Formula** for thermodynamic VPD, taking Temperature and Humidity to calculate exact kPa atmospheric stress.
*   **Logistic Sigmoid Drought Physics:** Modeled via an **S-Curve** (`1 / (1 + e^(-k*(x-x0)))`). Stress is ignored for initial days, spikes violently on critical days, and asymptotes to pure desiccation.
*   **FAO-56 Effective Rainfall Hydrology:** Predicts rain via API and calculates the **Effective Rain** (80% absorption), directly subtracting the millimeter volume from current irrigation demand.
*   **Dynamic Interpolated Truth Weights:** `(wSensor * SoilDryness) + (wClimate * ETScore)`. Weights linearly interpolate to always sum to 1.0 based on physical hardware health.

---

## ⚙️ 4. Micro-Pulse Duty Cycling
To prevent hydrophobic surface runoff (where water slides off dry dirt instead of sinking), the system forces **Pulse Irrigation**. The asynchronous engine turns the pump ON for 2s, OFF for 2s (capillary soak phase), overriding standard static intervals.

---

## 📊 5. Edge MLOps & Data Analytics
*   **Local Aggregation:** The ESP32 logs thousands of points a day into 64-bit IEEE-754 accumulators, burning daily summaries into non-volatile SPIFFS flash memory.
*   **CI/CD Pipeline Integration:** Edge-aggregated CSVs are structured for direct MLOps ingestion to retrain the Edge Impulse neural network weights dynamically without human intervention.
Real-time visual reporting of the hardware `HealthScore` and predictive Irrigation Demand.
