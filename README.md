# apex_dma_kvm_pub 3.0
 Apex Legends QEMU/KVM hack memflow

UnknownCheats thread: https://www.unknowncheats.me/forum/apex-legends/406426-kvm-vmread-apex-esp-aimbot.html

Bone IDs reference: https://www.unknowncheats.me/wiki/Apex_Legends_Bones_and_Hitboxes

Game version (Steam & origin): v3.0.1.29

That's made to work on QEMU/KVM, so we dont talk about Linux Host, Windows VM, passthrough and detection configuration

- Tested on Last release of Fedora, Ubuntu and ProxMox
- Working on Windows 10 20H1 (never below)

- 1920*1080 (need to be changed manually - search 2560 and/or 1440 in apex_dma.cpp)
- 2560*1440 (default)

INSTALL : 
 - Download sources from there *.zip or git
 - Extract it
 - Install Cargo & Rust with the famous curl https://sh.rustup.rs/ -sSf | sh (dont forget to add your USER)
 - Install memflow with the famous curl --proto '=https' --tlsv1.2 -sSf https://sh.memflow.io/ | sh (dont forget to add your USER)
 - check https://github.com/memflow/memflow-kvm and/or https://github.com/memflow/memflowup (better) to install memflow-kvm connector.
 - Compile with the build.sh to see if any errors.
 - When build is ok without errors, start Overlay (obfuscated) then Client (obfuscated)
 - On the Linux console (host) start the server by : sudo ./apex_dma
    
VISUALS :
 - ESP Box, XP Level, Line, Name, Distance
 - ESP Seer Health and Shield
 - Circle FOV
 - Players Glow (GREEN when visible, RED when not visible and YELLOW when knocked - Can be changed in the overlay's menu)
 - Items Glow (complete disable)
 - Spectators count + Name List (Name List temporary disable but you can active it)

FEATURES :
 - Work in BoderLess and FullScreen
 - work with DirectX 12
 - Added a visual DOT to know if you're connected on the server (GREEN DOT) or disconnected (RED DOT)
 - Added the BruteForce and optimization CR3/DTB fix by MisterY
 - Press F1 (To Activate GloW, ESP Seer Health/Shield and Aimbot based on default conf)
 - Dynamic FOV/AIM/SMOOTH [70m by default] Process Updated & Optimized 04/12/2024 - Settings added in overlay
 - AutoSuperGlide (nothing to do, it do for you :) ) !Someone tell me to check that, i will.
 - AutoWallJump updated (Slide then jump on a wall, dont touch anything else and it'll auto-walljump)
 - 1V1 (F9)
 - Training with dummies (F10)

!!! WARNING !!!

 - If using Client.exe and Overlay.exe - obfuscate both files to avoid detection.
 - If using Client.exe and Overlay from NVIDIA - obfuscate both files to avoid detection.

<img src="https://github.com/albatror/adkv/blob/master/demo/settingsS.png" style="display: block; margin: auto;" />
<img src="https://github.com/albatror/adkv/blob/master/demo/settingsS2.png" style="display: block; margin: auto;" />
<img src="https://github.com/albatror/adkv/blob/master/demo/Demo2.png" style="display: block; margin: auto;" />
<img src="https://github.com/albatror/adkv/blob/master/demo/settingsN2.png" style="display: block; margin: auto;" />
<img src="https://github.com/albatror/adkv/blob/master/demo/Demo4.png" style="display: block; margin: auto;" />
<img src="https://github.com/albatror/adkv/blob/master/demo/ingame1.png" style="display: block; margin: auto;" />
<img src="https://github.com/albatror/adkv/blob/master/demo/ingame2.png" style="display: block; margin: auto;" />
<img src="https://github.com/albatror/adkv/blob/master/demo/Demo5.png" style="display: block; margin: auto;" />
<img src="https://github.com/albatror/adkv/blob/master/demo/connected.png" style="display: block; margin: auto;" />
