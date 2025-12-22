# 🏔️ Pyramid Sentinel - Hardware Blueprint & Assembly Guide

**System Version:** v5.0.0-TITANIUM  
**Document Type:** Technical Blueprint & Construction Manual  
**Classification:** Engineering Specification

---

## 📐 System Overview

The Pyramid Sentinel is a 4-sided surveillance system housed in a pyramid structure. Each face contains one ESP32-CAM module providing 60° field of view coverage. The central Neuro-Core ESP32 manages all sensors, displays, and communications.

### Key Specifications:

| Parameter | Value |
|-----------|-------|
| **Structure** | Pyramid (4 triangular faces) |
| **Base Dimensions** | ~30cm × 30cm (adjustable) |
| **Height** | ~25cm (adjustable) |
| **Camera Coverage** | 360° (4×60° + overlap) |
| **Power Requirement** | 5V DC, ~3A total |
| **Network** | WiFi 802.11 b/g/n (2.4GHz) |
| **Operating Range** | WiFi: 30m, PIR: 7m, Ultrasonic: 4m |

---

## 🎯 Component Inventory

### Core Processing Units:

| Component | Quantity | Purpose | Location |
|-----------|----------|---------|----------|
| **ESP32 Dev Board (30-pin)** | 1 | Neuro-Core controller | Center of pyramid |
| **ESP32-CAM (AI-Thinker)** | 4 | Camera modules | One per face |

### Sensors:

| Component | Quantity | Purpose | Location |
|-----------|----------|---------|----------|
| **HC-SR501 PIR Sensor** | 2 | Motion detection | Mid-height, opposite sides |
| **HC-SR04 Ultrasonic** | 1 | Proximity detection | Base, front face |

### Display & Indicators:

| Component | Quantity | Purpose | Location |
|-----------|----------|---------|----------|
| **ILI9341 TFT (320×240)** | 1 | Status display | Front face, center |
| **Red LED (5mm)** | 1 | High threat indicator | Top of pyramid |
| **Yellow LED (5mm)** | 1 | Medium threat indicator | Top of pyramid |
| **Green LED (5mm)** | 1 | Armed status indicator | Top of pyramid |

### Audio & Controls:

| Component | Quantity | Purpose | Location |
|-----------|----------|---------|----------|
| **Passive Buzzer** | 1 | Audio alerts | Near Neuro-Core |
| **Tactile Button** | 1 | ARM/DISARM toggle | Front face, accessible |

### Power & Wiring:

| Component | Quantity | Purpose |
|-----------|----------|---------|
| **USB Cables (Micro-B)** | 5 | Power for ESP32s |
| **Jumper Wires (M-M)** | ~40 | Connections |
| **Jumper Wires (M-F)** | ~20 | Sensor connections |
| **Breadboard (optional)** | 1-2 | Prototyping |
| **Resistors (220Ω)** | 3 | LED current limiting |

---

## 🔌 Neuro-Core Pin Mapping

### ESP32 Dev Board (30-pin) - Complete Pin Assignment:

```
                    ┌─────────────────┐
                    │   ESP32 BOARD   │
                    │   NEURO-CORE    │
                    └─────────────────┘
                            │
        ┌───────────────────┴──────────────────┐
        │                                      │
    GPIO 32 ──── Ultrasonic TRIG               │
    GPIO 33 ──── Ultrasonic ECHO               │
    GPIO 25 ──── Red LED (via 220Ω)            │
    GPIO 26 ──── Yellow LED (via 220Ω)         │
    GPIO 27 ──── Green LED (via 220Ω)          │
    GPIO 23 ──── TFT MOSI (SPI)                │
    GPIO 19 ──── TFT MISO (SPI)                │
    GPIO 18 ──── TFT SCK (SPI)                 │
    GPIO 15 ──── TFT CS (Chip Select)          │
    GPIO  2 ──── TFT DC (Data/Command)         │
    GPIO  4 ──── TFT RST (Reset)               │
    GPIO 13 ──── Buzzer (Passive)              │
    GPIO 14 ──── ARM Button (INPUT_PULLUP)     │
    GPIO 16 ──── PIR Sensor 1 (Digital Input)  │
    GPIO 17 ──── PIR Sensor 2 (Digital Input)  │
        │                                      │
    3.3V ────── TFT VCC, Sensors VCC           │
    GND  ────── Common Ground (All components) │
    5V   ────── (Optional) External power      │
        │                                      │
        └──────────────────────────────────────┘
```

