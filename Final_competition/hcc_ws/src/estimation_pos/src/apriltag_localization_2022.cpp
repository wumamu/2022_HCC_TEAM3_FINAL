#include <stdlib.h>
#include <iostream>
#include <vector>
#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>
#include <tf/LinearMath/Matrix3x3.h>
#include <tf/LinearMath/Vector3.h>
#include <geometry_msgs/Transform.h>
#include <nav_msgs/Odometry.h>

using namespace std;
using namespace ros;


tf::StampedTransform echo_transform;
tf::TransformListener *tf_listener;
tf::StampedTransform min_distance_trans;
tf::StampedTransform camera_transform;
tf::StampedTransform tag_transforms[6];
int id;
double min_distance = 100;
double check_time[6] = {0};
double local_time = 0;
Publisher transform_pub;


void listener(){
	// use tf_listener to get the transformation from camera_link to tag 0
	id = -1;
	min_distance = 100;
	for (int i = 0; i < 5; i++){
		string parent_id = "camera_link";
		string child_id = "tag_" + std::to_string(i);

		tf_listener->waitForTransform(child_id, parent_id, ros::Time::now(), ros::Duration(0.07));
		try {
 
			tf_listener->lookupTransform(parent_id, child_id, ros::Time(0), echo_transform);
			if (check_time[i] != echo_transform.stamp_.toSec()){
				std::cout << "At time " << std::setprecision(16) << echo_transform.stamp_.toSec() << std::endl;
				cout << "Frame id:" << echo_transform.frame_id_ << ", Child id:" << echo_transform.child_frame_id_ << endl;
				double yaw, pitch, roll;
				echo_transform.getBasis().getRPY(roll, pitch, yaw);
				tf::Quaternion q = echo_transform.getRotation();
				tf::Vector3 v = echo_transform.getOrigin();
				std::cout << "- Translation: [" << v.getX() << ", " << v.getY() << ", " << v.getZ() << "]" << std::endl;
				std::cout << "- Rotation: in Quaternion [" << q.getX() << ", " << q.getY() << ", "
									<< q.getZ() << ", " << q.getW() << "]" << std::endl;
				std::cout << "-  echo time:" << echo_transform.stamp_.toSec() << endl;
				std::cout << "- local time:" << local_time << endl;
						
				// hint:
				// double dist;
				// dist = ???;
				/************************************************************** 
				//                                                           //
				//                                                           //
				//                 Student Implementation                    //
				//                                                           //
				//                                                           //
				**************************************************************/
				//ORIGIN: Please calucluate the distance from tag_0 to other tags.
				double dist;
				double x = v.x();
				double y = v.y();
				double z = v.z();
				dist = sqrt(pow(x,2)+pow(y,2)+pow(z,2));
				cout << "distance: " << dist << endl << endl;
				//#############################################################
				local_time = echo_transform.stamp_.toSec(); // record the time you catch the transform
				check_time[i] = local_time;
				/*
					find the closet distance from the tag to camera_link (remember to modify the parent_id).  //
				*/

				if (min_distance > dist){
					id = i;   
					min_distance = dist;
					min_distance_trans = echo_transform;
				}
			}
			// find the closet tag to localization
			/************************************************************** 
			//                                                           //
			//                                                           //
			//                 Student Implementation                    //
			//                                                           //
			//                                                           //
			**************************************************************/

		}
		catch (tf::TransformException& ex)
		{
			std::cout << "Exception thrown1:" << ex.what() << std::endl;
		}
	}

	/* localize the robot position */ 

	if(id >= 0) {
		/*
		        Find transformation matrix from "camera_color_optical_frame" to "origin".
		    */
		// tf::Transform localization_trans;
		// string parent_id = "camera_color_optical_frame";
  //   string child_id = "origin";
  //   tf_listener->waitForTransform(child_id, parent_id, ros::Time::now(), ros::Duration(0.001));
  //   try {

  //       tf_listener->lookupTransform(parent_id, child_id, ros::Time(0), echo_transform);
  //       // tf::Transform localization_trans;
  //       // /*
  //       //     Find transformation matrix from echo_transform and min_distance_trans.
  //       // */

  //       // localization_trans = echo_transform * min_distance_trans.inverse();

  //       // cout << "=================================\n";
  //       // cout << "*get the robot position*\n";
  //       // cout << "rotation:\n";
  //       tf::Quaternion q = localization_trans.getRotation();
  //       // cout << "[" << q.getX() << ", " << q.getY() << ", "<< q.getZ() << ", " << q.getW() << "]" << endl;
  //       tf::Vector3 v = localization_trans.getOrigin();
  //       // cout << "translation:\n";
  //       // cout << "[" << v.getX() << ", " << v.getY() << ", " << v.getZ() << "]" << endl;
  //       // cout << "=================================\n";
  //   }
  //   catch (tf::TransformException& ex)
  //   {
  //       std::cout << "Exception thrown:" << ex.what() << std::endl;
  //   }
		// /**************************************************************
		// //                 Student Implementation                    //
		// **************************************************************/

		tf::Transform localization_trans;
		localization_trans = (tag_transforms[id] * min_distance_trans.inverse() * camera_transform).inverse();
		/* publish the transform */
		tf::Quaternion q = localization_trans.getRotation();
		tf::Vector3 v = localization_trans.getOrigin();

		/* publish the transform */
		nav_msgs::Odometry trans_odem;
		trans_odem.pose.pose.position.x = v.getX(); //implement
		trans_odem.pose.pose.position.y = v.getY();
		trans_odem.pose.pose.position.z = v.getZ();
		trans_odem.pose.pose.orientation.x = q.getX();
		trans_odem.pose.pose.orientation.y = q.getY();
		trans_odem.pose.pose.orientation.z = q.getZ();
		trans_odem.pose.pose.orientation.w = q.getW();
		trans_odem.header.stamp = ros::Time::now();
		// transform_pub.publish(???);
		transform_pub.publish(trans_odem);
	}
	return;
}

