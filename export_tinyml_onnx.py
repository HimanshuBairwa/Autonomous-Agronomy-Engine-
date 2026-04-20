import numpy as np
from sklearn.ensemble import IsolationForest
from skl2onnx import convert_sklearn
from skl2onnx.common.data_types import FloatTensorType

def main():
    print("[INFO] Initializing Autonomous Agronomy TinyML Export...")
    
    # 1 simmulate agricultural machine sensor Data (Vibration, Acoustic)
    # 1000 normal operating samples, 3 sensor features (matching the sponsor's dataset requirements)
    X_train = np.random.normal(0, 0.1, (1000, 3))
    
    # 2 train a lightweight Anomaly detector suitable for edge devices
    print("[INFO] Training Isolation Forest for Edge Anomaly Detection...")
    model = IsolationForest(n_estimators=50, random_state=42)
    model.fit(X_train)
    
    # 3 convert the python model to C++ compatible ONNX format
    # we define the input tensor shape for 3 floating-point sensor inputs
    initial_type = [('sensor_input', FloatTensorType([None, 3]))]
    
    try:
        onnx_model = convert_sklearn(model, initial_types=initial_type, target_opset={'': 15, 'ai.onnx.ml': 3})

        
        # 4 save the compiled model for the ESP32 firmware
        onnx_filename = "embedded_anomaly_detector.onnx"
        with open(onnx_filename, "wb") as f:
            f.write(onnx_model.SerializeToString())
            
        print(f"[SUCCESS] Model successfully compiled and exported to {onnx_filename}")
        print("[DEPLOY] Ready for C++ implementation on ESP32 Microcontroller.")
    except Exception as e:
        print(f"[ERROR] ONNX Export failed: {e}")

if __name__ == "__main__":
    main()