### Pin Usage Summary:

| GPIO | Component | Type | Notes |
|------|-----------|------|-------|
| 32 | Ultrasonic TRIG | Output | 10μs pulse trigger |
| 33 | Ultrasonic ECHO | Input | Pulse width = distance |
| 25 | Red LED | Output | DAC capable pin |
| 26 | Yellow LED | Output | DAC capable pin |
| 27 | Green LED | Output | Touch capable pin |
| 23 | TFT MOSI | SPI | VSPI MOSI |
| 19 | TFT MISO | SPI | VSPI MISO |
| 18 | TFT SCK | SPI | VSPI SCK |
| 15 | TFT CS | Output | Chip select |
| 2 | TFT DC | Output | Data/Command select |
| 4 | TFT RST | Output | Reset (active low) |
| 13 | Buzzer | Output | PWM for tones |
| 14 | ARM Button | Input | Internal pullup |
| 16 | PIR Sensor 1 | Input | Digital HIGH on motion |
| 17 | PIR Sensor 2 | Input | Digital HIGH on motion |

---

## 📷 Camera Module Configuration

### ESP32-CAM Pin Connections (All 4 Units):

Each ESP32-CAM is self-contained with onboard camera. Only power and programming connections needed:

```
ESP32-CAM Module:
┌─────────────────────┐
│  ┌─────────────┐    │
│  │   CAMERA    │    │  ← OV2640 Camera Module
│  └─────────────┘    │
│                     │
│  [ESP32-S Chip]     │
│                     │
│  5V  ──────────────────── Power (+5V)
│  GND ──────────────────── Ground
│  U0R ──────────────────── RX (programming)
│  U0T ──────────────────── TX (programming)
│  IO0 ──────────────────── GND (flash mode)
│                     │
│  GPIO 33 (Built-in) │  ← Camera data pins
│  GPIO 32 (Built-in) │     (internal connections)
│  ...                │
└─────────────────────┘
```

### Camera Network Configuration:

| Camera | Sector | Static IP | WiFi SSID | WiFi Password |
|--------|--------|-----------|-----------|---------------|
| braincam1 | NORTH | 192.168.4.2 | PyramidNet | pointbreak |
| braincam2 | EAST | 192.168.4.3 | PyramidNet | pointbreak |
| braincam3 | SOUTH | 192.168.4.4 | PyramidNet | pointbreak |
| braincam4 | WEST | 192.168.4.5 | PyramidNet | pointbreak |

---

## 🏗️ Pyramid Structure - Four Side Views

### NORTH SIDE - Camera 1 (Front Face)

![NORTH SIDE Blueprint](C:/Users/Admin/.gemini/antigravity/brain/7e0cc679-392c-45bc-aabf-eabab175b0c2/pyramid_north_blueprint_1766335388267.png)

**Component Layout:**

```
                      ▲ (Apex)
                     /│\
                    / │ \
                   /  │  \
                  /   │   \
                 /    │    \
                /  [CAM 1]  \
               /   (NORTH)   \
              /    60° FOV    \
             /                 \
            /  ┌───────────┐   \
           /   │ TFT SCREEN│    \
          /    │ 320×240px │     \
         /     └───────────┘      \
        /                          \
       /        [PIR 1]             \
      /      (Motion Sensor)         \
     /                                \
    /    ┌─────────────────────┐      \
   /     │  NEURO-CORE ESP32   │       \
  /      │  (Central Hub)      │        \
 /       └─────────────────────┘         \
/___________[ULTRASONIC]__________________\
            (HC-SR04)
            Base Level

FIELD OF VIEW:
     ╱────────────────────╲
    ╱  60° Coverage Cone   ╲
   ╱                        ╲
  ●─────────────────────────●
  Detection Range: ~5-7m
```

**Key Features:**
- **Camera 1**: Mounted at upper third of north face
- **TFT Display**: Centered below camera, visible from front
- **PIR Sensor 1**: Mid-height on left side
- **Ultrasonic Sensor**: Base level, centered
- **Neuro-Core**: Internal, center of pyramid
- **Field of View**: 60° cone covering north sector

