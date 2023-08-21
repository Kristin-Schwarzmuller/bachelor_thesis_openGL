# Image Generator

## Table of Contents
- [Spherical Coordinate System](#Spherical)
- [Scenery](#Scenery)
  - [Models](#models)
  - [Light](#light)
  - [Camera](#light)
- [Image Types](#Image)
- [Code Description](#Code)
- [Autopilot](#Autopilot)

This OpenGL program is designed to automatically generate and save images. A model is displayed at the origin, and the camera and light positions change throughout the different images, always pointing to the origin. The position of the camera and the light was measured in spherical coordinates. In total, five parameters were changed during the image generation: azimuth and elevation of the light and the camera, and the model at the center of the scene.

<div id='Spherical'/>
  
## Spherical Coordinate System 

<img src="https://github.com/Kristin-Schwarzmuller/thesis_openGL/blob/master/example_images/spericalCoo.png" 
alt="Spherical Coordinate System" 
width="400"/>

<div id='Scenery'/>
  
## Scenery

### Models
Seven 3D models were illuminated: the Stanford Buddha and bunny as well as a cube, sphere, cone, cylinder, and torus.

Examples, all with a camera position with cam_azimuth = 0°, cam_elevation = 30°, and light position light_azimuth = 315° and light_elevation = 45°:

![RGB image of a Buddha](https://github.com/MolineraNegra/thesis_openGL/blob/master/example_images/rgb/buddha00002438-0-30-315-45.png)
![RGB image of a Bunny](https://github.com/MolineraNegra/thesis_openGL/blob/master/example_images/rgb/bunny00043910-0-30-315-45.png)
![RGB image of a Ball](https://github.com/MolineraNegra/thesis_openGL/blob/master/example_images/rgb/ball00106118-0-30-315-45.png)
![RGB image of a Cone](https://github.com/MolineraNegra/thesis_openGL/blob/master/example_images/rgb/cone00095750-0-30-315-45.png)

A larger collection of the generated images can be found [here](https://github.com/Kristin-Schwarzmuller/thesis_openGL/blob/master/201129Collection_201129_2031_final.pdf).

### Light
Azimuth was modified in 5° steps from 0° to 355°, to distribute the light samples in a full circle around the center object. The elevation was raised in 5° steps from 5° to 90°. The range of the elevation does not start with zero because in that case, the light ray would be partly covered by the base surface. The light source is directional, the distance to the origin was always kept constant and amounted to 200 units.
![Movement of light gif](https://github.com/Kristin-Schwarzmuller/thesis_openGL/blob/master/example_images/light.gif)

### Camera
The elevation was varied in the same way as for the light. The rotation around the model was achieved by rotating the model itself in the opposite direction and holding the camera in position. By keeping the camera constant, the relative light thus always remained just the absolute value, so no further calculation is needed. Otherwise, the difference between azimuth values of the light and cam would have to be calculated to get the light position in relation to the cam. The step size of azimuth was 45°, and for elevation, it was 30°. The range for elevation starts from a shallow 5° to have a better view of the base surface and the shadow on it, whereby the further steps were defined as if it would start at zero, so the next step was 30. The azimuth ranges from 0° to 315°, if the model is not symmetrical, like the Buddha and the bunny. By rotating the cube by 90° and more, identical images are received, so its maximal rotation is 45°. Due to the axial symmetry of the torus, images are captured from 0°, 45°, 90°, and 135°. The sphere, the cone, and the cylinder are not rotated because from all sides the same images are generated. The distance from the origin to the camera is 120 units.

![Movement of camera gif](https://github.com/Kristin-Schwarzmuller/thesis_openGL/blob/master/example_images/camera.gif)

<div id='Image'/>
  
## Image Types

In addition to common red, green, and blue (RGB) channel images (left), depth images (second left), RGB-Depth images (middle), shadow candidate images (second right), and normal images (right) are generated and saved.

![RGB image](https://github.com/MolineraNegra/thesis_openGL/blob/master/example_images/types/R-1-bunny-0-5-290-35.png)
![Depth image](https://github.com/MolineraNegra/thesis_openGL/blob/master/example_images/types/D-1-bunny-0-5-290-35.png)
![RGB-D image](https://github.com/MolineraNegra/thesis_openGL/blob/master/example_images/types/RD-1-bunny-0-5-290-35.png)
![Shadow candidate image](https://github.com/MolineraNegra/thesis_openGL/blob/master/example_images/types/S-bunny00042522-0-5-290-35.png)
![Normals image](https://github.com/MolineraNegra/thesis_openGL/blob/master/example_images/types/N1-bunny-0-5-290-35.png)

<div id='Code'/>
  
## Code Description

### main

The main class creates a CGRenderer object in which the majority of the program is executed.

### CGRenderer

The principal task of the CGRenderer class is to render into two framebuffers: the default framebuffer to display the scene on the screen and an image buffer to generate textures containing the information of the scene for the image storage. A third framebuffer is used to render the shadow map into a separate texture used to render the shadows in the scene. After the setup, the CGRenderer calls the functions render() and update() alternately.

In render(), three functions are called:
- shadowmap_pass(): prepares the shadow calculation
- final_pass(): renders and stores the images
- canvas_pass(): shows the RGB image on the screen

update() asks the Autopilot class to step() through all combinations of the parameters. If not all images have been created yet, it demands the next parameters through getValues(). The CGRenderer updates the scene by repositioning the light source, elevating the camera, turning the model, choosing the model, and refreshing the screenshot names. Subsequently, render() is called again.

#### Automatic Adjustment of the Shadow Map Projection

The use of the resolution of the shadow map should be as effective as possible. To increase the quality of the shadow, a high resolution of 4096x4096 pixels was used for the shadow map. But in this experimental setup, the light and camera settings change continuously on a large scale. Therefore, it is necessary to adjust the shadow map projection matrix dynamically for each frame. The region of interest for the shadow map projection is just where the model is located. To get the minimal and maximal values of the x, y, and z values of the model, the so-called bounding box is spanned around it. It is defined as wide as necessary to cover all vertices of the model but as tight as possible. The bounding box is calculated for every model individually.

To obtain the coordinates of the bounding box from the view of the light, they are transformed from world space into light view space by multiplication with the model view matrix of the light source, called shadow_view_model. By multiplying the results with a standardized orthogonal matrix, the values are projected onto the light’s clipping plane.

Now, by determining the minimal and maximal x and y values, the optimal extents of the two-dimensional shadow map projection are obtained. By projecting an object onto a plane, the z value determines the depth that is clipped off and no more of interest.

![Shadow map projection algorithm image](https://github.com/MolineraNegra/thesis_openGL/blob/master/example_images/Shadowmap_projection.jpeg)

<div id='Autopilot'/>

### Autopilot

Before the CGRenderer can use the functions of the Autopilot, it needs to be initialized by passing two vectors and two floats. The first vector encompasses the maximum azimuth of the camera that each model will be rotated without receiving identical images. The second vector contains the names of the models (Buddha, bunny, box, etc.) necessary for labeling. The two float values describe the distance from the origin to the light source and from the origin to the camera and are needed to calculate Cartesian coordinate positions to place the camera and the light into world space.

During the setup, a new folder with a timestamp and subfolders is created to store the generated images organized by model and the image type (RGB, RGB-D, normals).

The core functionality is the step() function that gets called by the CGRenderer class. Within the step() function, the Autopilot ticks one step further through all linear combinations of values that must be executed to get all different variations of images. First, it tries to increase the elevation of the light. If the maximum value of 90° is reached, it will increase the azimuth of the light. If the light has already rotated 355° around the model, the Autopilot will raise the elevation of the camera and reset the angles of the light. If the limit of 90° is reached, it tries to increase the azimuth of the camera. The camera is not moved horizontally, but the model itself is turned in the opposite direction. Depending on the model’s symmetry, the maximal value of the camera's azimuth ranges from 0° to 315°. If all values of the light and camera have been processed, a new model is loaded, the angles of the light and camera movement are reset to their starting values, and the procedure is performed again. If all combinations of all angles and models have been calculated and rendered, the returned values to the CGRenderer are no longer updated by the Autopilot.

The CGRenderer calls a second function getValues() from the Autopilot class to receive the current values, to use them in its update and render functions. The Autopilot returns the values of light and camera position, the model rotation, ID of the model, and the image paths for storage. The light and camera positions are not returned in spherical but in Cartesian coordinates to be able to position them in world space. The image names are composed of a counter that runs from 0 to 129599, the model name, and the current light and camera settings. Finally, the Autopilot stores all computed values into a CSV file to label the data set.
