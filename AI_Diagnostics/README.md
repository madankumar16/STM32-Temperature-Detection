# AI Diagnostic Engine

This module receives temperature telemetry from the ESP32/STM32 embedded system and performs transparent rule-based fault classification. An optional LLM layer converts the diagnosis into an engineer-readable explanation and troubleshooting checklist.

## Install

```bash
python -m pip install -r requirements.txt
```

## Test without an LLM

```bash
python diagnostic_engine.py --temperature 37.2 --raw 462
python diagnostic_engine.py --temperature 30.0 --raw 372
python diagnostic_engine.py --temperature 37.0 --raw 0
```

## Optional LLM explanation

Set an API key in your local environment and optionally set `OPENAI_MODEL` to the model available to your account. Do not commit API keys to GitHub.

```bash
export OPENAI_API_KEY="YOUR_KEY"
python diagnostic_engine.py --temperature 37.2 --raw 462 --llm
```

On Windows PowerShell:

```powershell
$env:OPENAI_API_KEY="YOUR_KEY"
python diagnostic_engine.py --temperature 37.2 --raw 462 --llm
```

## Data flow

ESP32/STM32 -> serial telemetry -> Python diagnostic engine -> rule-based fault classification -> optional LLM explanation -> engineer action.

The LLM is an explanation/assistance layer. Experimental fault-detection accuracy should be measured using the deterministic classifier and a labeled test dataset rather than claimed from LLM output alone.
