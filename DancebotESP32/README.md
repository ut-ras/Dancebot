# README.md

## Launching PlatformIO project

## Building and Flashing Code

### Build Issues

- Linker may fail the first and second time and so up to three build calls are required.
- On the second build call, ESP Async Web Server might throw an ambiguous
  overload error, see [this issue](https://github.com/me-no-dev/ESPAsyncWebServer/pull/970#issuecomment-977031547) and modify .pio/libdeps accordingly.

## Loading Data Files to SPIFFS

https://randomnerdtutorials.com/esp32-vs-code-platformio-spiffs/
