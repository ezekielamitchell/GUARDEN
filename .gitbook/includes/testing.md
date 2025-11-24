---
description: >-
  Ensure either the setup.sh or setup.py script has been ran prior to testing
  attempt.
---

# Testing

## MQTT Status

```bash
# Enable Mosquitto to start on boot
sudo systemctl enable mosquitto

# Start Mosquitto service
sudo systemctl start mosquitto

# Check status
sudo systemctl status mosquitto
```

## MQTT Testing

After running the setup shell script, in two separate terminal windows, complete the following prompts below:

### Terminal A - Subscriber

```bash
# Run the mqtt_sub_test.sh script
cd tests/edge
./mqtt_sub_test.sh
```

```bash
# Direct testing
# Subscribe to a test topic (in one terminal)
cd tests/edge
mosquitto_sub -h localhost -t test/topic
```

### Terminal B - Publisher

```bash
# Run the mqtt_pub_test.sh script
cd tests/edge
./mqtt_pub_test.sh
```

```bash
# Direct testing
# Publish a message (in another terminal)
cd tests/edge
mosquitto_pub -h localhost -t test/topic -m "MQTT pass"
```