---

### EAST SIDE - Camera 2

![EAST SIDE Blueprint](C:/Users/Admin/.gemini/antigravity/brain/7e0cc679-392c-45bc-aabf-eabab175b0c2/pyramid_east_blueprint_1766335411053.png)

**Component Layout:**

```
                      ▲ (Apex)
                     /│\
                    / │ \
                   /  │  \
                  / [LED] \
                 /  Array  \
                /  🔴🟡🟢   \
               /   (Top)    \
              /              \
             /   [CAM 2]      \
            /    (EAST)        \
           /     60° FOV        \
          /                      \
         /                        \
        /      ┌──────────┐        \
       /       │ ESP32-CAM│         \
      /        │  Module  │          \
     /         └──────────┘           \
    /              │                   \
   /               │ (Wiring)           \
  /         ┌──────┴──────┐             \
 /          │ NEURO-CORE  │              \
/___________└─────────────┘_______________\
                Base

WIRING DETAIL:
Camera → Neuro-Core:
  Power: 5V + GND
  Data: WiFi (wireless)
  
Internal Cable Routing:
  ┌─────────────┐
  │   CAM 2     │
  └──────┬──────┘
         │ USB Power
         ↓
  ┌─────────────┐
  │ NEURO-CORE  │
  └─────────────┘
```

**Key Features:**
- **Camera 2**: Mounted on east face
- **LED Array**: Red, Yellow, Green at apex
- **Internal Wiring**: USB power cable from base to camera
- **WiFi Connection**: Wireless data to Neuro-Core
- **Coverage**: East sector, 60° FOV

---

### SOUTH SIDE - Camera 3 (Rear Face)

**Component Layout:**

```
                      ▲ (Apex)
                     /│\
                    / │ \
                   /  │  \
                  /   │   \
                 /  [CAM 3] \
                /   (SOUTH)  \
               /    60° FOV   \
              /                \
             /                  \
            /                    \
           /     [PIR 2]          \
          /   (Motion Sensor)      \
         /                          \
        /    ┌─────────────────┐     \
       /     │   NEURO-CORE    │      \
      /      │                 │       \
     /       │  ┌───────────┐  │        \
    /        │  │  BUZZER   │  │         \
   /         │  └───────────┘  │          \
  /          │                 │           \
 /           └─────────────────┘            \
/________________[OPEN BOTTOM]_______________\
                (Access Panel)

INTERNAL COMPONENT VIEW:
┌─────────────────────────────────┐
│         NEURO-CORE ESP32        │
│  ┌─────────────────────────┐    │
│  │ GPIO 32 → Ultrasonic T  │    │
│  │ GPIO 33 → Ultrasonic E  │    │
│  │ GPIO 25 → Red LED       │    │
│  │ GPIO 26 → Yellow LED    │    │
│  │ GPIO 27 → Green LED     │    │
│  │ GPIO 13 → Buzzer        │    │
│  │ GPIO 14 → ARM Button    │    │
│  │ GPIO 16 → PIR 1         │    │
│  │ GPIO 17 → PIR 2         │    │
│  └─────────────────────────┘    │
└─────────────────────────────────┘
```

**Key Features:**
- **Camera 3**: South-facing coverage
- **PIR Sensor 2**: Mid-height on right side
- **Open Bottom**: Access to internal components
- **Buzzer**: Mounted near Neuro-Core
- **GPIO Connections**: All sensors wired to Neuro-Core

---

### WEST SIDE - Camera 4 (Open Side)

**Component Layout - EXPLODED VIEW:**

