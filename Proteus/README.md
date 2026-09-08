# Proteus Simulation

The repository documents the logical circuit and firmware interfaces, but does not include a proprietary Proteus project file. Build the simulation using the component list and pin map in `Hardware/Pin_Connections.md`.

Recommended simulation chain:

`LM35 → STM32F103C8T6 ADC → LCD`

and

`Temperature >= 50 C → Buzzer + LED`

Use the appropriate STM32F103C8T6 device model and ensure the simulated clock and supply match the firmware configuration.
