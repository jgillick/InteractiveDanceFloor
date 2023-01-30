# Dance Floor Tile Firmware

This firmware runs inside the ESP32 C3 chip in each dance floor tile and controls the LEDs and touch sensors.

## Getting Started

### Prerequisite

Setup the [Host software](../host/) and have it running on a domain that is accessible on the wifi network that the ESP32 will be connecting to. For example, do not have it running on `localhost`.

Setup SSL for the host software and save the root cert key to `firmware/server_certs/host_cert.pem`.

**Tip:** For development use (localtunnel)[https://www.npmjs.com/package/localtunnel] (ngrok does not work with ESP32, for some reason) to give your host a publicly accessible URL. For your convenience, the root key for localtunnel is already in server_certs. Just rename it to `host_cert.pem` and you should be good to go.

### Build & flash firmware

- Install [ESP32 IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#installation)
- Configure wifi and the host software URL domain with: `idf.py menuconfig`
- Build: `idf.py menuconfig`
- Flash and monitor the output: `idf.py flash monitor`