```
                      ▲ (Apex)
                     /│\
                    / │ \
                   /  │  \
                  / [LED] \
                 /  🔴🟡🟢 \
                /   Array  \
               /            \
              /   [CAM 4]    \
             /    (WEST)      \
            /     60° FOV      \
           /                    \
          /   ╔═══════════════╗  \
         /    ║ OPEN SIDE     ║   \
        /     ║ (Showcase)    ║    \
       /      ║               ║     \
      /       ║  ┌─────────┐  ║      \
     /        ║  │ TFT     │  ║       \
    /         ║  │ DISPLAY │  ║        \
   /          ║  └─────────┘  ║         \
  /           ║  ┌─────────┐  ║          \
 /            ║  │ ESP32   │  ║           \
/             ║  │ NEURO   │  ║            \
              ║  │ CORE    │  ║
              ║  └─────────┘  ║
              ║  ┌─────────┐  ║
              ║  │ PIR 1   │  ║
              ║  └─────────┘  ║
              ║  ┌─────────┐  ║
              ║  │ PIR 2   │  ║
              ║  └─────────┘  ║
              ║  ┌─────────┐  ║
              ║  │ULTRASON │  ║
              ║  └─────────┘  ║
              ╚═══════════════╝
              Base (Open Access)

COMPLETE WIRING DIAGRAM:
                [CAM 1]     [CAM 2]
                   │           │
                   └─────┬─────┘
                         │ (WiFi)
                   ┌─────▼─────┐
    [PIR 1] ──────►│           │◄────── [PIR 2]
                   │  NEURO-   │
    [ULTRA] ──────►│   CORE    │◄────── [BUTTON]
                   │           │
    [BUZZER]◄──────┤  ESP32    │──────► [TFT]
                   │           │
    [LED R] ◄──────┤           │
    [LED Y] ◄──────┤           │
    [LED G] ◄──────┤           │
                   └─────┬─────┘
                         │ (WiFi)
                   ┌─────┴─────┐
                   │           │
                [CAM 3]     [CAM 4]
```

**Key Features:**
- **Open Side**: Full view of internal electronics
- **Component Showcase**: All sensors and boards visible
- **Wiring Harness**: Complete connection diagram
- **Educational Display**: Perfect for demonstrations
- **Camera 4**: West-facing coverage

---

## 🔧 Detailed Wiring Schematics

### TFT Display Connection (SPI):

```
ILI9341 TFT          ESP32 Neuro-Core
┌─────────────┐      ┌─────────────┐
│             │      │             │
│  VCC  ──────┼──────┤ 3.3V        │
│  GND  ──────┼──────┤ GND         │
│  CS   ──────┼──────┤ GPIO 15     │
│  RESET ─────┼──────┤ GPIO 4      │
│  DC   ──────┼──────┤ GPIO 2      │
│  MOSI ──────┼──────┤ GPIO 23     │
│  SCK  ──────┼──────┤ GPIO 18     │
│  LED  ──────┼──────┤ 3.3V        │
│  MISO ──────┼──────┤ GPIO 19     │
│             │      │             │
└─────────────┘      └─────────────┘

SPI Configuration:
- Frequency: 40MHz
- Mode: SPI_MODE0
- Bit Order: MSB First
- Display Size: 320×240 pixels
- Color Depth: 16-bit (RGB565)
```

### PIR Sensor Connection:

```
HC-SR501 PIR         ESP32 Neuro-Core
┌─────────────┐      ┌─────────────┐
│             │      │             │
│  VCC  ──────┼──────┤ 5V or 3.3V  │
│  OUT  ──────┼──────┤ GPIO 16/17  │
│  GND  ──────┼──────┤ GND         │
│             │      │             │
│ [Sensitivity]      │             │
│ [Time Delay]       │             │
│ [Trigger Mode]     │             │
└─────────────┘      └─────────────┘

Settings:
- Sensitivity: Mid to High
- Time Delay: 2-5 seconds
- Trigger Mode: Repeatable (H)
- Detection Range: 3-7 meters
- Detection Angle: 120°
```

### Ultrasonic Sensor Connection:

```
HC-SR04              ESP32 Neuro-Core
┌─────────────┐      ┌─────────────┐
│             │      │             │
│  VCC  ──────┼──────┤ 5V          │
│  TRIG ──────┼──────┤ GPIO 32     │
│  ECHO ──────┼──────┤ GPIO 33     │
│  GND  ──────┼──────┤ GND         │
│             │      │             │
│  [TX]  [RX] │      │             │
│   │     │   │      │             │
└───┴─────┴───┘      └─────────────┘

Operation:
- Trigger: 10μs HIGH pulse
- Echo: Pulse width = distance
- Range: 2cm - 400cm
- Accuracy: ±3mm
- Measurement Angle: 15°
```

