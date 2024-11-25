// main.c

#include "tx_api.h"
#include "tx_application.h"
#include "iot_hub_connection.h"

int main(void) {
    // Start ThreadX kernel
    tx_kernel_enter();

    // Set up connection to Azure IoT Hub using MQTT
    mqtt_setup();

    // ThreadX initialization and application setup
    tx_application_define(NULL);

    // Main loop for telemetry sending
    while (1) {
        send_telemetry();  // Send telemetry periodically
        tx_thread_sleep(1000);  // Sleep between messages
    }

    return 0;
}
