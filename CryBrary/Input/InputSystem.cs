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
			ActionmapEvents.Invoke(new ActionMapEventArgs(keyEvent, action, value));
		}

		static void OnKeyEvent(string keyName, float value)
		{
			if(KeyEvents != null)
				KeyEvents(new KeyEventArgs(keyName, value));
		}

		static void OnMouseEvent(int x, int y, MouseEvent mouseEvent, int wheelDelta)
		{
			MouseX = x;
			MouseY = y;

			if(MouseEvents != null)
				MouseEvents(new MouseEventArgs(x, y, wheelDelta, mouseEvent));
		}

		public static int MouseX { get; set; }
		public static int MouseY { get; set; }
		#endregion

		public static event KeyEventDelegate KeyEvents;
		public static event MouseEventDelegate MouseEvents;

		public static ActionmapHandler ActionmapEvents = new ActionmapHandler();

		static void OnScriptInstanceDestroyed(CryScriptInstance instance)
		{
			foreach (KeyEventDelegate d in KeyEvents.GetInvocationList())
			{
				if (d.Target == instance)
					KeyEvents -= d;
			}

			foreach (MouseEventDelegate d in MouseEvents.GetInvocationList())
			{
				if (d.Target == instance)
					MouseEvents -= d;
			}

			ActionmapEvents.RemoveAll(instance);
		}
	}

	public class ActionmapHandler
	{
		public ActionmapHandler()
		{
			actionmapDelegates = new Dictionary<string, List<ActionMapEventDelegate>>();
		}

		public void Add(string actionMap, ActionMapEventDelegate eventDelegate)
		{
			List<ActionMapEventDelegate> eventDelegates;
			if (!actionmapDelegates.TryGetValue(actionMap, out eventDelegates))
			{
				NativeMethods.Input.RegisterAction(actionMap);

				eventDelegates = new List<ActionMapEventDelegate>();
				actionmapDelegates.Add(actionMap, eventDelegates);
			}

			eventDelegates.Add(eventDelegate);
		}

		public bool Remove(string actionMap, ActionMapEventDelegate eventDelegate)
		{
			List<ActionMapEventDelegate> eventDelegates;
			if (actionmapDelegates.TryGetValue(actionMap, out eventDelegates))
				return eventDelegates.Remove(eventDelegate);

			return false;
		}

		public int RemoveAll(object target)
		{
			int numRemoved = 0;

			foreach (var actionMap in actionmapDelegates)
				numRemoved += actionMap.Value.RemoveAll(x => x.Target == target);

			return numRemoved;
		}

		internal void Invoke(ActionMapEventArgs args)
		{
			List<ActionMapEventDelegate> eventDelegates;
			if (actionmapDelegates.TryGetValue(args.ActionName, out eventDelegates))
				eventDelegates.ForEach(x => x(args));
		}

		Dictionary<string, List<ActionMapEventDelegate>> actionmapDelegates;

		/*
		public static void operator +(ActionmapHandler handler, ActionMapEventDelegate eventDelegate)
		{
		}
		
		public static void operator -(ActionmapHandler handler, ActionMapEventDelegate eventDelegate)
		{
		}
		*/
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