### LED Array Connection:

```
LEDs (3x)            ESP32 Neuro-Core
                     ┌─────────────┐
Red LED:             │             │
  Anode ─┬─[220Ω]───┤ GPIO 25     │
  Cathode─┴──────────┤ GND         │
                     │             │
Yellow LED:          │             │
  Anode ─┬─[220Ω]───┤ GPIO 26     │
  Cathode─┴──────────┤ GND         │
                     │             │
Green LED:           │             │
  Anode ─┬─[220Ω]───┤ GPIO 27     │
  Cathode─┴──────────┤ GND         │
                     │             │
                     └─────────────┘

LED Specifications:
- Forward Voltage: 2.0-2.2V (Red/Yellow), 3.0-3.2V (Green)
- Forward Current: 20mA
- Resistor: 220Ω (limits current to ~15mA)
```

### Buzzer Connection:

```
Passive Buzzer       ESP32 Neuro-Core
┌─────────────┐      ┌─────────────┐
│             │      │             │
│  (+) ───────┼──────┤ GPIO 13     │
│  (-) ───────┼──────┤ GND         │
│             │      │             │
└─────────────┘      └─────────────┘

Tone Generation:
- Method: PWM (tone() function)
- Frequencies Used:
  - ARM: 1200Hz, 1600Hz
  - DISARM: 1600Hz, 1200Hz
  - Alert: 2500Hz
  - High Alert: 3000Hz
  - Max Alert: 3800Hz / 2200Hz alternating
```

### ARM Button Connection:

```
Tactile Button       ESP32 Neuro-Core
┌─────────────┐      ┌─────────────┐
│             │      │             │
│  Pin 1 ─────┼──────┤ GPIO 14     │
│  Pin 2 ─────┼──────┤ GND         │
│             │      │             │
└─────────────┘      └─────────────┘

Configuration:
- Mode: INPUT_PULLUP (internal 45kΩ pullup)
- Active: LOW (when pressed)
- Debounce: 500ms delay in code
- Function: Toggle ARM/DISARM state
```

---

## ⚡ Power Distribution

### Power Requirements:

| Component | Voltage | Current | Power |
|-----------|---------|---------|-------|
| ESP32 Neuro-Core | 5V | 500mA | 2.5W |
| ESP32-CAM (×4) | 5V | 300mA each | 6.0W |
| TFT Display | 3.3V | 100mA | 0.33W |
| PIR Sensors (×2) | 5V | 15mA each | 0.15W |
| Ultrasonic | 5V | 15mA | 0.075W |
| LEDs (×3) | 3.3V | 15mA each | 0.15W |
| Buzzer | 3.3V | 30mA | 0.1W |
| **TOTAL** | **5V** | **~2.5A** | **~12.5W** |

### Power Distribution Diagram:

```
                    ┌─────────────────┐
                    │  POWER SOURCE   │
                    │  5V / 3A Supply │
                    └────────┬────────┘
                             │
                    ┌────────┴────────┐
                    │   Power Hub     │
                    │  (USB Hub or    │
                    │   Breadboard)   │
                    └────────┬────────┘
                             │
        ┌────────────────────┼────────────────────┐
        │         │          │          │         │
    ┌───▼───┐ ┌──▼──┐   ┌──▼──┐   ┌──▼──┐   ┌──▼──┐
    │ Neuro │ │CAM 1│   │CAM 2│   │CAM 3│   │CAM 4│
    │ Core  │ │     │   │     │   │     │   │     │
    └───┬───┘ └─────┘   └─────┘   └─────┘   └─────┘
        │
        ├─── TFT (3.3V from ESP32)
        ├─── PIR 1 (5V)
        ├─── PIR 2 (5V)
        ├─── Ultrasonic (5V)
        ├─── LEDs (3.3V via resistors)
        └─── Buzzer (3.3V)

Common Ground: All GND pins connected together
```

### Recommended Power Solutions:

**Option 1: USB Hub (Portable)**
- 5-port powered USB hub (5V/3A minimum)
- Connect all 5 ESP32 devices via USB
- Portable, easy to set up
- Good for demonstrations

**Option 2: Bench Power Supply (Lab)**
- Adjustable DC power supply (5V/5A)
- Use breadboard for distribution
- More stable power
- Good for development

