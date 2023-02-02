# Host Software

This is the software that connects to all the floor tiles and controls what they do.

## Generate SSL Certs

You need to have SSL setup for connections to the ESP32 nodes ([instructions](https://adamtheautomator.com/https-nodejs/)).

```bash
mkdir cert
cd cert
openssl genrsa -out key.pem
openssl req -new -key key.pem -out csr.pem
openssl x509 -req -days 9999 -in csr.pem -signkey key.pem -out cert.pem
```
