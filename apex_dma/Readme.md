#  apex_dma_kvm_pub
#####  UnknownCheats thread: https://www.unknowncheats.me/forum/apex-legends/406426-kvm-vmread-apex-esp-aimbot.html
#new features

- Xp level ESP
-  seerhealthESP/smooth aimbot from https://kgithub.com/KrackerCo/apex_dma_kvm_pub.
-  platform userid ESP,i also keep one html file,you can input uid to search player's rank,selcet legend.
> idont know why after i add rank ESP,the client will crash,but i keep void"GetJson", these code are paste from https://www.unknowncheats.me/forum/apex-legends/520061-rank-statistics-players.html,
if somebody can finish this work, +pull request please
-  support UTF-8 ESP for japanese/chinese
-   free camera
-  client's stream proof,include commandline,you can change it to view offset in client's main.cpp 
    void ShowWindow
- you can enable aimbot/esp for your teammates
# how to use
## guest
-   use vmprotect or rewrite your own version
-  release x64, use custom overlay.
## linux
 - install 'libcurl' and 'jsoncpp',if cant complie ,check the Makefile
####  Archlinux
`    sudo  pacman -S jsoncpp`
`    sudo  pacman -S curl`
 - for rank esp, you need your own apikey,reg one from apexlegendsapi
 -  what's "add_off"?  , host need it to read the setting of guest,it's in apex_dma.cpp
 
* i have no time to  teach about GPU passthrough or how to install this repo, because i only have a few time .....sorry of that*