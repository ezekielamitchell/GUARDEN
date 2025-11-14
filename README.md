# GUARDEN - Garden Urban AI Rodent Detection and Environment Node

**Seattle University - College of Science and Engineering** **ECEGR 4640: Internet of Things (IoT)**

**Team:** Ezekiel Mitchell, Rollan Cabalar, Narely Rivas Castellon, Tony Tran, Brandon Vu **Community Partner:** Danny Woo Community Garden (Liaison: Andy Allen, aallen@interimcda.org)

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE/) [![Platform](https://img.shields.io/badge/platform-ESP32--C3-green.svg)](https://www.espressif.com/en/products/socs/esp32-c3) [![Python](https://img.shields.io/badge/python-3.8+-blue.svg)](https://www.python.org/downloads/)

***

## Overview

The Danny Woo Community Garden (established 1975) is a 1.5-acre urban P-Patch serving Seattle's International District/Chinatown community. Rat infestations significantly reduce harvest yields. GUARDEN offers a data-driven solution utilizing distributed AI-powered camera nodes to monitor and map rat activity, facilitating targeted pest control.

### Key Features

* **Edge AI Detection** - On-device TensorFlow Lite inference on ESP32-C3
* **Low Power Operation** - Motion-triggered wake with deep sleep (10µA standby)
* **Privacy-First** - Metadata-only transmission, no images leave devices
* **Real-Time Monitoring** - Local web dashboard with live visualization
* **Data-Driven Insights** - Activity heatmaps, peak detection times, nesting area identification
* **Offline Operation** - Fully self-contained system with no internet dependency
* **Local Network** - Pi 3B+ acts as WiFi access point for ESP32 nodes and user access

***

### Technical Specifications

| Component         | Specification            | Performance             | Power           |
| ----------------- | ------------------------ | ----------------------- | --------------- |
| **Processor**     | ESP32-C3 RISC-V @ 160MHz | <250ms inference target | 240mA active    |
| **Camera**        | OV2640 2MP sensor        | 640×480 capture         | 80mA            |
| **AI Model**      | TensorFlow Lite INT8     | >90% accuracy target    | 160mA           |
| **Motion Sensor** | PIR sensor               | Event-driven wake       | 50µA standby    |
| **Sleep Mode**    | Deep sleep               | Extended battery        | 10µA            |
| **Connectivity**  | WiFi 802.11 b/g/n + MQTT | Reliable mesh           | 120mA TX        |
| **Storage**       | MicroSD + PostgreSQL     | Offline resilience      | 20mA write      |
| **Power**         | Solar + 5000mAh LiPo     | 10+ day runtime         | Self-sustaining |

### Performance Targets

| Metric             | Target   |
| ------------------ | -------- |
| Detection Latency  | <250ms   |
| Battery Life       | 7+ days  |
| Detection Accuracy | >90%     |
| Network Uptime     | >95%     |
| Storage Capacity   | 30+ days |

***

## Hardware Components

| Component                | Quantity | Unit Cost | Total  | Notes                                  |
| ------------------------ | -------- | --------- | ------ | -------------------------------------- |
| Grove Vision AI V2 Kit   | 4        | -         | -      | Provided on campus                     |
| Raspberry Pi 3B+         | 1        | $35       | $35    | Local server + WiFi AP                 |
| MicroSD Card (64GB)      | 1        | $12       | $12    | For Pi database                        |
| MicroSD Cards (32GB)     | 4        | $8        | $32    | For ESP32 local queuing                |
| LiPo Batteries (5000mAh) | 4        | $15       | $60    | 3.7V with protection                   |
| Miscellaneous            | -        | -         | \~$110 | Wire, connectors, mounting, enclosures |

**Projected Total:** \~$250 (including taxes)

**Network Architecture:**

* **Pi 3B+:** WiFi Access Point (SSID: GUARDEN-Local) - creates local network
* **ESP32 Nodes:** Connect to Pi's WiFi AP for MQTT communication
* **User Devices:** Connect to Pi's WiFi AP to access the web dashboard
* **No Internet Required:** Fully offline, self-contained system

**Power System:**

* **Hub (Pi 3B+):** Direct AC power or large power bank
* **Edge Nodes:** Solar panels + LiPo battery + backup

***

## Project Structure

```
GUARDEN/
├── src/
│   ├── edge/              # ESP32 firmware and edge detection
│   ├── fog/               # Raspberry Pi hub services
│   └── web/               # Web dashboard and API
├── tests/                 # Unit and integration tests
├── data/                  # Datasets and trained models
├── scripts/               # Training, deployment, utilities
├── hardware/              # BOM, assembly guides, enclosures
├── docs/                  # Documentation
└── config/                # Configuration files
```

***

## Development Timeline

| Phase               | Start      | End        | Deliverable       | Status        |
| ------------------- | ---------- | ---------- | ----------------- | ------------- |
| Order Components    | ASAP       | 10/28/2025 | Hardware acquired | ⏳ In Progress |
| Hardware Systems    | 10/28/2025 | 11/11/2025 | Assembled nodes   | 📋 Planned    |
| AI Detection Model  | 10/28/2025 | 11/11/2025 | Trained model     | 📋 Planned    |
| Web Dashboard       | 10/28/2025 | 11/11/2025 | Live dashboard    | 📋 Planned    |
| Finished Prototype  | 10/28/2025 | 11/11/2025 | Working prototype | 📋 Planned    |
| On-site Preparation | 11/11/2025 | 11/25/2025 | Site survey       | 📋 Planned    |
| Field Testing       | 11/25/2025 | 12/01/2025 | System validation | 📋 Planned    |
| Field Deployment    | 12/05/2025 | 12/05/2025 | Live deployment   | 📋 Planned    |
| Documentation       | 12/05/2025 | 12/09/2025 | Complete docs     | 📋 Planned    |
| Final Presentation  | 12/10/2025 | 12/10/2025 | Results           | 📋 Planned    |

***

## Team

**Students:**

* Ezekiel Mitchell (@ezekielmitchell) - AI Detection Model
* Rollan Cabalar - Web Dashboard & Backend
* Narely Rivas Castellon - On-site Preparation
* Tony Tran - On-site Preparation
* Brandon Vu - On-site Preparation

**Community Partner:**

* Danny Woo Community Garden
* Liaison: Andy Allen (aallen@interimcda.org)

***

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE/) file for details.

***

***

## Contact

**Project Repository:** [https://github.com/ezekielmitchell/GUARDEN](https://github.com/ezekielmitchell/GUARDEN) **Team Email:** emitchell4@seattleu.edu
