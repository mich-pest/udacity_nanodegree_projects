# Control_Trajectory_Tracking_project

## How to run 
If you are using the Udacity workspace some specific errors can occur. 

First of all clone this repository
```bash
git clone https://github.com/udacity/nd013-c6-control-starter.git
```
Enable super user mode
```bash
su - student 
```
It can happen that this will say permission denied, ignore and continue the procedure.
Then reach this location in the filesystem and starts the CARLA simulator
```bash
cd /opt/carla-simulator/
SDL_VIDEODRIVER=offscreen ./CarlaUE4.sh -opengl
```
Then in another terminal run these commands
```bash
cd nd013-c6-control-starter/project
./install-ubuntu.sh && cd pid_controller/ && rm -rf rpclib && git clone https://github.com/rpclib/rpclib.git
cmake .
make
```
Then come back to the `project` folder and run the main program
```bash
cd nd013-c6-control-starter/project
./run_main_pid.sh 
./run_main_pid.sh
```
In case of errors, try again this command (press ctrl + C before) since it could silently fail.

If error bind is already in use, or address already being used
```bash
ps -aux | grep carla
kill id
```

## Results and explanations
### Parameter tuning
The value choosen for the two pid initialization are here reported.
```cpp
pid_throttle.Init(0.2,0.001,0.06,1.0,-1.0);
pid_steer.Init(0.3,0.001,0.78,1.2,-1.2);
```

![plot](https://user-images.githubusercontent.com/74416077/198833701-099053fa-9c5f-4392-adf5-868417ada282.png)
![plot](https://user-images.githubusercontent.com/74416077/198833698-64da39bb-6482-44b9-a5bd-aa5d0892d454.png)

The pictures represent the error made by the car (blue line) with respect to the given trajectory.
In each figure respectively, you can find the steering output (orange line) and the throttle/brake output (green/orange lines) provided by the controller.
On x axis, the iteration number is reported, meaning a sequence of sampled time instants in simulation. 
In the specific run associated to these plots, the mean error for throttle is about 1 and the mean steering error approximates zero. This means that, at least in steer command case, the controller reacts rapidly to the rapid growth of error, thanks to the high weight given to the derivative part in that case.
As you can see from the plot, the steering output tends to overlap with the error graph, often exceeding its absolute value. In throttle case the control is not excellent but at least it does not require the brake intervention due to a too high error value. It could be the cause for increasing the integral coefficient for the throttle but this approach could introduce instability and, for this reason, this trade-off is preferred.   

### What is the effect of the PID according to the plots, how each part of the PID affects the control command?
The proportional part generates an adjustment that increased directly with the measure of the error and so it is able to converge but it can produce offset, possibly due to disturbances. 
The integral part instead, which contribution is depends on the accumulated error over time, is able to reduce this offset, leading to zero error, meaning that the control variable (e.g. velocity of the car) coincide with the desired value. Finally, with the derivative part, the correction command is based on the rate of change of the error, how fast the error variation is: this actually solve the problem of compensate the errors, not depending on how fast the error occurrs.

### How would you design a way to automatically tune the PID parameters?
I would implement a twiddle mechanism to fine tune the PID parameters. To do this, the error computed at each step in the simulation is needed. A new script has to be design in order to run the simulation several time before the real test, in such a way to obtain the error done for each value assigned to a specific parameter (with some termination condition for the execution - e.g. detection of collision with obstacles/walls).
However, I would start assigning a constant value to the velocity and start tuning the steering pid parameters first.  
The error assigned in the the UpdateError function can be used to increase or decrease the gains obtaining an overall improved performance (e.g. smoother motion). 

### PID controller is a model free controller, i.e. it does not use a model of the car. Could you explain the pros and cons of this type of controller?
Surely this kind of controllers are:
* very easy and fast fast to develop and deploy in practice since the logic on which they are based is relatively simple
* there are very well known algorithms (e.g. twiddle) to tune it in a short time, reaching rapidly the best performance they can deliver

However, including some information about the vehicle model in the control policy could be helpful to

* increase the general robustness of the system, especially when accelerations increase a lot
* depending on the level of accuracy of the model, they can ensure an increased smoothness also in complex scenarios.

