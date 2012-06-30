using System;

namespace CryEngine.Native
{
    internal interface INativeLevelMethods
    {
        IntPtr GetCurrentLevel();
        IntPtr LoadLevel(string name);
        bool IsLevelLoaded();
        void UnloadLevel();
        string GetName(IntPtr levelPtr);
        string GetPath(IntPtr levelPtr);
        string GetPaks(IntPtr levelPtr);
        string GetDisplayName(IntPtr levelPtr);
        int GetHeightmapSize(IntPtr levelPtr);
        int GetGameTypeCount(IntPtr levelPtr);
        string GetGameType(IntPtr levelPtr, int index);
        bool SupportsGameType(IntPtr levelPtr, string gameTypeName);
        string GetDefaultGameType(IntPtr levelPtr);
        bool HasGameRules(IntPtr levelPtr);
    }
}