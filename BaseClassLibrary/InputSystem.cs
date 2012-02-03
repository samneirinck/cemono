using System.Runtime.CompilerServices;

namespace CryEngine
{
	public class InputSystem
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void RegisterAction(string actionName);

		public static void OnActionTriggered(string action, ActionActivationMode activationMode, float value)
		{
		}

		public enum ActionActivationMode
		{
			Invalid = 0,
			/// <summary>
			/// Used when the action key is pressed
			/// </summary>
			OnPress,
			/// <summary>
			/// Used when the action key is released
			/// </summary>
			OnRelease,
			/// <summary>
			/// Used when the action key is held
			/// </summary>
			OnHold,
			Always,

			Retriggerable,
			NoModifiers,
			ConsoleCmd,
			/// <summary>
			/// Used when analog compare op succeeds
			/// </summary>
			AnalogCmd
		}
	}
}
