using CryEngine;
using CryEngine.Serialization;
using CryEngine.Initialization;
using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using Xunit;

namespace CryBrary.Tests.Serialization
{
	public class ScriptsTests : CryBraryTests
	{
		[Fact]
		public void Serialize_Native_Scripts()
		{
			var serializer = new CrySerializer();

			using(var stream = new MemoryStream())
			{
				var scriptManager = new ScriptManager();

				scriptManager.AddScriptInstance(new NativeEntity(1, IntPtr.Zero), ScriptType.Entity);
				scriptManager.AddScriptInstance(new NativeEntity(2, IntPtr.Zero), ScriptType.Entity);
				scriptManager.AddScriptInstance(new NativeActor(3), ScriptType.Actor);

				serializer.Serialize(stream, scriptManager.Scripts);

				scriptManager.Scripts = serializer.Deserialize(stream) as List<CryScript>;
				Assert.NotNull(scriptManager.Scripts);
				Assert.Equal(3, scriptManager.Scripts.Count);

				var entityScript = scriptManager.FindScript(ScriptType.Entity, x => x.Type == typeof(NativeEntity));
				Assert.NotSame(default(CryScript), entityScript);
				Assert.NotNull(entityScript.ScriptInstances);

				Assert.Equal<int>(1, (entityScript.ScriptInstances[0] as NativeEntity).Id);
				Assert.Equal<int>(2, (entityScript.ScriptInstances[1] as NativeEntity).Id);

				var actorScript = scriptManager.FindScript(ScriptType.Entity, x => x.Type == typeof(NativeActor));
				Assert.NotSame(default(CryScript), actorScript);
				Assert.NotNull(actorScript.ScriptInstances);

				Assert.Equal<int>(3, (actorScript.ScriptInstances[0] as NativeActor).Id);
			}
		}
        
        [Fact]
        public void Serialize_CryScript_List()
        {
            var serializer = new CrySerializer();

            using (var stream = new MemoryStream())
            {
                var list = new List<CryScript>();

				CryScript script;
				if (CryScript.TryCreate(typeof(NativeActor), out script))
				{
					script.ScriptInstances = new List<CryScriptInstance>();
					script.ScriptInstances.Add(new NativeActor(759));
					script.ScriptInstances.Add(new NativeActor(5));

					list.Add(script);
				}

				if (CryScript.TryCreate(typeof(NativeEntity), out script))
				{
					script.ScriptInstances = new List<CryScriptInstance>();
					script.ScriptInstances.Add(new NativeEntity(987, IntPtr.Zero));
					script.ScriptInstances.Add(new NativeEntity(8, IntPtr.Zero));
					script.ScriptInstances.Add(null);

					list.Add(script);
				}

                serializer.Serialize(stream, list);

                serializer = new CrySerializer();
                list = serializer.Deserialize(stream) as List<CryScript>;

                Assert.NotNull(list);
                Assert.NotEmpty(list);

                Assert.Equal(2, list.Count);

                var nativeActorScript = list.ElementAt(0);
                Assert.NotNull(nativeActorScript.ScriptInstances);
                Assert.Equal(2, nativeActorScript.ScriptInstances.Count);

                Assert.NotNull(nativeActorScript.ScriptInstances.ElementAt(0));
                Assert.Equal<int>(759, (nativeActorScript.ScriptInstances.ElementAt(0) as EntityBase).Id);

                Assert.NotNull(nativeActorScript.ScriptInstances.ElementAt(1));
                Assert.Equal<int>(5, (nativeActorScript.ScriptInstances.ElementAt(1) as EntityBase).Id);

               var  nativeEntityScript = list.ElementAt(1);

                Assert.NotNull(nativeEntityScript.ScriptInstances);
                Assert.Equal(3, nativeEntityScript.ScriptInstances.Count);

                Assert.NotNull(nativeEntityScript.ScriptInstances.ElementAt(0));
                Assert.Equal<int>(987, (nativeEntityScript.ScriptInstances.ElementAt(0) as EntityBase).Id);

                Assert.NotNull(nativeEntityScript.ScriptInstances.ElementAt(1));
                Assert.Equal<int>(8, (nativeEntityScript.ScriptInstances.ElementAt(1) as EntityBase).Id);

                Assert.Null(nativeEntityScript.ScriptInstances.ElementAt(2));
            }
        }

        public void DelayedMethod() { }

        class TestEntity : Entity
        {
            public TestEntity() { delayedFunc = new DelayedFunc(MyDelayedFunc, 1337); }
            void MyDelayedFunc() { }

            DelayedFunc delayedFunc;
        }

        [Fact]
        public void Entity_With_DelayedFunc()
        {
            var serializer = new CrySerializer();

            using (var stream = new MemoryStream())
            {
				var scriptManager = new ScriptManager();

				scriptManager.AddScriptInstance(new TestEntity(), ScriptType.Entity);

				serializer.Serialize(stream, scriptManager.Scripts);

				scriptManager.Scripts = serializer.Deserialize(stream) as List<CryScript>;
            }
        }
	}
}
