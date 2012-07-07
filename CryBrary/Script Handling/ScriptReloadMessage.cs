using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.Diagnostics;
using System.CodeDom.Compiler;

namespace CryEngine.Initialization
{
	public partial class ScriptReloadMessage : Form
	{
		public ScriptReloadMessage(Exception exception, bool canRevert)
		{
			InitializeComponent();

			tryAgainButton.Click += (s, a) => ScriptManager.Instance.OnReload();
			revertButton.Click += (s, a) => ScriptManager.Instance.OnRevert();
			exitButton.Click += (s, a) => Process.GetCurrentProcess().Kill();

			if (!canRevert)
				revertButton.Enabled = false;

			errorBox.Text = exception.ToString();
		}
	}
}
