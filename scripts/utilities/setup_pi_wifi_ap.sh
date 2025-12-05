#!/bin/bash
# Raspberry Pi WiFi Access Point Setup Script
# This script configures the Pi as a WiFi access point with MQTT broker

set -e

SSID="dannywoo"
PASSWORD="dannywoo"
AP_IP="192.168.45.1"

echo "========================================="
echo "Setting up Raspberry Pi WiFi Access Point"
echo "========================================="
echo "SSID: $SSID"
echo "Password: $PASSWORD"
echo "AP IP: $AP_IP"
echo ""
read -p "Continue? (y/n) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    exit 1
fi

# Update system
echo "Updating system..."
sudo apt-get update

# Install required packages
echo "Installing hostapd and dnsmasq..."
sudo apt-get install -y hostapd dnsmasq

# Stop services for configuration
echo "Stopping services..."
sudo systemctl stop hostapd
sudo systemctl stop dnsmasq

# Configure static IP for wlan0
echo "Configuring static IP..."
sudo tee /etc/dhcpcd.conf > /dev/null << EOF
interface wlan0
    static ip_address=${AP_IP}/24
    nohook wpa_supplicant
EOF

# Configure dnsmasq
echo "Configuring DHCP server..."
sudo mv /etc/dnsmasq.conf /etc/dnsmasq.conf.backup || true
sudo tee /etc/dnsmasq.conf > /dev/null << EOF
interface=wlan0
dhcp-range=192.168.45.2,192.168.45.20,255.255.255.0,24h
domain=wlan
address=/gw.wlan/${AP_IP}
EOF

# Configure hostapd
echo "Configuring WiFi AP..."
sudo tee /etc/hostapd/hostapd.conf > /dev/null << EOF
interface=wlan0
driver=nl80211
ssid=${SSID}
hw_mode=g
channel=7
wmm_enabled=0
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=0
wpa=2
wpa_passphrase=${PASSWORD}
wpa_key_mgmt=WPA-PSK
wpa_pairwise=TKIP
rsn_pairwise=CCMP
EOF

# Point hostapd to config
sudo tee -a /etc/default/hostapd > /dev/null << EOF
DAEMON_CONF="/etc/hostapd/hostapd.conf"
EOF

# Enable IP forwarding (optional, for internet sharing)
echo "Enabling IP forwarding..."
sudo sed -i 's/#net.ipv4.ip_forward=1/net.ipv4.ip_forward=1/' /etc/sysctl.conf

# Enable services to start on boot
echo "Enabling services..."
sudo systemctl unmask hostapd
sudo systemctl enable hostapd
sudo systemctl enable dnsmasq

# Start services
echo "Starting services..."
sudo systemctl start hostapd
sudo systemctl start dnsmasq

echo ""
echo "========================================="
echo "Setup Complete!"
echo "========================================="
echo "WiFi AP Details:"
echo "  SSID: $SSID"
echo "  Password: $PASSWORD"
echo "  Pi IP: $AP_IP"
echo ""
echo "Reboot your Pi to ensure all changes take effect:"
echo "  sudo reboot"
echo ""
echo "After reboot, check status with:"
echo "  sudo systemctl status hostapd"
echo "  sudo systemctl status dnsmasq"
echo ""
