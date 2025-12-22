## Pyramid Sentinel – Hardware Assembly Manual

### 1. Overview

This guide explains how to physically build the Pyramid Sentinel:

- **Neuro‑Core**: 1× ESP32 Dev Board (30‑pin) handling sensors, TFT, WiFi AP, siren.
- **Cameras**: 4× AI‑Thinker ESP32‑CAM modules (NORTH, EAST, SOUTH, WEST).
- **Sensors**: 2× PIR (HC‑SR501), 1× Ultrasonic (HC‑SR04).
- **Outputs**: ILI9341 TFT, 3× LEDs, buzzer, ARM button.
- **Power**: 5 V, ≥3 A (USB hub, bench supply, or power bank).

---

### 2. Parts Checklist

- **Brains**
  - 1× ESP32 Dev Board (30‑pin) – Neuro‑Core.
  - 4× AI‑Thinker ESP32‑CAM – camera nodes.
- **Sensors**
  - 2× HC‑SR501 PIR motion sensors.
  - 1× HC‑SR04 Ultrasonic distance sensor.
- **Outputs**
  - 1× ILI9341 SPI TFT (320×240).
  - 3× 5 mm LEDs (red, yellow, green).
  - 3× 220 Ω resistors (for LEDs).
  - 1× Passive buzzer (3–5 V).
  - 1× Tactile push button.
- **Power & wiring**
  - 5× USB cables or 5 V wiring.
  - 5 V, ≥3 A supply (powered USB hub / bench PSU / large power bank).
  - Breadboard or perfboard, jumper wires (M‑M and M‑F).
- **Mechanical**
  - Cardboard / foamboard / acrylic sheets for the pyramid.
  - Hot glue or strong double‑sided tape.
  - Zip ties / adhesive cable clips.

---

### 3. Neuro‑Core Wiring (ESP32 Dev Board)

Use this mapping (matches `brain.cc` / `Blueprint.md`).

#### 3.1 Power Rails

- 5 V rail:
  - Feeds HC‑SR501 PIRs, HC‑SR04 Ultrasonic, and ESP32 5 V pin (if used).
- 3.3 V rail:
  - Feeds TFT and any 3.3 V logic.
- GND:
  - **All** modules must share common ground.

#### 3.2 Ultrasonic Sensor (HC‑SR04)

- VCC → **5 V**
- GND → **GND**
- TRIG → **GPIO 32**
- ECHO → **GPIO 33**

#### 3.3 PIR Sensors (HC‑SR501)

- **PIR 1**
  - VCC → **5 V**
  - GND → **GND**
  - OUT → **GPIO 16**

- **PIR 2**
  - VCC → **5 V**
  - GND → **GND**
  - OUT → **GPIO 17**

#### 3.4 Status LEDs

Each LED anode passes through 220 Ω to the GPIO; all cathodes go to GND.

- Red LED anode → 220 Ω → **GPIO 25**, cathode → **GND**
- Yellow LED anode → 220 Ω → **GPIO 26**, cathode → **GND**
- Green LED anode → 220 Ω → **GPIO 27**, cathode → **GND**

#### 3.5 Buzzer

- Buzzer **+** → **GPIO 13**
- Buzzer **–** → **GND**

#### 3.6 ARM/DISARM Button

- One button leg → **GPIO 14**
- Other button leg → **GND**

(`brain.cc` configures this as `INPUT_PULLUP`, so pressed = LOW.)

#### 3.7 TFT Display (ILI9341 SPI, 320×240)

TFT → ESP32 Dev Board:

- VCC → **3.3 V**
- GND → **GND**
- CS → **GPIO 15**
- RESET → **GPIO 4**
- DC/RS → **GPIO 2**
- MOSI → **GPIO 23**
- SCK → **GPIO 18**
- MISO → **GPIO 19**
- LED/BL → **3.3 V** (or 3.3 V via a small resistor if you want dimmer backlight).

Double‑check all SPI connections before power‑up.

---

### 4. ESP32‑CAM Wiring (Each Camera Node)

#### 4.1 Power

- 5 V → ESP32‑CAM 5 V pin.
- GND → common **GND** (shared with Neuro‑Core).

