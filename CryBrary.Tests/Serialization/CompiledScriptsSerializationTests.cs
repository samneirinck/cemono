using System.Collections.Generic;
using System.IO;
using System.Linq;
using NUnit.Framework;

using CryEngine;
using CryEngine.Initialization;
using CryEngine.Serialization;

namespace CryBrary.Tests.Serialization
{
	[TestFixture]
	public class CompiledScriptsSerializationTests : CryBraryTests
	{
		class Player : Actor { }

		class MyEntity : Entity { }

		abstract class AbstractEntity : Entity { }
		class EntityFromAbstract : AbstractEntity { }

		[GameRules(Default = true)]
		class Gamemode : GameRules { }

		[Test]
		public void TestCase_ScriptManager_CompiledScripts_Bug()
		{
			var scriptManager = new ScriptManager();

			ScriptManager.ProcessType(typeof(MyEntity));
			ScriptManager.ProcessType(typeof(AbstractEntity));
			ScriptManager.ProcessType(typeof(EntityFromAbstract));

			ScriptManager.AddScriptInstance(new MyEntity(), ScriptType.Entity);
			ScriptManager.AddScriptInstance(new NativeEntity(), ScriptType.Entity);

			using(var stream = new MemoryStream())
			{
				var serializer = new CrySerializer();

				serializer.Serialize(stream, ScriptManager.CompiledScripts);

				scriptManager = null;
				serializer = null;

				serializer = new CrySerializer();

				ScriptManager.CompiledScripts = serializer.Deserialize(stream) as Dictionary<ScriptType, List<CryScript>>;
				Assert.IsNotNull(ScriptManager.CompiledScripts);

				var entityScripts = ScriptManager.CompiledScripts[ScriptType.Entity];
				Assert.IsNotNull(entityScripts);
				Assert.IsNotEmpty(entityScripts);

				var myEntityScript = ScriptManager.FirstOrDefaultScript(ScriptType.Entity, x => x.Type == typeof(MyEntity));
				Assert.AreNotEqual(myEntityScript, default(CryScript));
				Assert.IsNotNull(myEntityScript.ScriptInstances);
				Assert.IsNotEmpty(myEntityScript.ScriptInstances);
				Assert.IsNotNull(myEntityScript.ScriptInstances[0]);
				Assert.AreNotEqual(0, myEntityScript.ScriptInstances[0].ScriptId);

				var nativeEntityScript = ScriptManager.FirstOrDefaultScript(ScriptType.Entity, x => x.Type == typeof(NativeEntity));
				Assert.AreNotEqual(nativeEntityScript, default(CryScript));
				Assert.IsNotNull(nativeEntityScript.ScriptInstances);
				Assert.IsNotEmpty(nativeEntityScript.ScriptInstances);
				Assert.IsNotNull(nativeEntityScript.ScriptInstances[0]);
				Assert.AreNotEqual(0, nativeEntityScript.ScriptInstances[0].ScriptId);
			}
		}
	}
}
