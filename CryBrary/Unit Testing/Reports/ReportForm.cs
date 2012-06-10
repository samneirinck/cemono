using System;
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
			Show();

			uxTestTree.Nodes.Clear();
			var root = uxTestTree.Nodes.Add("Tests");

			var overallSuccess = true;
			var overallIgnored = false;

			foreach(var collection in report.Collections)
			{
				var collectionNode = root.Nodes.Add(collection.Name);

				var ignored = false;
				var failed = false;

				foreach(var test in collection.Results)
				{
					if(test.Result == TestResult.Failure)
					{
						overallSuccess = false;
						failed = true;
					}
					else if(test.Result == TestResult.Ignored && !failed)
					{
						if(overallSuccess)
							overallIgnored = true;

						ignored = true;
					}

					var image = GetImageIndex(test.Result);
					var node = new TreeNode(test.Name, image, image);
					collectionNode.Nodes.Add(node);
				}

				var collectionImage = GetImageIndex(failed, ignored);
				collectionNode.ImageIndex = collectionImage;
				collectionNode.SelectedImageIndex = collectionImage;
			}

			var rootImage = GetImageIndex(!overallSuccess, overallIgnored);
			root.ImageIndex = rootImage;
			root.SelectedImageIndex = rootImage;
		}

		private int GetImageIndex(bool failed, bool ignored)
		{
			return failed ? GetImageIndex(TestResult.Failure) :
				ignored ? GetImageIndex(TestResult.Ignored) :
				GetImageIndex(TestResult.Success);
		}

		private int GetImageIndex(TestResult result)
		{
			switch(result)
			{
				case TestResult.Success:
					return 0;
				case TestResult.Failure:
					return 1;
				case TestResult.Ignored:
					return 2;
			}

			throw new ArgumentException("TestResult value is not supported by the report UI.", "result");
		}

		private void OnClose(object sender, FormClosingEventArgs e)
		{
			TestManager.Run -= OnTestsRun;
		}
	}
}
