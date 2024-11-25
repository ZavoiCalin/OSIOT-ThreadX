// iot_hub_connection.c

#include "MQTTAsync.h"
#include "iot_hub_connection.h"

MQTTAsync client;
int connected = 0;

void on_connect(void* context, MQTTAsync_successData* response) {
    printf("Connected to Azure IoT Hub successfully!\n");
    connected = 1;
}

void on_connect_failure(void* context, MQTTAsync_failureData* response) {
    printf("Failed to connect to Azure IoT Hub\n");
    connected = 0;
}

int connect_to_iothub() {
    int rc;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = IOT_HUB_HOSTNAME "/" DEVICE_ID "/?api-version=2021-04-12";
    conn_opts.password = SAS_TOKEN;
    conn_opts.onSuccess = on_connect;
    conn_opts.onFailure = on_connect_failure;
    conn_opts.context = client;

    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS) {
        printf("Failed to start connect, return code %d\n", rc);
        return rc;
    }
    return 0;
}

void mqtt_setup() {
    int rc;
    MQTTAsync_create(&client, "ssl://" IOT_HUB_HOSTNAME ":" MQTT_PORT, MQTT_CLIENT_ID,
                     MQTTCLIENT_PERSISTENCE_NONE, NULL);

    MQTTAsync_setCallbacks(client, NULL, NULL, NULL, NULL);

    if ((rc = connect_to_iothub()) != 0) {
        printf("Connection attempt to Azure IoT Hub failed!\n");
    }
}

void send_telemetry() {
    if (connected) {
        printf("Sending telemetry data...\n");

        // Create telemetry payload
        char payload[128];
        sprintf(payload, "{\"temperature\": %f}", 22.5);  // Simulating temperature data

        MQTTAsync_message message = MQTTAsync_message_initializer;
        message.payload = payload;
        message.payloadlen = (int)strlen(payload);
        message.qos = 1;
        message.retained = 0;

        int rc;
        if ((rc = MQTTAsync_sendMessage(client, TOPIC, &message, NULL)) != MQTTASYNC_SUCCESS) {
            printf("Failed to send message, return code %d\n", rc);
        } else {
            printf("Message sent to Azure IoT Hub\n");
        }
    }
}
