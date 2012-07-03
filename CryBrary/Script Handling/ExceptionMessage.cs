using System;
using System.Diagnostics;
using System.Windows.Forms;

namespace CryEngine
{
	internal partial class ExceptionMessage : Form
	{
		public ExceptionMessage(Exception ex, bool fatal)
		{
			InitializeComponent();

			uxContinueBtn.Click += (s, a) => Close();
			uxReportBtn.Click += (s, a) => Process.Start("http://www.crydev.net/posting.php?mode=post&f=314");
			uxCancelBtn.Click += (s, a) => Process.GetCurrentProcess().Kill();

			if(fatal)
			{
				uxStackTextbox.AppendText("Exceptions are currently treated as fatal errors (mono_exceptionsTriggerFatalErrors is true).\n");
				uxStackTextbox.AppendText("The application cannot continue.\n\n");
			}

			uxStackTextbox.AppendText(ex.ToString());

			if(fatal)
				uxContinueBtn.Enabled = false;
		}
	}
}
