using System;
using CryEngine;
using CryEngine.Initialization;
using CryEngine.Native;
using Moq;
using NUnit.Framework;

namespace CryBrary.Tests
{
	public abstract class CryBraryTests
	{
		[SetUp]
		public void Init()
		{
			InitializeLoggingMethods();
		    InitializeCVarMethods();

			ScriptManager.IgnoreExternalCalls = true;
		}

        private void InitializeCVarMethods()
        {
            var cvarMethodsMock = new Mock<INativeCVarMethods>();

            CVar.Methods = cvarMethodsMock.Object;
        }

        [TearDown]
        public void PostTest()
        {
            if (ScriptManager.Scripts != null)
                ScriptManager.Scripts.Clear();
        }

		private void InitializeLoggingMethods()
		{
			var loggingMethodsMock = new Mock<INativeLoggingMethods>();
			Action<string> loggingMethod = msg => Console.WriteLine(msg);

			loggingMethodsMock.Setup(m => m.Log(It.IsAny<string>())).Callback(loggingMethod);
			loggingMethodsMock.Setup(m => m.LogAlways(It.IsAny<string>())).Callback(loggingMethod);
			loggingMethodsMock.Setup(m => m.Warning(It.IsAny<string>())).Callback(loggingMethod);

			Debug.NativeLoggingMethods = loggingMethodsMock.Object;
		}
	}
}