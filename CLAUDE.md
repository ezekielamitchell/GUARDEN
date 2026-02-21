# GUARDEN - Claude Code Project Memory

## Project Identity
**GUARDEN** = Garden Urban AI Rodent Detection and Environment Node
**Client:** Danny Woo Community Garden, Seattle International District
**Course:** Seattle University ECEGR 4640 - Internet of Things (IoT)
**Developer:** Ezekiel A. Mitchell (emitchell4@seattleu.edu) — solo project

---

## System Architecture (3-Layer IoT)

```
[Edge]  ESP32-C3 nodes  →  PIR wake → OV2640 capture → TFLite inference → MQTT publish
          ↓
[Fog]   Raspberry Pi 5B+  →  WiFi AP + MQTT broker (Mosquitto) + PostgreSQL + Python services
          ↓
[Cloud] Local web dashboard  →  Heatmaps, activity timelines, nesting area visualization
```

**Key constraint:** Fully offline. No internet dependency. Pi is the network hub.

---

## Hardware

| Component | Part | Notes |
|---|---|---|
| Edge MCU | ESP32-C3 | RISC-V @ 160MHz, WiFi 802.11 b/g/n |
| Camera | OV2640 2MP | 640×480 capture |
| Motion | PIR sensor | Event-driven wake from deep sleep |
| Hub | Raspberry Pi 5B+ | WiFi AP, fog compute, storage |
| Power | Solar + 5000mAh LiPo | Target: 10+ day runtime |
| Storage | MicroSD (edge) + PostgreSQL (fog) | Offline resilience |

---

## Project Structure

```
GUARDEN/
├── CLAUDE.md                    # ← You are here
├── README.md                    # Project overview (GitBook-formatted)
├── .gitbook/                    # GitBook documentation config
│
├── edge/                        # ESP32-C3 firmware (PlatformIO project)
│   ├── platformio.ini
│   └── src/
│       ├── main.cpp             # Entry point, deep sleep logic
│       ├── camera.cpp/.h        # OV2640 init + capture
│       ├── detector.cpp/.h      # TFLite inference wrapper
│       ├── mqtt_client.cpp/.h   # WiFi + MQTT publish
│       ├── model_data.h         # Auto-generated C array from quantize.py
│       └── config.h             # Pin defs, thresholds, node ID
│
├── fog/                         # Raspberry Pi hub (all services)
│   ├── broker/
│   │   └── mosquitto.conf       # Copy to /etc/mosquitto/conf.d/
│   ├── ingestion/
│   │   └── main.py              # MQTT subscriber → PostgreSQL
│   ├── dashboard/
│   │   ├── app.py               # Flask app + all API routes
│   │   ├── templates/
│   │   │   └── index.html       # Single page dashboard
│   │   └── static/              # CSS/JS overrides if needed
│   ├── db/
│   │   └── schema.sql           # DB init (run once)
│   ├── setup.sh                 # One-shot Pi setup script
│   └── requirements.txt
│
├── model/                       # ML pipeline (run on laptop/PC)
│   ├── train.py                 # Dataset → MobileNetV2 → .h5
│   ├── quantize.py              # .h5 → INT8 .tflite + model_data.h
│   ├── evaluate.py              # Accuracy + latency benchmarks
│   ├── requirements.txt
│   └── guarden_v1_int8.tflite   # Final model (committed to repo)
│
├── config/
│   ├── nodes.yaml               # Node IDs, locations, GPS coords
│   └── .env.example             # Environment variable template
│
└── tests/
    ├── test_ingestion.py        # Fog ingestion unit tests
    ├── test_dashboard.py        # Flask API endpoint tests
    └── test_model.py            # TFLite shape/dtype/accuracy tests
```

---

## Tech Stack

### Edge (ESP32-C3)
- **Build system:** PlatformIO (not Arduino IDE)
- **Language:** C/C++
- **AI runtime:** TensorFlow Lite Micro (INT8 quantized)
- **Protocol:** MQTT over WiFi
- **Power:** Deep sleep between detections (10µA standby)

### Fog (Raspberry Pi 5B+)
- **OS:** Raspberry Pi OS (64-bit)
- **MQTT broker:** Mosquitto
- **Language:** Python 3.10+
- **Database:** PostgreSQL
- **WiFi AP:** hostapd + dnsmasq

### Dashboard
- **Framework:** Flask (preferred for simplicity)
- **Visualization:** Leaflet.js for garden map heatmap, Chart.js for timelines
- **Data:** Served from PostgreSQL via Flask API

### ML Pipeline
- **Training:** Python, TensorFlow/Keras
- **Model:** MobileNetV2 transfer learning (fine-tuned for rat detection)
- **Quantization:** INT8 post-training quantization → .tflite
- **Dataset:** Roboflow rodent dataset (preferred) or custom labeled set
- **Target accuracy:** >65% on 640×480 outdoor/low-light images

---

## Performance Targets

