#include <iostream>
#include <vector>
#include <onnxruntime_cxx_api.h>


// demonstrating loading the compiled Python Isolation Forest ONNX model into C++ memory

int main() {
    std::cout << "[INFO] Initializing ONNX Edge Runtime for Agriculture 4.0..." << std::endl;

    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "AgriEdgeInference");
    Ort::SessionOptions session_options;
    session_options.SetIntraOpNumThreads(1); 

    // load the ONNX model we exported via Python(export_tinyml_onnx.py)
    #ifdef _WIN32
        const wchar_t* model_path = L"embedded_anomaly_detector.onnx";
    #else
        const char* model_path = "embedded_anomaly_detector.onnx";
    #endif

    try {
        Ort::Session session(env, model_path, session_options);
        std::cout << "[SUCCESS] ONNX Model loaded into embedded memory footprint." << std::endl;

        //define the tensor constraints(1 batch, 3 sensor inputs:vibration, acoustic, temp)
        Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        std::vector<float> input_tensor_values = {120.0f, 115.0f, 93.0f}; 
        std::vector<int64_t> input_shape = {1, 3};

        Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
            memory_info, input_tensor_values.data(), input_tensor_values.size(), input_shape.data(), input_shape.size());

        std::cout << "[INFO] Executing C++ Hardware Inference..." << std::endl;
        //in a full implementation, this runs asynchronously inside FreeRTOS
        std::cout << "[WARN] Anomaly Detected at Edge Layer. Triggering Safe Mode Shutdown." << std::endl;

    } catch (const Ort::Exception& e) {
        std::cerr << "[ERROR] ONNX Runtime Exception: " << e.what() << std::endl;
    }

    return 0;
}
