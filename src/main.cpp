#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "raylib.h"

struct RobotState {
    float pos_x;
    float pos_y;
    float heading_rad;
    uint8_t state_code;
};

#pragma pack(push, 1)
struct TelemetryPacket {
    float pos_x;
    float pos_y;
    float heading_rad;
    uint8_t state_code;
};
#pragma pack(pop)

RobotState g_robot_state = {400, 400, 0, 0};
std::mutex g_state_mutex;
std::atomic<bool> g_server_running{true};

void udp_server_thread() {
    int sock_fd;
    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "[ ERROR ] socket creation " << __func__ << std::endl;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(sock_fd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "[ ERROR ] socket binding " << __func__ << std::endl;
        close(sock_fd);
        return;
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    TelemetryPacket packet;
    socklen_t len = sizeof(client_addr);

    std::cout << "[ INFO ] Udp server listeing and running..." << std::endl;

    while (g_server_running) {
        int nbytes = recvfrom(sock_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&client_addr, &len);
        if (nbytes == sizeof(TelemetryPacket)) {
            std::lock_guard<std::mutex> lock(g_state_mutex);
            g_robot_state.pos_x = packet.pos_x;
            g_robot_state.pos_y = packet.pos_y;
            g_robot_state.heading_rad = packet.heading_rad;
            g_robot_state.state_code = packet.state_code;
        }
    }

    close(sock_fd);
    std::cout << "Server stopped\n";
}

void DrawRoomGrid(uint16_t screen_width, uint16_t screen_height, uint8_t cell_size) {
    for (auto i{0}; i < screen_width; i += cell_size) {
        DrawLine(i, 0, i, screen_height, LIGHTGRAY);
    }

    for (auto i{0}; i < screen_height; i += cell_size) {
        DrawLine(0, i, screen_width, i, LIGHTGRAY);
    }
}

int main(void) {
    const uint16_t screen_width = 800;
    const uint16_t screen_height = 800;

    std::thread server_thread(udp_server_thread);

    InitWindow(screen_width, screen_height, "Vacuum PoC");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        RobotState curr_state;
        {
            std::lock_guard<std::mutex> lock(g_state_mutex);
            curr_state = g_robot_state;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRoomGrid(screen_width, screen_height, 10);
        DrawCircle(static_cast<int>(curr_state.pos_x), static_cast<int>(curr_state.pos_y), 25, BLUE);

        int lineEndX = curr_state.pos_x + 25 * cos(curr_state.heading_rad);
        int lineEndY = curr_state.pos_y + 25 * sin(curr_state.heading_rad);
        DrawLineEx({curr_state.pos_x, curr_state.pos_y}, {static_cast<float>(lineEndX), static_cast<float>(lineEndY)}, 3.0f, RED);

        DrawText("Visualizer Env Isolated & Running", 10, 10, 20, DARKBROWN);
        DrawText(TextFormat("Robot POS: [%.1f, %.1f]", curr_state.pos_x, curr_state.pos_y), 10, 40, 20, DARKGRAY);

        EndDrawing();
    }

    g_server_running = false;
    CloseWindow();

    server_thread.join();
    return 0;
}
