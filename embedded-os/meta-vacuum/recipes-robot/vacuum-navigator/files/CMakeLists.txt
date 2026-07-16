cmake_minimum_required(VERSION 3.8)
project(vacuum_navigator)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

add_executable(vacuum_node
	src/vacuum_node.cpp
)

ament_target_dependencies(vacuum_node rclcpp)

install(TARGETS
	vacuum_node
	DESTINATION lib/${PROJECT_NAME}
)

ament_package()
