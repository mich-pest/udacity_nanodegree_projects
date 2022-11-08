def next_frame_callback(vis_lidar_pc):
    vis_lidar_pc.close()
        
def exit_callback(vis_lidar_pc):
    vis_lidar_pc.destroy_window()
# visualize lidar point-cloud
def show_pcl(pcl):
    # step 1 : initialize open3d with key callback and create window
    vis_lidar_pc = o3.visualization.VisualizerWithKeyCallback()
    vis_lidar_pc.create_window(window_name='Lidar Point Cloud')
    # step 2 : create instance of open3d point-cloud class
    pcd = o3.geometry.PointCloud()
    # step 3 : set points in pcd instance by converting the point-cloud into 3d vectors (using open3d function Vector3dVector)
    # The reason in this step is that point cloud first three channels have (x,y,z) cartesian location and we want them as vectors.
    pcd.points = o3.utility.Vector3dVector(pcl[:,:3])
    # step 4 : for the first frame, add the pcd instance to visualization using add_geometry; for all other frames, use update_geometry instead
    vis_lidar_pc.add_geometry(pcd) 
   
    # step 5 : visualize point cloud and keep window open until right-arrow is pressed (key-code 262) 
    # GLFW_KEY code is used in Open3D library: https://www.glfw.org/docs/latest/group__keys.html
    right_arrow_key_code = 262
    space_bar_key_code = 32
    vis_lidar_pc.register_key_callback(right_arrow_key_code, next_frame_callback)
    vis_lidar_pc.register_key_callback(space_bar_key_code, exit_callback)
    vis_lidar_pc.update_renderer()
    vis_lidar_pc.poll_events()    
    vis_lidar_pc.run()  



    -------------------------------------------------------


    def show_pcl(pcl):
        ####### ID_S1_EX2 START #######     
        #######
        print("student task ID_S1_EX2")
        
        def continue_callback(vis):
            vis.close()
        def stop_callback(vis):
            vis.destroy_window()

        # step 1 : initialize open3d with key callback and create window
        vis = o3d.visualization.VisualizerWithKeyCallback()
        vis.create_window()
        # step 2 : create instance of open3d point-cloud class
        point_cloud = o3d.geometry.PointCloud()
        # step 3 : set points in pcd instance by converting the point-cloud into 3d vectors (using open3d function Vector3dVector)
        point_cloud.points = o3d.utility.Vector3dVector(pcl[:, 0:3])
        # step 4 : for the first frame, add the pcd instance to visualization using add_geometry; for all other frames, use update_geometry instead
        vis.add_geometry(point_cloud)
        vis.update_geometry(point_cloud) # <----------
        vis.update_renderer()
        vis.poll_events()
        # step 5 : visualize point cloud and keep window open until right-arrow is pressed (key-code 262)
        vis.register_key_callback(262, stop_callback)
        vis.register_key_callback(32, continue_callback)
        vis.run()
        #######
        ####### ID_S1_EX2 END ####### 