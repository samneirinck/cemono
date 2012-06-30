using System;
using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal class NativeLevelMethods : INativeLevelMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static IntPtr _GetCurrentLevel();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static IntPtr _LoadLevel(string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static bool _IsLevelLoaded();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _UnloadLevel();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static string _GetName(IntPtr levelPtr);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static string _GetPath(IntPtr levelPtr);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static string _GetPaks(IntPtr levelPtr);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static string _GetDisplayName(IntPtr levelPtr);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static int _GetHeightmapSize(IntPtr levelPtr);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static int _GetGameTypeCount(IntPtr levelPtr);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static string _GetGameType(IntPtr levelPtr, int index);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static bool _SupportsGameType(IntPtr levelPtr, string gameTypeName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static string _GetDefaultGameType(IntPtr levelPtr);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static bool _HasGameRules(IntPtr levelPtr);

        public IntPtr GetCurrentLevel()
        {
            return _GetCurrentLevel();
        }

        public IntPtr LoadLevel(string name)
        {
            return _LoadLevel(name);
        }

        public bool IsLevelLoaded()
        {
            return _IsLevelLoaded();
        }

        public void UnloadLevel()
        {
            _UnloadLevel();
        }

        public string GetName(IntPtr levelPtr)
        {
            return _GetName(levelPtr);
        }

        public string GetPath(IntPtr levelPtr)
        {
            return _GetPath(levelPtr);
        }

        public string GetPaks(IntPtr levelPtr)
        {
            return _GetPaks(levelPtr);
        }

        public string GetDisplayName(IntPtr levelPtr)
        {
            return _GetDisplayName(levelPtr);
        }

        public int GetHeightmapSize(IntPtr levelPtr)
        {
            return _GetHeightmapSize(levelPtr);
        }

        public int GetGameTypeCount(IntPtr levelPtr)
        {
            return _GetGameTypeCount(levelPtr);
        }

        public string GetGameType(IntPtr levelPtr, int index)
        {
            return _GetGameType(levelPtr, index);
        }

        public bool SupportsGameType(IntPtr levelPtr, string gameTypeName)
        {
            return _SupportsGameType(levelPtr, gameTypeName);
        }

        public string GetDefaultGameType(IntPtr levelPtr)
        {
            return _GetDefaultGameType(levelPtr);
        }

        public bool HasGameRules(IntPtr levelPtr)
        {
            return _HasGameRules(levelPtr);
        }
    }
}
