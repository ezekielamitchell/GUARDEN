---
layout:
  width: wide
  title:
    visible: true
  description:
    visible: true
  tableOfContents:
    visible: true
  outline:
    visible: true
  pagination:
    visible: true
  metadata:
    visible: true
---

# GUARDEN - Garden Urban AI Rodent Detection and Environment Node

<div align="center" data-with-frame="true"><figure><picture><source srcset=".gitbook/includes/.gitbook/assets/guarden_dark (1).png" media="(prefers-color-scheme: dark)"><img src=".gitbook/assets/guarden.png" alt="" width="375"></picture><figcaption><p>image [1] logo</p></figcaption></figure></div>

<p align="center"><strong>Seattle University - College of Science and Engineering</strong><br><br><strong>ECEGR 4640: Internet of Things (IoT)</strong></p>

<p align="center">Rollan Cabalar, Narely Rivas Castellon, Ezekiel A. Mitchell, Tony Tran, Brandon Vu<br>{rcabalar, nrivascastellon, emitchell4, ttran23, bvu}@seattleu.edu<br><br>Danny Woo Community Garden (Andy Allen, aallen@interimcda.org)</p>

<p align="center"><a href="LICENSE/"><img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License"></a> <a href="https://www.espressif.com/en/products/socs/esp32-c3"><img src="https://img.shields.io/badge/platform-ESP32--C3-green.svg" alt="Platform"></a> <a href="https://www.python.org/downloads/"><img src="https://img.shields.io/badge/python-3.8+-blue.svg" alt="Python"></a></p>

***

## Overview

The Danny Woo Community Garden (established 1975) is a 1.5-acre urban P-Patch serving Seattle's International District/Chinatown community. Rat infestations significantly reduce harvest yields. GUARDEN offers a data-driven solution utilizing distributed AI-powered camera nodes to monitor and map rat activity, facilitating targeted pest control.

***

## Key Features

* **Edge AI Detection** - On-device TensorFlow Lite inference on ESP32-C3
* **Low Power Operation** - Motion-triggered wake with deep sleep (10µA standby)
* **Privacy-First** - Metadata-only transmission, no images leave devices
* **Real-Time Monitoring** - Local web dashboard with live visualization
* **Data-Driven Insights** - Activity heatmaps, peak detection times, nesting area identification
* **Offline Operation** - Fully self-contained system with no internet dependency
* **Local Network** - Pi 3B+ acts as WiFi access point for ESP32 nodes and user access

***

## Technical Specifications

<table data-full-width="true"><thead><tr><th>Component</th><th>Specification</th><th>Performance</th><th>Power</th></tr></thead><tbody><tr><td><strong>Processor</strong></td><td>ESP32-C3 RISC-V @ 160MHz</td><td>&#x3C;250ms inference target</td><td>240mA active</td></tr><tr><td><strong>Camera</strong></td><td>OV2640 2MP sensor</td><td>640×480 capture</td><td>80mA</td></tr><tr><td><strong>AI Model</strong></td><td>TensorFlow Lite INT8</td><td>>90% accuracy target</td><td>160mA</td></tr><tr><td><strong>Motion Sensor</strong></td><td>PIR sensor</td><td>Event-driven wake</td><td>50µA standby</td></tr><tr><td><strong>Sleep Mode</strong></td><td>Deep sleep</td><td>Extended battery</td><td>10µA</td></tr><tr><td><strong>Connectivity</strong></td><td>WiFi 802.11 b/g/n + MQTT</td><td>Reliable mesh</td><td>120mA TX</td></tr><tr><td><strong>Storage</strong></td><td>MicroSD + PostgreSQL</td><td>Offline resilience</td><td>20mA write</td></tr><tr><td><strong>Power</strong></td><td>Solar + 5000mAh LiPo</td><td>10+ day runtime</td><td>Self-sustaining</td></tr></tbody></table>

***

## Performance Targets

<table><thead><tr><th>Metric</th><th>Target</th><th>Status<select><option value="sqgq1MesM8XL" label="Achieved" color="blue"></option><option value="83KQpvtBIYxJ" label="In progress" color="blue"></option></select></th></tr></thead><tbody><tr><td>Detection Latency</td><td>&#x3C;1s</td><td><span data-option="83KQpvtBIYxJ">In progress</span></td></tr><tr><td>Battery Life</td><td>3+ days</td><td><span data-option="83KQpvtBIYxJ">In progress</span></td></tr><tr><td>Detection Accuracy</td><td>>65%</td><td><span data-option="83KQpvtBIYxJ">In progress</span></td></tr><tr><td>Network Uptime</td><td>>20%</td><td><span data-option="83KQpvtBIYxJ">In progress</span></td></tr><tr><td>Storage Capacity</td><td>3+ days</td><td><span data-option="83KQpvtBIYxJ">In progress</span></td></tr></tbody></table>

***

## Project Structure

```
GUARDEN/
├── src/
│   ├── edge/              # ESP32 firmware and edge detection
│   ├── fog/               # Raspberry Pi hub services
│   └── cloud/             # Web dashboard and API
├── tests/                 # Unit and integration tests
├── data/                  # Datasets and trained models
├── scripts/               # Training, deployment, utilities
├── hardware/              # BOM, assembly guides, enclosures
├── docs/                  # Documentation
└── config/                # Configuration files
```