**Option 3: Battery Bank (Field)**
- 10,000+ mAh USB power bank
- Multiple USB outputs
- Portable operation (4-6 hours)
- Perfect for park/school deployment

---

## 📏 Physical Dimensions & Mounting

### Pyramid Dimensions:

```
                      ● Apex
                     /│\
                    / │ \
                   /  │  \
                  /   │   \  Height: 25cm
                 /    │    \
                /     │     \
               /      │      \
              /       │       \
             /        │        \
            /         │         \
           /          │          \
          /           │           \
         /            │            \
        /             │             \
       /              │              \
      /               │               \
     /                │                \
    /                 │                 \
   /                  │                  \
  /___________________│___________________\
  ←──────── 30cm ─────┴───── 30cm ───────→
              (Base Square)

Face Angle: 60° from horizontal
Slant Height: ~27cm
Internal Volume: ~6,250 cm³
```

### Component Mounting Positions:

**Vertical Layout (Side View):**

```
Height   Component
─────────────────────────────
25cm  ─  ● Apex (LED Array)
         │
22cm  ─  ┌─────────┐
         │ Camera  │ (Upper third)
20cm  ─  └─────────┘
         │
15cm  ─  ┌─────────┐
         │   TFT   │ (Middle)
13cm  ─  └─────────┘
         │
10cm  ─  ◉ PIR Sensor (Mid-height)
         │
 5cm  ─  ┌─────────┐
         │ Neuro-  │ (Lower third)
 3cm  ─  │  Core   │
         └─────────┘
         │
 0cm  ─  ▓▓▓▓▓▓▓▓▓ Base
         [Ultrasonic]
```

### Mounting Methods:

**Cameras:**
- Hot glue or double-sided tape
- Position at 20-22cm height
- Angle slightly downward (10-15°)
- Ensure clear field of view

**Neuro-Core:**
- Central mounting platform
- Velcro or standoffs
- Easy access for USB programming
- Cable management to all sensors

**TFT Display:**
- Front face, centered
- Visible from outside
- Secure mounting (screws or adhesive)
- Wiring routed internally

**Sensors:**
- PIR: Mid-height, opposite sides
- Ultrasonic: Base level, front
- Clear line of sight
- No obstructions

**LEDs:**
- Apex or top platform
- Visible from all sides
- Diffuser for better visibility
- Secure wiring

---

## 🛠️ Assembly Instructions

### Step 1: Prepare the Pyramid Structure

1. **Build or acquire pyramid frame**
   - Material: Cardboard, acrylic, or 3D printed
   - Dimensions: 30cm base, 25cm height
   - One side should be removable/open for access

2. **Mark component positions**
   - Use measurements from dimension diagram
   - Mark camera positions on each face
   - Mark sensor positions

3. **Drill/cut mounting holes**
   - Camera lens openings
   - TFT display cutout
   - Sensor mounting points
   - Cable routing holes

### Step 2: Wire the Neuro-Core

1. **Prepare breadboard or PCB**
   - Layout all connections
   - Use jumper wires

2. **Connect TFT Display**
   - Follow SPI wiring diagram
   - Test with example sketch

3. **Connect sensors**
   - PIR sensors to GPIO 16, 17
   - Ultrasonic to GPIO 32, 33
   - Test each sensor individually

4. **Connect indicators**
   - LEDs with 220Ω resistors
   - Buzzer to GPIO 13
   - Button to GPIO 14

5. **Upload firmware**
   - Upload `brain.cc` to Neuro-Core
   - Verify serial output
   - Test TFT boot sequence

### Step 3: Prepare Cameras

1. **Flash each ESP32-CAM**
   - Use FTDI programmer
   - Upload `braincam1.cc` through `braincam4.cc`
   - Verify correct IP assignment

2. **Test camera modules**
   - Power on individually
   - Check WiFi connection
   - Verify stream endpoint

3. **Mount cameras**
   - Position on each pyramid face
   - Secure with adhesive
   - Route power cables internally

### Step 4: Final Assembly

1. **Install Neuro-Core**
   - Mount in center of pyramid
   - Connect all sensors
   - Verify all connections

2. **Mount TFT display**
   - Install on front face
   - Connect to Neuro-Core
   - Test display output

