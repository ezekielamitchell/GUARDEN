#!/bin/bash
# Raspberry Pi WiFi Access Point Diagnostic Script
# Run this on your Raspberry Pi to diagnose WiFi AP issues

echo "========================================="
echo "Raspberry Pi WiFi AP Diagnostic"
echo "========================================="
echo ""

echo "--- 1. Checking WiFi Interface Status ---"
ip addr show wlan0
echo ""

echo "--- 2. Checking hostapd Status ---"
sudo systemctl status hostapd --no-pager | head -20
echo ""

echo "--- 3. Checking dnsmasq Status ---"
sudo systemctl status dnsmasq --no-pager | head -20
echo ""

echo "--- 4. Active WiFi Connections ---"
sudo iw dev wlan0 station dump
echo ""

echo "--- 5. Checking hostapd Configuration ---"
if [ -f /etc/hostapd/hostapd.conf ]; then
    echo "hostapd.conf exists:"
    sudo cat /etc/hostapd/hostapd.conf | grep -v "^#" | grep -v "^$"
else
    echo "WARNING: /etc/hostapd/hostapd.conf NOT FOUND!"
fi
echo ""

echo "--- 6. Checking dnsmasq Configuration ---"
if [ -f /etc/dnsmasq.conf ]; then
    echo "dnsmasq.conf relevant lines:"
    sudo cat /etc/dnsmasq.conf | grep -v "^#" | grep -v "^$"
else
    echo "WARNING: /etc/dnsmasq.conf NOT FOUND!"
fi
echo ""

echo "--- 7. Checking IP Forwarding ---"
echo "IP Forward status: $(cat /proc/sys/net/ipv4/ip_forward)"
echo ""

echo "--- 8. Checking DHCP Leases ---"
if [ -f /var/lib/misc/dnsmasq.leases ]; then
    echo "Active DHCP leases:"
    cat /var/lib/misc/dnsmasq.leases
else
    echo "No DHCP leases file found"
fi
echo ""

echo "--- 9. Checking MQTT Broker Status ---"
sudo systemctl status mosquitto --no-pager | head -20
echo ""

echo "--- 10. Network Interfaces ---"
ip link show
echo ""

echo "========================================="
echo "Diagnostic Complete"
echo "========================================="
echo ""
echo "COMMON ISSUES:"
echo "1. If hostapd is 'inactive' - AP is not running"
echo "2. If no DHCP leases - devices can't get IPs"
echo "3. If mosquitto is 'inactive' - MQTT broker down"
echo "4. Check that wlan0 has IP 192.168.45.1"
echo ""
