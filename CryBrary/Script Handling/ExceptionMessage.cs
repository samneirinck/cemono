using System;
using System.Diagnostics;
using System.Windows.Forms;
using CryEngine.Extensions;

namespace CryEngine
{
	internal partial class ExceptionMessage : Form
	{
		public ExceptionMessage(Exception ex, bool fatal)
		{
			InitializeComponent();

			uxContinueBtn.Click += (s, a) => Close();
			uxReportBtn.Click += (s, a) => Process.Start("http://www.crydev.net/posting.php?mode=post&f=375");
			uxCancelBtn.Click += (s, a) => Process.GetCurrentProcess().Kill();

			if(fatal)
			{
				uxStackTextbox.Append("Exceptions are currently treated as fatal errors (mono_exceptionsTriggerFatalErrors is true).");
				uxStackTextbox.NewLine();
				uxStackTextbox.Append("The application cannot continue.");
				uxStackTextbox.NewLine(2);
			}

			uxStackTextbox.Append(ex.ToString());

			var selected = ActiveControl;
			ActiveControl = uxStackTextbox;
			uxStackTextbox.ScrollTo(0);
			ActiveControl = selected;

			if(fatal)
				uxContinueBtn.Enabled = false;
		}
	}
}
