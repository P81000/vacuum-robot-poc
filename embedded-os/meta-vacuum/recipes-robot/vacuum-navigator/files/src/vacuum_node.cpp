#include <rclcpp/rclcpp.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cmath>
#include <cstring>

#define QEMU_IP_BRIDGE  "10.0.2.2"
#define PORT            8080

#pragma pack(push, 1)
struct TelemetryPacket {
    float pos_x;
    float pos_y;
    float heading_rad;
    uint8_t state_code;
};
#pragma pack(pop)

class VacuumNode : public rclcpp::Node {
public:
    VacuumNode() : Node("vacuum_navigator"), angle_(0.0) {
        sock_ = socket(AF_INET, SOCK_DGRAM, 0);
        server_addr_.sin_family = AF_INET;
        server_addr_.sin_port = htons(PORT);
        inet_pton(AF_INET, QEMU_IP_BRIDGE, &server_addr_.sin_addr);

        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(100),
            std::bind(&VacuumNode::timer_callback, this)
        );
        
        RCLCPP_INFO(this->get_logger(), "Vacuum Node Started! Sending Binary UDP to 10.0.2.2:8080");
    }

    ~VacuumNode() {
        close(sock_);
    }

private:
    void timer_callback() {
        float radius    = 100.0;
        float center_x  = 400.0;
        float center_y  = 400.0;
        
        TelemetryPacket packet;
        packet.pos_x = center_x + radius * cos(angle_);
        packet.pos_y = center_y + radius * sin(angle_);
        packet.heading_rad = angle_ + (M_PI / 2.0);
        packet.state_code = 1;

        angle_ += 0.05;

        sendto(sock_, &packet, sizeof(packet), 0,
               (struct sockaddr*)&server_addr_, sizeof(server_addr_));
    }

    int sock_;
    struct sockaddr_in server_addr_;
    float angle_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<VacuumNode>());
    rclcpp::shutdown();
    return 0;
}
