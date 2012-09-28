# Contributing to the CryMono project

// TODO: Coding style

# Modifying project files
Avoid changes to the project files (*.csproj, *.vcxproj) that are specific to your environment. This might include, but is not limited to:
- Linker paths (c++)
- Output directories (c++,c#)
- Conditial compilation symbols (c#) or defines that should not be in the default build (c++)
- Build Events

Instead, it's recommended to keep the project file clean, and place the specific configuration you use while developing in a .user file.
For example, one could create a CryBrary.csproj.user file in the same directory as the CryBrary.csproj:
```xml
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup>
    <OutputPath>d:\games\MyGame\Bin32\</OutputPath>
  </PropertyGroup>
</Project>
```
Now, when building in visual studio, the output dll can be found in the d:\games\MyGame\Bin32\ directory.

