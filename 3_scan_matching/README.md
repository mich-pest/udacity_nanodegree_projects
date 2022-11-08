
## Scan Matching with ICP

![img](https://user-images.githubusercontent.com/74416077/200648479-b6daf092-980f-45a9-b875-5d8483a928d6.png)

In this project there are three major steps to perform scan matching.

First of all, make use of `cloudFiltered` and `scanCloud` functions in order to filter the point cloud using a voxel grid.
Then, *ICP matching algorithm* is used to find the pose transformation.
Find pose transform by using ICP and transform the scan so it aligns with ego's actual pose and render that scan of the environment.
In the end, we are able to transform the filtered scan using the calculated transform into a new point cloud using pcl.
