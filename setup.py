## Setup script for a new Pi 5B
import subprocess
import sys

def run(cmd, check=True):
    print('->', ' '.join(cmd))
    subprocess.run(cmd, check=check)

def main():
    try:
        # Update package list
        run(["sudo", "apt", "update"])

        # Install Mosquitto broker and clients (MQTT)
        run(["sudo", "apt", "install", "-y", "mosquitto", "mosquitto-clients"])

        # Install Python MQTT client (Debian package)
        run(["sudo", "apt", "install", "-y", "python3-paho-mqtt"])

        # Enable and start Mosquitto
        run(["sudo", "systemctl", "enable", "mosquitto"])
        run(["sudo", "systemctl", "start", "mosquitto"])

        # Show status (will print journal status to terminal)
        run(["sudo", "systemctl", "status", "mosquitto"])

        # Make test scripts executable (uses sudo to match original behaviour)
        run(["sudo", "chmod", "+wrx", "tests/fog/mqtt_sub_test.sh", "tests/fog/mqtt_pub_test.sh"])

        print("Setup completed.")
    except subprocess.CalledProcessError as e:
        print("Command failed: ", e, file=sys.stderr)
        sys.exit(e.returncode)

if __name__ == '__main__':
    main()