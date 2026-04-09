<p align="center">
  <h1 align="center">🌾 Autonomous Agronomy Engine (Edge AI for Agriculture 4.0)</h1>
  <p align="center">
    <strong>Fault-Tolerant, Climate-Aware, Micro-Pulse Edge Irrigation Platform</strong>
    <br/>
    <em>Designed for European Summer of Code (ESoC) "Embedded AI in Agriculture 4.0"</em>
  </p>
</p>

---

> [!WARNING]  
> **🚨 ACADEMIC RESEARCH NOTICE 🚨**  
> The core C++ physics engine and embedded neural network weights driving this system are currently **closed-source** as they are undergoing rigorous peer review for an upcoming academic journal publication.  
> 
> To protect proprietary IP, this repository serves as an architectural showcase. The files provided here outline the **FreeRTOS structure, MLOps Integration, and the Executive Telemetry Dashboard**. Stub functions are provided in place of the proprietary thermodynamic and agronomic algorithms. Once the paper is published, the full embedded source code will be made public under an open-source license.

---

## 📖 Executive Summary

Modern agriculture wastes billions of liters of water due to "dumb" timer-based irrigation and linear threshold logic. The **Autonomous Agronomy Engine** is a PhD-grade edge-computing framework deployed on the ESP32. It transcends traditional IoT by actively calculating thermodynamic physics locally and running embedded Machine Learning (TinyML) inference to predict precise crop saturation needs, neutralizing hardware anomalies, and preventing superficial runoff via micro-pulse duty cycling.

![Dashboard Preview Placeholder](dashboard.png)
*(Figure 1: The V3.0 Live Telemetry Web Dashboard, hosted entirely on the ESP32 via AsyncWebServer)*

---

## ⚙️ The 4 Modes of Operation

The state-machine architecture supports four distinct operational pipelines:

1. 🕹️ **MANUAL MODE**: Secure user override bypassing all intelligence layers.
2. 🎛️ **BASIC AUTO**: Standard physical-sensor IoT loop. Acts as an ultimate fallback if Wi-Fi and Edge Neural Networks fail.
3. 🧠 **SMART AUTO (Agronomic Calculus)**: The core engine. Combines physical soil sensor readings with cloud climate forecasts using dynamically interpolated mathematical trust weights.
4. 🤖 **EDGE AI PREDICTIVE**: Evaluates the localized micro-climate using a custom quantized Machine Learning model transpiled directly into raw C++ logic.

---

## 🛡️ The 3 Layers of Defense (Fault Tolerance)

In industrial Agriculture 4.0, hardware breaks. This framework guarantees absolute resilience via a **5-Rule Anomaly Detection** algorithm that dynamically assigns a `HealthScore` (0.0 to 1.0) to hardware metrics.

If an anomaly is detected (e.g., disconnected ADC, frozen reading buffer, massive electrical volatility, or physically impossible climate disagreement), the system gracefully collapses into **SAFE MODE**. Trust weights are dynamically shifted 100% to meteorological modeling and the 7-day non-volatile SPIFFS historical trend buffer to keep crops alive without overflowing fields.

---

## 🧬 The Intelligence Layer

*(Exact mathematical formulas redacted pending publication)*

1. **Vapor Pressure Deficit (VPD) Thermodynamics:** Instead of using raw temperature and humidity, the system calculates the localized VPD using thermodynamic algorithms. This models the exact atmospheric suction pulling water from crop stomata.
2. **Logistic Sigmoid Drought Physics:** Biological crop desiccation is not linear. Our engine tracks time-since-precipitation using an S-Curve (Sigmoid) function, modeling the initial plant resistance followed by the rapid wilting phase.
3. **FAO-56 Effective Rainfall Hydrology:** It calculates expected localized rainfall and immediately generates an offset demand score (factoring in 20% surface runoff), guaranteeing maximum rainwater utilization before the pump is even activated.

---

## 🚰 Micro-Pulse Duty Cycling

Flooding desperately dry soil for 10 seconds straight results in hydrophobic surface runoff. To maximize capillary action geometry, the engine utilizes asynchronous **Pulse Duty Cycling**. Water is dispensed in micro-bursts (e.g., 2s ON, 2s OFF), allowing perfect root saturation and aggressively minimizing thousands of liters of agricultural water waste.

---

## 🖥️ V3.0 Executive Dashboard

The system runs a complete asynchronous UI on the edge-device itself:
- **Glassmorphism CSS Engine:** High performance visual blur rendering.
- **Live Telemetry:** Integrates `Chart.js` for frictionless, zero-latency scrolling data visualization directly from the edge memory state.
- **SVG Radial Confidence Dials:** Real-time visual reporting of the hardware `HealthScore` and predictive Irrigation Demand.
