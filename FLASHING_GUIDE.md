# 🏔️ PYRAMID SENTINEL: FLASHING & DEPLOYMENT GUIDE
**Project Version:** 5.0.0 "TITANIUM"

This guide explains how to upload the "Sentinel" software to your hardware using the unified ESP-IDF build system.

---

## 🛠️ PREREQUISITES
1. **ESP-IDF Environment:** Ensure you have the [Espressif IDE](https://dl.espressif.com/dl/esp-idf/) or the VS Code ESP-IDF Extension installed.
2. **Hardware Drivers:** Install the CP210x or CH340 drivers (depending on your board) so your PC can "see" the COM ports.
3. **Hardware Check:**
   - **Brain:** ESP32 Dev Board (30-pin).
   - **Cameras:** ESP32-CAM (AI-Thinker model).

---

## 🚀 THE "SWITCH" MECHANISM
Because this is a unified dashboard, you must tell the compiler *which* board you are currently flashing. You do this by editing exactly **one line** in the project.

### File to Edit: `main/CMakeLists.txt`

| To Flash This... | Change the `SRCS` line to: |
| :--- | :--- |
| **Neuro Core (Brain)** | `idf_component_register(SRCS "core_main.cpp"` |
| **Camera 1 (North)** | `idf_component_register(SRCS "cam_main.cpp"` |
| **Camera 2 (East)** | `idf_component_register(SRCS "cam_main2.cpp"` |
| **Camera 3 (South)** | `idf_component_register(SRCS "cam_main3.cpp"` |
| **Camera 4 (West)** | `idf_component_register(SRCS "cam_main4.cpp"` |

---

## 📥 UPLOAD STEPS (VS CODE)
1. **Open the Project:** Open the `sentinel-dashboard` folder in VS Code.
2. **Select Port:** Click the **COM Port** icon in the bottom toolbar and select your board.
3. **Select Target:** Ensure the target is set to `esp32` (bottom toolbar).
4. **Set Source:** Edit `main/CMakeLists.txt` (as shown above) to select the correct code.
5. **Flash:** Click the **⚡ Flash** icon (Thunderbolt) or the **"Build, Flash and Monitor"** icon (Flame/Phone icon).

---

## 📥 UPLOAD STEPS (COMMAND LINE)
Open your terminal in the root folder and run:

```powershell
# 1. Select the source in main/CMakeLists.txt first!

# 2. Build and Flash
idf.py -p COM[PORT] flash monitor
```
*(Replace `COM[PORT]` with your port, e.g., `COM3`)*

---

## 🕵️ TROUBLESHOOTING
*   **"Brownout detector was triggered":** Your USB port isn't giving enough power. Use a powered USB hub or a better cable.
*   **"Failed to connect to ESP32":** 
    - For the **Dev Board**: Hold the `BOOT` button while the terminal says "Connecting...".
    - For the **ESP32-CAM**: Ensure the `IO0` pin is connected to `GND` during flashing!
*   **"Camera Not Found":** Triple-check that the camera ribbon cable is seated perfectly in the connector.

---

## 📡 POST-FLASHING VERIFICATION
1. Open the **Serial Monitor** (115200 baud).
2. Look for the message: `PHASE_1: NETWORK ONLINE` or `KERNEL_FULLY_DEPLOYED`.
3. The Brain will create a WiFi network named: **"PyramidNet"**.
4. Connect your phone/PC to "PyramidNet" (Pass: `pointbreak`) and navigate to `192.168.4.1`.

---

> **Note:** If you modify physical pins in the code, remember to update the labels in `brain.cc` and `core_main.cpp` headers to keep your documentation accurate.
