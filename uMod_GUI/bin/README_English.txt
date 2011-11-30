WARNING: You use this program at your own risk!
1) If the program crash, you won't get probably any support from the game developer,
    but you can report the bug under http://code.google.com/p/texmod/issues/list
2) Games can detected if they are modified, so you might risk a ban
    of your online account!
3) This is an open-source project. The code can be retrieved, modified and compiled
    by everyone. Download Universal Modding Engine only from sources you trust!
    Download it by your self and don't use versions, send to you by team or guild members!
    http://code.google.com/p/texmod/downloads/list

Universal Modding Engine uses the D3DX9_43.dll (32bit). Due to the EULA this dll
cannot be delivered together with Universal Modding Engine . If D3DX9_43.dll is not 
installed on your system, Universal Modding Engine will give you a hint at program start.


What can Universal Modding Engine (uMod) V1.0?

-extract and save single textures from a DX9 game (the target texture can be toggled in the game)
-extract and save all textures from a DX9 game
-load textures into a game to replace target textures
-support single dds texture
-support zip-files as whole mod package
-support the original TexMod *.tpf files

All these options can be switched on or off, while the game is already running!
So you can search for a texture in the game, save it to disk, edit it,
load it into the game, edit it again and load it into the game again, ...,
actiavate or deactivate mods and all without a restart of the game.

side note: If "save all textures" is switched on, the texture will only be saved, if
the textures are loaded by the game and only in the moment they are loaded.
If you switch this option on, while a map is loaded, probably nothing will hapen,
because all textures are loaded for this map. Change the Map or reload it again.

Zip files can include a "texmod.def" file which contains the hash and the file name.
Each line should be in the format "hash|filename.dds"
If it does not contain a "texmod.def" each file will be unpacked and those which match
the wildcard "*_hash.dds" will be used.
The zip file can include a "Comment.txt" file. The content will be shown as comment 
in the GUI.

If you load single files, they should match the wildcard "*_hash.dds"


How does Universal Modding Engine interact with the game?

Universal Modding Engine intercept the connection between the game and DirectX.
It is required, that Universal Modding Engine is running before the game is started.

The GUI of Universal Modding Engine act as a server. A game which is successfully
intercepted with the Universal Modding Engine-dll will connect on program start to the server.
The GUI of Universal Modding Engine can handle multiple games at the same time. It will
also process each instance of a game as separate process (if the same game
was started more than once).


How to get Universal Modding Engine work?

There are three ways how Universal Modding Engine can intercept the DirectX connection:
(Do NOT use more than one method simultaneously!)

1) Add the games-binary through the menu "Main->Add game"
    For Steam see below.
    
    known problems: Guild Wars (Win XP)
    
2) Start the game directly through uMod though the menu
    "Main->Start game through uMod" or
    "Main->Start game through uMod (with command line)".
    The game start immediately.

3) Copy the d3d9.dll (from the Universal Modding Engine directory) into the game directory.
    Some games load a dll first from their own directory before they look up the system directory.
    Only for these games this method will work.
    WARNING: never copy this dll into your system directory!!
    
    known problems: Guild Wars

If you have chosen the first or third method, you simply start Universal Modding Engine
and afterwards the game. Do NOT start the game through Universal Modding Engine.

If the game starts and all works fine, a new tab opens immediately in uMod.
In this tab you can now mod the game. Press the "update" button to commit
the changes to the game. You can also save your current settings as a template.
One template can be set as default for a game, which will be loaded and 
and committed automatically when you start this game the next time.

To load a mod, you must set the check mark of the file. If you wish to unload a mod,
just remove the check mark and click on update again. 

Clicking on update will only update the differences (if packages have been
removed from the list, you toggled check marks or changed the order). The reload 
button forces to reload from disk (if you have edited the texture itself).

Due to the fact that different mods can modify the same target texture, only the
mod-texture of the first file in the list is taken into account. The action of this file
(load or unload) is proceeded regardless of what the following mods are opposed
to do with their mod-textures.


How to get Universal Modding Engine work together with Steam?

Universal Modding Engine looks for the name and the path of the executed binary.
Thus you shall not add the steam.exe but rather the game.exe
e.g.: C:\Steam\SteamApps\acoount_name\portal\hl2.exe
If you cannot find the exe, just start the game and use the TaskManager
to search for it. You can figure out the path through right click->properties.