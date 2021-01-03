# cshumandetection
**Counter-Strike: Global Offensive Human Detection**  

Generates training data (csgonav, ImageGeneration) for a tensorflow object detection model (cshd.ipynb). Model inference is fed through an app that adjusts player's crosshair to the target.  

End goal: Alternative to traditional csgo cheat that uses machine learning to detect enemies instead of directly reading from the game's memory.  

Explanation of how the program works:  

CBL Generation: Potential camera locations are found by determining which spots in a grid on a 2d image representation of the map are within the map. Each camera's viewable area is determined by tracing rays from the camera to an obstacle. Bot positions are generated within the viewable area.  

Image generation: The CSGO engine is hooked and direct calls are read/written to it. Using this, the player/spectator (camera) and the bot (enemy player model) are positioned. Screenshots are taken and compressed to jpeg files, which make up the dataset. Bounding boxes of where the enemy player model is on the screen are calculated using the world2screen function.  

Machine learning: The dataset and annotations are used to train the "ssd_resnet50_v1_fpn_640x640" model. Model reaches ~0.7 loss on roughly 1% of the dataset (2.5 million images for one map). Training takes ~10 hours. Inference runs in roughly 200ms.  

App: Csgo screen output is captured and the current frame is run through the model. screen2angle is used to determine the mouse movement necessary to shift the player crosshair's to the desired location. Using the output from the model, the crosshair is moved by simulating mouse movements.  

*much of the files/data/dataset generated are not included due to the file size limits of github. A fresh run is __NECESSARY__.  

csgonav: Generates pairs of camera and bot positions (CBL) from a given nav (map) file  

externalbase: Accesses csgo memory and writes and reads from it  

ImageGeneration: Generates images from given CBL files  

ImageGenerationEx: Mouse moving utility  

inferno2map: edited map that uses only one player model  

scripts: bot config that controls bot behaviour  

tensorflow/exported-model: app that captures csgo video output, runs inference on it and moves the crosshair accordingly  

**Additional features to be added:**  

**Data generation:**  

[ ] Auto-restart hung csgo.exe

[ ] Implement bsp/mdl visibility check

[ ] Implement randomjump/crouch

[ ] Implement math world2screen

[ ] Ensure that format of all files is standard

[ ] Outsource pitch and yaw calculation

[ ] Only use one bone in read world2screen

[ ] Separate functions into separate header files

  

**Machine learning: ** 

[ ] Train mobilenet model

[ ] Use adam optimizer

[ ] Determine strength of model of far/close/smoked/mollied/flashed/jumping/crouching targets

[ ] Use tensorflow optimization library

[ ] Convert model to c++

[ ] Convert model to tflite

  

**Cheat:**  

[ ] Get lower-latency video streaming

[X] Get more accurate mouse moving function

[ ] Maximize memory used by inference model

[X] Use lower-latency sendinput methods

[ ] Implement Inter-Process Communication for streaming and sending input

[X] Implement accurate cpp mouse mover

[X] Screen2mouse for y axis

[ ] Implement reverse world2screen

[ ] Implement multithreading
