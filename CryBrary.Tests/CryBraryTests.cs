using System;
using CryEngine;
using CryEngine.Initialization;
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

			loggingMethodsMock.Setup(m => m._Log(It.IsAny<string>())).Callback(loggingMethod);
			loggingMethodsMock.Setup(m => m._LogAlways(It.IsAny<string>())).Callback(loggingMethod);
			loggingMethodsMock.Setup(m => m._Warning(It.IsAny<string>())).Callback(loggingMethod);

			Debug.Methods = loggingMethodsMock.Object;

			var entityMethodsMock = new Mock<INativeEntityMethods>();
			EntityBase.Methods = entityMethodsMock.Object;

			var actorMethodsMock = new Mock<INativeActorMethods>();
			Actor.Actormethods = actorMethodsMock.Object;
		}
	}
}