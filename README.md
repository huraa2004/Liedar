# LiDAR 3D Scanner

A 3D LiDAR scanner built on the **TI MSP432E401Y** microcontroller. The device rotates a **VL53L1X time-of-flight sensor** using a stepper motor to capture distance measurements in a full 360° sweep. Multiple sweeps are stacked vertically to produce a 3D point cloud, which is transmitted over UART to a PC and rendered interactively using Open3D.

---

## How It Works

### Overview

The system operates in two modes controlled by a button (Port J interrupt):

- **Scan Mode** — the motor rotates the ToF sensor through 360°, taking a distance reading every 11.25°. Up to 20 sweeps can be collected, each representing a different height level.
- **Transmit Mode** — all collected scan data is sent over UART to the PC, where a Python script reconstructs and renders the 3D point cloud.

```
[Button Press] ──► [Scan Mode]
                      │
                      ▼
              Motor rotates 11.25°
              VL53L1X reads distance
              Repeat × 32 samples = 360°
              Repeat for each height level
                      │
[Button Press] ──► [Transmit Mode]
                      │
                      ▼
              UART → PC → Python → Open3D
```

---

### Firmware (C — MSP432E401Y)

The firmware is structured into focused modules:

| Module | Description |
|---|---|
| `main.c` | Entry point. Initializes PLL, SysTick, and ports. Runs the main loop dispatching scan and transmit tasks. |
| `VL531X.c/h` | Core scanning logic. Manages ToF sensor state, drives scan rotations, buffers data, and handles UART transmission. |
| `Motor.c/h` | Stepper motor control. Drives 4-step CW/CCW sequences on Port H. 2048 steps = one full rotation. |
| `I2C.c/h` | I2C driver for communicating with the VL53L1X sensor (address `0x29`). |
| `uart.c/h` | UART driver for serial communication to the PC at 115200 baud. |
| `Ports.c/h` | GPIO initialization for motor, LEDs, and sensor pins. |
| `Interrupts.c/h` | Port J button interrupt — toggles between scan and transmit mode. |
| `VL53L1X_api.c/h` | ST's Ultra-Light Driver for the VL53L1X sensor. |

#### Scan Parameters

| Parameter | Value |
|---|---|
| Samples per sweep | 32 |
| Angle per sample | 11.25° |
| Max sweeps (height levels) | 20 |
| Z spacing between levels | 400 mm |
| Motor steps per sample | 64 |
| Full rotation steps | 2048 |

#### Data Buffer & Transmission

Scan data is stored in a `uint16_t transmittedData[20][32]` array (max 640 distance values). When transmit mode is triggered:

1. `DATA STARTS` marker is sent over UART
2. `measurementNum` and `numSamples` are sent as single bytes
3. All distance values are sent as 16-bit little-endian pairs
4. `DATA ENDS` marker is sent

---

### PC Visualization (Python)

Two scripts are included:

**`main.py`** — Live capture and visualization. Connects to the MSP432 over serial (COM3, 115200 baud), waits for the `DATA STARTS` marker, reads all distance bytes, and converts them to 3D coordinates:

```
x = distance × sin(angle)
y = distance × cos(angle)
z = sweep_index × 400
```

The resulting point cloud is saved to `scan_data.xyz` and rendered in an Open3D window with green connecting lines and purple points.

**`vis_saved_data.py`** — Offline viewer. Loads a previously saved `scan_data.xyz` file and renders it with the same line mesh and a coordinate frame axis.

---

## Hardware

- **Microcontroller:** TI MSP432E401Y (TM4C1294-based dev board)
- **ToF Sensor:** STMicroelectronics VL53L1X (I2C, up to 4m range)
- **Motor:** 28BYJ-48 stepper motor (driven via ULN2003 on Port H)
- **Interface:** USB-UART bridge (COM3)

---

## Project Structure

```
LiDAR/
├── Deliverable 2/
│   ├── Final_Project/
│   │   ├── Project Source Code/    # Firmware (C)
│   │   │   ├── main.c
│   │   │   ├── VL531X.c/h          # ToF sensor & scan logic
│   │   │   ├── Motor.c/h           # Stepper motor driver
│   │   │   ├── I2C.c/h             # I2C driver
│   │   │   ├── uart.c/h            # UART driver
│   │   │   ├── Ports.c/h           # GPIO setup
│   │   │   ├── Interrupts.c/h      # Button interrupt
│   │   │   ├── VL53L1X_api.c/h     # ST ULD driver
│   │   │   └── ...
│   │   └── PC visualizatiion/      # Python scripts
│   │       ├── main.py             # Live capture + render
│   │       └── vis_saved_data.py   # Visualize saved data
│   └── Deliverable 2.cproject.yml  # Keil/CMSIS project config
├── scan_data.xyz                   # Example captured scan
└── README.md
```

---

## Getting Started

### Firmware

1. Open the project in **Keil Studio** or **VS Code with the Keil CMSIS extension**
2. Build the `Deliverable 2` project targeting `MSP432E401Y`
3. Flash to the dev board

### Python Visualization

Install dependencies:

```bash
pip install pyserial open3d numpy
```

**Live capture** (board connected via USB):
```bash
# Update COM3 in main.py to match your port (e.g. /dev/ttyUSB0 on Linux/Mac)
python main.py
```

**View saved scan data:**
```bash
python vis_saved_data.py
```

### Usage

1. Power on the board — the status LED confirms initialization
2. Press the **onboard button** to start a scan sweep
3. Move the scanner to the next height and press again to add another layer
4. Press the button once more to transmit all data to the PC

---

## Dependencies

| Tool | Purpose |
|---|---|
| Keil Studio / CMSIS Toolchain | Build & flash firmware |
| ST VL53L1X ULD | Sensor driver (bundled) |
| Python 3.x | PC-side scripts |
| `pyserial` | UART communication |
| `open3d` | 3D point cloud rendering |
| `numpy` | Data processing |