int main(int argc, char** argv){
	ros::init(argc, argv, "apriltag_localization");
	ros::NodeHandle nh;
	tf_listener = new tf::TransformListener();
	
	// write the publisher
	// transform_pub = nh.advertise<nav_msgs::Odometry>("???", ???);
	// transform_pub = nh.advertise<nav_msgs::Odometry>("odom_trans", Odometry);
	transform_pub = nh.advertise<nav_msgs::Odometry>("odom_trans", 1000);

	bool find = false;

	/* get the transform from "camera_color_optical_frame" to "camera_link" */
	string parent_id = "camera_color_optical_frame";   // implememt
	string child_id = "camera_link";	
	while (!find) {
		tf_listener->waitForTransform(child_id, parent_id, ros::Time::now(), ros::Duration(0.7));
		try {
			tf_listener->lookupTransform(parent_id, child_id, ros::Time(0), camera_transform);
			cout << "Get transform from \"camera_color_optical_frame\" to \"camera_link\"!!!!!!!\n";
			find = true;
		}
		catch (tf::TransformException& ex){
			std::cout << "Exception thrown2:" << ex.what() << std::endl;
		}
	}

	/* get the transform from "map_tag" to "origin" */
	for (int i = 0; i < 5; i++) {
		parent_id = "map_tag_" + std::to_string(i);  // implememt
		child_id = "origin";
		tf_listener->waitForTransform(child_id, parent_id, ros::Time::now(), ros::Duration(0.7));
		try {
			tf_listener->lookupTransform(parent_id, child_id, ros::Time(0), tag_transforms[i]);
			cout << "Get transform from map_tag_" << i << " to \"origin\"\n";
		}
		catch (tf::TransformException& ex){
			std::cout << "Exception thrown3:" << ex.what() << std::endl;
		}    
	}

	while (ros::ok())
	{
		ros::spinOnce();
		listener();
	}
	
	return 0;
}