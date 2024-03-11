# StreamDebugger

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Continuous Integration](https://github.com/mathieucarbou/StreamDebugger/actions/workflows/ci.yml/badge.svg)](https://github.com/mathieucarbou/StreamDebugger/actions/workflows/ci.yml)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/mathieucarbou/library/StreamDebugger.svg)](https://registry.platformio.org/libraries/mathieucarbou/StreamDebugger)

Just allows easier debugging of Serial-based communication on Arduino.

StreamDebugger class is an Arduino Stream, that dumps all data to another Stream for debug purposes.

## Changes in this fork

This fork is based on [https://github.com/vshymanskyy/StreamDebugger](https://github.com/vshymanskyy/StreamDebugger)

**Changes:**

- Added `directAccessNonBlocking` (no while loop)
- Added `setDataStream()` and `setDumpStream()`
- Added CI
- Spy on `write()`, `availableForWrite()`, `readBytes()`, `readString()`
- Added callbacks `onRead()` and `onWrite()`

## Usage:

```cpp
#include <StreamDebugger.h>

StreamDebugger Serial1Dbg1(Serial1, Serial);

// or, if you do not have access to the streams yet...
StreamDebugger Serial1Dbg2;

// for callback example
StreamDebugger Serial1Dbg3(Serial1);

void setup() {
  // Setup debug stream
  Serial.begin(115200);
  while (!Serial);

  // Setup data stream
  Serial1.begin(9600);
  while (!Serial1);

  // Now use StreamDbg instead of Serial1
  Serial1Dbg1.println("Hello world!");
  // The message is sent to Serial1,
  // and automatically duplicated to Serial

  Serial1Dbg2.setDataStream(&Serial1);
  Serial1Dbg2.setDumpStream(&Serial);

  Serial1Dbg3.onRead([](const uint8_t* buffer, size_t size) {
    Serial.print("Read: ");
    Serial.write(buffer, size);
  });

  Serial1Dbg3.onWrite([](const uint8_t* buffer, size_t size) {
    Serial.print("Write: ");
    Serial.write(buffer, size);
  });
}

void loop() {
  Serial1Dbg1.directAccessNonBlocking();
  Serial1Dbg2.directAccessNonBlocking();
  delay(0);
}
```
