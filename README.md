# ESP8266 P1 to WiFi

Simple Arduino Sketch that can be used to stream P1 data (Dutch Smart Meter Requirements/DSMR) over WiFi using an ESP8266.
Can be used to integrate a smart meter into Home Assistant without having to run a cable from your server to your smart meter.

:warning: Works only with DSMR 5.0 smart meters, which supply 5V at 250 mA!
The DSMR 4.0 specification requires the P1 port to supply 5V at 100 mA, which is insufficient.
Your smart meter's DSMR version should be printed on the smart meter itself.

# Requirements

Everything I've used is listed below, including a link to the product page at the store I bought them at.

* [ESP8266 ESP01-S](https://www.tinytronics.nl/shop/nl/communicatie/wi-fi/esp8266-wifi-module-esp-01s-1mb)
* [AMS1117-3.3](https://www.tinytronics.nl/shop/nl/spanning-converters/step-down/ams1117-3.3v-spanningsregelaar-module-compact)
* [RJ12 6P6C to Jump Wires Adapter](https://www.tinytronics.nl/shop/nl/kabels/adapters/rj12-naar-6-pins-dupont-jumper-adapter)
* [Breadboard](https://www.tinytronics.nl/shop/nl/prototyping/breadboards/breadboard-170-points-zwart)
* [Jump Wires (Male to Male)](https://www.tinytronics.nl/shop/nl/kabels/prototype-draden/dupont-jumper-draad-male-male-10cm-10-draden)
* [Jump Wires (Male to Female)](https://www.tinytronics.nl/shop/nl/kabels/prototype-draden/dupont-jumper-draad-male-female-10cm-10-draden)
* [FT232RL 3.3v-5v TTL USB Serial Port Adapter](https://www.tinytronics.nl/shop/nl/communicatie/serieel/ft232rl-3.3v-5v-ttl-usb-serial-port-adapter)

# Flashing the Firmware

1. Install the Arduino IDE with the ESP8266 Arduino Core (see [documentation](https://arduino-esp8266.readthedocs.io/en/latest/installing.html#boards-manager))
1. Clone this repository and open it in the Arduino IDE
1. Adjust the settings in the Sketch to your needs  
    You should at least adjust the `SSID` and `PASSWORD` settings so the ESP can connect to your network  
    :warning: The ESP8266 can only connect to 2.4GHz networks
1. Use the USB Serial Port Adapter to flash the firmware (see [Flashing Schema](#flashing-schema))  
    :warning: Make sure you only supply 3.3V to your ESP8266

1. Connect all components (see [Runtime Schema](#runtime-schema))
1. Plug it into your smart meter
1. Enable the DSMR integration in Home Assistant (see [Home Assistant Configuration](#home-assistant-configuration))

## Flashing Schema

| From                        | To                       |
| --------------------------- | ------------------------ |
| USB Serial Port Adapter VCC | ESP8266 3V3, ESP8266 EN  |
| USB Serial Port Adapter GND | ESP8266 GND, ESP8266 IO0 |
| USB Serial Port Adapter TX  | ESP8266 RXD              |
| USB Serial Port Adapter RX  | ESP8266 TXD              |

## Runtime Schema

| From | To |
| ---------------------- | ------------------------------------- |
| P1 pin 1 (5V)          | Voltage Regulator VIn, P1 pin 2 (RTS) |
| P1 pin 6 (GND)         | Voltage Regulator GND, ESP8266 GND    |
| P1 pin 5 (TX)          | ESP8266 RX                            |
| Voltage Regulator VOut | ESP8266 3V3, ESP8266 EN               |

## Home Assistant Configuration

Example configuration for the [DSMR Slimme Meter integration](https://www.home-assistant.io/integrations/dsmr/) below:

```yaml
sensor:
  - platform: dsmr
    port: 1337
    host: 192.168.1.130
    dsmr_version: 5
```

# Troubleshooting

The ESP8266's IO2 pin can be connected to your USB Serial Port Adapter's RX pin for debugging purposes.

## Example Debug Output

```
* VERSION         2021.01.23+1
Network Settings:
* HOSTNAME        p1
* SSID            SomeSSID
* PASSWORD        REDACTED
* MAX_CLIENTS     4
* PORT            1337
Serial Settings:
* DEBUG_BAUD_RATE 9600
* P1_BAUD_RATE    115200
* P1_MODE         28
* P1_INVERT       1
General Settings:
BUFFER_SIZE       128
Starting WiFi connection
Waiting for WiFi connection...
Waiting for WiFi connection...
Waiting for WiFi connection...
Waiting for WiFi connection...
WiFi connected at 192.168.1.130
Starting server
Starting OTA server
Client 192.168.1.163 accepted
```