| Metric | Target |
|---|---|
| Detection latency | <1 second |
| Battery life | 3+ days (target: 10+ days) |
| Detection accuracy | >65% |
| Network uptime | >20% |
| Storage capacity | 3+ days of data |

---

## Key Design Decisions

1. **Privacy-first:** Only metadata (timestamp, node ID, confidence score, location) is transmitted. Images NEVER leave the device.
2. **Offline-first:** System must work with zero internet. All services run on the Pi.
3. **Edge inference only:** TFLite runs on-device. Pi does NOT do image processing.
4. **Motion-triggered:** PIR wakes ESP32 from deep sleep. No continuous polling.
5. **MQTT for comms:** Lightweight pub/sub ideal for constrained IoT devices.

---

## MQTT Topic Schema

```
guarden/nodes/{node_id}/detection     # Detection event payload
guarden/nodes/{node_id}/status        # Heartbeat / health
guarden/nodes/{node_id}/config        # Remote config push
guarden/system/alerts                 # System-wide alerts
```

### Detection Payload (JSON)
```json
{
  "node_id": "node_01",
  "timestamp": 1708900000,
  "confidence": 0.87,
  "location": {"lat": 47.598, "lng": -122.318},
  "battery_mv": 3850,
  "temp_c": 12.4
}
```

---

## PostgreSQL Schema (Core Tables)

```sql
-- Detection events
CREATE TABLE detections (
  id SERIAL PRIMARY KEY,
  node_id VARCHAR(20),
  timestamp TIMESTAMPTZ,
  confidence FLOAT,
  lat FLOAT,
  lng FLOAT,
  battery_mv INT,
  temp_c FLOAT
);

-- Node registry
CREATE TABLE nodes (
  node_id VARCHAR(20) PRIMARY KEY,
  location_label VARCHAR(100),
  lat FLOAT,
  lng FLOAT,
  installed_at TIMESTAMPTZ,
  active BOOLEAN DEFAULT TRUE
);
```

---

## Development Workflow

### ESP32 Firmware
```bash
# Build and flash
cd edge && pio run --target upload --environment esp32c3

# Monitor serial
pio device monitor --baud 115200
```

### Raspberry Pi Services
```bash
# One-shot setup (run once)
sudo bash fog/setup.sh

# Manual service control
sudo systemctl restart guarden-ingestion
sudo systemctl restart guarden-dashboard
sudo journalctl -u guarden-ingestion -f
```

### ML Training (run on laptop)
```bash
# Install deps
pip install -r model/requirements.txt

# Train model
python3 model/train.py --dataset /path/to/dataset --epochs 30 --output model/

# Quantize → generates model/guarden_v1_int8.tflite + edge/src/model_data.h
python3 model/quantize.py --model model/guarden_v1.h5 --dataset /path/to/dataset

# Evaluate
python3 model/evaluate.py --model model/guarden_v1_int8.tflite --dataset /path/to/dataset
```

### Tests
```bash
# Run all Python tests
pytest tests/ -v

# Run specific test file
pytest tests/test_dashboard.py -v
```

---

## Build Order (Follow This Sequence)

1. **Dataset** → Download from Roboflow, preprocess to 640×480
2. **Model** → MobileNetV2 transfer learning → INT8 TFLite export
3. **Edge firmware** → PIR + camera + TFLite + MQTT (PlatformIO)
4. **Fog layer** → Mosquitto + PostgreSQL + Python ingestion
5. **Dashboard** → Flask API + Leaflet heatmap + Chart.js timelines
6. **Integration** → End-to-end testing, power profiling, field deployment

---

## Claude Code Guidelines

- **NEVER run git commands** — no `git commit`, `git push`, `git add`, or any other git operations. Ezekiel handles all version control manually.
- **Always use PlatformIO** for ESP32 builds, never raw Arduino CLI
- **Keep edge code minimal** — every byte of RAM matters on ESP32-C3 (400KB RAM)
- **Python services** should use `asyncio` + `aiomqtt` for non-blocking MQTT
- **No external API calls** — all services must work offline on local network
- **Config via environment variables** — never hardcode IPs, credentials, or node IDs
- **Test each layer independently** before integration
- When writing ESP32 code, always account for deep sleep wake stubs
- Dashboard must work on mobile (garden staff use phones)

---

## Common Pitfalls to Avoid

- ESP32-C3 has no PSRAM — model must fit in 400KB RAM after quantization
- OV2640 initialization order matters — follow Espressif's camera driver sequence
- Mosquitto on Pi must be configured to allow anonymous connections on local AP
- PostgreSQL `TIMESTAMPTZ` — always store UTC, convert to local for display
- TFLite INT8 models require input normalization to [-1, 1] or [0, 1] depending on model

---

## File Naming Conventions

- ESP32 source: `snake_case.cpp` / `snake_case.h`
- Python: `snake_case.py`
- Config files: `kebab-case.yaml` or `kebab-case.json`
- Models: `guarden_v{version}_int8.tflite`
- Test files: `test_{module_name}.py` / `test_{module_name}.cpp`
