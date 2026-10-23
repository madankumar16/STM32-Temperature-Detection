<h1 align="center">🌡️ STM32 Temperature Monitor</h1>

<p align="center">
  An STM32 Blue Pill reads an LM35 sensor, shows the temperature on an LCD,<br>
  and raises an alarm when it gets too hot — or when the sensor itself fails.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/MCU-STM32F103C8T6-03234B?style=flat-square&logo=stmicroelectronics&logoColor=white">
  <img src="https://img.shields.io/badge/Language-Embedded%20C-00599C?style=flat-square&logo=c&logoColor=white">
  <img src="https://img.shields.io/badge/HAL-STM32CubeIDE-03234B?style=flat-square">
  <img src="https://github.com/madankumar16/STM32-Temperature-Detection/actions/workflows/c-cpp.yml/badge.svg">
</p>

<!-- 📸 Add a photo or short GIF of your real board here — it's the first thing visitors look at.
     Example: ![Board running](Documentation/board.jpg) -->

---

## ✨ What it does

| | Feature | How |
|---|---|---|
| 🌡️ | **Reads temperature** | LM35 on ADC1 channel 0 (PA0), 12-bit conversion |
| 📟 | **Shows it live** | 16x2 LCD, updated every 500 ms |
| 🚨 | **High-temperature alarm** | Red LED + buzzer turn on at **50 °C** (change it in `lm35.h`) |
| 🛡️ | **Detects a broken sensor** | A raw ADC value near 0 or near 4095 means the wire is loose or shorted — the LCD shows `SENSOR FAULT` and the alarm turns on |
| 🧩 | **Clean drivers** | One small module each for ADC, LM35, LCD, LED and buzzer |
| ✅ | **Checked on every push** | GitHub Actions compiles the C code with `arm-none-eabi-gcc` and tests the Python fault rules |

---

## 🧠 How it works

```text
 LM35 ──► ADC (PA0) ──► Is the reading valid?
                          │
              ┌───────────┴───────────┐
           No (≈0 or ≈4095)          Yes
              │                       │
      LCD: "SENSOR FAULT"     Convert to °C, show on LCD
      LED + buzzer ON                 │
                              ≥ 50 °C ? ── yes ──► LED + buzzer ON
                                      └── no ───► LED + buzzer OFF
```

**The maths:**

```text
Voltage     = ADC × 3.3 / 4095
Temperature = Voltage × 100      (LM35 gives 10 mV per °C)
```

---

## 🔌 Hardware

| Part | Job |
|---|---|
| STM32F103C8T6 (Blue Pill) | The brain |
| LM35 | Temperature sensor |
| 16x2 HD44780 LCD + 10k pot | Display + contrast |
| 5V buzzer | Sound alarm |
| Red LED + 220 Ω | Light alarm |
| ST-Link V2 | Flashing and debugging |

### Pin map

| Signal | Pin | | Signal | Pin |
|---|---|---|---|---|
| LM35 OUT | PA0 | | LCD D4 | PB14 |
| LCD RS | PB12 | | LCD D5 | PB15 |
| LCD E | PB13 | | LCD D6 | PA8 |
| Buzzer | PB0 | | LCD D7 | PA9 |
| Red LED | PB1 | | SWDIO / SWCLK | PA13 / PA14 |

Full wiring: [`Hardware/Pin_Connections.md`](Hardware/Pin_Connections.md) · Parts list: [`Hardware/Bill_of_Materials.md`](Hardware/Bill_of_Materials.md)

---

## 🚀 Build and flash

> This repo holds the application code and drivers. The STM32 HAL, startup file and linker script come from STM32CubeIDE.

1. In **STM32CubeIDE**, create a project for **STM32F103C8T6**.
2. Set **PA0** as ADC1 channel 0 (12-bit, software trigger).
3. Set **PB0, PB1** and the LCD pins as GPIO outputs. Keep **PA13/PA14** free for SWD.
4. Copy in `STM32_Project/Core/Src` and the `STM32_Project/Drivers/*` folders, and add the driver folders to the include paths.
5. Build, then flash with ST-Link.
6. Test the alarm on the real board before trusting it.

---

## 📁 Project layout

```text
STM32_Project/
├── Core/Src/main.c          ← main loop: read, check, display, alarm
└── Drivers/
    ├── ADC/                 ← ADC setup and reads
    ├── LM35/                ← raw value → °C, fault limits
    ├── LCD/                 ← 16x2 LCD in 4-bit mode
    ├── LED/  Buzzer/        ← alarm outputs
ESP32/                       ← same idea on ESP32, sends data over Serial
AI_Diagnostics/              ← Python rules that name the fault type
Documentation/  Hardware/    ← block diagram, circuit, parts, wiring
```

---

## 🔬 Bonus: ESP32 version + fault diagnosis

- [`ESP32/`](ESP32) — an Arduino sketch that averages 10 readings and sends the temperature and fault type over Serial.
- [`AI_Diagnostics/`](AI_Diagnostics) — Python rules that sort readings into clear fault types (normal, too low, too high, disconnected, shorted, out of range). An optional LLM can explain the result in plain English. See also [LLM-Embedded-Diagnostics](https://github.com/madankumar16/LLM-Embedded-Diagnostics).

---

## 🗺️ Next steps

- [ ] Average several ADC samples for smoother readings
- [ ] Add hysteresis so the alarm doesn't flicker near 50 °C
- [ ] Move the main loop to FreeRTOS tasks
- [ ] Add photos and a demo video of the real board

---

<p align="center">Made by <a href="https://github.com/madankumar16">Madan Kumar S</a> · ECE, VTU '28 · Open to embedded internships</p>
