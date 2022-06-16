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

// read file list
#include <dirent.h>
#include <typeinfo>
#include <boost/lexical_cast.hpp>
#include <cstring>
#include <string>
#include <fstream>

using namespace std;
using namespace ros;


tf::StampedTransform echo_transform;
tf::TransformListener *tf_listener;
tf::StampedTransform min_distance_trans;
tf::StampedTransform camera_transform;
tf::StampedTransform tag_transforms[6];

tf::StampedTransform camera_transform2;

int id;
double min_distance = 100;
double check_time[6] = {0};
double local_time = 0;
Publisher transform_pub;



void listener(){
	// use tf_listener to get the transformation from camera_link to tag 0
	id = -1;
	// min_distance = 100;
	min_distance = 100;
	for (int i = 0; i < 5; i++){
		string parent_id = "camera_link";
		string child_id = "tag_" + std::to_string(i);

		tf_listener->waitForTransform(child_id, parent_id, ros::Time::now(), ros::Duration(0.07));
		try {
 
			tf_listener->lookupTransform(parent_id, child_id, ros::Time(0), echo_transform);
			if (check_time[i] != echo_transform.stamp_.toSec()){
				std::cout << "#####################################################################" << endl;
				std::cout << "Tag id is " << i << endl;
				std::cout << "At time " << std::setprecision(16) << echo_transform.stamp_.toSec() << std::endl;
				// cout << "Frame id:" << echo_transform.frame_id_ << ", Child id:" << echo_transform.child_frame_id_ << endl;
				double yaw, pitch, roll;
				echo_transform.getBasis().getRPY(roll, pitch, yaw);
				tf::Quaternion q = echo_transform.getRotation();
				tf::Vector3 v = echo_transform.getOrigin();
				// std::cout << "- Translation: [" << v.getX() << ", " << v.getY() << ", " << v.getZ() << "]" << std::endl;
				// std::cout << "- Rotation: in Quaternion [" << q.getX() << ", " << q.getY() << ", " << q.getZ() << ", " << q.getW() << "]" << std::endl;
				std::cout << "- echo time:" << echo_transform.stamp_.toSec() << endl;
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
				cout << "distance from tag " << i << ": " << dist << endl << endl;
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
					// cout << 
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
		//check file list
		struct dirent *d;
		string file_name = to_string(id) + ".txt";
		int find_ = 0;
		DIR *dr;
		dr = opendir(".");
		cout << "Finding " << file_name << "..." << endl;
		if(dr!=NULL) {
		    // cout<<"List of Files & Folders:-\n";
		    for(d=readdir(dr); d!=NULL; d=readdir(dr)) {
		        // cout<<d->d_name<<endl;
		        if(boost::lexical_cast<std::string>(d->d_name)==file_name) {
		        	cout << "Exist " << file_name << endl;
		        	find_ = 1;
		        	break;
		        } 
		    }
		    closedir(dr);
		}
		else {
		    cout<<"Error Occurred!" << endl;
		}
		//if not found we create
		if(!find_) {
			cout << "Not Exist " << file_name << endl;  
	        std::ofstream outfile (file_name);
	        // outfile << echo_transform.stamp_.toSec() << std::endl;
	        outfile.close();
		}

		/*
		        Find transformation matrix from "camera_color_optical_frame" to "origin".
		*/

		// /**************************************************************
		// //                 Student Implementation                    //
		// **************************************************************/

		tf::Transform localization_trans;
		// localization_trans = (tag_transforms[id] * min_distance_trans.inverse() * camera_transform).inverse();
		localization_trans = (tag_transforms[id] * min_distance_trans.inverse() * camera_transform);
		//tag_transforms: tag - origin
		//min_distance_trans: camera_link - tag 
		//tag_transform * min_distance_trans = tf1 (origin - camera link) 
		//camera_transform: camera - optical  (tf_cam)


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
		trans_odem.header.stamp = min_distance_trans.stamp_;
		transform_pub.publish(trans_odem);
		// std::cout << "- Translation: [" << v.getX() << ", " << v.getY() << ", " << v.getZ() << "]" << std::endl;
		// std::cout << "- Rotation: in Quaternion [" << q.getX() << ", " << q.getY() << ", " << q.getZ() << ", " << q.getW() << "]" << std::endl;
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
	// transform_pub = nh.advertise<nav_msgs::Odometry>("odom_trans", 1000);
	transform_pub = nh.advertise<nav_msgs::Odometry>("apriltag_localization", 1000);

	bool find = false;

	/* get the transform from "camera_color_optical_frame" to "camera_link" */
	string child_id = "camera_color_optical_frame";   // implememt
	string parent_id = "camera_link";	
	// string parent_id = "camera_color_optical_frame";   // implememt
	// string child_id = "camera_link";	


	while (!find) {
		tf_listener->waitForTransform(child_id, parent_id, ros::Time::now(), ros::Duration(0.7));
		try {
			tf_listener->lookupTransform(parent_id, child_id, ros::Time(0), camera_transform);
			// cout << "Get transform from \"camera_color_optical_frame\" to \"camera_link\"!!!!!!!\n";
			find = true;
		}
		catch (tf::TransformException& ex){
			std::cout << "Exception thrown2:" << ex.what() << std::endl;
		}
	}

	/* get the transform from "map_tag" to "origin" */
	for (int i = 0; i < 5; i++) {
		// if(i==2){
		// 	continue;
		// }
		child_id = "map_tag_" + std::to_string(i);  // implememt
		parent_id = "origin";
		// parent_id = "map_tag_" + std::to_string(i);  // implememt
		// child_id = "origin";
		tf_listener->waitForTransform(child_id, parent_id, ros::Time::now(), ros::Duration(0.7));
		try {
			tf_listener->lookupTransform(parent_id, child_id, ros::Time(0), tag_transforms[i]);
			// cout << "Get transform from map_tag_" << i << " to \"origin\"\n";
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