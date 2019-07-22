## PointCloudia


PointCloudia is a tool for selecting and exporting a subset of points within a point cloud, possibly for the purpose of labeling the point cloud.

For now, the program is primarily meant to be used with LiDAR point clouds.

### Dependencies

- Unreal Engine (You need to have plugins in your 4_19/Engine/Plugins folder):
    - Version 4.19
    - [UE4.19 PointCloudPlugin](https://forums.unrealengine.com/community/community-content-tools-and-tutorials/1430363-point-cloud-plugin)
    - [UE4.19 Rama's VictoryPlugin](https://forums.unrealengine.com/development-discussion/blueprint-visual-scripting/4014-39-rama-s-extra-blueprint-nodes-for-you-as-a-plugin-no-c-required?3851-(39)-Rama-s-Extra-Blueprint-Nodes-for-You-as-a-Plugin-No-C-Required!=)
- las2txt
    - Install LASTools directly from their github repository. Use the las2txt tools they have.
- [lidar-orto-photo project by Slemenik](https://github.com/slemenik/lidar-orto-photo)

### Install

- Generate the source files by right clicking on the ```.uproject``` file and clicking Generate Source Files.
- Open the ```.sln``` in Visual Studio 2017

### Prepare

#### Prepare your LAS point cloud
    - Download and set up the ```LazPreprocessor``` repository in the ```FloatingObjectSegmentation``` organization.
    - Follow the instructions given in the readme of the repository to acquire all files needed for visualizing.
#### Configure 
- ```PointCloudRenderingComponent.h``` to use the above files for the point cloud file, class file and floating object segment file.

### Controls

- **WASDQE**: move your pawn around.

#### Selection box
- **Left mouse click**: Spawns a new selection box. If a selection box is spawned, then this will finish the selection and export the file.
- **UIOJKL**: move the selection box around.
- **1**: Switch to translation mode.
- **2**: Switch to rotation mode.
- **3**: Switch to scaling mode.

#### Labeling
- **V**: move to next candidate
- **6**: Label current candidate floating
- **7**: Label current candidate likely floating
- **8**: Label current candidate maybe floating
- **9**: Label current candidate not floating
n
#### View mode
- **B**: Change color mode
- **N**: Change filter mode
- **M**: Change currently viewed RBNN value

#### Augmentation controls
Above you is a pointer that represents the direction of the airplane. It determines which way the plane is flying while scanning the augmentables with the RIEGL scanner.
- **U** - rotate airplane by yaw left
- **O** - rotate airplane by yaw right
- **SPACE BAR** - start the augmentation

### Outputs

#### Label files

- save format

```
[label]\n
[RBNN radius]\n
[\n separated indices of points in original cloud]
```
- **$label** = <int> {0, 1, 2, 3} -> 0=Floating, 1=LikelyFloating, 2=MaybeFloating, 3=NotFloating
- **$RBNN radius** = <int> - radius in meters

- path to save:
    - $DIRECTORY(POINT_CLOUD_FILE)/labels/<POINT_CLOUD_FILE_NAME>-<GUID>.txt
    
#### Augmentation files

### Programming trace
Idea and program, dependency trace is on this [link](https://docs.google.com/document/d/1yHzkerjFujqTkWjpkrrbrEc0v5QezEtqUxGGcvH7Hhw/edit?usp=sharing)