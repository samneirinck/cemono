CryMono - Mono game logic support for CryENGINE3
	by Ink Studios Ltd. (Based on 'cemono' by Sam 'ins\' Neirinck)

# Description
CryMono brings the power of C# into the world of CryENGINE3, allowing game logic to be scripted in a faster and easier to use lanaguage than Lua.
	
## Using CryMono 
In order to use CryMono, you'll need to download the latest core directory structure from our Downloads page; https://github.com/inkdev/CryMono/downloads.

### Using CryMono in an existing CryENGINE project
See the article on our Wiki here; https://github.com/inkdev/CryMono/wiki/Integrating-CryMono-into-an-existing-CryENGINE-project

### Using the included CryMono Game sample

// TODO

#### Visual Studio 2010
Open MonoGameSample.sln and build all projects.

#### Source directory structure 
Our Visual Studio projects have been set up to expect all contents to be placed inside a folder within the Code folder shipped with the CryENGINE Free SDK.

Example:
D:\Dev\INK\CryENGINE\Code\CryMono

Using another folder structure is up to the developer, but will require customization in order to get Visual Studio to output compiled libraries to the correct location.

### CryMono source access
As of version 0.2, we no longer ship source to CryMono.dll.

If you are interested in obtaining source for this, please contact us at business@inkdev.net

# IDE Compatibility
The solution has been worked on internally using Visual Studio 2010, but should support 2005 and up in theory. Express versions are supported as of CryMono v0.2.

### Debugging managed code
Debugging managed (C#) code is not supported yet, but will be supported in the near future.
