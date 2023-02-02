# VRExample
Bunch of basic VR functionality stuff made from the ground up (Singleplayer and MP)

*Gameplay videos:*
General:  https://youtu.be/PYTSOalylf8
Fullbody multiplayer with Vive Trackers: https://youtu.be/Z5rUVazP51w

*Project snippets made with those plugins or their parts:*
- https://youtu.be/L5hir20Se20 (shipped, cannot be checked up remotely unfortunately)
- https://youtu.be/VSTlapUz1u4 (was never released but is production ready)
- https://youtu.be/5jgDOPOuyro (never left prototype phase)
- https://youtu.be/fjtwR_1W2WQ (first VR prototype a few years back, most things were reworked and improved and became VRBase plugin. Hands with physics actually havent been reworked for this project, havent had time to do it)

*General overview:*
Combines work from multiple plugins, each one extends the functionaly of a former (Regular VR plugin -> Multiplayer VR -> Fullbody VR)

*Regular VR plugin Overview (plugin name - VRBase)*
Consists of 2 levels: general singleplayer VR and level for Input Test (everything supports all PCVR headsets)

Movement and rotation:
- multiple teleport types (arc, straight line, rotatable arc - default)
- snap turn
Interactable objects:
- easily overridable player input on hover, pick up, etc
- grab, drop, pick up from other hand, interact from the distance (UI and regular level actors)
- up to 2 hands interaction support on the same object
- pickables objects with distance from the hand priority
- distance parameter for interaction for each individial actor
- everything is easily extandable and overridable

*Multiplayer VR plugin Overview (plugin name - VRMultiplayer)*
- adds support for VR and NonVR players in multiplayer
- new VR locomotion as is regular NonVR, though teleport feature technically supports multiplayer
- current multiplayer made using OnlineSubsystems. Was tested with Steam OSS and kind of worked on EOS too (voice chat problems). Should also work with Oculus OSS.

*Fullbody VR plugin Overview (plugin name - VRFullBody)*
- Adds 3Dof pawn (3 degrees of freedom - HMD + 2 hands) pawn with and up to 8 Vive Trackers
- 3Dof Pawn is very WIP, with good knowledge in animation and IK much needed
- Tested in a multiplayer game with 3 players (NonVR + 3DoF VR + Trackers VR), with players more then 850km apart
- Was not tested on UE5.1 so might not work. Video with trackers calibration and multiplayer using Steam was shown above 

*Known issues and WIP things:*
 - After upgrading to 5.1 something weird happened to Slate focus so VR keyboard does not work. Also 3DoF fullbody started to twitch a lot more as seen on a video. Was  
 - VR Full body is work in progress ... (downgrade to 5.0 for SteamVR and 3dof animations needs an improvement especially when player bends or knees)