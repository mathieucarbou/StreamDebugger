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
