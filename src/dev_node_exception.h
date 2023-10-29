/*
 * H9 project
 *
 * Created by crowx on 2023-10-14.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include <exception>
#include <string>

#include "h9frame.h"

class DevNodeException: public std::exception {
  protected:
    std::string msg;

    DevNodeException() = default;

  public:
    explicit DevNodeException(std::string msg):
        msg(std::move(msg)) {}

    virtual const char* what() const noexcept {
        return msg.c_str();
    }
};

class NodeException: public DevNodeException {
  public:
    explicit NodeException(int code) {
        msg = "Node exception: " + std::to_string(code) + " - " + H9frame::error_to_string(H9frame::from_underlying<H9frame::Error>(code)) + ".";
    }
};

class DeviceException: public DevNodeException {
  protected:
    DeviceException() = default;
};

class TimeoutException: public DeviceException {
  public:
    TimeoutException() {
        msg = "Timeout exception";
    }
};

class MalformedFrameException: public DeviceException {
  public:
    MalformedFrameException() {
        msg = "Malformed frame exception";
    }
};

class SizeMismatchException: public DeviceException {
  public:
    SizeMismatchException() {
        msg = "Size mismatch exception";
    }
};

class DeviceNotExistException: public DeviceException {
  public:
    DeviceNotExistException() {
        msg = "Node not exist exception";
    }
};

class InvalidRegisterException: public DeviceException {
  protected:
    const std::uint8_t reg;
    explicit InvalidRegisterException(std::uint8_t reg): reg(reg) {
        msg = "Register " + std::to_string(reg);
    }
};

class RegisterNotExistException: public InvalidRegisterException {
  public:
    explicit RegisterNotExistException(std::uint8_t reg): InvalidRegisterException(reg) {
        msg += " does not exist.";
    }
};

class RegisterNotWritableException: public InvalidRegisterException {
  public:
    explicit RegisterNotWritableException(std::uint8_t reg): InvalidRegisterException(reg) {
        msg += " is not writable.";
    }
};

class RegisterNotReadableException: public InvalidRegisterException {
  public:
    explicit RegisterNotReadableException(std::uint8_t reg): InvalidRegisterException(reg) {
        msg += " is not readable.";
    }
};

class UnsupportedRegisterDataConversionException: public InvalidRegisterException {
  public:
    explicit UnsupportedRegisterDataConversionException(std::uint8_t reg): InvalidRegisterException(reg) {
        msg += " unsupported data conversion.";
    }
};