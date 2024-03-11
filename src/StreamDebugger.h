/**
 * @file       StreamDebugger.h
 * @author     Volodymyr Shymanskyy
 * @copyright  Copyright (c) 2016 - 2024 Volodymyr Shymanskyy, Mathieu Carbou
 * @date       Nov 2016
 * @license    This project is released under the MIT License (MIT)
 */

#pragma once

#include <Arduino.h>

#define STREAM_DEBUGGER_VERSION "2.0.1"
#define STREAM_DEBUGGER_VERSION_MAJOR 2
#define STREAM_DEBUGGER_VERSION_MINOR 0
#define STREAM_DEBUGGER_VERSION_REVISION 1
#define STREAM_DEBUGGER_FORK_mathieucarbou

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

    virtual size_t write(uint8_t ch) {
      if (_dump)
        _dump->write(ch);
      return _data ? _data->write(ch) : 0;
    }
    virtual int read() {
      if (!_data)
        return -1;
      int ch = _data->read();
      if (ch != -1 && _dump) {
        _dump->write(ch);
      }
      return ch;
    }
    virtual int available() { return _data ? _data->available() : 0; }
    virtual int peek() { return _data ? _data->peek() : -1; }
    virtual void flush() {
      if (_data)
        _data->flush();
    }

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

    void setDataStream(Stream* data) {
      _data = data;
    }

    void setDumpStream(Stream* dump) {
      _dump = dump;
    }

  private:
    Stream* _data = nullptr;
    Stream* _dump = nullptr;
};
