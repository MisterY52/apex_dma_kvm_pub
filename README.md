#  apex_dma_kvm_pub
#####  UnknownCheats thread: https://www.unknowncheats.me/forum/apex-legends/406426-kvm-vmread-apex-esp-aimbot.html
#new features

-  Esp for player's Level
-  seerhealthESP / some parts of aimbot codes,paste from https://github.com/KrackerCo/apex_dma_kvm_pub.
-  support UTF-8 text
-  free camera
-  overlay's streamproof by setwindowdisplaffinity
-  client's streamproof,(hide the CMD window), disable/enable it by : main.cpp (void ShowWindow)
-  you can enable aimbot/esp for your teammates
-  press mouse4 to disable visible check(Aim at enemies behind smoke)
-  display legend's name (https://www.unknowncheats.me/forum/3915178-post14046.html)
-  Spectator List (Display Sepctator's Name and XP level)
# how to use
## guest
-  ctrl+h to replace "placeholder" to other contents to bypass memory detection.
-  in CustomOverlay: main.cpp
-  better use VMprotect
-  build in release x64, use custom overlay for streamproof.
## linux
 -  you could change Recoil setting strength in Game.cpp
 -  Add_off is the offset of client.exe. it would show on the window of client.exe
## Documents about Bypassing
 - https://github.com/WCharacter/RDTSC-KVM-Handler
 - https://github.com/A1exxander/KVM-Spoofing

*If you are concerned with Passthrough/bypass , ask Unknowncheats community for help*


### Changelog
#### 2023/12/20(UTC+8) 
-  Freecamera will be disable when player is not alive.
-  Spectator List
-  Fix some bugs of Legend's name ESP.
-  Added rcs setting to client