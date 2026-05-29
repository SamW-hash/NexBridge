#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <sstream>
#include <cstdint>

int main() {
    const char* portName = "/dev/ttyUSB0";

    int serialPort = open(portName, O_RDWR | O_NOCTTY);
    if (serialPort < 0) {
        std::cerr << "Failed to open " << portName << ": "
                  << std::strerror(errno) << '\n';
        return 1;
    }

    termios tty{};
    if (tcgetattr(serialPort, &tty) != 0) {
        std::cerr << "Failed to get serial settings: "
                  << std::strerror(errno) << '\n';
        close(serialPort);
        return 1;
    }

    cfmakeraw(&tty);
    // NexStar serial is 8N1 and communicates at a 9600 baudrate
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    tty.c_cflag |= CLOCAL | CREAD;
    tty.c_cflag &= ~CRTSCTS; // disable hardware flow control


    tty.c_cc[VMIN] = 0; // Minimum number of characters to read
    tty.c_cc[VTIME] = 50; // Maximum wait time

    if (tcsetattr(serialPort, TCSANOW, &tty) !=0) {
        std::cerr << "Failed to set serial settings: "
                  << std::strerror(errno) << '\n';
        close(serialPort);
        return 1;
    }

    std::string command = "z"; /*
                                  Z = AZM-ALT
                                  z = precise AZM-ALT
                                  E = RA/DEC
                                  e = precise RA/DEC
                                                      */
    int bytesWritten = write(serialPort, command.c_str(), command.size());
    tcdrain(serialPort);
    if (bytesWritten < 0) {
        std::cerr << "Failed to write command: "
                  << std::strerror(errno) << '\n';
        close(serialPort);
        return 1;
    }

    std::string response;
    char ch;

    while (true) {
        int bytesRead = read(serialPort, &ch, 1);
        if (bytesRead < 0) {
            std::cerr << "Failed to read: "
                      << std::strerror(errno) << '\n';
            close(serialPort);
            return 1;
        }
        if (bytesRead == 0) {
            std::cerr << "Timed out waiting for response.\n";
            close(serialPort);
            return 1;
        }
        response += ch;
        if (ch == '#') {
            break;
        }
    }

    // Convert response to degrees
    response.pop_back();
    size_t commaPos = response.find(',');
    std::string azHex = response.substr(0, commaPos);
    std::string altHex = response.substr(commaPos + 1);
    uint32_t azRaw = std::stoul(azHex, nullptr, 16);
    uint32_t altRaw = std::stoul(altHex, nullptr, 16);
    double azDeg = static_cast<double>(azRaw) / 4294967296.0 * 360;
    double altDeg = static_cast<double>(altRaw) / 4294967296.0 * 360;

    std::cout << "Azimuth: " << azDeg << " degrees\n";
    std::cout << "Altitude: " << altDeg << " degrees\n";

    return 0;
}