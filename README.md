# PixelArt BLE Canvas (16×16)

An interactive, wireless pixel art canvas and ambient display powered by an ESP32 and four chained 8×8 WS2812B RGB LED matrices. Draw directly on a touch-enabled mobile web app, sending pixel data in real time over Bluetooth Low Energy (BLE) using the Nordic UART Service (NUS).

---

## Features

- **Direct Bluetooth Control:** Uses Bluetooth Low Energy (BLE) via the Web Bluetooth API (supported via Bluefy on iOS and Chrome on Android/Desktop), keeping your phone connected to regular Wi-Fi or cellular networks.
- **Low-Latency Live Drawing:** Touch/drag support allows continuous drawing across the 16×16 matrix.
- **Built-in Standalone Animations:** Non-blocking effects (Fire, Rainbow Swirl) rendered directly on the ESP32.
- **Dynamic Brightness Control:** On-the-fly dimming via Web UI.
- **Hardware Power Protection:** FastLED software current capping (`5V @ 450mA`) prevents USB brownouts and resets.
- **Single-File Web Interface:** Lightweight HTML/CSS/JavaScript with zero external dependencies, ready to host on GitHub Pages or Netlify.

---

## Hardware Requirements

- **Microcontroller:** ESP32-WROOM-32 Development Board (38-pin or 30-pin)
- **LED Matrix:** 4× 8×8 WS2812B rigid PCB modules chained into a 16×16 layout (256 LEDs total)
- **Diffuser:** Architectural tracing paper or drafting vellum
- **Power:** Standard 5V USB connection (PC port or 5V/1A+ phone adapter)
- **Enclosure:** Shadow-box photo frame

---

## Wiring & Pinout

Connect the first matrix panel's **DIN** (Data In) to the ESP32:

| Component | Matrix Pin | ESP32 Pin |
| :--- | :--- | :--- |
| **Power (+5V)** | Red Wire / `5V` / `VCC` | `VIN` or `5V` rail |
| **Ground (GND)** | Black Wire / `GND` | `GND` |
| **Data Signal** | White Wire / `DIN` | `GPIO 16` |

> **Note on Chaining:** Connect `DOUT` of Panel 1 to `DIN` of Panel 2, `DOUT` of Panel 2 to `DIN` of Panel 3, and `DOUT` of Panel 3 to `DIN` of Panel 4. Ensure 5V and GND rails are bridged across all four panels.

---

## Matrix Mapping Architecture

The display is assembled from four 8×8 panels arranged in four quadrants:

```text
+---------------+---------------+
|  Quadrant 0   |  Quadrant 1   |
|   (Top-Left)  |  (Top-Right)  |
|   LEDs 0-63   |  LEDs 64-127  |
+---------------+---------------+
|  Quadrant 2   |  Quadrant 3   |
| (Bottom-Left) | (Bottom-Right)|
|  LEDs 128-191 |  LEDs 192-255 |
+---------------+---------------+