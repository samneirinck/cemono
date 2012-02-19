CryMono (v0.4) - Mono game logic support for CryENGINE3
	by Ink Studios Ltd. (Based on 'cemono' by Sam 'ins\' Neirinck)

# Description
CryMono brings the power of C# into the world of CryENGINE3, allowing game logic to be scripted in a faster and easier to use lanaguage than Lua.
	
## Using CryMono 
In order to use CryMono, you'll need to download the latest core directory structure from our Downloads page; https://github.com/inkdev/CryMono/downloads.

### Documentation
For instructions on all things CryMono, see our Confluence setup here; http://docs.inkdev.net/display/CryMono/Home

### Samples

At the moment we have two publicly available samples powered by CryMono, they can be downloaded here:
* MiniGameSample - https://github.com/inkdev/CryMono-Mini-Game-Sample | A C# CryENGINE minigame sample built using a skeleton game dll
* GameSample - https://github.com/inkdev/CryMono-Game-Sample | The default CryENGINE project with CryMono built-in.

#### Source directory structure 
Our Visual Studio projects have been set up to expect all contents to be placed inside a folder within the Code folder shipped with the CryENGINE Free SDK.

Example:
D:\Dev\INK\CryENGINE\Code\CryMono

Using another folder structure is up to the developer, but will require customization in order to get Visual Studio to output compiled libraries to the correct location.

# IDE Compatibility
The solution has been worked on internally using Visual Studio 2010, but should support 2005 and up in theory. Express versions are supported as of CryMono v0.2.

### Debugging managed code
Debugging managed (C#) code is not supported yet, but will be supported in the near future.
