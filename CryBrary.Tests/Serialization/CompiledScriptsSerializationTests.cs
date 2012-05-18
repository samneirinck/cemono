using System.Collections.Generic;
using System.IO;
using System.Linq;
using NUnit.Framework;

using CryEngine;
using CryEngine.Initialization;
using CryEngine.Serialization;

namespace CryBrary.Tests.Serialization
{
	class CompiledScriptsSerializationTests : CryBraryTests
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

			scriptManager.ProcessType(typeof(MyEntity));
			scriptManager.ProcessType(typeof(AbstractEntity));
			scriptManager.ProcessType(typeof(EntityFromAbstract));

			using(var stream = new MemoryStream())
			{
				var serializer = new CrySerializer();

				serializer.Serialize(stream, ScriptManager.CompiledScripts);

				scriptManager = null;
				serializer = null;

				serializer = new CrySerializer();

				ScriptManager.CompiledScripts = serializer.Deserialize(stream) as Dictionary<ScriptType, List<CryScript>>;
			}
		}
	}
}
