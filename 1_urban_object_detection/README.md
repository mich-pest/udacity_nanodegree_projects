# Object detection in an Urban Environment - Project Writeup
## Project overview
<!-- Project overview: This section should contain a brief description of the project and what we are trying to achieve. Why is object detection such an important component of self-driving car systems? -->
The perception layer, and specifically object detections algorithms, are massively deployed in the development of autonomous cars and they are extremely important. Indeed, their inference define the foundational knowledge on which the entire architecture will reason upon and then make decision that can really affect the vehicle performance as well as the level of safety and the quality of user experience offered to the passengers.
In this project we will see how to design an object detection layer using a pretrained Neural Network (NN), relying on [TensorFlow Object Detection API](https://github.com/tensorflow/models/tree/master/research/object_detection) and leveraging on data from the [Waymo Open dataset](https://waymo.com/open/).

<p align="center">
  <img src="https://user-images.githubusercontent.com/74416077/182271939-76dd8a82-f360-4841-af3e-6e2da59b1052.png" height="400" /> 
  

<!-- Set up: This section should contain a brief description of the steps to follow to run the code for this repository. -->
## Setup
First of all, be sure to have all dependencies needed for running the project correctly like [matplotlib](https://matplotlib.org/), [jupyter](https://jupyter.org/), [TensorFlow](https://www.tensorflow.org/resources/learn-ml?gclid=Cj0KCQjw852XBhC6ARIsAJsFPN2DWdACH52AaQ_ait4nFosEGm_qHqhAJgaJNYbgMT2We22C9E5GWksaAjksEALw_wcB), [TensorFlow Object Detection API](https://github.com/tensorflow/models/blob/master/research/object_detection/g3doc/tf2.md) (doc available [here](https://tensorflow-object-detection-api-tutorial.readthedocs.io/en/latest/install.html#tensorflow-object-detection-api-installation)) installed on your machine: all of them can be found in the `requirements.txt`.

After that, go to the desired path and download this repository with:
```bash
git clone https://github.com/mich-pest/Urban_Object_Detection.git
```
Once all files are installed, you can inspect the code in the jupyter notebooks simply executing:
```bash
cd Urban_Object_Detection
./launch_jupyter.sh
```

### Train a pretrained model
In this project both SSD ResNet50 640x640 and SSD ResNet101 640x640 were trained.
In general, if you want to train a pre-trained model (e.g. ssd_resnet50_v1_fpn_640x640_coco17_tpu-8), download it in `experiments/pretrained_model` folder using this command: 
```bash
cd experiments/pretrained_model

wget http://download.tensorflow.org/models/object_detection/tf2/20200711/ssd_resnet50_v1_fpn_640x640_coco17_tpu-8.tar.gz

tar -xvzf ssd_resnet50_v1_fpn_640x640_coco17_tpu-8.tar.gz

rm -rf ssd_resnet50_v1_fpn_640x640_coco17_tpu-8.tar.gz
```
For other model names simply substitute `ssd_resnet50_v1_fpn_640x640_coco17_tpu-8` with another model listed [here](https://github.com/tensorflow/models/tree/master/research/object_detection/configs/tf2) (modifying extension `.config` with `.tar.gz`).
Then, go to the root folder of the project and generate a new config file for the new model using the dedicated script (again substitute the name for your model name):
```bash
cd ../../
python edit_config.py --train_dir /data/train/ --eval_dir data/val/ --batch_size 2 --checkpoint experiments/pretrained_model/ssd_resnet50_v1_fpn_640x640_coco17_tpu-8/checkpoint/ckpt-0 --label_map experiments/label_map.pbtxt
```
Finally move the new config file to the `experiments/reference/` directory with:
```bash
mv pipeline_new.config experiments/reference/
```

In order to launch the training process, type the following command in the root folder of this repository:
```bash
python experiments/model_main_tf2.py --model_dir=experiments/reference/ --pipeline_config_path=experiments/reference/pipeline_new.config
```
During training it is possible to activate a [TensorBoard](https://www.tensorflow.org/tensorboard) panel to monitor the loss, the provided input and further useful information with:
```bash
python -m tensorboard.main --logdir experiments/reference/
```
Then, you can stop training and launch the evaluation process to verify your model's performance on unseen data with:
```bash
python experiments/model_main_tf2.py --model_dir=experiments/reference/ --pipeline_config_path=experiments/reference/pipeline_new.config --checkpoint_dir=experiments/reference/
```
Pay attention to not overlap these two processes: this could lead to some errors.
Now, since the evaluation is performed for 1 epoch only, a blue dot should appear in the TensorBoard interface.

IMPORTANT: You can delete checkpoints file, auto-generated during the process, but be sure to keep `tf.events` files in `train` and `eval` folder.

## Dataset

<!-- Dataset
 - Dataset Analysis: This section should contain a quantitative and qualitative description of the dataset. It should include images, charts, and other visualizations. -->
The [Waymo Open dataset](https://waymo.com/open/) is one of the largest and most diverse autonomous driving datasets ever released. 
Data can be retrieved from Google Cloud Bucket at this [link](https://console.cloud.google.com/storage/browser/waymo_open_dataset_v_1_2_0_individual_files). The classes of interest are 3: vehicles, pedestrians and cyclists.
In this wide collection of labeled images are depicted a lot of different scenarios. This data variety is very important since it will directly determine the capabilities that our final model will be able to exhibit. As can be seen running `Exploratory Data Analysis.ipynb`, data represents urban scenes in pretty much all light and traffic conditions, with low and high density of item detected by the camera, including several blurred images. 

<p align="center">
  <img src="https://user-images.githubusercontent.com/74416077/182249404-b2528d09-f924-4f52-a26c-aaf9e9001eab.png" height="185" />
  <img src="https://user-images.githubusercontent.com/74416077/182249417-4cdc07cc-2c2f-4803-9f78-51c6deaf5b82.png" height="185" /> 
  <!--<img src="https://user-images.githubusercontent.com/74416077/182249428-99ab932a-0f95-4498-b533-f75d58c5c9b6.png" height="185" /> -->
 <img src="https://user-images.githubusercontent.com/74416077/182249598-087778d7-d03e-4161-bd1f-fa1a6fefff66.png" height="185" />
 <img src="https://user-images.githubusercontent.com/74416077/182250796-fe0d3a7f-4fda-44c0-94ea-0a6ef269a89e.png" height="185" />
</p>

 <!-- Cross-validation: This section should detail the cross-validation strategy and justify your approach. -->
 We consider 100 tfrecord items in total: 87 for training (`train` folder), 10 for validation (`val` folder), 3 for testing (`test` folder). This data split as well as the conversion from Waymo Tf record to Tf Object Detection API format was already provided. 
 Indeed, the majority of the dataset is used to train the model and adjust weights in the NN, while only a small amount of them is used for validation, to measure model performance, and testing. 
 ### Classes distribution
As it is possible to verify with the following image reporting classes distribution, an high number of images includes the presence of vehicles (class 1) which is good considering that we are moving on the street and we very likely expect to see other cars around the ego vehicle. Pedestrians (class 2) also appear a reasonable amount of times: this seems enough since, for example, they do not appear in highway scenarios. Compared to the previous ones, cyclists (class 4) are definitely more rarely detected, given the lower probability to find them in cities (e.g. depending on wheater). This proportion between classes looks well suited for the prefixed goal of urban roads context. However, if the final application has to be deployed in another kind of specific environment (e.g. closed to traffic area), these proportions have to be modified accordingly to adjust model's performances.
 
<p align="center">
<img src="https://user-images.githubusercontent.com/74416077/183241783-425584e8-f2fc-4d05-8eac-1dc133cb6138.png" height="250" />
</p>

<!-- Training
 - Reference experiment: This section should detail the results of the reference experiment. It should include training metrics, Tensorboard charts, and a detailed explanation of the algorithm's performance.
 -->
## Training
### Reference Experiment
As said before, I focused on SSD Resnet 50 640x640 and SSD Resnet 101 640x640 models but many more are included in the [TensorFlow 2 Detection Model Zoo](https://github.com/tensorflow/models/blob/master/research/object_detection/g3doc/tf2_detection_zoo.md), a collection of models pre-trained on the [COCO 2017 dataset](https://cocodataset.org/#home). Since for each model the API configuration file has to be modified accordingly, you can find a proposed `.config` for each of them at [this page](https://github.com/tensorflow/models/tree/master/research/object_detection/configs/tf2). In experiments/reference folder 3 files of this kind are provided: `pipeline101basic_new.config`, a basic configuration (without data augmentation) for training the ResNet101 model, `pipeline101_new.config`, a complete configuration for training the ResNet101 model, `pipeline_new.config`, a complete configuration for training the ResNet50 model. 

In this case the training process involves the Resnet 50 and it is stopped after approximatively 1000 steps for memory reasons.

<p align="center">
  <img src="https://user-images.githubusercontent.com/74416077/182268171-613e9ca3-9452-4a23-ad9c-7a462912f9a6.png" width="800" />
  <img src="https://user-images.githubusercontent.com/74416077/182268172-a3b65820-b8fe-42f0-9baa-76055316cc1b.png" width="800" />
</p>

The results in terms of loss shows, as expected, a general decreasing trend while the blue dot, representing the validation loss, seems to be quite close to the orange curve representing instead the training loss, probably indicating an overfitting behaviour.

Just to verify, a similar process is carried out with the ResNet 101, leading to similar results. 
<p align="center">
  <img src="https://user-images.githubusercontent.com/74416077/182271519-3b017cbf-3505-4595-a518-2045845f4636.png" height="100" /> 
 <img src="https://user-images.githubusercontent.com/74416077/182271518-b65b5146-0d6c-42c1-b8e6-676a7da43269.png" height="100" />
</p>

<!-- Improve on the reference: This section should highlight the different strategies you adopted to improve your model. It should contain relevant figures and details of your findings. -->
### Improvement
In order to improve the performances of the model, some data augmentations techniques offered by the TensorFlow Object Detection API in the [proto file](https://github.com/tensorflow/models/blob/master/research/object_detection/protos/preprocessor.proto) have been deployed in this project. Their effect can be easily visualized in the `Explore augmentations.ipynb`.
The used methods allows for better generalization in the urban context since they simulate a potentially real situation that does not appear in the original dataset. 
For example, random_crop_image augmentation is applied in order to emulate random occlusions in the field of view of the camera (e.g. a building, a tree, another vehicle). 
Similarly, random_adjust_saturation and random_adjust_brightness are useful to provide unusual combination of light and focus conditions in the perceived data, possibly due to critical wheather conditions in which the car has to move (e.g. a storm) or even some hardware damage (e.g. camera failure).
This gives to the model the possiblity to learn how to perform well also in these critical scenarios, ensuring more robust behaviour in the testing phase.
Finally, random_horizontal_flip is a simple solution to enlarge the dataset obtaining similar images with respect to the original ones with different objects arrangement in the scene. Some examples of these transformations are reported in the following.
<p align="center">
  <img src="https://user-images.githubusercontent.com/74416077/182249606-93d01f91-9e56-4a32-a64d-e02b27cb5da6.png" height="150" />
  <img src="https://user-images.githubusercontent.com/74416077/182249611-499e5174-4a8d-454d-95a0-c7e8c102bdf3.png" height="150" /> 
  <img src="https://user-images.githubusercontent.com/74416077/182249615-41883619-6b9e-4992-afd3-530fee23ae40.png" height="150" />
  <img src="https://user-images.githubusercontent.com/74416077/182251600-4cc71dc4-8b4e-439f-ab9d-4bcff4fddf09.png" height="150" />
  <img src="https://user-images.githubusercontent.com/74416077/182251711-25ec22a1-074b-468c-84c0-8ff5fddbe0fb.png" height="150" />
</p>
 
