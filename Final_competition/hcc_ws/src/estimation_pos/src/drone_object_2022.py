#!/usr/bin/env python3

import rospy
import numpy as np
import message_filters
import cv2, os
from cv_bridge import CvBridge, CvBridgeError
from darknet_ros_msgs.msg import BoundingBoxes
from sensor_msgs.msg import Image, CameraInfo
from geometry_msgs.msg import PointStamped
from nav_msgs.msg import Odometry
import tf
from tf.transformations import quaternion_matrix
import math
from os import listdir
import time


index = 0
my_order = []

# with open("EEGanswer.txt", "r") as tf:
#     my_order = tf.read().split('\n')
#     print(my_order)

pub = rospy.Publisher('/object_pose', PointStamped, queue_size=10)
rospy.init_node('drone_Object', anonymous=True)
rospy.loginfo("Start D435_Object_Distance")
cv_bridge = CvBridge()

print('Try to get camera info...')

msg = rospy.wait_for_message('/camera/color/camera_info', CameraInfo, timeout=None)
#     [fx'  0  cx' Tx]
# P = [ 0  fy' cy' Ty]
#     [ 0   0   1   0]
print('Get camera info')
fx = msg.P[0]
fy = msg.P[5]
cx = msg.P[2]
cy = msg.P[6]

transform_time = 0.0
transform = Odometry()

tvmonitor = np.zeros(3)
bottle = np.zeros(3)
teddy_bear = np.zeros(3)
keyboard = np.zeros(3)

tvmonitor_vec = []
bottle_vec = []
teddy_bear_vec = []
keyboard_vec = []


tag_order = ['0.txt', '1.txt', '3.txt', '4.txt']
old_id = -1
now_id = -1

count = np.zeros(4, dtype=int)

def main():
	print("Start running main()")
	global count
	global old_id
	global now_id
	old_id = int(tag_order[0].split('.')[0])
	now_id = int(tag_order[0].split('.')[0])
	# depth_image_sub = message_filters.Subscriber('???', ???)
	# bb_sub = message_filters.Subscriber('???', ???)
	# ts = message_filters.ApproximateTimeSynchronizer([???, ???], ???, ???)
	# ts.registerCallback(???)
	depth_image_sub = message_filters.Subscriber('/camera/aligned_depth_to_color/image_raw', Image)
	bb_sub = message_filters.Subscriber('/darknet_ros/bounding_boxes', BoundingBoxes)
	ts = message_filters.ApproximateTimeSynchronizer([depth_image_sub, bb_sub], 10, 0.5)
	ts.registerCallback(callback)
	# rospy.Subscriber("???", ???, ???) 
	rospy.Subscriber("apriltag_localization", Odometry, transform_cb) #p3 subscribe then do call back function
	rospy.spin()
	write_file(int(tag_order[3].split('.')[0]))
	print("End write file")
	time.sleep(5)
	exec(open('/home/wumamu/2022_HCC_TEAM3_FINAL/Final_competition/hcc_ws/src/estimation_pos/src/count.py').read())
	print("End output")
	# output txt
	

def write_file(file_name_id):
	# global old_id
	# global now_id
	print("#########################################")
	print("Now is writeing to " + str(file_name_id) + '.txt\n')
	path = str(file_name_id) + '.txt'
	with open(path, 'w') as f:
		f.write("tvmonitor_vec\n")
		f.write(str(np.array(tvmonitor_vec).reshape(-1,3)))
		# f.write('\n'.join(tvmonitor_vec))
		f.write("\n--------------------------\n")
		f.write("bottle_vec\n")
		f.write(str(np.array(bottle_vec).reshape(-1,3)))
		# f.write('\n'.join(bottle_vec))
		f.write("\n--------------------------\n")
		f.write("teddy_bear_vec\n")
		f.write(str(np.array(teddy_bear_vec).reshape(-1,3)))
		# f.write('\n'.join(teddy_bear_vec))
		f.write("\n--------------------------\n")
		f.write("keyboard_vec\n")
		f.write(str(np.array(keyboard_vec).reshape(-1,3)))
		# f.write('\n'.join(keyboard_vec))
		# update old _id
		# old_id = now_id
		print ("$$$$$$$$$$ clear list $$$$$$$$$$")
		tvmonitor_vec.clear()
		bottle_vec.clear()
		teddy_bear_vec.clear()
		keyboard_vec.clear()

