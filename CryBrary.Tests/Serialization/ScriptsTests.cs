using CryEngine;
using CryEngine.Serialization;
using CryEngine.Initialization;

using NUnit.Framework;

using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;

namespace CryBrary.Tests.Serialization
{
    [TestFixture]
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

        [Test]
        public void CryScript_List()
        {
            var serializer = new CrySerializer();

            using (var stream = new MemoryStream())
            {
                var list = new List<CryScript>();

                var nativeActorScript = new CryScript(typeof(NativeActor));
                nativeActorScript.ScriptInstances = new List<CryScriptInstance>();
                nativeActorScript.ScriptInstances.Add(new NativeActor(759));
                nativeActorScript.ScriptInstances.Add(new NativeActor(5));

                var nativeEntityScript = new CryScript(typeof(NativeEntity));
                nativeEntityScript.ScriptInstances = new List<CryScriptInstance>();
                nativeEntityScript.ScriptInstances.Add(new NativeEntity(987, IntPtr.Zero));
                nativeEntityScript.ScriptInstances.Add(new NativeEntity(8, IntPtr.Zero));
                nativeEntityScript.ScriptInstances.Add(null);

                list.Add(nativeActorScript);
                list.Add(nativeEntityScript);

                serializer.Serialize(stream, list);

                serializer = new CrySerializer();
                list = serializer.Deserialize(stream) as List<CryScript>;

                Assert.IsNotNull(list);
                Assert.IsNotEmpty(list);

                Assert.AreEqual(2, list.Count);

                nativeActorScript = list.ElementAt(0);
                Assert.IsNotNull(nativeActorScript.ScriptInstances);
                Assert.AreEqual(2, nativeActorScript.ScriptInstances.Count);

                Assert.IsNotNull(nativeActorScript.ScriptInstances.ElementAt(0));
                Assert.AreEqual(759, (nativeActorScript.ScriptInstances.ElementAt(0) as EntityBase).Id);

                Assert.IsNotNull(nativeActorScript.ScriptInstances.ElementAt(1));
                Assert.AreEqual(5, (nativeActorScript.ScriptInstances.ElementAt(1) as EntityBase).Id);

                nativeEntityScript = list.ElementAt(1);

                Assert.IsNotNull(nativeEntityScript.ScriptInstances);
                Assert.AreEqual(3, nativeEntityScript.ScriptInstances.Count);

                Assert.IsNotNull(nativeEntityScript.ScriptInstances.ElementAt(0));
                Assert.AreEqual(987, (nativeEntityScript.ScriptInstances.ElementAt(0) as EntityBase).Id);

                Assert.IsNotNull(nativeEntityScript.ScriptInstances.ElementAt(1));
                Assert.AreEqual(8, (nativeEntityScript.ScriptInstances.ElementAt(1) as EntityBase).Id);

                Assert.IsNull(nativeEntityScript.ScriptInstances.ElementAt(2));
            }
        }
	}
}
