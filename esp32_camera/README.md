# GUARDEN ESP32 Camera Module

ESP32 Wrover-E camera code for the GUARDEN (Garden Urban AI Rodent Detection and Environment Node) project.

## Hardware Requirements

- **ESP32 Wrover-E** module with PSRAM
- **OV2640 Camera** module (standard ESP32-CAM configuration)
- **WiFi network** for connectivity
- Optional: **SD card module** for local image storage
- Optional: **External antenna** for better WiFi range

## Pin Configuration

The camera pins are configured for the standard ESP32-CAM board (AI-Thinker):

| Function | GPIO Pin |
|----------|----------|
| PWDN     | 32       |
| RESET    | -1       |
| XCLK     | 0        |
| SIOD     | 26       |
| SIOC     | 27       |
| Y9       | 35       |
| Y8       | 34       |
| Y7       | 39       |
| Y6       | 36       |
| Y5       | 21       |
| Y4       | 19       |
| Y3       | 18       |
| Y2       | 5        |
| VSYNC    | 25       |
| HREF     | 23       |
| PCLK     | 22       |
| Flash LED| 4        |

## Software Requirements

### PlatformIO Installation

1. Install [Visual Studio Code](https://code.visualstudio.com/)
2. Install the [PlatformIO IDE extension](https://platformio.org/install/ide?install=vscode)

### Alternative: Arduino IDE

If using Arduino IDE:
1. Install [ESP32 board support](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html)
2. Install the **ESP32 Camera** library from Library Manager

## Configuration

### 1. WiFi Settings

Edit `src/main.cpp` and update your WiFi credentials:

```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### 2. Roboflow API Settings

Update your Roboflow API details:

```cpp
const char* roboflow_api_url = "https://detect.roboflow.com/YOUR_MODEL/YOUR_VERSION";
const char* roboflow_api_key = "YOUR_ROBOFLOW_API_KEY";
```

You can get these from your Roboflow project dashboard.

### 3. Detection Interval

Adjust the capture frequency (in milliseconds):

```cpp
const unsigned long DETECTION_INTERVAL = 5000; // 5 seconds
```

## Building and Uploading

### Using PlatformIO

```bash
# Navigate to the esp32_camera directory
cd esp32_camera

# Build the project
pio run

# Upload to ESP32
pio run --target upload

# Monitor serial output
pio device monitor
```

### Using PlatformIO GUI

1. Open the `esp32_camera` folder in VS Code
2. Click the PlatformIO icon in the sidebar
3. Select **Build** under PROJECT TASKS
4. Select **Upload** to flash the device
5. Select **Monitor** to view serial output

## Camera Settings

The camera is configured for optimal rodent detection:

- **Resolution**: UXGA (1600x1200) - can be changed in `camera_config.h`
- **Format**: JPEG for easy transmission
- **Quality**: 12 (0-63, lower = higher quality)
- **Frame Buffer**: Stored in PSRAM

### Adjusting Resolution

Edit `include/camera_config.h` and change `frame_size`:

```cpp
.frame_size = FRAMESIZE_UXGA,  // Options: QVGA, VGA, SVGA, XGA, SXGA, UXGA
```

Available frame sizes:
- `FRAMESIZE_QVGA` (320x240)
- `FRAMESIZE_VGA` (640x480)
- `FRAMESIZE_SVGA` (800x600)
- `FRAMESIZE_XGA` (1024x768)
- `FRAMESIZE_SXGA` (1280x1024)
- `FRAMESIZE_UXGA` (1600x1200)

## Features

### Current Features

- ✅ Camera initialization with error handling
- ✅ WiFi connectivity
- ✅ Automatic image capture at intervals
- ✅ Roboflow API integration for rodent detection
- ✅ Flash LED indicator for events
- ✅ Serial debug output
- ✅ Brownout protection

### Planned Features

- 📋 SD card image storage
- 📋 Deep sleep mode for battery operation
- 📋 Motion detection trigger
- 📋 MQTT publishing for IoT integration
- 📋 Web interface for configuration
- 📋 OTA (Over-The-Air) updates

## Troubleshooting

### Camera Initialization Failed

- Check all pin connections
- Ensure camera module is properly seated
- Verify power supply is adequate (500mA minimum)
- Check if PSRAM is enabled in board settings

### WiFi Connection Issues

- Verify SSID and password are correct
- Check WiFi signal strength
- Ensure 2.4GHz WiFi (ESP32 doesn't support 5GHz)
- Move closer to router or use external antenna

### Brown-out Detector Triggered

- Use a better quality power supply
- Add bulk capacitors (100-1000µF) near power input
- Reduce camera resolution or JPEG quality

### Upload Failed

- Press and hold the BOOT/IO0 button during upload
- Reduce upload speed in `platformio.ini`
- Check USB cable (use data cable, not charge-only)
- Verify correct COM port is selected

## Serial Monitor Output

Expected output on successful startup:

```
=== GUARDEN ESP32 Camera Starting ===
Initializing camera...
Camera initialized successfully
Initializing WiFi...
Connecting to WiFi..........
WiFi connected
IP address: 192.168.1.100
=== Setup Complete ===
Starting rodent detection system...

--- Capturing image ---
Camera capture success: 45678 bytes
Sending to Roboflow for analysis...
HTTP Response code: 200
Response: {"predictions": [...]}
Detection analysis complete
```

## Power Consumption

- **Active (WiFi + Camera)**: ~200-300mA
- **Idle (WiFi on)**: ~80-100mA
- **Deep Sleep**: ~10µA (when implemented)

For battery operation, implement deep sleep mode between captures.

## License

Part of the GUARDEN project - Garden Urban AI Rodent Detection and Environment Node

## Support

For issues or questions:
1. Check the troubleshooting section above
2. Review ESP32-CAM documentation
3. Check Roboflow API documentation
4. Open an issue in the GUARDEN repository
