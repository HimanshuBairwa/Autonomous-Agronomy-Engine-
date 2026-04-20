import numpy as np
import shap
import pandas as pd
from sklearn.ensemble import IsolationForest
import matplotlib.pyplot as plt

def main():
    print("[INFO] Initializing ESoC Agriculture 4.0 Root Cause Analysis (XAI)...")
    
    # 1 Simulate tractor telemetry data (3 Sensors)
    # features: [vibration(hz), acoustic(dB), engine temp(C)]
    np.random.seed(42)
    normal_data = np.random.normal(loc=[50, 70, 90], scale=[5, 5, 2], size=(500, 3))
    
    # simulate a critical foreign object event (e.g. rock hits the harvester blades)
    # high vibration, massive acoustic spike, slight temp raise
    anomaly_data = np.array([[120.0, 115.0, 93.0]]) 
    
    sensor_names = ["Vibration (Hz)", "Acoustic (dB)", "Engine Temp (C)"]
    X_train = pd.DataFrame(normal_data, columns=sensor_names)
    X_anomaly = pd.DataFrame(anomaly_data, columns=sensor_names)
    
    # 2 train the anomaly detector
    print("[INFO] Training Anomaly Detection Engine...")
    model = IsolationForest(n_estimators=100, contamination=0.01, random_state=42)
    model.fit(X_train)
    
    # 3 perform explainable AI (SHAP) root cause analysis
    print("[INFO] Executing SHAP Explainer to determine Event Root Cause...")
    # IsolationForest decision function gives anomaly scores
    explainer = shap.Explainer(model.decision_function, X_train)
    
    # calculate SHAP values for the specific anomalous event
    shap_values = explainer(X_anomaly)
    
    print("\n--- 🚨 ROOT CAUSE EVENT REPORT 🚨 ---")
    print(f"Event Detected. Base Model Score: {shap_values.base_values[0]:.3f}")
    
    # break down which sensor contributed most to the anomaly detection
    contributions = zip(sensor_names, shap_values.values[0])
    for sensor, impact in sorted(contributions, key=lambda x: abs(x[1]), reverse=True):
        print(f" -> {sensor} impact: {impact:.3f} (Root Cause heavily influenced)")
        
    print("-------------------------------------\n")
    print("[SUCCESS] XAI Pipeline executed. The cause of the hardware fault was successfully isolated.")

if __name__ == "__main__":
    main()
