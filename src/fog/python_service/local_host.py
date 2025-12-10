from PyAccessPoint import pyaccesspoint
import socket
import time

ssid = 'dannywoo'
password = 'dannywoo'
ip = '192.168.45.1'
wlan = 'wlan0'
netmask='255.255.255.0'
inet = None

ap = pyaccesspoint.AccessPoint(wlan='wlan0', inet=inet, ip='192.168.45.1', ssid='dannywoo', password='dannywoo')

def ap_setup():
    try:
        ap.start()
        print("Access Point Started!")
        print(f"SSID: {ssid}")
        print(f"Password: {password}")
        print(f"IP Address: {ip}")
        print("Access point is running. Press Ctrl+C to stop.")

        # Keep running indefinitely
        while True:
            time.sleep(1)

    except KeyboardInterrupt:
        print("\nStopping access point...")
        ap.stop()
        print("Access point stopped.")
    except Exception as e:
        print(f"Error: {e}")
        ap.stop()
    
# def open_socket():
#     address = socket.getaddrinfo("0.0.0.0", 80)[0][-1]
#     s = socket.socket()
#     s.bind(address)
#     s.listen(3)
#     
#     return(s)

#s = open_socket()

ap_setup()

# try:
#     while True:
#         client = s.accept()[0]
#         request = client.recv(1024)
#         request = str(request)
#         print(request)
#         
#         client.close()
#     
# except OSError as e:
#     print("Error: connection terminated")
