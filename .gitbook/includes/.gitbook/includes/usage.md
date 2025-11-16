# Usage

### Deploying Detection Nodes

```bash
# 1. Configure node settings
vim config/edge_config.json

# 2. Flash firmware
scripts/deployment/flash_firmware.sh

# 3. Verify connectivity
mosquitto_sub -t 'guarden/#' -v
```

### Accessing the Dashboard

The local web dashboard provides:

* Real-time detection timeline with live updates
* Activity heatmap by camera location
* Peak hours analysis and pattern detection
* System health monitoring (battery levels, connectivity)
* Historical data visualization

**Access Steps:**

1. Connect your device (phone/laptop/tablet) to WiFi network `GUARDEN-Local`
2. Password: `guarden2024`
3. Open browser and navigate to: `http://192.168.4.1`
4. No internet connection required - fully offline operation

### Exporting Data

```bash
python scripts/utilities/export_data.py --start-date 2025-11-01 --end-date 2025-11-07
```

Data exports as CSV for analysis.

### Generating Reports

```bash
python scripts/utilities/generate_report.py --period weekly
```

Generates PDF reports with activity zones, peak times, and pest control recommendations.
