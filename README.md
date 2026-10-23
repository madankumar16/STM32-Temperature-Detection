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
| 🌡️ | **Reads temperature** | LM35 on ADC1 channel 0 (PA0), 12-bit. The ADC is calibrated at power-up and each reading is the average of 16 samples |
| 📟 | **Shows it live** | 16x2 LCD, updated every 500 ms |
| 🚨 | **High-temperature alarm** | Red LED + buzzer turn on at **50.0 °C** and stay on until it drops **below 48.0 °C** (2 °C hysteresis, so no buzzer chatter). Change it in `App/temp_logic.h` |
| 🛡️ | **Detects a broken sensor** | A raw ADC value of 5 or less, or 4090 or more, means the wire is loose or shorted — the LCD shows `SENSOR FAULT` and the alarm turns on. Once the sensor is OK again, the alarm simply follows the current temperature |
| 🧩 | **Clean drivers** | One small module each for LM35, LCD, LED and buzzer, plus a hardware-free logic module (`App/temp_logic.c`) |
| ✅ | **Checked on every push** | GitHub Actions runs the unit tests, builds and links the full firmware with `arm-none-eabi-gcc`, and tests the Python fault rules |

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
                              ≥ 50.0 °C ? ── yes ──► LED + buzzer ON
                              < 48.0 °C ? ── yes ──► LED + buzzer OFF
                              in between  ─────────► keep what it was
```

The alarm uses the same rounded value the LCD shows, so `50.0 C` on the screen always means the alarm is on.

**The maths:**

```text
ADC         = average of 16 readings
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
| LM35 VCC | 5V (needs ≥ 4 V) | | LCD D4 | PB14 |
| LM35 OUT | PA0 | | LCD D5 | PB15 |
| LCD RS | PB12 | | LCD D6 | PA8 |
| LCD E | PB13 | | LCD D7 | PA9 |
| Buzzer | PB0 | | SWDIO / SWCLK | PA13 / PA14 |
| Red LED | PB1 | | | |

Full wiring: [`Hardware/Pin_Connections.md`](Hardware/Pin_Connections.md) · Parts list: [`Hardware/Bill_of_Materials.md`](Hardware/Bill_of_Materials.md)

---

## 🚀 Build and flash

### Option 1: command line (no IDE)

With `arm-none-eabi-gcc` installed:

```bash
mkdir -p third_party
git clone --depth 1 https://github.com/STMicroelectronics/stm32f1xx_hal_driver third_party/hal
git clone --depth 1 https://github.com/STMicroelectronics/cmsis_device_f1      third_party/device
git clone --depth 1 https://github.com/ARM-software/CMSIS_5                    third_party/CMSIS_5
cd STM32_Project && make
```

This links a complete image (`build/stm32_temperature.bin`, about 11 KB) using ST's startup file and linker script. Flash it at `0x08000000` with ST-Link or STM32CubeProgrammer. GitHub Actions does the same build on every push and attaches the firmware as a downloadable artifact.

### Option 2: STM32CubeIDE

1. In **STM32CubeIDE**, create a project for **STM32F103C8T6**.
2. Set **PA0** as ADC1 channel 0 (12-bit, software trigger).
3. Set **PB0, PB1** and the LCD pins as GPIO outputs. Keep **PA13/PA14** free for SWD.
4. Copy in `STM32_Project/Core/Src`, `STM32_Project/Core/Inc`, `STM32_Project/App` and the `STM32_Project/Drivers/*` folders, and add `App` and the driver folders to the include paths.
5. Build, then flash with ST-Link.

> `STM32_Temperature.ioc` is a human-readable pin reference, not a CubeMX-generated file. If your CubeIDE project already has `stm32f1xx_it.c` / `stm32f1xx_it.h`, keep only one copy of each.

Either way, test the alarm on the real board before trusting it.

### 🧪 Unit tests

The conversion, averaging, fault check and alarm hysteresis are plain C with no HAL, so they are tested on your PC with just `gcc`:

```bash
make -C STM32_Project test
```

The tests live in [`tests/test_temp_logic.c`](tests/test_temp_logic.c) and also run in CI on every push.

---

## 📁 Project layout

```text
STM32_Project/
├── Core/Src/main.c          ← main loop: read, display, drive the alarm
├── Core/Src/stm32f1xx_it.c  ← interrupt handlers (SysTick)
├── App/temp_logic.c         ← raw → °C, averaging, fault check, hysteresis (no HAL)
├── Drivers/
│   ├── LM35/                ← ADC sampling for the sensor
│   ├── LCD/                 ← 16x2 LCD in 4-bit mode
│   └── LED/  Buzzer/        ← alarm outputs
└── Makefile                 ← command-line build + `make test`
tests/                       ← host unit tests for App/temp_logic.c
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

- [ ] Move the main loop to FreeRTOS tasks
- [ ] Add photos and a demo video of the real board

---

<p align="center">Made by <a href="https://github.com/madankumar16">Madan Kumar S</a> · ECE, VTU '28 · Open to embedded internships</p>
