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

## CryGame / CryENGINE interface customizations
A few changes have been made to CryCommon interfaces to provide functionality we have required during development of CryMono.

### GameStartup.cpp - CryGame

1. Find method 'CGameStartup::InitFramework'.
2. Add gEnv->pGameFramework = m_pGameFramework after ModuleInitISystem(m_pFramework->GetISystem(),"CryGame");

### ISystem.h - CryCommon

1. Add struct IMonoScriptSystem; after struct IOutputPrintSink; declaration.
2. Add IMonoScriptSystem *pMonoScriptSystem; to the end of the SSystemGlobalEnvironment struct declaration.

### CryModuleDefs.h - CryCommon

1. Add eCryM_Mono after eCryM_Launcher.

# IDE Compatibility
The solution has been worked on internally using Visual Studio 2010, but should support 2005 and up in theory.

### Debugging managed code
Debugging managed code is not supported yet, but will be supported on MonoDevelop in the near future.