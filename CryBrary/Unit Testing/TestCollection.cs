using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.IO;
using System.Reflection;

namespace CryEngine.Testing
{
	internal sealed class TestCollection
	{
		public object Instance { get; set; }
		public IEnumerable<MethodInfo> Tests { get; set; }

		public int Run()
		{
			Debug.LogAlways("	Running test collection for {0}:", Instance.GetType().Name);

			return (from test in Tests
					let result = RunTest(test)
					where !result
					select result).Count();
		}

		private bool RunTest(MethodInfo test)
		{
			Debug.LogAlways("		Running test: {0}", test.Name);

			try
			{
				test.Invoke(Instance, null);
				Debug.LogAlways("			Test successful!");
				return true;
			}
			catch(Exception ex)
			{
				// The main exception will always be a TargetInvocationException because we invoke via reflection
				var inner = ex.InnerException;

				var trace = new StackTrace(inner, true);
				var firstFrame = trace.GetFrame(0);

				// FIXME: Assert will report exceptions from inside CryBrary
				// Drop to the second frame if we detect that's the case
				if(firstFrame.GetFileName().Contains(Path.Combine("CryMono", "CryBrary")))
					firstFrame = trace.GetFrame(1);

				Debug.LogAlways("			({0}) {1} (thrown at line {2} of {3})", inner.GetType().Name, inner.Message,
					firstFrame.GetFileLineNumber(), firstFrame.GetFileName());

				return false;
			}
		}
	}
}
