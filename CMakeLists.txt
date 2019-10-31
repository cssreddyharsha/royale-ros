cmake_minimum_required(VERSION 2.8.3)
project(argus_ros)

set(ARGUS_ROOT "/opt/argus" CACHE STRING "Root directory to argus SDK")
set(argus_DIR "/opt/argus/share")

find_package(argus REQUIRED
  PATHS ${argus_ROOT}/share
  )

find_package(Boost REQUIRED)

MESSAGE(STATUS "ARGUS_ROOT: " ${argus_ROOT})
MESSAGE(STATUS "argus_LIB_DIR: " ${argus_LIB_DIR})
MESSAGE(STATUS "argus_LIBS: " ${argus_LIBS})

find_package(Boost REQUIRED)

find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  image_transport
  message_generation
  message_runtime
  nodelet
  pcl_ros
  roscpp
  roslint
  sensor_msgs
  std_msgs
  tf2_ros
  )

#######################################
## Declare ROS messages and services ##
#######################################
add_service_files(
  FILES
  Config.srv
  Dump.srv
  Start.srv
  StartRecord.srv
  Stop.srv
  StopRecord.srv
  )

add_message_files(
  FILES
  CameraOpStatus.msg
  ExposureTimes.msg
  SetExposureTime.msg
  SetExposureTimes.msg
  )

generate_messages(
  DEPENDENCIES
  std_msgs
  )

###################################
## catkin specific configuration ##
###################################
catkin_add_env_hooks(argus_env SHELLS sh)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS message_runtime nodelet roscpp sensor_msgs std_msgs
  )

#############
##  Build  ##
#############

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-conversion -std=c++14")

#catkin_lint: ignore_once external_directory
include_directories(
  include
  ${argus_ROOT}/include
  ${Boost_INCLUDE_DIRS}
  ${catkin_INCLUDE_DIRS}
  )

link_directories(
  ${argus_LIB_DIR}
  )

add_library(${PROJECT_NAME} src/camera_nodelet.cpp)
target_link_libraries(${PROJECT_NAME}
  ${catkin_LIBRARIES}
  ${argus_LIBS}
  )
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_generate_messages_cpp)

add_executable(${PROJECT_NAME}_lscam src/lscam.cpp)
target_link_libraries(${PROJECT_NAME}_lscam
    ${argus_LIBS}
  )
add_dependencies(${PROJECT_NAME}_lscam ${PROJECT_NAME}_generate_messages_cpp)

add_executable(${PROJECT_NAME}_config src/config.cpp)
target_link_libraries(${PROJECT_NAME}_config
  ${catkin_LIBRARIES}
  )
add_dependencies(${PROJECT_NAME}_config ${PROJECT_NAME}_generate_messages_cpp)

add_executable(${PROJECT_NAME}_dump src/dump.cpp)
target_link_libraries(${PROJECT_NAME}_dump
  ${catkin_LIBRARIES}
  )
add_dependencies(${PROJECT_NAME}_dump ${PROJECT_NAME}_generate_messages_cpp)

#############
## Install ##
#############

install(TARGETS
  ${PROJECT_NAME}
  ${PROJECT_NAME}_config
  ${PROJECT_NAME}_dump
  ${PROJECT_NAME}_lscam
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  )

install(DIRECTORY
  include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
  )

install(FILES nodelets.xml
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
  )

install(DIRECTORY launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
  )

install(DIRECTORY doc
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
  )

######################
## Node-level tests ##
######################
