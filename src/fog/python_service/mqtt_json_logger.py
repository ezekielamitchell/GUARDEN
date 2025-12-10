import paho.mqtt.client as mqtt
import json
import datetime
import sys
import os
from pathlib import Path

# --- Configuration ---
BROKER_ADDRESS = "192.168.45.1"  # dannywoo broker on this Pi
MQTT_TOPIC = "guarden"  # Topic that NodeA publishes to
JSON_DIR = "/home/pi/GUARDEN/data/raw/json_logs"
JSON_FILE = os.path.join(JSON_DIR, "node_data.json")

# --- Setup ---
def setup_json_storage():
    """Ensures the JSON storage directory exists."""
    try:
        Path(JSON_DIR).mkdir(parents=True, exist_ok=True)
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] JSON storage ready: {JSON_DIR}")
    except Exception as e:
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] FATAL: Could not create directory: {e}")
        sys.exit(1)

def log_to_json(payload_data):
    """Saves incoming MQTT data to a JSON file."""
    try:
        # Parse the incoming JSON
        data = json.loads(payload_data)

        # Add receive timestamp
        data["received_at"] = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        # Load existing data or create new list
        if os.path.exists(JSON_FILE):
            with open(JSON_FILE, 'r') as f:
                try:
                    all_data = json.load(f)
                    if not isinstance(all_data, list):
                        all_data = [all_data]
                except json.JSONDecodeError:
                    all_data = []
        else:
            all_data = []

        # Append new data
        all_data.append(data)

        # Write back to file with pretty formatting
        with open(JSON_FILE, 'w') as f:
            json.dump(all_data, f, indent=2)

        # Console output
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] Saved: {data.get('node_id', 'UNKNOWN')} @ {data.get('ip_address', 'N/A')}")
        print(f"    Data: {payload_data}")

    except json.JSONDecodeError as e:
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] JSON Parse Error: {e}")
        print(f"    Raw payload: {payload_data}")
    except Exception as e:
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] Error saving data: {e}")

# --- MQTT Callbacks ---
def on_connect(client, userdata, flags, reason_code, properties):
    """Handles broker connection status."""
    if reason_code == 0:
        client.subscribe(MQTT_TOPIC)
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] ✓ Connected to {BROKER_ADDRESS}")
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] ✓ Subscribed to topic: {MQTT_TOPIC}")
    else:
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] ✗ Connection failed: {reason_code}")

def on_message(client, userdata, msg):
    """Processes incoming MQTT messages."""
    payload_str = msg.payload.decode("utf-8")
    print(f"\n[{datetime.datetime.now().strftime('%H:%M:%S')}] Message received on {msg.topic}")
    log_to_json(payload_str)

def on_disconnect(client, userdata, flags, reason_code, properties):
    """Handles disconnection."""
    print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] Disconnected (code: {reason_code})")

# --- Main ---
if __name__ == "__main__":
    print("=" * 50)
    print("GUARDEN MQTT JSON Logger")
    print("=" * 50)
    print(f"Broker: {BROKER_ADDRESS}:1883")
    print(f"Topic: {MQTT_TOPIC}")
    print(f"JSON File: {JSON_FILE}")
    print("=" * 50)

    setup_json_storage()

    # Create MQTT client
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="guarden_json_logger")
    client.on_connect = on_connect
    client.on_message = on_message
    client.on_disconnect = on_disconnect

    try:
        print(f"\n[{datetime.datetime.now().strftime('%H:%M:%S')}] Connecting to broker...")
        client.connect(BROKER_ADDRESS, 1883, 60)
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] Starting loop... (Press Ctrl+C to stop)")
        client.loop_forever()
    except KeyboardInterrupt:
        print(f"\n[{datetime.datetime.now().strftime('%H:%M:%S')}] Shutting down...")
        client.disconnect()
    except Exception as e:
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] FATAL: {e}")
        sys.exit(1)
