#pragma once

#include <string>
#include <iostream>
#include <utility>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <termios.h>

class SerialLinkReader {
public:
    SerialLinkReader(std::string deviceName, const uint32_t baudRate) : deviceName(std::move(deviceName)),
                                                                        baudRate(baudRate) {}

    uint32_t open() {
        deviceDescriptor = serialOpen(deviceName.c_str(), baudRate);
        if (deviceDescriptor < 0) {
            std::cerr << "ERROR: Failed to open device name " << deviceName << std::endl;
        }
        wiringPiSetup();
        configure();
        std::cout << "Opened serial link device " << deviceName << ": descriptor=" << deviceDescriptor << std::endl;

        return deviceDescriptor;
    }

    void configure() const {
        struct termios options{};
        tcgetattr(deviceDescriptor, &options);   // Read current options
        options.c_cflag &= ~CSIZE;  // Mask out size
        options.c_cflag |= CS7;     // Or in 7-bits
        options.c_cflag |= PARENB;  // Enable Parity - even by default
        tcsetattr(deviceDescriptor, 0, &options);   // Set new options
    }

    void readBuffer(char* buffer) const {
        if (deviceDescriptor <= 0) {
            std::cerr << "ERROR: device is not open: " << deviceName << std::endl;
            return;
        }

        ssize_t nbBytesToRead = sizeof(buffer);
        ssize_t nbBytesReceived = read(deviceDescriptor, buffer, nbBytesToRead);
        if (nbBytesReceived == -1) {
            std::cerr << "Error reading from serial port" << std::endl;
            return;
        } else if (nbBytesReceived == 0) {
            std::cerr << "No more data" << std::endl;
            return;
        } else {
            buffer[nbBytesReceived] = '\0';
            std::cout << buffer; //Read serial data
        }
    }

    void close() const {
        serialClose(deviceDescriptor);
    }

    void flush() const {
        serialFlush(deviceDescriptor);
    }

    const std::string& getDeviceName() const {
        return deviceName;
    }

private:
    const std::string deviceName;
    const uint32_t baudRate;
    uint32_t deviceDescriptor = -1;
};
