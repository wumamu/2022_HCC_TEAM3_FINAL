gnome-terminal  --tab  --working-directory="/home/wumamu/2022_HCC_TEAM3_FINAL/Final_competition/hcc_ws" -e 'bash -c "source devel/setup.bash ; rosparam set use_sim_time true; exec bash" '\
                --window \
                --tab  --working-directory="/home/wumamu/2022_HCC_TEAM3_FINAL/Final_competition/hcc_ws" -e 'bash -c "source devel/setup.bash ; roslaunch estimation_pos hcc2022_final_map.launch; exec bash" '\
                --tab  --working-directory="/home/wumamu/2022_HCC_TEAM3_FINAL/Final_competition/hcc_ws" -e 'bash -c "source devel/setup.bash ; roslaunch estimation_pos localization_final_2022.launch; exec bash" '\
                --tab  --working-directory="/home/wumamu/2022_HCC_TEAM3_FINAL/Final_competition/hcc_ws" -e 'bash -c "source devel/setup.bash ; roslaunch darknet_ros yolo_v3.launch; exec bash" '\
                --tab  --working-directory="/home/wumamu/2022_HCC_TEAM3_FINAL/Final_competition/hcc_ws" -e 'bash -c "source devel/setup.bash ; rosrun estimation_pos apriltag_localization_2022; exec bash" '\
                --tab  --working-directory="/home/wumamu/2022_HCC_TEAM3_FINAL/Final_competition/hcc_ws" -e 'bash -c "source devel/setup.bash ; rosrun estimation_pos drone_object_2022.py; exec bash" '\
                

exit 0