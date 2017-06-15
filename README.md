# ARTIK SDK Examples

This repository contains example programs to be built against the ARTIK SDK available on Samsung ARTIK boards (www.artik.io).
These programs must be built natively on the ARTIK board, after making sure the ARTIK SDK development packages are installed
(follow [instructions](https://developer.artik.io/documentation/api/ARTIK.html)).

## ADC
### [artik_adc_test.c](/adc/artik_adc_test.c)
This example reads a converted analog input every second and display the result in the console.

## Bluetooth
### [artik_bluetooth_test.c](/bluetooth/artik_bluetooth_test.c)
This example scans for the surrounding Bluetooth devices, display them, then try to connect to the device
whose address is passed as a parameter.

### [artik_bluetooth_test_advertisement.c](/bluetooth/artik_bluetooth_test_advertisement.c)
This test advertises some sample LE services for the surrounding devices to detect.

### [artik_bluetooth_test_agent.c](/bluetooth/artik_bluetooth_test_agent.c)
This program starts a pairing agent to enable the user to authorize devices trying to pair with the device.

### [artik_bluetooth_test_avrcp.c](/bluetooth/artik_bluetooth_test_avrcp.c)
This program connects to a remote device exposing the AVRCP profile (e.g. smartphone), and exposes some commands
to let the user control audio/video playback on the remote device.

### [artik_bluetooth_test_ftp.c](/bluetooth/artik_bluetooth_test_ftp.c)
This program connects to a remote device exposing the FTP server profile, and exposes some commands to let the user
browse and exchange files with the remote device.

### [artik_bluetooth_test_gatt_server.c](/bluetooth/artik_bluetooth_test_gatt_server.c)
This program connects exposed and advertises a GATT server profile with a battery level service. It supports notification
and will send the battery level to registered clients every second.

### [artik_bluetooth_test_gatt_client_rw.c](/bluetooth/artik_bluetooth_test_gatt_client_rw.c)
This program connects to a remote device exposing the GATT server profile and the battery level service, then tries to read
and write the battery level value. It is meant to be tested against the [artik_bluetooth_test_gatt_server.c](/bluetooth/artik_bluetooth_test_gatt_server.c)
program.

### [artik_bluetooth_test_gatt_client.c](/bluetooth/artik_bluetooth_test_gatt_client.c)
This program connects to a remote device exposing the GATT server profile and the battery level service, then tries to read
the service properties and the battery level value. It also registers notification from the battery level service, and displays its value
whenever a notification is received. It is meant to be tested against the [artik_bluetooth_test_gatt_server.c](/bluetooth/artik_bluetooth_test_gatt_server.c)
program.

### [artik_bluetooth_test_nap.c](/bluetooth/artik_bluetooth_test_nap.c)
This program starts a PAN server on the target with proper network configuration, and allows remote PANU devices to join the network.

### [artik_bluetooth_test_panu.c](/bluetooth/artik_bluetooth_test_panu.c)
This program scans for bluetooth devices, then allows connecting to a PAN server as a PANU device. It is meant to be tested against
[artik_bluetooth_test_nap.c](/bluetooth/artik_bluetooth_test_nap.c)

### [artik_bluetooth_test_spp.c](/bluetooth/artik_bluetooth_test_spp.c)
This program starts a SPP server, pending on SPP clients to connect. It also displays any data received from the clients.

### [artik_bluetooth_test_spp_client.c](/bluetooth/artik_bluetooth_test_spp_client.c)
This program starts a SPP client and tries to connect to a remote SPP server. It then sends any data input on the keyboard over the SPP
channel.

## Cloud
## [artik_cloud_test.c](cloud/artik_cloud_test.c)
This program uses configured ARTIK Cloud credentials to issue requests to the cloud over commonly used REST APIs.

## GPIO
## [artik_gpio_test.c](gpio/artik_gpio_test.c)
This program changes the color of a RGB LED connected to the ARTIK board sequentially. It also prints out events received by pressing
a button connected to the ARTIK board using a GPIO.

## HTTP
## [artik_http_test.c](http/artik_http_test.c)
This program issues some POST and GET requests to the HTTP test service http://httpbin.org

## [artik_http_openssl_test.c](http/artik_http_openssl_test.c)
This program allows testing various TLS parameters against a HTTPS server (e.g. "openssl s_server")

## I2C
## [artik_i2c_test.c](i2c/artik_i2c_test.c)
This programs tries to read and write registers on the CW2015 battery charger chip over I2C.

## LWM2M
## [artik_lwm2m_test_client.c](lwm2m/artik_lwm2m_test_client.c)
This programs connects to ARTIK Cloud's LWM2M server and exposes some objects. It then shows a console allowing the user
to read or write some the LWM2M's objects properties.

## Media
## [artik_media_test.c](media/artik_media_test.c)
This program plays a sound file specified by the user as a parameter.

## MQTT
## [artik_mqtt_cloud_test.c](mqtt/artik_mqtt_cloud_test.c)
This programs connects to ARTIK Cloud through MQTT and sends a test message to the cloud. Upon receiving an action from the cloud,
it will display the action's data then disconnect from the server.

## Network
## [artik_network_test.c](network/artik_network_test.c)
This program displays the external public IP of the device, checks the online status of the board, then executes a manual
disconnection/reconnection of the network interface to observe proper notification of the online status change.

## [artik_dhcp_client_test.c](network/artik_dhcp_client_test.c)
This programs starts a DHCP client to retrieve an IP address on the network.

## PWM
## [artik_pwm_test.c](pwm/artik_pwm_test.c)
This program will output a PWM signal on one of the ARTIK board's PWM output for a duration of 3 seconds. If a buzzer is connected
to this output, a sound should be heard.

## Security
## [artik_security_test.c](security/artik_security_test.c)
This programs calls the Secure Element related APIs to display the builtin certificate, its associated serial number, and generate
true random bytes.

## Sensor
## [artik_sensor_test.c](sensor/artik_sensor_test.c)
This program lists all the sensors available on the board, then display their measured data every 2 seconds until the user hits
Ctrl+C.

## Serial
## [artik_serial_test.c](serial/artik_serial_test.c)
This loopback test opens a serial port on the board, sends some data over it then compare with the received bytes. This test requires
the TX and RX pins of the serial port to be physically looped on the board using a wire.

## SPI
## [artik_spi_test.c](spi/artik_spi_test.c)
This loopback test sends some data over the SPI bus then compare with the received bytes. This test requires
the MOSI and MISO pins of the SPI bus to be physically looped on the board using a wire.

## Time
## [artik_time_test.c](time/artik_time_test.c)
This program displays the current time, sets two alarms to be triggered in the next seconds, then check that the alarms are properly
fired. It also tests time synchronization with a NTP server.

## Websocket
## [artik_websocket_test.c](websocket/artik_websocket_test.c)
This program opens a websocket to echo.websocket.org, sends a test message and display received frames.

## [artik_websocket_client_test.c](websocket/artik_websocket_client_test.c)
This program opens a websocket to the specified server with specified TLS configuration parameters. It then sends a test message
and displays incoming frames.

## [artik_websocket_cloud_test.c](websocket/artik_websocket_cloud_test.c)
This program opens a websocket to ARTIK Cloud, sends a test message to the cloud every second and display any actions received from the
cloud. The program exits automatically after 10 seconds.

## Wi-Fi
## [artik_wifi_test.c](wifi/artik_wifi_test.c)
This program scans the surrounding Wi-Fi access points and tries to connect to a preconfigured hotspot.

## ZigBee
## [artik_zigbee_test.c](zigbee/artik_zigbee_test.c)
This programs exposes a command-line from where it is possible to expose ZigBee services, join or form a network, detect remote
devices, then trigger actions on remote ZigBee devices.

