import paho.mqtt.client as mqtt
import sqlite3
import json
import datetime
import sys

# --- Configuration ---
BROKER_ADDRESS = "192.168.1.237"
MQTT_TOPIC = "endr/secure"
DB_FILE = "/home/pi/GUARDEN/data/raw/node_a_log.db"

# --- Database Setup & Logging ---
def setup_database():
    """Initializes the SQLite database and creates the table."""
    try:
        conn = sqlite3.connect(DB_FILE)
        cursor = conn.cursor()
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS node_status (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                node_id TEXT,
                ip_address TEXT,
                data_timestamp TEXT,
                log_timestamp TEXT,
                full_payload TEXT
            )
        ''')
        conn.commit()
        conn.close()
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] Database ready: {DB_FILE}")
    except sqlite3.Error as e:
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] FATAL DB ERROR: {e}")
        sys.exit(1)

def log_message(payload_data):
    """Parses JSON and inserts data into the database."""
    try:
        data = json.loads(payload_data)
        
        conn = sqlite3.connect(DB_FILE)
        cursor = conn.cursor()
        
        node_id = data.get("node_id", "UNKNOWN")
        ip_address = data.get("ip_address", "0.0.0.0")
        data_timestamp = data.get("timestamp", "N/A")
        log_timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        cursor.execute('''
            INSERT INTO node_status (node_id, ip_address, data_timestamp, log_timestamp, full_payload)
            VALUES (?, ?, ?, ?, ?)
        ''', (node_id, ip_address, data_timestamp, log_timestamp, payload_data))
        
        conn.commit()
        conn.close()
        
        # log output
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] Logged: {node_id} @ {ip_address}")

    except Exception as e:
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] Error: {e}")


# --- MQTT Callbacks ---
def on_connect(client, userdata, flags, rc):
    """Handles broker connection status."""
    if rc == 0:
        client.subscribe(MQTT_TOPIC)
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] Connected. Subscribing to {MQTT_TOPIC}")
    else:
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] Connection failed: {rc}. Check broker status.")

def on_message(client, userdata, msg):
    """Processes incoming messages."""
    payload_str = msg.payload.decode("utf-8")
    log_message(payload_str)


if __name__ == "__main__":
    setup_database()
    
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    
    try:
        client.connect(BROKER_ADDRESS, 1883, 60)
        client.loop_forever()
    except Exception as e:
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] Failed to connect to broker: {e}")