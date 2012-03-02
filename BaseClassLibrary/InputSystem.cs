using System.Runtime.CompilerServices;

using System.Collections.Generic;

namespace CryEngine
{
	public class InputSystem
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _RegisterAction(string actionName);

		public static void RegisterAction(string actionName, InputActionDelegate actionDelegate)
		{
			if (inputActionDelegates == null)
				inputActionDelegates = new Dictionary<string, InputActionDelegate>();

			if (!inputActionDelegates.ContainsKey(actionName))
			{
				inputActionDelegates.Add(actionName, actionDelegate);

				_RegisterAction(actionName);
			}
			else
				Console.LogAlways("[Warning] Attempted to register duplicate input action {0}", actionName);
		}

		public delegate void InputActionDelegate(KeyEvent keyEvent, float value);

		public static void OnActionTriggered(string action, KeyEvent keyEvent, float value)
		{
			if (inputActionDelegates.ContainsKey(action))
				inputActionDelegates[action](keyEvent, value);
			else
				Console.LogAlways("Attempted to invoke unregistered action {0}", action);
		}

		private static Dictionary<string, InputActionDelegate> inputActionDelegates = new Dictionary<string, InputActionDelegate>();

		public delegate void MouseEventDelegate(int x, int y, MouseEvent mouseEvent, int wheelDelta);

		public static void OnMouseEvent(int x, int y, MouseEvent mouseEvent, int wheelDelta)
		{
			if(MouseEvents != null)
				MouseEvents(x, y, mouseEvent, wheelDelta);
		}

		public static event MouseEventDelegate MouseEvents;
	}

	public enum MouseEvent
	{
		Move,

		LeftButtonDown,
		LeftButtonUp,
		LeftButtonDoubleClick,
		RightButtonDown,
		RightButtonUp,
		RightButtonDoubleClick,
		MiddleButtonDown,
		MiddleButtonUp,
		MiddleButtonDoubleClick,

		Wheel,
	}

	public enum KeyEvent
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
