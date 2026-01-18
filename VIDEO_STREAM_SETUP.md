# 📹 Sentinel System: Video Stream Setup Guide

To get your 3-camera surveillance system streaming video to your dashboard, follow these exact steps.

---

### 1. Hardware Preparation
Ensure you have the following hardware ready:
*   **1x ESP32 (Core/Hub)**: This acts as the brain and WiFi router.
*   **3x ESP32-CAM Units**: These are your camera nodes.
*   **Power Supply**: Ensure each ESP32-CAM has a stable 5V power source (they draw significant current during streaming).

---

### 2. Flashing the Code
You must upload the specific files to each device using the Arduino IDE or VS Code/PlatformIO.

| Device | File to Upload | Role | Static IP |
| :--- | :--- | :--- | :--- |
| **Main ESP32** | `core_main.cpp` | WiFi Hub & Dashboard Host | `192.168.4.1` |
| **Camera 1** | `cam_main.cpp` | North Sector Streamer | `192.168.4.2` |
| **Camera 2** | `cam_main2.cpp` | East Sector Streamer | `192.168.4.3` |
| **Camera 3** | `cam_main3.cpp` | South Sector Streamer | `192.168.4.4` |

> **Note**: When uploading to ESP32-CAM, ensure you select the **"AI Thinker ESP32-CAM"** board in your IDE settings.

---

### 3. Establishing the Network
1.  Power on the **Main ESP32** first.
2.  Wait about 10 seconds for it to create the WiFi network.
3.  Power on all **three ESP32-CAM units**. They will automatically search for and connect to the "PyramidNet" WiFi created by the Core.

---

### 4. Connecting Your Computer
1.  On your computer, open your WiFi settings.
2.  Connect to the network named **"PyramidNet"**.
3.  Use the password: `pointbreak`

---

### 5. Accessing the Video Stream
Once connected to the WiFi, you have two ways to see the video:

#### A. Via the Integrated Dashboard (Recommended)
1.  Open the `pyramid.html` file in any modern web browser (Chrome or Edge recommended).
2.  The dashboard will automatically connect to the Core via WebSockets.
3.  Click the **"Load Stream"** button on any camera tile to start the live MJPEG video feed.

#### B. Direct Stream Access
If you want to test a camera individually, you can enter these URLs directly into your browser:
*   **Cam 1**: `http://192.168.4.2/stream`
*   **Cam 2**: `http://192.168.4.3/stream`
*   **Cam 3**: `http://192.168.4.4/stream`

---

### 🛠️ Troubleshooting
*   **No Video?** Check if the ESP32-CAM is getting enough power. If the onboard "Flash" LED blinks or the device restarts, the power supply is too weak.
*   **Can't Connect?** Ensure your computer is on the **PyramidNet** WiFi. You cannot access the cameras if you are on your home/office WiFi.
*   **Laggy Video?** The system uses MJPEG. Ensure the cameras are within good range of the Main ESP32 hub.