Use a powered USB hub or a 5 V rail with enough current for all four ESP32‑CAMs.

#### 4.2 Programming with FTDI (Desktop Arduino IDE)

For each ESP32‑CAM when flashing:

- ESP32‑CAM **U0R** → FTDI **TX**
- ESP32‑CAM **U0T** → FTDI **RX**
- ESP32‑CAM **GND** → FTDI **GND**
- ESP32‑CAM **IO0** → **GND** (only during flashing)
- Power the module from 5 V (from FTDI if supported or external 5 V supply with common GND).

Upload the correct sketch (`braincam1`–`braincam4`) to each camera.  
After uploading:

- Remove IO0‑GND jumper.
- Reset the ESP32‑CAM to boot normally.

#### 4.3 Optional Status LED

Your firmware uses **GPIO 33** as `LED_STATUS`. Optional external LED:

- LED anode → 220 Ω → **GPIO 33**
- LED cathode → **GND**

This is not mandatory; the on‑board flash LED can still be used for illumination.

---

### 5. Pyramid Body Construction

#### 5.1 Dimensions (recommended)

- Base square: **30 cm × 30 cm**.
- Height: **25 cm** (approx.).
- Four identical triangular faces.

You can scale these dimensions, but keep enough room for the TFT and internals.

#### 5.2 Cutting Panels

1. Cut **4 identical isosceles triangles** for the faces.
2. Cut **1 base square** for the bottom.
3. Decide on one removable section:
   - Either make one face removable, or
   - Attach the base with screws/Velcro so it can open for access.

#### 5.3 Mark Component Locations

**Front (NORTH) face**

- Near upper third: circular/oval opening for **Camera 1** (ESP32‑CAM lens).
- Middle: rectangular window for **TFT display**.
- Bottom‑center: two round holes for **HC‑SR04** (TRIG/ECHO transducers).
- Mid‑side: hole for **PIR 1**.
- Near bottom or side: hole for **ARM button**.

**Other faces**

- EAST, SOUTH, WEST faces:
  - One opening each for **Cameras 2–4**.
- SOUTH (rear) face:
  - Hole for **PIR 2**.

**Pyramid apex**

- Three small holes for **Red, Yellow, Green LEDs**.

---

### 6. Mounting Components

#### 6.1 Cameras

For each ESP32‑CAM:

- Position the module inside the pyramid with the camera module poking through its face opening.
- Fix the PCB using:
  - Hot glue, or
  - Small 3D‑printed brackets, or
  - Foam tape and cable ties.
- Route the USB/power cable from each camera down toward the base or out the back.

#### 6.2 TFT Screen

- Place TFT directly behind its front cutout.
- Secure using:
  - Screws and standoffs (if the PCB has mount holes), or
  - Strong double‑sided tape.
- Ensure the ribbon cables and connections to the ESP32 Dev Board are not under strain.

#### 6.3 Neuro‑Core ESP32

- Mount the ESP32 Dev Board on:
  - An internal platform, or
  - The base plate using standoffs or foam tape.
- Keep wires short and grouped:
  - One bundle for TFT,
  - One for sensors,
  - One for LEDs and buzzer.
- Make sure you can still reach the USB port and reset/boot buttons with the pyramid opened.

#### 6.4 Sensors, LEDs, and Button

- **PIRs**:
  - Mount PIR 1 on the NORTH/front face, mid‑height.
  - Mount PIR 2 on the SOUTH/rear face, mid‑height.
- **Ultrasonic**:
  - Mount at the bottom of the front face, level with the ground and pointing straight outward.
- **LEDs**:
  - Insert LEDs at the apex so they are visible from all sides.
  - Run their wires down along an inside edge to the Neuro‑Core.
- **Buzzer**:
  - Place on the internal platform or near a vent so sound is clear.
- **Button**:
  - Install in its hole on the front or base where you can easily press it.

#### 6.5 Cable Management

- Route all wires along edges or corners of the pyramid.
- Bundle related wires together with zip ties.
- Avoid blocking camera fields of view or covering sensor windows.

---

### 7. Power Distribution

#### 7.1 Recommended: Powered USB Hub