def transform_cb(msg):
	global transform_time
	global transform
	transform_time = msg.header.stamp.to_sec()
	transform = msg
	# print("Get transform time")
	# print(transform_time)

def callback(depth_img, bb):
	local_time = depth_img.header.stamp.to_sec()
	# print("Get local_time")
	# print(local_time)

	# you could set the time error (local_time - transform_time) by yourseelf    
	if abs(local_time - transform_time) < 1 and transform_time != 0:
		print("\nTime error:", local_time - transform_time)
		
		# hint: http://docs.ros.org/en/jade/api/tf/html/python/transformations.html
		# You could use "quaternion_matrix" function to find the 4x4 transform matrix
		global_transform = quaternion_matrix([transform.pose.pose.orientation.x, transform.pose.pose.orientation.y, transform.pose.pose.orientation.z, transform.pose.pose.orientation.w])
		global_transform[0][3] = transform.pose.pose.position.x
		global_transform[1][3] = transform.pose.pose.position.y
		global_transform[2][3] = transform.pose.pose.position.z
		# tf2
		# print("transform")
		# print(global_transform)
		try:
			cv_depthimage = cv_bridge.imgmsg_to_cv2(depth_img, "32FC1")
			cv_depthimage2 = np.array(cv_depthimage, dtype=np.float32)
		except CvBridgeError as e:
			print(e)

		for i in bb.bounding_boxes:
			now_files = listdir("./")
			global old_id
			global now_id
			# read lastest file name
			for j in tag_order:
				if j in now_files:
					# print("----------------------- ", j)
					now_id = int(j.split('.')[0])
			if now_id!=old_id:
				# update of tag
				write_file(old_id)
				old_id = now_id
			x_mean = (i.xmax + i.xmin) / 2
			y_mean = (i.ymax + i.ymin) / 2
			# calculate pos remove divergence
			if i.Class == "tvmonitor":
			# if i.Class == my_order[index]:
				############################
				#  Student Implementation  #
				############################
				rospy.loginfo("see tvmonitor")
				# rospy.loginfo("see " + my_order[index])
				zc = cv_depthimage2[int(y_mean)][int(x_mean)]
				v1 = np.array(getXYZ(x_mean, y_mean, zc, fx, fy, cx, cy))
				print(v1/1000)
				point_message = PointStamped()
				point_message.header = depth_img.header
				point_message.header.frame_id = "camera_color_optical_frame"
				# point_message.point.x = v1[0]/1000 #(x->meter)
				# point_message.point.y = v1[1]/1000
				# point_message.point.z = v1[2]/1000
				
				xyz = [v1[0]/1000, v1[1]/1000, v1[2]/1000, 1]
				xyz = np.array(xyz).reshape(4,1)
				trans = global_transform.dot(xyz)
				point_message.point.x = trans[0]
				point_message.point.y = trans[1]
				point_message.point.z = trans[2]
				print("tvmonitor x: " ,point_message.point.x)
				print("tvmonitor y: " ,point_message.point.y)
				print("tvmonitor z: " ,point_message.point.z)
				tvmonitor_vec.append([point_message.point.x, point_message.point.y, point_message.point.z])
				pub.publish(point_message)
			if i.Class == "teddy bear":
				rospy.loginfo("see teddybear")
				zc = cv_depthimage2[int(y_mean)][int(x_mean)]
				v1 = np.array(getXYZ(x_mean, y_mean, zc, fx, fy, cx, cy))
				print(v1/1000)
				point_message = PointStamped()
				point_message.header = depth_img.header
				point_message.header.frame_id = "camera_color_optical_frame"
				# point_message.point.x = v1[0]/1000 #(x->meter)
				# point_message.point.y = v1[1]/1000
				# point_message.point.z = v1[2]/1000
				xyz = [v1[0]/1000, v1[1]/1000, v1[2]/1000, 1]
				xyz = np.array(xyz).reshape(4,1)
				trans = global_transform.dot(xyz)
				point_message.point.x = trans[0]
				point_message.point.y = trans[1]
				point_message.point.z = trans[2]
				print("teddybear x: " ,point_message.point.x)
				print("teddybear y: " ,point_message.point.y)
				print("teddybear z: " ,point_message.point.z)
				teddy_bear_vec.append([point_message.point.x, point_message.point.y, point_message.point.z])
				pub.publish(point_message)
			if i.Class == "keyboard":
				rospy.loginfo("see keyboard")
				zc = cv_depthimage2[int(y_mean)][int(x_mean)]
				v1 = np.array(getXYZ(x_mean, y_mean, zc, fx, fy, cx, cy))
				print(v1/1000)
				point_message = PointStamped()
				point_message.header = depth_img.header
				point_message.header.frame_id = "camera_color_optical_frame"
				# point_message.point.x = v1[0]/1000 #(x->meter)
				# point_message.point.y = v1[1]/1000
				# point_message.point.z = v1[2]/1000
				# print(global_transform*point_message.point)
				xyz = [v1[0]/1000, v1[1]/1000, v1[2]/1000, 1]
				xyz = np.array(xyz).reshape(4,1)
				trans = global_transform.dot(xyz)
				point_message.point.x = trans[0]
				point_message.point.y = trans[1]
				point_message.point.z = trans[2]
				print("keyboard x: ",point_message.point.x)
				print("keyboard y: ",point_message.point.y)
				print("keyboard z: ",point_message.point.z)
				keyboard_vec.append([point_message.point.x, point_message.point.y, point_message.point.z])
				pub.publish(point_message)
			if i.Class == "bottle":
				rospy.loginfo("see bottle")
				zc = cv_depthimage2[int(y_mean)][int(x_mean)]
				v1 = np.array(getXYZ(x_mean, y_mean, zc, fx, fy, cx, cy))
				print(v1/1000)
				point_message = PointStamped()
				point_message.header = depth_img.header
				point_message.header.frame_id = "camera_color_optical_frame"
				# point_message.point.x = v1[0]/1000 #(x->meter)
				# point_message.point.y = v1[1]/1000
				# point_message.point.z = v1[2]/1000
				# print(global_transform*point_message.point)
				xyz = [v1[0]/1000, v1[1]/1000, v1[2]/1000, 1]
				xyz = np.array(xyz).reshape(4,1)
				trans = global_transform.dot(xyz)
				point_message.point.x = trans[0]
				point_message.point.y = trans[1]
				point_message.point.z = trans[2]
				print("keyboard x: " ,point_message.point.x)
				print("keyboard y: " ,point_message.point.y)
				print("keyboard z: " ,point_message.point.z)
				bottle_vec.append([point_message.point.x, point_message.point.y, point_message.point.z])
				pub.publish(point_message)



			

def getXYZ(xp, yp, zc, fx,fy,cx,cy):
	#### Definition:
	# cx, cy : image center(pixel)
	# fx, fy : focal length
	# xp, yp : index of the depth image
	# zc: depth
	inv_fx = 1.0/fx
	inv_fy = 1.0/fy
	x = (xp-cx) *  zc * inv_fx
	y = (yp-cy) *  zc * inv_fy
	z = zc
	return (x,y,z,1)


if __name__ == '__main__':
	# with open("EEGanswer.txt", "r") as tf:
	# 	my_order = tf.read().split('\n')
	# 	print(my_order)
	main()

