#!/bin/bash
# GUARDEN — One-shot Raspberry Pi Setup Script
# Run as: sudo bash fog/setup.sh

set -e
echo "=== GUARDEN Pi Setup ==="

# ─── System packages ──────────────────────────────────────────────────────────
apt-get update -qq
apt-get install -y \
    mosquitto mosquitto-clients \
    postgresql postgresql-client \
    python3-pip python3-venv \
    hostapd dnsmasq \
    git curl

# ─── Mosquitto ────────────────────────────────────────────────────────────────
echo "--- Configuring Mosquitto ---"
cp fog/broker/mosquitto.conf /etc/mosquitto/conf.d/guarden.conf
systemctl enable mosquitto
systemctl restart mosquitto
echo "Mosquitto: OK"

# ─── PostgreSQL ───────────────────────────────────────────────────────────────
echo "--- Configuring PostgreSQL ---"
systemctl enable postgresql
systemctl start postgresql

sudo -u postgres psql -c "CREATE USER guarden WITH PASSWORD 'guarden';" 2>/dev/null || true
sudo -u postgres psql -c "CREATE DATABASE guarden OWNER guarden;" 2>/dev/null || true
sudo -u postgres psql -U guarden -d guarden -f fog/db/schema.sql
echo "PostgreSQL: OK"

# ─── Python environment ───────────────────────────────────────────────────────
echo "--- Setting up Python env ---"
python3 -m venv /opt/guarden/venv
/opt/guarden/venv/bin/pip install -q -r fog/requirements.txt
echo "Python venv: OK"

# ─── WiFi Access Point (hostapd + dnsmasq) ───────────────────────────────────
echo "--- Configuring WiFi AP ---"
cat > /etc/hostapd/hostapd.conf <<EOF
interface=wlan0
driver=nl80211
ssid=GUARDEN_AP
hw_mode=g
channel=6
wmm_enabled=0
macaddr_acl=0
auth_algs=1
wpa=2
wpa_passphrase=guarden2025
wpa_key_mgmt=WPA-PSK
wpa_pairwise=TKIP
rsn_pairwise=CCMP
EOF

cat > /etc/dnsmasq.conf <<EOF
interface=wlan0
dhcp-range=192.168.4.2,192.168.4.20,255.255.255.0,24h
EOF

# Static IP for wlan0
cat >> /etc/dhcpcd.conf <<EOF
interface wlan0
    static ip_address=192.168.4.1/24
    nohook wpa_supplicant
EOF

systemctl unmask hostapd
systemctl enable hostapd dnsmasq
systemctl restart hostapd dnsmasq
echo "WiFi AP: OK (SSID: GUARDEN_AP)"

# ─── Systemd services ─────────────────────────────────────────────────────────
echo "--- Creating systemd services ---"

cat > /etc/systemd/system/guarden-ingestion.service <<EOF
[Unit]
Description=GUARDEN MQTT Ingestion Service
After=network.target mosquitto.service postgresql.service

[Service]
ExecStart=/opt/guarden/venv/bin/python3 /opt/guarden/fog/ingestion/main.py
WorkingDirectory=/opt/guarden
EnvironmentFile=/opt/guarden/.env
Restart=always
RestartSec=5
User=pi

[Install]
WantedBy=multi-user.target
EOF

cat > /etc/systemd/system/guarden-dashboard.service <<EOF
[Unit]
Description=GUARDEN Web Dashboard
After=network.target postgresql.service guarden-ingestion.service

[Service]
ExecStart=/opt/guarden/venv/bin/python3 /opt/guarden/fog/dashboard/app.py
WorkingDirectory=/opt/guarden
EnvironmentFile=/opt/guarden/.env
Restart=always
RestartSec=5
User=pi

[Install]
WantedBy=multi-user.target
EOF

systemctl daemon-reload
systemctl enable guarden-ingestion guarden-dashboard
systemctl start guarden-ingestion guarden-dashboard

echo ""
echo "=== GUARDEN Setup Complete ==="
echo "Dashboard: http://192.168.4.1:5000"
echo "MQTT Broker: 192.168.4.1:1883"
echo "WiFi SSID: GUARDEN_AP / guarden2025"
