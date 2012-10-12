using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace CryEngine.Native
{
    internal class NativeCVarMethods : INativeCVarMethods
    {
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void _Execute(string command, bool silent);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void _RegisterCommand(string name, string description, CVarFlags flags);

		// CVars
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void _RegisterCVarFloat(string name, ref float val, float defaultVal, CVarFlags flags,
                                                       string description);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void _RegisterCVarInt(string name, ref int val, int defaultVal, CVarFlags flags,
                                                     string description);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void _RegisterCVarString(string name, [MarshalAs(UnmanagedType.LPStr)] string val,
                                                        string defaultVal, CVarFlags flags, string description);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float _GetCVarFloat(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern int _GetCVarInt(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string _GetCVarString(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void _SetCVarFloat(string name, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void _SetCVarInt(string name, int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void _SetCVarString(string name, string value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool _HasCVar(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void _HandleException(Exception ex);

		public void Execute(string command, bool silent)
		{
			_Execute(command, silent);
		}

        public void RegisterCommand(string name, string description, CVarFlags flags)
        {
            _RegisterCommand(name, description, flags);
        }

        public void RegisterCVarFloat(string name, ref float val, float defaultVal, CVarFlags flags,
                                                       string description)
        {
            _RegisterCVarFloat(name, ref val, defaultVal, flags, description);
        }

        public void RegisterCVarInt(string name, ref int val, int defaultVal, CVarFlags flags,
                                                     string description)
        {
            _RegisterCVarInt(name, ref val, defaultVal, flags, description);
        }

        public void RegisterCVarString(string name, string val,
                                 string defaultVal, CVarFlags flags, string description)
        {
            _RegisterCVarString(name,val,defaultVal,flags,description);
        }

        public float GetCVarFloat(string name)
        {
            return _GetCVarFloat(name);
        }

        public int GetCVarInt(string name)
        {
            return _GetCVarInt(name);
        }

        public string GetCVarString(string name)
        {
            return _GetCVarString(name);
        }

        public void SetCVarFloat(string name, float value)
        {
            _SetCVarFloat(name,value);
        }

        public void SetCVarInt(string name, int value)
        {
            _SetCVarInt(name,value);
        }

        public void SetCVarString(string name, string value)
        {
            _SetCVarString(name,value);
        }

        public bool HasCVar(string name)
        {
            return _HasCVar(name);
        }

        public void HandleException(Exception ex)
        {
            _HandleException(ex);
        }
    }
}