using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CryEngine.Extensions
{
	public static class WinFormsExtensions
	{
		public static void Append(this TextBoxBase textBox, string format, params object[] args)
		{
			textBox.AppendText(string.Format(format, args));
		}
	}
}
