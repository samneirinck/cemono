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
		    InitializeMocks();

			ScriptManager.IgnoreExternalCalls = true;
		}

        private void InitializeMocks()
        {
            // Gotta mock em all
            NativeMethods.Actor = new Mock<INativeActorMethods>().Object;
            NativeMethods.CVar = new Mock<INativeCVarMethods>().Object;
            NativeMethods.Debug = new Mock<INativeDebugMethods>().Object;
            NativeMethods.Engine3D = new Mock<INative3DEngineMethods>().Object;
            NativeMethods.Entity = new Mock<INativeEntityMethods>().Object;
            NativeMethods.FlowNode = new Mock<INativeFlowNodeMethods>().Object;
            NativeMethods.GameRules = new Mock<INativeGameRulesMethods>().Object;
            NativeMethods.Input = new Mock<INativeInputMethods>().Object;
            NativeMethods.ItemSystem = new Mock<INativeItemSystemMethods>().Object;
            NativeMethods.Level = new Mock<INativeLevelMethods>().Object;
            NativeMethods.Material = new Mock<INativeMaterialMethods>().Object;
            NativeMethods.Particle = new Mock<INativeParticleEffectMethods>().Object;
            NativeMethods.Physics = new Mock<INativePhysicsMethods>().Object;
            NativeMethods.Renderer = new Mock<INativeRendererMethods>().Object;
            NativeMethods.ScriptTable = new Mock<INativeScriptTableMethods>().Object;
            NativeMethods.Time = new Mock<INativeTimeMethods>().Object;
            NativeMethods.UI = new Mock<INativeUIMethods>().Object;
            NativeMethods.View = new Mock<INativeViewMethods>().Object;

            var logMock = new Mock<INativeLoggingMethods>();
            Action<string> loggingMethod = msg => Console.WriteLine(msg);

            logMock.Setup(m => m.Log(It.IsAny<string>())).Callback(loggingMethod);
            logMock.Setup(m => m.LogAlways(It.IsAny<string>())).Callback(loggingMethod);
            logMock.Setup(m => m.Warning(It.IsAny<string>())).Callback(loggingMethod);
            
            NativeMethods.Log = logMock.Object;
        }

        private void InitializeCVarMethods()
        {
            var cvarMethodsMock = new Mock<INativeCVarMethods>();

            NativeMethods.CVar = cvarMethodsMock.Object;
        }

        [TearDown]
        public void PostTest()
        {
            if (ScriptManager.Scripts != null)
                ScriptManager.Scripts.Clear();
        }
    }
}