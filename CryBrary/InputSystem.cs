using System.Runtime.CompilerServices;

using System.Collections.Generic;

namespace CryEngine
{
	public static class InputSystem
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _RegisterAction(string actionName);

		/// <summary>
		/// Registers an event declared in the players actionmap. Without invoking this, Your KeyEventDelegate will never be invoked with the new action.
		/// </summary>
		/// <param name="actionName"></param>
		public static void RegisterAction(string actionName)
		{
			_RegisterAction(actionName);
		}

		public delegate void KeyEventDelegate(object sender, KeyEventArgs e);

		public static void OnActionTriggered(string action, KeyEvent keyEvent, float value)
		{
			if(KeyEvents != null)
				KeyEvents(null, new KeyEventArgs(keyEvent, action, value));
		}

		public static event KeyEventDelegate KeyEvents;

		public delegate void MouseEventDelegate(object sender, MouseEventArgs e);

		public static void OnMouseEvent(int x, int y, MouseEvent mouseEvent, int wheelDelta)
		{
			if(MouseEvents != null)
				MouseEvents(null, new MouseEventArgs(x, y, wheelDelta, mouseEvent));
		}

		public static event MouseEventDelegate MouseEvents;
	}

	public class KeyEventArgs : System.EventArgs
	{
		public KeyEventArgs(KeyEvent keyEvent, string actionName, float value)
		{
			KeyEvent = keyEvent;
			ActionName = actionName;
			Value = value;
		}

		public string ActionName { get; set; }
		public KeyEvent KeyEvent { get; set; }
		public float Value { get; set; }
	}

	public class MouseEventArgs : System.EventArgs
	{
		public MouseEventArgs(int x, int y, int wheelDelta, MouseEvent mouseEvent)
		{
			X = x;
			Y = y;
			WheelDelta = wheelDelta;
			MouseEvent = mouseEvent;
		}

		public int X { get; private set; }
		public int Y { get; private set; }
		public int WheelDelta { get; private set; }

		public MouseEvent MouseEvent { get; private set; }
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
