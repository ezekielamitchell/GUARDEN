#pragma once

bool mqtt_connect();
void mqtt_publish_detection(float confidence, int battery_mv);
void mqtt_publish_status();
void mqtt_disconnect();
