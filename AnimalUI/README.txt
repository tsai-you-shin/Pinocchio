# README for Animal UI

## Building
- Please first follow the building instruction of original Pinocchio library to setup the environment. 
  The guildeline is in the BUILDING section under ./README.text.
- For AnimalUI, just run make (under LINUX and other UNIX) to build the file. 

## Running AnimalUI
- The related input files (e.g. XXX.obj XXX.off) are put under ./AnimalUI/data directory.
- First try running with the given object:
```
./animalUI ./data/bear_original_shrinked_7removed.off -skel ./data/pinocchino_skeleton.txt

```
After about 10 seconds, a window would opened and you will see a deformed bear in it.
if you want to test different deformation you can edit the function $computeTransforms()$ in ./AnimalUI/defmesh.cpp.

## Output Files
In the window, use the left mouse button to pan, wheel to zoom, and right mouse button to rotate the view. 
Pressing S toggles the skeleton display, F toggles flat shading, G toggles ground display, and T resets the view.

When AnimalUI runs, it also outputs the embedded skeleton to skeleton.out and the attachment (bone weights) to attachment.out.
Each line in skeleton.out corresponds to a joint and is of the form: "idx x y z prev" where prev is the index of the previous joint.
Each line in attachment.out corresponds to a mesh vertex and consists of bone weights for each bone in the order of skeleton joints.
