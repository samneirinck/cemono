using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
	public class UI
	{
		public static void OnEvent()
		{
			Events(null, new UIEventArgs());
		}

		public delegate void UIEventDelegate(object sender, UIEventArgs e);

		/// <summary>
		/// UI.Events += MyUIEventDelegateMethod;
		/// </summary>
		public static event UIEventDelegate Events;
	}

	public class UIEventArgs : System.EventArgs
	{
		public UIEventArgs()
		{

		}
	}
}
