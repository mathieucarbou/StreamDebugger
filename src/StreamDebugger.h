/**
 * @file       StreamDebugger.h
 * @author     Volodymyr Shymanskyy
 * @copyright  Copyright (c) 2016 - 2024 Volodymyr Shymanskyy, Mathieu Carbou
 * @date       Nov 2016
 * @license    This project is released under the MIT License (MIT)
 */

#pragma once

#include <Stream.h>
#include <functional>

#define STREAM_DEBUGGER_VERSION "2.1.0"
#define STREAM_DEBUGGER_VERSION_MAJOR 2
#define STREAM_DEBUGGER_VERSION_MINOR 1
#define STREAM_DEBUGGER_VERSION_REVISION 0
#define STREAM_DEBUGGER_FORK_mathieucarbou

typedef std::function<void(const uint8_t* buffer, size_t size)> StreamDebuggerCallback;

class StreamDebugger : public Stream {
  public:
    StreamDebugger() {}

    explicit StreamDebugger(Stream& data) {
      setDataStream(&data);
    }

    StreamDebugger(Stream& data, Stream& dump) {
      setDataStream(&data);
      setDumpStream(&dump);
    }

    virtual ~StreamDebugger() {}

    // Print

    virtual size_t write(uint8_t ch) {
      if (!_data)
        return 0;
      size_t c = _data->write(ch);
      if (c) {
        if (_dump)
          _dump->write(ch);
        if (_onWrite)
          _onWrite(&ch, 1);
      }
      return c;
    }

    virtual size_t write(const uint8_t* buffer, size_t size) {
      if (!_data)
        return 0;
      size_t c = _data->write(buffer, size);
      if (c) {
        if (_dump)
          _dump->write(buffer, c);
        if (_onWrite)
          _onWrite(buffer, c);
      }
      return c;
    }

    virtual int availableForWrite() {
      return _data ? _data->availableForWrite() : 0;
    }

    virtual void flush() {
      if (_data)
        _data->flush();
      if (_dump)
        _dump->flush();
    }

    // Stream

    virtual int available() { return _data ? _data->available() : 0; }

    virtual int read() {
      if (!_data)
        return -1;
      int ch = _data->read();
      if (ch != -1) {
        if (_dump)
          _dump->write(ch);
        if (_onRead)
          _onRead((const uint8_t*)&ch, 1);
      }
      return ch;
    }

    virtual int peek() { return _data ? _data->peek() : -1; }

    virtual size_t readBytes(char* buffer, size_t length) {
      if (!_data)
        return 0;
      size_t count = _data->readBytes(buffer, length);
      if (count) {
        if (_dump)
          _dump->write(buffer, count);
        if (_onRead)
          _onRead((const uint8_t*)buffer, count);
      }
      return count;
    }

    virtual String readString() {
      if (!_data)
        return emptyString;
      String str = _data->readString();
      if (str.length()) {
        if (_dump)
          _dump->print(str);
        if (_onRead)
          _onRead((const uint8_t*)str.c_str(), str.length());
      }
      return str;
    }

    // StreamDebugger

    void directAccess() {
      int r;
      while (true) {
        if (_data && _data->available() && (r = _data->read()) != -1 && _dump)
          _dump->write(r);
        if (_dump && _dump->available() && (r = _dump->read()) != -1 && _data)
          _data->write(r);
        delay(0);
      }
    }

    void directAccessNonBlocking() {
      int r;
      while (_data && _data->available() && (r = _data->read()) != -1)
        if (_dump)
          _dump->write(r);
      while (_dump && _dump->available() && (r = _dump->read()) != -1)
        if (_data)
          _data->write(r);
    }

    void setDataStream(Stream* data) { _data = data; }
    void setDumpStream(Stream* dump) { _dump = dump; }
    void onRead(StreamDebuggerCallback onRead) { _onRead = onRead; }
    void onWrite(StreamDebuggerCallback onWrite) { _onWrite = onWrite; }

  private:
    Stream* _data = nullptr;
    Stream* _dump = nullptr;
    StreamDebuggerCallback _onRead = nullptr;
    StreamDebuggerCallback _onWrite = nullptr;
};
