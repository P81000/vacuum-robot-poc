SUMMARY = "ROS2 Vacuum Navigator"

LICENSE = "CLOSED"

ROS_BPN = "vacuum_navigator"

inherit ros_distro_humble
inherit ros_superflore_generated

ROS_BUILD_TYPE = "ament_cmake"

inherit ros_${ROS_BUILD_TYPE}

SRC_URI = "\
    file://CMakeLists.txt \
    file://package.xml \
    file://src \
"

S = "${WORKDIR}"

ROS_BUILDTOOL_DEPENDS = "\
    ament-cmake-native \
"

ROS_BUILD_DEPENDS = "\
    rclcpp \
"

ROS_EXEC_DEPENDS = "\
    rclcpp \
"

DEPENDS = "${ROS_BUILD_DEPENDS} ${ROS_BUILDTOOL_DEPENDS}"
RDEPENDS:${PN} += "${ROS_EXEC_DEPENDS}"
