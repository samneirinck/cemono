

namespace CryEngine
{
	public delegate void Callback();

	public static class CallbackHandler
	{
		public static void RegisterCallback(string func, string className, Callback callback)
		{
			NativeMethods._RegisterCallback(func, className, callback);
		}
	}
}
