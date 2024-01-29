#  apex_dma_kvm_pub
#####  UnknownCheats thread: https://www.unknowncheats.me/forum/apex-legends/406426-kvm-vmread-apex-esp-aimbot.html

#About this project
-  codes in guest are hash(to avoid anticheat's check), if you want to read it, switch to the code-view branch
-  update rarely 

#Developing Features:

Means there's function to display/print them, but not in ESP.

-  Detect player's rank/rankDiv/rankPosition
   results will be in command line , i haven't add it to ESP yet.
   Click the Refresh first when the match begin. sometimes you have to wait for couple seconds, you can click the "Render" CheckBox to render playerlist to check
   if it's wrong, but remember to turn it off because it will replace the spectator list.
   * You will need a api_key to use it  https://apex-legends-api.readthedocs.io/en/latest/index.html
-  Check Pros : to prevent you from killing them and got banned
   CAUTION: the userid of pros only includes few of them.It's not so accurate.

#new features

-  Esp for player's Level
-  seerhealthESP / some parts of aimbot codes,paste from https://github.com/KrackerCo/apex_dma_kvm_pub.
-  support UTF-8 text
-  free camera
-   overlay's streamproof by setwindowdisplaffinity
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
#### 2024/1/29(UTC+8) 
-  Pros check for ESP
-  Function to get player's rank
-  Modified the ADD_OFF check part. 
   because sometimes it cause the client crush on my PC :(