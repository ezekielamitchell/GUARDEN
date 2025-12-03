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

# Background

## MQTT (Message Queuing Telemetry Transport)

MQTT is a lightweight, publish-subscribe messaging protocol designed for fast, efficient communication between devices over unreliable or low-bandwidth networks.

{% hint style="warning" %}
MQTT does NOT generate, modify, or store data. It only relays messages between various devices connected to it.
{% endhint %}

* **Architecture:** It uses a broker to manage message distribution between publishers (senders) and subscribers (receivers).
* **Use Cases:** Commonly used for IoT (Internet of Things) applications, such as sensor networks and smart devices.
* **Advantages:** Low overhead, minimal resource use, and real-time, reliable messaging even on constrained devices and networks.
* **Communication:** Supports topics to organize messages, with clients subscribing to only the data they need.

{% hint style="info" %}
In GUARDEN, each ESP32-C3 node receives rodent detection results from its connected Grove Vision AI module and uses MQTT to publish these detection events to a central broker. This allows real-time, efficient, and reliable transmission of alerts from the garden nodes to the cloud dashboard and alerting system.
{% endhint %}
