# Contributing

Thank you for your interest in improving the STM32 Temperature Detection System!

This project uses an STM32F103C8T6 (Blue Pill) with an LM35 temperature sensor, a 16x2 LCD, and visual/audible high-temperature alarms.

## Ways to contribute

- Report a reproducible bug or incorrect instruction.
- Improve the README or the hardware and circuit documentation.
- Improve the C drivers or firmware.
- Add tests or clarify the build and flashing steps.

## Before making a change

1. Check the existing README and documentation so your change matches the project's current design.
2. For a bug, open an issue with the steps to reproduce it, expected behavior, actual behavior, and any relevant build output.
3. For a larger change, open an issue first to discuss the proposed approach.

## Development workflow

1. Fork the repository if you are contributing from a separate account, then clone your fork. If you have write access to this repository, you can create a feature branch directly.
2. Create a descriptive branch, for example `docs/fix-wiring-notes` or `fix/lm35-conversion`.
3. Make one focused change at a time.
4. Review your changes with `git diff` and ensure you have not included generated build files, secrets, or unrelated changes.
5. Build the firmware in STM32CubeIDE when the change affects C code or project configuration.
6. Test hardware-related changes safely when the required board and components are available. Clearly state when a change has not been tested on physical hardware.
7. Commit with a clear message, such as `docs: clarify sensor wiring` or `fix: correct temperature conversion`.
8. Push your branch and open a pull request describing the change, reason, and tests performed.

## Firmware and hardware notes

- Keep pin assignments consistent with the README, circuit documentation, and STM32CubeIDE configuration.
- Explain any changes to ADC settings, GPIO assignments, alarm thresholds, or sensor calculations.
- Do not claim hardware validation unless the updated firmware has actually been tested on the board.
- Do not commit passwords, access tokens, private keys, or unrelated personal information.

## Pull request checklist

- [ ] The change has a clear purpose and focused scope.
- [ ] Documentation matches the actual implementation.
- [ ] Firmware builds successfully, if applicable, or any build failure is described.
- [ ] Hardware testing status is stated honestly.
- [ ] The pull request description explains what changed and why.

Thank you for helping make this project easier to understand, build, and maintain.
