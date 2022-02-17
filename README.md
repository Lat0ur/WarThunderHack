# Prediction marker for Arcade/Realistic/Sim Air Battle
## **EAC DETECTED**. 
### Check EAC checkbox in game launcher
## *OUTDATED*

Last Update: 23.01.2021

War Thunder version: 3.51

# How To Use
1. Compile using VS2019+;
2. Inject .dll by arbitrary injector (i.e. [GHInjector](https://guidedhacking.com/resources/guided-hacking-dll-injector.4/));
3. Fix prediction marker scale using Num+/Num-.

> To uninject use *Insert*.  

> Simple hide/unhide boxes/markers by *F9*.

> Num+/Num- alse affect drawing distance.

# How to update
Update offsets in *offsets.h* file.

# Screenshots
Example 1 | Example 2
:-------------------------:|:-------------------------:
![Alt text](screenshots//2021-01-23_2-52-57.png?raw=true "Title") | ![Alt text](screenshots//2021-01-23_2-57-05.png?raw=true "Title")


# Requirements
* MinHookLib (already included: *ROOT*/Libs)
* DirectX SDK

# Bugs
* Fake boxes without players. Your client should get get info about players coordinates. I'll happen when someone of ally met an enemy.

# See also
* [D3D11Hook by Rebzzel](https://github.com/Rebzzel/Universal-D3D11-Hook/tree/master)
* [MinHook](https://github.com/TsudaKageyu/minhook)
