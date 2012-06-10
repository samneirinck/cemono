using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using CryEngine.Testing.Internals;

namespace CryEngine.Testing
{
	public static class TestManager
	{
		internal static List<TestCollection> TestCollections { get; private set; }

		public static event Action<TestReport> Run;

		public static void RunTests(string[] args, string fullCommandLine)
		{
			var timer = new Stopwatch();
			timer.Start();

			var testResults = (from testCollection in TestCollections
							   select testCollection.Run()).ToList();

			timer.Stop();

			var report = new TestReport { Collections = testResults, TimeTaken = timer.Elapsed };

			formListener = new ReportForm();

			if(Run != null)
				Run(report);
		}

		public static void Init()
		{
			TestCollections = new List<TestCollection>();
			CCommand.Register("tester_run", RunTests, "Runs the feature tester");

			listener = new ConsoleTestListener();
		}

		private static ConsoleTestListener listener;
		private static ReportForm formListener;
	}
}
