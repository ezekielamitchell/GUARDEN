# Setup script for configuring a new Pi 5

# Update package list
sudo apt update

# Install Mosquitto broker and clients (MQTT)
sudo apt install -y mosquitto mosquitto-clients

# Install Python MQTT client (Debian package)
sudo apt install -y python3-paho-mqtt

# Enable Mosquitto to start on boot
sudo systemctl enable mosquitto

# Start Mosquitto service
sudo systemctl start mosquitto

# Check status
sudo systemctl status mosquitto

# Enable MQTT testing scripts
chmod +wrx mqtt_sub_test.sh mqtt_pub_test.sh