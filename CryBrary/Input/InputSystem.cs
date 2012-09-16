using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Linq;

using CryEngine.Native;

namespace CryEngine
{
	public delegate void ActionMapEventDelegate(ActionMapEventArgs e);

	public delegate void KeyEventDelegate(KeyEventArgs e);
	public delegate void MouseEventDelegate(MouseEventArgs e);

	public static class Input
	{
		#region Events
		static void OnActionTriggered(string action, KeyEvent keyEvent, float value)
		{
			actionmapDelegates[action](new ActionMapEventArgs(keyEvent, action, value));
		}

		static void OnKeyEvent(string keyName, float value)
		{
			if(KeyEvents != null)
				KeyEvents(new KeyEventArgs(keyName, value));
		}

		static void OnMouseEvent(int x, int y, MouseEvent mouseEvent, int wheelDelta)
		{
			mousePosition.X = x;
			mousePosition.Y = y;

			if(MouseEvents != null)
				MouseEvents(new MouseEventArgs(x, y, wheelDelta, mouseEvent));
		}

		static Vec2 mousePosition;
		public static Vec2 MousePosition { get { return mousePosition; } set { mousePosition = value; } }
		#endregion

		/// <summary>
		/// Registers an event declared in the players actionmap. Without invoking this, Your KeyEventDelegate will never be invoked with the new action.
		/// </summary>
		/// <param name="actionName"></param>
		/// <param name="eventDelegate"></param>
		public static void RegisterAction(string actionName, ActionMapEventDelegate eventDelegate)
		{
			if(!actionmapDelegates.ContainsKey(actionName))
			{
                NativeMethods.Input.RegisterAction(actionName);

				actionmapDelegates.Add(actionName, eventDelegate);
			}
		}

		/// <summary>
		/// Removes the delegate from
		/// </summary>
		/// <param name="eventDelegate"></param>
		/// <returns>The number of removed actions</returns>
		public static bool UnregisterAction(ActionMapEventDelegate eventDelegate)
		{
			var matches = actionmapDelegates.Where(x => x.Value == eventDelegate).ToList();
			foreach (var match in matches)
				actionmapDelegates.Remove(match.Key);

			return matches.Count() > 0;
		}

		/// <summary>
		/// Removes all actions linked to the specified object.
		/// </summary>
		/// <param name="owner"></param>
		/// <returns>The number of removed actions</returns>
		public static int UnregisterActions(object owner)
		{
			var matches = actionmapDelegates.Where(x => x.Value.Target == owner).ToList();
			foreach (var match in matches)
				actionmapDelegates.Remove(match.Key);

			return matches.Count();
		}

		static Dictionary<string, ActionMapEventDelegate> actionmapDelegates = new Dictionary<string, ActionMapEventDelegate>();

		public static event KeyEventDelegate KeyEvents;

		public static event MouseEventDelegate MouseEvents;
	}

	public class ActionMapEventArgs : EventArgs
	{
		public ActionMapEventArgs(KeyEvent keyEvent, string actionName, float value)
		{
			KeyEvent = keyEvent;
			ActionName = actionName;
			Value = value;
		}

        public string ActionName { get; private set; }
        public KeyEvent KeyEvent { get; private set; }
        public float Value { get; private set; }
	}

	public class KeyEventArgs : EventArgs
	{
		public KeyEventArgs(string actionName, float value)
		{
			ActionName = actionName;
			Value = value;
		}

        public string ActionName { get; private set; }
        public float Value { get; private set; }
	}

	public class MouseEventArgs : EventArgs
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
