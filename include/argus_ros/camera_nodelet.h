// -*- c++ -*-
/*
 * Copyright (C) 2017 Love Park Robotics, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distribted on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ROYALE_ROS_CAMERA_NODELET_H__
#define __ROYALE_ROS_CAMERA_NODELET_H__

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include <argus_ros/Config.h>
#include <argus_ros/Dump.h>
#include <argus_ros/SetExposureTime.h>
#include <argus_ros/SetExposureTimes.h>
#include <argus_ros/Start.h>
#include <argus_ros/Stop.h>
#include <image_transport/image_transport.h>
#include <nodelet/nodelet.h>
#include <ros/ros.h>
#include <argus.hpp>

//-------------- BNR -----------/
#include <argus/IEvent.hpp>

#include <argus_ros/CameraOpStatus.h>
#include <argus_ros/StartRecord.h>
#include <argus_ros/StopRecord.h>
#include <ros/package.h>
#include <json.hpp>

#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
//------------------------------/

using json = nlohmann::json;

namespace argus_ros {
/**
   * This class implements both the ROS nodelet interface and
   * the Argus IExtendedDataListener interface. It is the central figure of this
   * application used to manage, configure, and acquire data from a single
   * gener8 camera.
   */
class CameraNodelet
    : public nodelet::Nodelet,
      public argus::IExtendedDataListener,
      public argus::IEventListener {
 private:
  //
  // Nodelet lifecycle functions
  //
  void onInit() override;

  //
  // ROS services
  //
  bool Dump(argus_ros::Dump::Request& req, argus_ros::Dump::Response& resp);
  bool Config(argus_ros::Config::Request& req,
              argus_ros::Config::Response& resp);
  bool Start(argus_ros::Start::Request& req,
             argus_ros::Start::Response& resp);
  bool Stop(argus_ros::Stop::Request& req, argus_ros::Stop::Response& resp);

  //-------------- BNR -----------/
  //
  // ROS services to record rrf files
  bool StartRecord(argus_ros::StartRecord::Request& req,
                   argus_ros::StartRecord::Response& resp);
  bool StopRecord(argus_ros::StopRecord::Request& req,
                  argus_ros::StopRecord::Response& resp);

  // Helper funtion to load the image mask
  bool ParseMaskFile();

  // Helper function to publish camera operating status
  void PublishCameraStatus();
  //------------------------------/

  //
  // Handlers for topic subscriptions
  //
  void SetExposureTimeCb(const argus_ros::SetExposureTime::ConstPtr& msg);
  void SetExposureTimesCb(const argus_ros::SetExposureTimes::ConstPtr& msg);

  //
  // Argus callback, basically functions as our main
  // publishing loop
  //
  void onNewData(const argus::IExtendedData* data) override;

  //-------------- BNR -----------/
  // Argus callback for events, mainly temperature events
  void onEvent(std::unique_ptr<argus::IEvent>&& event) override;
  //------------------------------/

  //
  // Helpers
  //
  void InitCamera();
  void RescheduleTimer();
  void CacheIntrinsics();
  void StartCameraStream();
  int SetConfigurationParams(json&, std::string&);

  //
  // State
  //

  // Acts as a s/w switch for the camera stream
  bool on_;
  std::mutex on_mutex_;

  std::unique_ptr<argus::ICameraDevice> cam_;

  //-------------- BNR -----------/
  // Data structure to store the unit vector data
  std::unique_ptr<argus::DepthData> uvec_data_;
  //------------------------------/

  std::mutex cam_mutex_;
  std::string access_code_;
  std::string serial_number_;
  float poll_bus_secs_;
  float timeout_secs_;
  std::string optical_frame_;
  std::string sensor_frame_;
  std::string initial_use_case_;

  bool instantiated_publishers_;
  std::uint32_t access_level_;
  ros::Time last_frame_;
  std::mutex last_frame_mutex_;

  //-------------- BNR -----------/
  // Mutex and variable to store the time at which the last
  // camera operating status was published
  std::mutex hw_mutex_;
  float stat_secs_;
  ros::Time last_stat_frame_;
  //------------------------------/

  ros::NodeHandle nh_, np_;
  ros::ServiceServer dump_srv_;
  ros::ServiceServer config_srv_;
  ros::ServiceServer start_srv_;
  ros::ServiceServer stop_srv_;

  //-------------- BNR -----------/
  ros::ServiceServer rrf_record_start_srv_;
  ros::ServiceServer rrf_record_stop_srv_;
  //------------------------------/

  ros::Timer timer_;

  ros::Subscriber exp_time_sub_;
  ros::Subscriber exp_times_sub_;

  std::unique_ptr<image_transport::ImageTransport> it_;
  std::vector<ros::Publisher> cloud_pubs_;
  std::vector<ros::Publisher> exposure_pubs_;
  std::vector<image_transport::Publisher> noise_pubs_;
  std::vector<image_transport::Publisher> gray_pubs_;
  std::vector<image_transport::Publisher> conf_pubs_;
  std::vector<image_transport::Publisher> xyz_pubs_;

  //-------------- BNR -----------/
  std::vector<image_transport::Publisher> depth_pubs_;
  std::vector<image_transport::Publisher> unit_vec_pubs_;
  ros::Publisher image_mask_pub_;
  ros::Publisher cam_hw_info_pub_;
  //------------------------------/
  // REP 104 suggests publishing the intrinsics with every frame
  // see: http://www.ros.org/reps/rep-0104.html
  //
  // So, we register each calibration message to a frame and on each stream
  // for mixed-mode use cases.
  std::vector<ros::Publisher> intrinsic_pubs_;
  sensor_msgs::CameraInfo intrinsic_msg_;
  std::mutex intrinsic_mutex_;

  std::string current_use_case_;
  std::mutex current_use_case_mutex_;
  std::map<std::string, std::vector<std::uint16_t> > stream_id_lut_;

  //-------------- BNR -----------/
  std::string config_file_;
  bool config_populated_;

  std::string image_mask_loc_;
  bool image_mask_loaded_;
  cv::Mat image_mask_;

  float cur_temp_;
  std::vector<float> cur_mod_freq_;
  std::vector<unsigned int> cur_illumin_;
  //------------------------------/
};  // end: class CameraNodelet

}  // end: namespace argus_ros

#endif  // __ROYALE_ROS_CAMERA_NODELET_H__
