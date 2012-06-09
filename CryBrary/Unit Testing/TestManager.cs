using System.Collections.Generic;
using System.Linq;

namespace CryEngine.Testing
{
	internal static class TestManager
	{
		public static List<TestCollection> TestCollections { get; private set; }

		public static void Run(string[] args, string fullCommandLine)
		{
			Debug.LogAlways("Test run started...");

			var testResults = (from testCollection in TestCollections
							   select testCollection.Run()).ToList();

			var collectionFailCount = testResults.Count(res => res > 0);
			var testFailCount = testResults.Sum(res => res);

			Debug.LogAlways("Test run finished. {0}", testFailCount == 0 ? "No errors found!" :
				string.Format("{0} total errors across {1} failing test collections.", testFailCount, collectionFailCount));
		}

		public static void Init()
		{
			TestCollections = new List<TestCollection>();
			CCommand.Register("tester_run", Run, "Runs the feature tester");
		}
	}
}
