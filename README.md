# Stream Power Erosion

This project is an adapted version of this paper: https://hal.science/hal-04049125v1

The user paints an **uplift** map, which specifies the local elevation speed of the ground. Fluvial erosion is continuously simulated through the **Stream Power Equation** from geomorphology. The equilibrium between those two forces results in a dendritic mountainous terrain with a correct outflowing water network. Mountainous ridges appear where high uplift values have been placed, which allows to interactively author the terrain.


## Compile

Clone the repository and its submodules using:
```
git clone --recursive https://github.com/Chrissblock99/StreamPowerErosion.git
```

Can be compiled on Windows or Linux, using the CMake file. Requires cmake**3**.


## Control

**ctrl + mouse wheel** to zoom in and out.  
**ctrl + left click** to add uplift and place mountains.
