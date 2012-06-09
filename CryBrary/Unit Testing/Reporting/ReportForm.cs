using System.Windows.Forms;

namespace CryEngine.Testing.Internals
{
	internal partial class ReportForm : Form, IReportListener
	{
		public ReportForm()
		{
			InitializeComponent();
			TestManager.Run += OnTestsRun;
		}

		public void OnTestsRun(TestReport report)
		{

		}
	}
}
