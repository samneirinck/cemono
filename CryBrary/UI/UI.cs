using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using CryEngine.Extensions;

namespace CryEngine
{
	public class UI
	{
		public delegate void FunctionCallback();

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _CreateEventSystem(string name, EventSystemType type);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _RegisterFunction(IntPtr eventSystemPtr, string name, string desc, object[] inputs, FunctionCallback callback);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _RegisterEvent(IntPtr eventSystemPtr, string name, string desc, object[] outputs);

		public enum EventSystemType
		{
			ToSystem,
			ToUI
		}
	}
}