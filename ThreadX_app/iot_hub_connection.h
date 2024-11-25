// iot_hub_connection.h

#ifndef IOT_HUB_CONNECTION_H
#define IOT_HUB_CONNECTION_H

#define IOT_HUB_HOSTNAME "your-iothub-hostname.azure-devices.net"
#define DEVICE_ID "your-device-id"
#define DEVICE_KEY "your-device-symmetric-key"
#define MQTT_PORT "8883"
#define SAS_TOKEN "your-sas-token"
#define MQTT_CLIENT_ID "your-device-client-id"
#define TOPIC "devices/" DEVICE_ID "/messages/events/"

void mqtt_setup();
void send_telemetry();

#endif
