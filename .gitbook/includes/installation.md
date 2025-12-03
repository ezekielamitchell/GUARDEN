---
layout:
  width: wide
  title:
    visible: true
  description:
    visible: false
  tableOfContents:
    visible: true
  outline:
    visible: true
  pagination:
    visible: true
  metadata:
    visible: false
---

# Installation

### Prerequisites

**Hardware:**

* ESP32-C3 with camera module (Grove Vision AI V2 Kit) × 4
* Raspberry Pi 5B+ (local server with built-in WiFi)
* MicroSD cards (32GB (min) for Pi, 32GB for each ESP32 node)
* LiPo batteries (5000mAh) and solar panels
* Weatherproof enclosures
* Power bank or AC adapter for Pi 5B+

**Software:**

* Raspberry Pi OS Lite (64-bit)
* Python 3.8+
* PlatformIO (ESP32 development)
* Mosquitto MQTT broker
* PostgreSQL 13+
* Nginx (web server)
* hostapd (WiFi access point software)

### Quick Start

```bash
# ============================================================
# RASPBERRY PI 5B+ SETUP (Local Server)
# ============================================================

# 1. Format setup script executable & run
chmod +wrx setup.sh
./setup.sh
```

### Full Start

<pre class="language-bash" data-expandable="true"><code class="lang-bash"><strong># ============================================================
</strong># RASPBERRY PI 5B+ SETUP (Local Server)
# ============================================================

# 1. Setup WiFi Access Point
sudo apt-get update
sudo apt-get upgrade -y


# Configure hostapd for WiFi AP
sudo nano /etc/hostapd/hostapd.conf
# Add:
#   interface=wlan0
#   ssid=GUARDEN-Local
#   hw_mode=g
#   channel=7
#   auth_algs=1
#   wpa=2
#   wpa_passphrase=guarden2024
#   wpa_key_mgmt=WPA-PSK

# Configure static IP
sudo nano /etc/dhcpcd.conf
# Add:
#   interface wlan0
#   static ip_address=192.168.4.1/24
#   nohook wpa_supplicant

# 2. Install system dependencies
sudo apt-get install postgresql mosquitto nginx python3-pip

# 3. Setup MQTT Broker
sudo systemctl enable mosquitto
sudo systemctl start mosquitto

# 4. Setup PostgreSQL database
sudo -u postgres createdb guarden
sudo -u postgres psql -c "CREATE USER guarden WITH PASSWORD 'guarden';"
sudo -u postgres psql -c "GRANT ALL PRIVILEGES ON DATABASE guarden TO guarden;"

# 5. Clone and setup GUARDEN
git clone https://github.com/ezekielmitchell/GUARDEN.git
cd GUARDEN

# 6. Install Python backend
cd src/fog/python_service
pip3 install -r requirements.txt
python3 init_db.py

# 7. Setup web dashboard
cd ../../web
sudo cp -r build/* /var/www/html/

# 8. Start services
sudo systemctl enable guarden-backend
sudo systemctl start guarden-backend
sudo systemctl restart nginx

# ============================================================
# ESP32-C3 NODE SETUP (Detection Nodes)
# ============================================================

# 1. Configure WiFi credentials in firmware
cd src/edge/firmware
nano include/config.h
# Set:
#   WIFI_SSID = "GUARDEN-Local"
#   WIFI_PASSWORD = "guarden2024"
#   MQTT_SERVER = "192.168.4.1"

# 2. Flash ESP32 firmware (requires PlatformIO)
pio run --target upload

# ============================================================
# ACCESS THE SYSTEM
# ============================================================

# 1. Connect your phone/laptop to WiFi network "GUARDEN-Local"
# 2. Open browser and navigate to: http://192.168.4.1
# 3. View live detections, heatmaps, and analytics
</code></pre>
