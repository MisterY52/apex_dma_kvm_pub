#  apex_dma_kvm_pub
#####  UnknownCheats thread: https://www.unknowncheats.me/forum/apex-legends/406426-kvm-vmread-apex-esp-aimbot.html
#new features

-  Xp Level ESP
-  seerhealthESP/some aimbot code,paste from https://github.com/KrackerCo/apex_dma_kvm_pub.
-  platform userid ESP,use it to search player's rank'.
> idont know why after add rank ESP,the client will crash,but i keep void"GetJson", these code are paste from https://www.unknowncheats.me/forum/apex-legends/520061-rank-statistics-players.html,
   if someone can finish it, + pull request please
-  support UTF-8 ESP for japanese and chinese
-  free camera
-  overlay's steam proof by setwindowdisplaffinity
-  client's stream proof,(hide commandline),check it  in client : main.cpp (void ShowWindow)
- you can enable aimbot/esp for your teammates
- press mouse4 to disable visible check
# how to use
## guest
-  to bypass EAC detection,use ctrl+h to replace "serial-string" to anything you want.
-  in  Guest: main.cpp,overlay.cpp,overlay.h
-  in CustomOverlay: main.cpp
-  better with VMprotect
-  release x64, use custom overlay.
## linux
 - change no-recoil strength in Game.cpp
 - for rank esp, you need your own apikey,reg one from apexlegendsapi, use index.html ,input userid to search player's rank
 -  what's "add_off"?  ,it's in apex_dma.cpp , will display in client's commandline
* i have no time to  teach about GPU passthrough or how to install this repo, because i only have a few time .....sorry of that*
