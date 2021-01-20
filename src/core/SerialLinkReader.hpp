#pragma once

#include <thread>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <termios.h>
#include <unistd.h>
#include <sstream>
#include <algorithm>

class SerialLinkReader {
public:
    explicit SerialLinkReader(std::string serialDeviceName) : deviceName(std::move(serialDeviceName)),
                                                              threadName("SerialLinkReceiverThread"),
                                                              deviceDescriptor(-1),
                                                              thread(nullptr),
                                                              isStopRequested(false) {}

    ~SerialLinkReader() {
        stopReceiving();
        close();
    }

    SerialLinkReader() = delete;
    SerialLinkReader(const SerialLinkReader&) = delete;
    SerialLinkReader& operator=(const SerialLinkReader&) = delete;

    uint32_t open() {
        deviceDescriptor = serialOpen(deviceName.c_str(), RPI_SERIAL_LINK_BAUD_RATE);
        if (deviceDescriptor < 0) {
            std::cerr << "ERROR: Failed to open device name " << deviceName << std::endl;
        }
        wiringPiSetup();
        configure();

        std::cout << "Opened serial link device " << deviceName << ": descriptor=" << deviceDescriptor << std::endl;
        return deviceDescriptor;
    }

    void startReceiving() {
        if (deviceDescriptor > 0) {
            createThread();
        } else {
            std::cerr << "ERROR: Serial link device not open" << std::endl;
        }
    }

    void stopReceiving() {
        if (!thread) {
            return;
        }
        std::cout << "Stop thread " << threadName << std::endl;
        isStopRequested = true;

        thread->join();
        thread = nullptr;
        isStopRequested = false;
        std::cout << "Thread " << threadName << " stopped successfully" << std::endl;
    }

    void waitUntilWeStopReceiving() {
        thread->join();
    }

    void close() {
        if (deviceDescriptor <= 0) {
            return;
        }
        if (thread) {
            std::cerr << "ERROR: Thread is still running" << std::endl;
            return;
        }
        std::cout << "Close device " << deviceName << std::endl;
        serialClose(deviceDescriptor);
    }

private:
    void configure() const {
        struct termios options{};
        tcgetattr(deviceDescriptor, &options);
        options.c_cflag &= ~CSIZE;  // Mask out size
        options.c_cflag |= CS7;     // Or in 7-bits
        options.c_cflag |= PARENB;  // Enable Parity - Even by default
        tcsetattr(deviceDescriptor, 0, &options);   // Set new options
    }

    int32_t readBuffer(char* buffer) const {
        if (deviceDescriptor <= 0) {
            std::cerr << "ERROR: device is not open: " << deviceName << std::endl;
            return -1;
        }

        ssize_t nbBytesToRead = sizeof(buffer);
        ssize_t nbBytesReceived = read(deviceDescriptor, buffer, nbBytesToRead);
        if (nbBytesReceived == -1) {
            std::cerr << "Error reading from serial port" << std::endl;
            return -1;
        } else if (nbBytesReceived == 0) {
            std::cerr << "ERROR: No more data" << std::endl;
            return 0;
        } else {
            buffer[nbBytesReceived] = '\0';
            return nbBytesReceived;
        }
    }

    void createThread() {
        if (!thread) {
            thread = std::make_unique<std::thread>(&SerialLinkReader::process, this);
            if (thread) {
                std::cout << "Created thread " << threadName << " successfully" << std::endl;
            } else {
                std::cerr << "ERROR: Failed to create thread " << threadName << std::endl;
            }
        } else {
            std::cerr << "ERROR: Thread " << threadName << " already exist" << std::endl;
        }
    }

    void process() {
        std::string concatenator;
        char buffer[100];

        while (!isStopRequested) {
            int32_t nbBytesReceived = readBuffer(buffer);
            if (nbBytesReceived) {
                std::string receivedString(buffer);

                // Find "StartOfText" character used as message separator
                if (uint32_t startOfText = receivedString.find('\x02') != std::string::npos) {
                    processMessage(concatenator);
                    concatenator.clear();
                    concatenator += receivedString.substr(startOfText + 1, receivedString.size());
                } else {
                    concatenator += receivedString;
                }
            }
        }
    }

    static void processMessage(std::string msg) {
        removeNonPrintableCharacters(msg);
        removeSpecialCharacters(msg);
        std::cout << msg << std::endl;
    }

    static void removeNonPrintableCharacters(std::string& str) {
        // get the ctype facet for wchar_t (Unicode code points in practice)
        typedef std::ctype<wchar_t> ctype;
        const auto& ct = std::use_facet<ctype>(std::locale());
        // remove non printable Unicode characters
        str.erase(std::remove_if(str.begin(),
                                 str.end(),
                                 [&ct](wchar_t ch) {
                                     return !ct.is(ctype::print, ch);
                                 }), str.end());
    }

    static void removeSpecialCharacters(std::string& str) {
        str.erase(std::remove_if(str.begin(),
                                 str.end(),
                                 [](char c) {
                                     return !(std::isalnum(c) || std::isspace(c));
                                 }), str.end());
    }

    static const uint32_t RPI_SERIAL_LINK_BAUD_RATE = 1200;
    const std::string deviceName;
    const std::string threadName;
    uint32_t deviceDescriptor;
    std::unique_ptr<std::thread> thread;
    bool isStopRequested;
};
