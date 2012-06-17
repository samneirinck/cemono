using CryEngine;
using CryEngine.Serialization;
using CryEngine.Initialization;

using NUnit.Framework;

using System;
using System.IO;
using System.Collections.Generic;

namespace CryBrary.Tests.Serialization
{
	class ScriptsTests : CryBraryTests
	{
		[Test]
		public void Serialize_Native_Scripts()
		{
			var serializer = new CrySerializer();

			using(var stream = new MemoryStream())
			{
				ScriptManager.AddScriptInstance(new NativeEntity(1, IntPtr.Zero), ScriptType.Entity);
				ScriptManager.AddScriptInstance(new NativeEntity(2, IntPtr.Zero), ScriptType.Entity);
				ScriptManager.AddScriptInstance(new NativeActor(3), ScriptType.Actor);

				serializer.Serialize(stream, ScriptManager.Scripts);

				ScriptManager.Scripts = serializer.Deserialize(stream) as List<CryScript>;
				Assert.IsNotNull(ScriptManager.Scripts);
				Assert.AreEqual(2, ScriptManager.Scripts.Count);

				var entityScript = ScriptManager.FindScript(ScriptType.Entity, x => x.Type == typeof(NativeEntity));
				Assert.AreNotSame(default(CryScript), entityScript);
				Assert.IsNotNull(entityScript.ScriptInstances);
				Assert.AreEqual(1, (entityScript.ScriptInstances[0] as EntityBase).Id);
				Assert.AreEqual(2, (entityScript.ScriptInstances[1] as EntityBase).Id);

				var actor = ScriptManager.Find<NativeActor>(ScriptType.Actor, x => x.Id == 3);
				Assert.IsNotNull(actor);
			}
		}
	}
}
