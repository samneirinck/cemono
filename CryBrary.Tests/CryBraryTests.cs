using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CryEngine;
using Moq;
using NUnit.Framework;

namespace CryBrary.Tests
{
	public class CryBraryTests
	{
		[SetUp]
		public void Init()
		{
			InitializeLoggingMethods();
		}

		private void InitializeLoggingMethods()
		{
			var loggingMethodsMock = new Mock<ILoggingMethods>();
			Action<string> loggingMethod = msg => System.Console.WriteLine(msg);

			loggingMethodsMock.Setup(m => m._Log(It.IsAny<string>())).Callback(loggingMethod);
			loggingMethodsMock.Setup(m => m._LogAlways(It.IsAny<string>())).Callback(loggingMethod);
			loggingMethodsMock.Setup(m => m._Warning(It.IsAny<string>())).Callback(loggingMethod);

			Debug.Methods = loggingMethodsMock.Object;
		}
	}
}