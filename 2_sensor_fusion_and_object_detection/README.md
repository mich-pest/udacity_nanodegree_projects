# 3D_Object_Detection
Objects detection in 3D point clouds given by measurements from LiDAR and camera from the Waymo Open Dataset

<p align = "center">
  <img src = "https://user-images.githubusercontent.com/74416077/187908854-df1ae58d-2185-4acd-9963-0c0c0c4fad81.gif" width=800 ></p><p align = "center">
</p>
<!-- https://user-images.githubusercontent.com/74416077/187894624-c8d68ef3-7177-4478-aa57-2b6371975d1c.png" -->

## How to run
This project uses [Waymo Open Dataset](https://waymo.com/open/terms) bucket that can be downloaded from this [page](https://console.cloud.google.com/storage/browser/waymo_open_dataset_v_1_2_0_individual_files;tab=objects?prefix=&forceOnObjectsSortingFiltering=false). In this case specifically, the needed sequences are listed in the following: 

`training_segment-1005081002024129653_5313_150_5333_150_with_camera_labels.tfrecord` <br>
`training_segment-10072231702153043603_5725_000_5745_000_with_camera_labels.tfrecord` <br>
`training_segment-10963653239323173269_1924_000_1944_000_with_camera_labels.tfrecord` <br>

Once downloaded, each of them has to appear in `dataset` folder.
You can download from [here](https://drive.google.com/drive/folders/1IkqFGYTF6Fh_d8J3UjQOSNJ2V42UDZpO?usp=sharing) pre-computed lidar detections in order to face the same data as input. These files have to be placed in the `results` folder.
Download [resnet pretrained model](https://drive.google.com/file/d/1RcEfUIF1pzDZco8PJkZ10OL-wLL2usEj/view?usp=sharing) and store it in the `tools/objdet_models/resnet/pretrained` folder. [Darknet pretrained model](https://drive.google.com/file/d/1Pqx7sShlqKSGmvshTYbNDcUEYyZwfn3A/view?usp=sharing) is instead available here. 


## Step 1 - EKF
First of all, in `filter.py` you can find an implementation of an Extended Kalman Filter (EKF) used to track single targets using real-world lidar data. Definition of matrix F, Q for the problem as well as prediction and update functions have been implemented. Here is reported the Root Mean Square Error (RMSE) related to this execution.
<p align = "center">
  <img src = "https://user-images.githubusercontent.com/74416077/187894689-516ff652-9502-4c4a-aeaa-b857f81a98a9.png" width=800 ></p><p align = "center">
</p>

### Setup
Choose second sequence in `loop_over_dataset.py`, uncommenting the corresponding line and commenting the remaining two
```python
data_filename = training_segment-10072231702153043603_5725_000_5745_000_with_camera_labels.tfrecord
```
Assign frames number to consider
```python
show_only_frames = [150, 200]
```
Make sure to select resnet model
```python 
configs_det = det.load_configs(model_name='fpn_resnet')
```
Set this variable to define y-range limit
```python
configs_det.lim_y = [-5, 10]
```
And assign these vectors to activate tracking and visualization features
```python 
exec_detection = []
exec_tracking = ['perform_tracking']
exec_visualization = ['show_tracks']
```

## Step 2 - Track management
Track management module stored in `student/trackmanagement.py` is needed to correctly perceive several vehicles simultaneously. In particular, here track initialization and deletion but also track state and score update have been implemented. Here attached the corresponding RMSE plot for this run.
<p align = "center">
  <img src = "https://user-images.githubusercontent.com/74416077/187894702-bbaa18fd-b076-440c-b735-89932132ecbb.png" width=800 ></p><p align = "center">
</p>

### Setup
Apply these modification in `loop_over_dataset.py`: limit the number of frames considered
```python
show_only_frames = [65, 100]
```
and modify the y-range limit
```python 
configs_det.lim_y = [-5, 15]
```

## Step 3 - Data association
In order to support multi-target tracking the framework relies on a single nearest neighbor data association method, which source code is in `student/association.py`, able to couple each measurement to each track in a robust way. More specifically, in the `associate` function all Mahalanobis distances for all tracks and measurements combinations are calculated and association matrix entries are assigned, also considering gating technique to reduce complexity. In the following RMSE plot for multi-target tracking is included.
<p align = "center">
  <img src = "https://user-images.githubusercontent.com/74416077/187894713-1738627e-25c5-4681-9c1b-d1f2ff153827.png" width=800 ></p><p align = "center">
</p>

### Setup 
In `loop_over_dataset.py`, select sequence 1 (commenting the remaining ones) to consider a more complex scenario with multiple targets.
```python
data_filename = training_segment-1005081002024129653_5313_150_5333_150_with_camera_labels.tfrecord
```
Then consider all frames available
```python 
show_only_frames = [0, 200]
```
and extend the y-range to the whole one
```python 
configs_det.lim_y = [-25, 25]
```
## Step 4 - Camera-lidar sensor fusion
In this final part the sensor fusion module for camera-lidar fusion is completed. Actually, the camera measurement model is added in order to consider both source of information in `student/measurements.py`. 
More precisely, nonlinear camera measurement function h(x) has been implemented, including measurement object initialization.
However, please note that in less trivial real-world applications several data source have to be monitored by this module. Then, the final RMSE plot the three valid tracks.
<p align = "center">
  <img src = "https://user-images.githubusercontent.com/74416077/187894731-ab8f30c9-4047-47e0-b8be-deb7ce600e54.png" width=800 ></p><p align = "center">
</p>

### Setup
Same as step 3.

## Benefits and challenges of camera-lidar fusion
Of course there are theoretical and practical benefits in combining two different types of sensors in the perception layer of a self-driving vehicle.  
In the specific case the double sensor setup seems not to introduce a better performance if compared to the lidar-only scenario, but surely in quite complex use case, with tens of vehicles around the ego one, a wider field of view and a more complete data flow, collected from different sources, is a great advantage and increases the situation awareness of the autonomous agent.

Going into the wild expose the vehicle to various potentially crtitical scenarios related to weather conditions, specific kind of objects detected (e.g. plates, reflectors or any high responding material) in which one sensor, for example, could introduce some false positive measurements that could be compensated by the other one, whose precision is not affected in that scenario, through the centralized tracking management module (e.g. rapidly decreasing the track score).
Finally, considering extreme cases, the redundancy introduced with both sensors increases the overall system fault tolerance, having a positive impact on its own robustness. 

## Further improvement
- Fine tuning of parameters included in `params.py` (e.g. standard deviations for measurements, thresholds for track states) to lower RMSE value  
- Introduce a more complex data association algorithm like [Global Nearest Neighbor (GNN)](http://ecet.ecs.uni-ruse.bg/cst/docs/proceedings/S3/III-7.pdf) in step 3
- Deploy a non-linear motion model (e.g. bicycle model), more suitable for vehicle, especially in urban context, instead of the linear motion model used here for highway scenarios.



<!--In this write-up we analyze several examples of vehicles appearing in the point cloud returned by LiDAR sensor onboard the Waymo vehicle. 

<p align = "center">
  <img src = "https://user-images.githubusercontent.com/74416077/185809003-1352eca5-9052-4c07-bbc2-bd6b9ff181a3.png" width=800 ></p><p align = "center">
Point cloud
</p>

## Visibility of vehicles in the point cloud
<p align = "center">
  <img src = "https://user-images.githubusercontent.com/74416077/185808403-6d3488d1-38ea-4c12-bce2-b629f9e29476.png" width=300 ></p><p align = "center">
img #1
</p>
In image #1 includes several cars positioned behind the ego vehicle: for this reason we can see a detailed front part with decreasing precision in their representation when increasing the distance.


<p align = "center">
  <img src = "https://user-images.githubusercontent.com/74416077/185808463-0822ae74-96c1-4b7b-bf3f-a6d94d1cef94.png" width=300 ></p><p align = "center">
img #2
</p>
In #2 mirrors are visible although the low level of visibility given the far position of the object.


<p align = "center">
  <img src = "https://user-images.githubusercontent.com/74416077/185808577-c23afce6-c9d7-4718-92e5-7afe5508d13a.png" width=400 ></p><p align = "center">
img #3
</p>
In #3 these cars are located on the left side of the ego vehicle but really close to it, so they are easily distinguishable. 


<p float="left">
  <img src="https://user-images.githubusercontent.com/74416077/185808636-79f3a0d6-2f27-4f93-ab00-5ffd76c5239e.png" height="200" />
  <img src="https://user-images.githubusercontent.com/74416077/185808659-14d8656f-2b31-4a7f-b5ab-a7d0a3254cd4.png" height="200" />
</p>
<p align = "center">
img #4 and #5
</p>
#4 and #5 represent vehicles perceived on the background, main features like wheels and windows are appreciable also in this case.


<p align = "center">
  <img src = "https://user-images.githubusercontent.com/74416077/185808703-1ea278ca-b69f-40dd-aed0-056a6294fa7f.png" width=300 ></p><p align = "center">
img #6
</p>
In #6 we can see several vehicles parked on the side of the road. Again, being very close to the sensor, all their feature are perceived and accurately represented in the point cloud.  


<p align = "center">
  <img src = "https://user-images.githubusercontent.com/74416077/185808721-db667d98-1b93-4abe-97ba-b40f8465b15b.png" width=300 ></p><p align = "center">
img #7
</p>
#7 enclose a very detailed representation of vehicles that follow the Waymo one in traffic. 


<p align = "center">
  <img src = "https://user-images.githubusercontent.com/74416077/185808762-2e8da824-9562-460b-83b5-775a91e9e184.png" width=300 ></p><p align = "center">
img #8
</p>
In #8 we have cars that preceed the ego vehicle, also at a great distance, cases in which visibility degrades significantly  


<p align = "center">
  <img src = "https://user-images.githubusercontent.com/74416077/185808778-f706b952-4a7d-4b01-a8e7-7e6c407b3c7e.png" width=400 ></p><p align = "center">
img #9
</p>
In #9 another distant car is seen sideways but still recognizable


<p align = "center">
  <img src = "https://user-images.githubusercontent.com/74416077/185809764-d406f922-5c98-4457-a383-9cf0399c482c.png" width=400 ></p><p align = "center">
img #10
</p>
In #10 a heavy-duty vehicle can be seen in the background, characterized by its rear bumper and wheels.

## Main features
Main recurring features: windshields and windows (e.g. img #2, #3, #7, #9), mirrors (e.g. #2, #7), wheels (e.g. #3, #4, #9, #10 ), bumpers (e.g. #6, #10). Furthermore, looking at the range image, these features appear to be quiet evident, especially preeceding vehicles' mirrors as well as bumpers and windows, that looks visible thanks to the dark background. 
<p align = "center">
  <img src = "https://user-images.githubusercontent.com/74416077/185809491-0eac32fe-4649-40bd-b184-6928ade651e4.png" width=800 ></p><p align = "center">
Range (up) and intensity image (down) stacked vertically
</p>


## Detection performances
Finally here it is reported the final evaluation of precision and recall over the whole considered dataset (ID_S4_EX3)

<p align = "center">
  <img src = "https://user-images.githubusercontent.com/74416077/185809260-60717e07-eaf6-4f14-9dec-8b2986764ea4.png" width=800 ></p><p align = "center">
Precision, recall and errors
</p>
-->
