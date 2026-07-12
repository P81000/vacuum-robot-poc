#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

static_assert(true);
#pragma pack(push, 1)
struct TelemetryPacket {
    float pos_x;
    float pos_y;
    float heading_rad;
    uint8_t state_code;
};
#pragma pack(pop)

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "[ ERROR ] socket creation failed" << std::endl;
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    std::cout << "[ INFO ] Mock Brain started. Sending data..." << std::endl;

    TelemetryPacket packet = {400, 400, 0.0f, 1};

    int side = 0;
    while (true) {
        packet.pos_x += std::cos(packet.heading_rad) * 2;
        packet.pos_y += std::sin(packet.heading_rad) * 2;

        side += 2;

        if (side >= 100)
        {
            side = 0;
            packet.heading_rad += M_PI / 6.0f;
        }

        sendto(sock_fd, &packet, sizeof(packet), 0, 
              (const struct sockaddr*)&server_addr, sizeof(server_addr));

        std::cout << "Sent POS: [" << packet.pos_x << ", " << packet.pos_y << "]" << std::endl;

        usleep(50000); 
    }

    close(sock_fd);
    return 0;
}
