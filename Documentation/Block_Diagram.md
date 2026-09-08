# Block Diagram

```text
             +----------------+
             |      LM35      |
             | Temperature    |
             |    Sensor      |
             +-------+--------+
                     |
                     | Analog voltage
                     v
             +-------+--------+
             | STM32F103C8T6  |
             |     ADC1       |
             +-------+--------+
                     |
             | Temperature calculation
             +----------+-----------+
                        |
          +-------------+-------------+
          |                           |
          v                           v
 +--------+---------+          +------+------+
 |     16x2 LCD     |          | Alarm logic |
 | Temperature °C  |          +------+------+
 +------------------+                 |
                              +-------+-------+
                              |               |
                              v               v
                           Buzzer          Red LED
```
