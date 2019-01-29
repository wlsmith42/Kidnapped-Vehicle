# **Kidnapped Vehicle Project**
---

[![Udacity - Self-Driving Car NanoDegree](https://s3.amazonaws.com/udacity-sdc/github/shield-carnd.svg)](http://www.udacity.com/drive)

The goals / steps of this project are the following:

* Implement a particle filter in C++.
* Use a multivariate gaussian distribution and a resampling wheel to caluclate & update weights of individual particles.
* Minimize the filters error to accurately locate a vehicle's position and heading based on current sensor measurements and a predefined map of landmarks.


[//]: # (Image References)

[image1]: ./images/simulator.gif "simulator"

---

# Overview
This repository contains the final project for the Localization course in Udacity's Self-Driving Car Nanodegree.

The directory structure of this repository is as follows:

```
root
|   build.sh
|   clean.sh
|   CMakeLists.txt
|   README.md
|   run.sh
|
|___data
|   |   
|   |   map_data.txt
|   
|   
|___src
    |   helper_functions.h
    |   main.cpp
    |   map.h
    |   particle_filter.cpp
    |   particle_filter.h
```


## Project Introduction
Your robot has been kidnapped and transported to a new location! Luckily it has a map of this location, a (noisy) GPS estimate of its initial location, and lots of (noisy) sensor and control data.

This project implements a 2 dimensional particle filter in C++. The particle filter is given a map and some initial localization information (analogous to what a GPS would provide). At each time step the filter will also get observation and control data.

Once the particle filter is run on the simulator (see below for simulator link), the results are shown in the gif below:

![alt text][image1]

For a more detailed explaination of how the particle filter works step-by-step, view the comments in `src/particle_filter.cpp`

## Running the Code
This project involves the Term 2 Simulator which can be downloaded [here](https://github.com/udacity/self-driving-car-sim/releases)



Some scripts have been included to streamline the process of running this project, these can be leveraged by executing the following in the top directory of the project:

1. ./clean.sh
2. ./build.sh
3. ./run.sh


## Inputs to the Particle Filter
You can find the inputs to the particle filter in the `data` directory.

#### The Map*
`map_data.txt` includes the position of landmarks (in meters) on an arbitrary Cartesian coordinate system. Each row has three columns
1. x position
2. y position
3. landmark id

### All other data the simulator provides, such as observations and controls.

> * Map data provided by 3D Mapping Solutions GmbH.

