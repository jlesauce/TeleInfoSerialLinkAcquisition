#include <iostream>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>

using namespace std;

int main(int argc, char **argv) {
    string serialDeviceAddress = "/dev/ttyAMA0";

    int levelSensor = serialOpen(serialDeviceAddress.c_str(), 19200);
    wiringPiSetup();
    serialPuts(levelSensor, "DP"); //Send command to the serial device

    while (true) {
        char buffer[100];
        ssize_t length = read(levelSensor, &buffer, sizeof(buffer));
        if (length == -1) {
            cerr << "Error reading from serial port" << endl;
            break;
        } else if (length == 0) {
            cerr << "No more data" << endl;
            break;
        } else {
            buffer[length] = '\0';
            cout << buffer; //Read serial data
        }
    }

    return 0;
}