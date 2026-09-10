"""
AI-assisted fault diagnosis for ESP32/STM32 temperature telemetry.

This module is intentionally transparent: it combines engineering rules
with an optional LLM explanation layer. It does not claim that an LLM alone
is a validated fault classifier.

Input examples:
    python diagnostic_engine.py --temperature 32.4 --raw 401
    python diagnostic_engine.py --temperature 37.1 --raw 461

For live ESP32 serial input, use --serial COM5 (Windows) or --serial /dev/ttyUSB0.
"""

from __future__ import annotations

import argparse
import json
import os
from dataclasses import dataclass, asdict
from typing import Optional


@dataclass
class Diagnosis:
    status: str
    fault: str
    severity: str
    confidence: float
    probable_cause: str
    recommended_action: str


def diagnose(temperature_c: float, raw: Optional[int] = None) -> Diagnosis:
    """Engineering-rule baseline used before any AI/LLM explanation."""
    if raw is not None and raw <= 5:
        return Diagnosis("FAULT", "SENSOR_LOW_OR_DISCONNECTED", "HIGH", 0.98,
                         "Sensor signal is near zero or the sensor connection may be open.",
                         "Inspect sensor VCC, GND, signal wire, connector and ADC pin.")

    if raw is not None and raw >= 4090:
        return Diagnosis("FAULT", "SENSOR_HIGH_OR_SHORT", "HIGH", 0.98,
                         "ADC is saturated near full scale, suggesting a short or abnormal sensor output.",
                         "Check the sensor signal for a short to supply and verify sensor wiring.")

    if temperature_c < 0 or temperature_c > 80:
        return Diagnosis("FAULT", "TEMPERATURE_OUT_OF_RANGE", "HIGH", 0.95,
                         "Measured temperature is outside the configured physical range.",
                         "Verify sensor placement, wiring, ADC configuration and incubator hardware.")

    if temperature_c < 35:
        return Diagnosis("WARNING", "LOW_TEMPERATURE", "MEDIUM", 0.90,
                         "Temperature is below the configured incubator operating band.",
                         "Check heater, relay/driver, power supply and sensor placement.")

    if temperature_c > 40:
        return Diagnosis("WARNING", "HIGH_TEMPERATURE", "HIGH", 0.90,
                         "Temperature is above the configured incubator operating band.",
                         "Check heater control, relay/driver, cooling/fan operation and sensor placement.")

    return Diagnosis("OK", "NORMAL", "LOW", 0.99,
                     "Temperature is inside the configured operating band.",
                     "Continue monitoring.")


def llm_prompt(d: Diagnosis, temperature_c: float, raw: Optional[int]) -> str:
    return f"""You are an embedded-systems diagnostic assistant.
Analyze this incubator telemetry and explain the result to an engineer.
Do not invent measurements or components that are not provided.

Temperature: {temperature_c:.2f} C
ADC raw: {raw if raw is not None else 'not provided'}
Rule-based status: {d.status}
Fault classification: {d.fault}
Confidence: {d.confidence:.2f}
Probable cause: {d.probable_cause}
Recommended action: {d.recommended_action}

Return: (1) diagnosis, (2) evidence, (3) checks to perform, (4) safety note.
"""


def optional_openai_explanation(prompt: str) -> Optional[str]:
    """Optional LLM layer. Requires OPENAI_API_KEY and the openai package.

    The rule-based result remains available if the LLM is not configured.
    """
    if not os.getenv("OPENAI_API_KEY"):
        return None

    try:
        from openai import OpenAI
        client = OpenAI()
        response = client.responses.create(
            model=os.getenv("OPENAI_MODEL", "gpt-5.6-mini"),
            input=prompt,
        )
        return response.output_text
    except Exception as exc:
        return f"LLM explanation unavailable: {exc}"


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser()
    p.add_argument("--temperature", type=float, required=True)
    p.add_argument("--raw", type=int, default=None)
    p.add_argument("--llm", action="store_true", help="Generate optional LLM explanation")
    return p.parse_args()


def main() -> None:
    args = parse_args()
    diagnosis = diagnose(args.temperature, args.raw)
    print(json.dumps(asdict(diagnosis), indent=2))

    if args.llm:
        explanation = optional_openai_explanation(
            llm_prompt(diagnosis, args.temperature, args.raw)
        )
        if explanation:
            print("\n--- LLM ENGINEER EXPLANATION ---\n")
            print(explanation)


if __name__ == "__main__":
    main()
