#include <iostream>
#include <csignal>

#include <core/SerialLinkReader.hpp>

#define RPI_SERIAL_LINK_DEVICE_NAME "/dev/ttyAMA0"

void signalHandler(int signum);

SerialLinkReader serialLinkReader(RPI_SERIAL_LINK_DEVICE_NAME);

int main(int argc, char** argv) {
    uint32_t deviceDescriptor = serialLinkReader.open();
    if (deviceDescriptor > 0) {
        signal(SIGINT, signalHandler);

        serialLinkReader.startReceiving();
        serialLinkReader.waitUntilWeStopReceiving();

        std::cout << "End of reception loop" << std::endl;
        return EXIT_SUCCESS;
    } else {
        std::cerr << "ERROR: Failed to open serial link" << std::endl;
        return EXIT_FAILURE;
    }
}

void signalHandler(int signum) {
    if (signum == SIGINT) {
        std::cout << std::endl;
        serialLinkReader.stopReceiving();
        serialLinkReader.close();
        exit(-3);  // Echoes -3  for ^C (ASCII 3)
    }
}