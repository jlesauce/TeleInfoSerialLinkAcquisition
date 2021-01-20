#include <iostream>
#include <csignal>

#include <ApplicationModel.hpp>
#include <core/SerialLinkReader.hpp>

void signalHandler(int signum);

SerialLinkReader serialLinkReader("/dev/ttyAMA0", ApplicationModel::RPI_SERIAL_LINK_BAUD_RATE);

int main(int argc, char** argv) {
    uint32_t deviceDescriptor = serialLinkReader.open();
    if (deviceDescriptor > 0) {
        signal(SIGINT, signalHandler);

        char buffer[100];
        while (true) {
            serialLinkReader.readBuffer(buffer);
        }

        serialLinkReader.close();
        return EXIT_SUCCESS;
    } else {
        std::cerr << "ERROR: Failed to open serial link" << std::endl;
        return EXIT_FAILURE;
    }
}

void signalHandler(int signum) {
    if (signum == SIGINT) { // ctrl+C pressed
        serialLinkReader.close();
        std::cout << "INFO: Closed device: " << serialLinkReader.getDeviceName() << std::endl;
        exit(-3);  // Echoes -3  for ^C (ASCII 3)
    }
}