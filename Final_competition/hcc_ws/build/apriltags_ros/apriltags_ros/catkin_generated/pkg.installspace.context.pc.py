# generated from catkin/cmake/template/pkg.context.pc.in
CATKIN_PACKAGE_PREFIX = ""
PROJECT_PKG_CONFIG_INCLUDE_DIRS = "${prefix}/include;/usr/include/eigen3".split(';') if "${prefix}/include;/usr/include/eigen3" != "" else []
PROJECT_CATKIN_DEPENDS = "apriltags;image_transport;roscpp;sensor_msgs;geometry_msgs;tf;cv_bridge;message_runtime;nodelet;std_msgs".replace(';', ' ')
PKG_CONFIG_LIBRARIES_WITH_PREFIX = "-lapriltag_detector".split(';') if "-lapriltag_detector" != "" else []
PROJECT_NAME = "apriltags_ros"
PROJECT_SPACE_DIR = "/home/mamu/hcc_2022/Final_competition/hcc_ws/install"
PROJECT_VERSION = "0.1.2"
