CryMono - Mono game logic support for CryENGINE3
	by Ink Studios Ltd. (Based on 'cemono' by Sam 'ins\' Neirinck)

# Description
CryMono brings the power of C# into the world of CryENGINE3, allowing game logic to be scripted in a faster and easier to use lanaguage than Lua.
	
## Compilation 
In order to compile and use CryMono, you'll need to download the latest core directory structure from our Downloads page; https://github.com/inkdev/CryMono/downloads.

Open CryMono.sln (Visual Studio 2010 or above, and build all projects.

### Source directory structure 
Our C++ and C# projects have been set up to expect all contents to be placed inside a folder within the Code folder shipped with the CryENGINE Free SDK.

Example:
D:\Dev\INK\CryENGINE\Code\CryMono\MonoDll

Using another folder structure is up to the user, but will require customization in order to get Visual Studio to output compiled libraries to the correct location.

## Integrating CryMono into an existing CryENGINE project
See the article on our Wiki here; https://github.com/inkdev/CryMono/wiki/Integrating-CryMono-into-an-existing-CryENGINE-project

# IDE Compatibility
The solution has been worked on internally using Visual Studio 2010, but should support 2005 and up in theory. Express versions are supported as of CryMono v0.2.

### Debugging managed code
Debugging managed code is not supported yet, but will be supported in the near future.