3. **Install sensors**
   - Mount PIR sensors
   - Mount ultrasonic sensor
   - Verify detection ranges

4. **Install LEDs and buzzer**
   - Position at apex
   - Connect to Neuro-Core
   - Test all indicators

5. **Cable management**
   - Route all cables neatly
   - Use zip ties or cable clips
   - Ensure no loose wires

6. **Power distribution**
   - Set up USB hub or power supply
   - Connect all 5 ESP32 devices
   - Verify stable power

### Step 5: System Testing

1. **Power on sequence**
   - Neuro-Core first
   - Wait for PyramidNet WiFi
   - Power on all cameras

2. **Network verification**
   - Check all IPs (192.168.4.1-5)
   - Ping each device
   - Verify WebSocket connections

3. **Functional testing**
   - Test ARM/DISARM
   - Trigger motion sensors
   - Verify camera streams
   - Test all indicators

4. **Final adjustments**
   - Adjust camera angles
   - Tune sensor sensitivity
   - Optimize cable routing

---

## 📊 System Architecture Diagram

```
┌─────────────────────────────────────────────────────────┐
│                    PYRAMID SENTINEL                     │
│                  SYSTEM ARCHITECTURE                    │
└─────────────────────────────────────────────────────────┘

                    ┌─────────────┐
                    │  WEB CLIENT │
                    │ (Dashboard) │
                    └──────┬──────┘
                           │ HTTP/WebSocket
                           │ (WiFi)
                    ┌──────▼──────┐
                    │ NEURO-CORE  │
                    │   ESP32     │
                    │ 192.168.4.1 │
                    └──────┬──────┘
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
    ┌───▼───┐         ┌───▼───┐         ┌───▼───┐
    │ LOCAL │         │NETWORK│         │CAMERA │
    │SENSORS│         │ MGMT  │         │ ARRAY │
    └───┬───┘         └───┬───┘         └───┬───┘
        │                 │                  │
    ┌───┴───┐         ┌───┴───┐         ┌───┴───┐
    │ PIR 1 │         │  AP   │         │ CAM 1 │
    │ PIR 2 │         │ Mode  │         │ .4.2  │
    │ULTRA  │         │ WiFi  │         │ NORTH │
    └───────┘         │DHCP   │         └───────┘
                      │WebSock│         ┌───────┐
    ┌───────┐         └───────┘         │ CAM 2 │
    │  TFT  │                           │ .4.3  │
    │Display│         ┌───────┐         │ EAST  │
    │320×240│         │ HTTP  │         └───────┘
    └───────┘         │Server │         ┌───────┐
                      │Port 80│         │ CAM 3 │
    ┌───────┐         └───────┘         │ .4.4  │
    │  LED  │                           │ SOUTH │
    │ Array │         ┌───────┐         └───────┘
    │R/Y/G  │         │WebSock│         ┌───────┐
    └───────┘         │Port 81│         │ CAM 4 │
                      └───────┘         │ .4.5  │
    ┌───────┐                           │ WEST  │
    │Buzzer │                           └───────┘
    │ Audio │
    └───────┘

    ┌───────┐
    │Button │
    │ ARM   │
    └───────┘

DATA FLOW:
1. Cameras detect motion → Send alert via WebSocket
2. Neuro-Core processes → Updates threat level
3. Local sensors trigger → Immediate response
4. Dashboard receives updates → Real-time display
5. User commands → Sent to Neuro-Core → System responds
```

---

## 🔬 Technical Specifications

### Network Specifications:

| Parameter | Value |
|-----------|-------|
| **WiFi Mode** | Access Point (AP) |
| **SSID** | PyramidNet |
| **Password** | pointbreak |
| **IP Range** | 192.168.4.0/24 |
| **Neuro-Core IP** | 192.168.4.1 |
| **Camera IPs** | 192.168.4.2 - 192.168.4.5 |
| **HTTP Port** | 80 |
| **WebSocket Port** | 81 |
| **DHCP Range** | 192.168.4.100 - 192.168.4.200 |
| **WiFi Range** | ~30 meters |
| **Max Clients** | 10 (4 cameras + 6 clients) |

### Camera Specifications:

| Parameter | Value |
|-----------|-------|
| **Sensor** | OV2640 (2MP) |
| **Resolution** | 1600×1200 (UXGA) max |
| **Frame Rate** | 15-30 FPS (adjustable) |
| **Field of View** | 60° diagonal |
| **Streaming Format** | MJPEG |
| **Compression** | JPEG (adjustable quality) |
| **Night Vision** | No (add IR LEDs if needed) |
| **Detection** | Face detection (built-in) |

### Sensor Specifications:

| Sensor | Range | Angle | Response Time |
|--------|-------|-------|---------------|
| **PIR (HC-SR501)** | 3-7m | 120° | <1 second |
| **Ultrasonic (HC-SR04)** | 2-400cm | 15° | 10ms |

### Display Specifications:

| Parameter | Value |
|-----------|-------|
| **Size** | 2.8" diagonal |
| **Resolution** | 320×240 pixels |
| **Color Depth** | 16-bit (65,536 colors) |
| **Interface** | SPI |
| **Refresh Rate** | ~4 FPS (dashboard) |
| **Backlight** | LED (always on) |

---

## 🎨 Customization Options

### Visual Customization:

1. **LED Colors**
   - Replace with RGB LEDs for more colors
   - Add LED strips for ambient lighting
   - Programmable effects

2. **Display Themes**
   - Modify TFT graphics in `brain.cc`
   - Change colors, fonts, layouts
   - Add custom logos

3. **Enclosure Design**
   - Different materials (wood, metal, acrylic)
   - Transparent sides for showcase
   - Custom paint/finish

### Functional Upgrades:

1. **Additional Sensors**
   - Temperature/humidity (DHT22)
   - Light sensor (LDR)
   - Gas sensor (MQ-2)
   - Sound sensor (microphone)

2. **Enhanced Cameras**
   - Add IR LEDs for night vision
   - Pan-tilt servos for movement
   - Higher resolution cameras

3. **Power Options**
   - Solar panel charging
   - Larger battery banks
   - UPS backup

4. **Connectivity**
   - Add 4G/LTE module
   - LoRa for long-range
   - Bluetooth for local control

---

## 📝 Maintenance & Troubleshooting

### Regular Maintenance:

- **Weekly**: Check all connections, clean camera lenses
- **Monthly**: Verify firmware versions, update if needed
- **Quarterly**: Deep clean, check for loose wires

### Common Issues:

| Issue | Cause | Solution |
|-------|-------|----------|
| Camera offline | Power issue | Check USB cable, try different port |
| TFT blank | Wiring loose | Verify SPI connections |
| PIR false triggers | Too sensitive | Adjust sensitivity potentiometer |
| WiFi unstable | Interference | Change WiFi channel in code |
| System reboots | Power insufficient | Use better power supply (3A+) |

---

## 📚 Additional Resources

### Code Repositories:
- `brain.cc` - Neuro-Core firmware
- `braincam1-4.cc` - Camera firmware
- `pyramid.html/css/js` - Web dashboard

### Documentation:
- `README.md` - Project overview
- `TEST.md` - Complete testing guide
- `Blueprint.md` - This document

### External References:
- ESP32 Datasheet
- ILI9341 TFT Documentation
- HC-SR501 PIR Sensor Guide
- HC-SR04 Ultrasonic Guide

---

## ✅ Final Checklist

Before deployment, verify:

- [ ] All components mounted securely
- [ ] All wiring connections solid
- [ ] Firmware uploaded to all 5 ESP32s
- [ ] PyramidNet WiFi broadcasting
- [ ] All cameras at correct IPs
- [ ] TFT display showing dashboard
- [ ] All sensors responding
- [ ] LEDs and buzzer functional
- [ ] WebSocket connections stable
- [ ] Dashboard accessible from laptop
- [ ] All 4 camera streams working
- [ ] Motion detection operational
- [ ] ARM/DISARM functioning
- [ ] Power supply adequate
- [ ] System tested end-to-end

---

**Document Version:** 1.0  
**Last Updated:** 2025-12-21  
**Created by:** Antigravity AI  
**System:** Pyramid Sentinel v5.0.0-TITANIUM

🏔️ **BUILD COMPLETE - DEPLOY WITH CONFIDENCE!** 🏔️