- Use a **powered USB hub** rated at **≥3 A @ 5 V**:
  - Port 1 → ESP32 Dev Board (Neuro‑Core).
  - Ports 2–5 → ESP32‑CAM #1–#4.
- This automatically provides common ground and simplifies wiring.

#### 7.2 Alternative: Bench Supply or 5 V Rail

- Use a 5 V regulated output feeding a power rail:
  - 5 V rail → ESP32 Dev Board 5 V pin and each ESP32‑CAM 5 V pin.
  - GND rail → all GND pins of every module.
- Double‑check polarity and avoid >5 V.

---

### 8. Bring‑Up and Test Procedure

#### 8.1 Flash Neuro‑Core (`brain.cc`)

1. Connect ESP32 Dev Board to your PC via USB.
2. In Arduino IDE:
   - Board: **ESP32 Dev Module** (or your exact 30‑pin variant).
   - Set correct COM port.
3. Upload `brain.cc` sketch.
4. Open Serial Monitor at **115200 baud** and reset the board.
5. Confirm:
   - Boot log shows AP setup.
   - TFT shows SENTINEL OS boot animation.
   - Wi‑Fi network **`PyramidNet`** appears (password `pointbreak`).

#### 8.2 Flash Cameras (`braincam1.cc`–`braincam4.cc`)

For each ESP32‑CAM:

1. Connect via FTDI (U0R/U0T/GND, IO0→GND for flashing).
2. In Arduino IDE:
   - Board: **AI Thinker ESP32‑CAM**.
   - Core version: **esp32 1.0.6** (or another 1.0.x with ESP‑CAM face libs).
3. Upload the appropriate sketch:
   - Cam 1 → `braincam1` (NORTH, 192.168.4.2).
   - Cam 2 → `braincam2` (EAST, 192.168.4.3).
   - Cam 3 → `braincam3` (SOUTH, 192.168.4.4).
   - Cam 4 → `braincam4` (WEST, 192.168.4.5).
4. Remove IO0‑GND jumper and reset each camera.
5. Check Serial Monitor:
   - Each node connects to `PyramidNet` and prints its static IP.
   - WebSocket connection to the brain is established.

#### 8.3 Network Verification

1. On a laptop/phone, connect to Wi‑Fi:
   - SSID: `PyramidNet`
   - Password: `pointbreak`
2. From a terminal/command prompt:
   - Ping Neuro‑Core: `ping 192.168.4.1`
   - Ping cameras:
     - `ping 192.168.4.2`
     - `ping 192.168.4.3`
     - `ping 192.168.4.4`
     - `ping 192.168.4.5`

All should respond with low latency (<10 ms on average).

#### 8.4 Dashboard Test

1. On the laptop connected to `PyramidNet`, open `pyramid.html` from the project folder
   (or serve it via a simple local web server).
2. Verify:
   - Tiles show **System Status**, **Last Alert**, and **Active Camera**.
   - WebSocket status shows **CONNECTED** when the brain WebSocket is up.
3. For each camera:
   - Click **Load** under the sector tile.
   - Confirm you see MJPEG video.

#### 8.5 Sensor and Alert Test

1. ARM the system:
   - Press the physical ARM button, or
   - Click **ARM** in the dashboard.
2. Trigger PIRs:
   - Walk in front of each PIR; observe TFT threat level and dashboard logs.
3. Trigger Ultrasonic:
   - Move a hand close to the ultrasonic; watch proximity and threat LEVEL tiles.
4. Trigger camera detection:
   - Walk into view; wait for face detection to trigger HUMAN_TARGET alerts
     and confirm logs + AI overlay on the web dashboard.

If all of the above work, your hardware assembly and wiring are correct.

---

### 9. Demo Tips

- Keep the pyramid open for judges to see:
  - Neuro‑Core board.
  - Sensor wiring.
  - ESP32‑CAM nodes behind each face.
- Emphasize:
  - **Edge AI**: detection computed on ESP32‑CAMs, not in the cloud.
  - **Distributed design**: 4 camera nodes + central brain.
  - **Offline operation**: works on its own Wi‑Fi network, no internet needed.


