using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Runtime.InteropServices;

namespace CryEngine.Extensions
{
	public static class HandleRefExtensions
	{
		public static bool IsValid(this HandleRef handleRef)
		{
			return handleRef.Handle == IntPtr.Zero;
		}
	}
}
