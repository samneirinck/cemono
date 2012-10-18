using System;
using Xunit;

using CryEngine;
using CryEngine.Initialization;

namespace CryBrary.Tests.Scripts
{
	public class EntityTests : CryBraryTests
	{
		public class MyEntity : Entity { }

		[Fact]
		public void Get_Mono_Entity_By_EntityId_Generic()
		{
			var scriptManager = new ScriptManager();
			ScriptManager.Instance = scriptManager;

			var entity = new MyEntity();
			entity.InternalSpawn(new EntityInfo { Id = 3 });
			scriptManager.AddScriptInstance(entity, ScriptType.Entity);

			var retrievedEntity = Entity.Get<MyEntity>(3);
			Assert.NotNull(retrievedEntity);
			Assert.Same(retrievedEntity, entity);
		}

		[Fact]
		public void Get_Mono_Entity_By_EntityId()
		{
			var scriptManager = new ScriptManager();
			ScriptManager.Instance = scriptManager;

			var entity = new MyEntity();
			entity.InternalSpawn(new EntityInfo { Id = 7 });
			scriptManager.AddScriptInstance(entity, ScriptType.Entity);

			var retrievedEntity = Entity.Get(7);
			Assert.NotNull(retrievedEntity);
			Assert.Same(retrievedEntity, entity);
		}

		[Fact]
		public void Get_Mono_Entity_By_EntityId_As_BaseType()
		{
			var scriptManager = new ScriptManager();
			ScriptManager.Instance = scriptManager;

			var entity = new MyEntity();
			entity.InternalSpawn(new EntityInfo { Id = 19 });
			scriptManager.AddScriptInstance(entity, ScriptType.Entity);

			var retrievedEntity = Entity.Get<Entity>(19);
			Assert.NotNull(retrievedEntity);
			Assert.Same(retrievedEntity, entity);
		}

		[Fact]
		public void Get_Mono_Entity_By_EntityId_As_BaseType2()
		{
			var scriptManager = new ScriptManager();
			ScriptManager.Instance = scriptManager;

			var entity = new MyEntity();
			entity.InternalSpawn(new EntityInfo { Id = 27 });
			scriptManager.AddScriptInstance(entity, ScriptType.Entity);

			var retrievedEntity = Entity.Get<EntityBase>(27);
			Assert.NotNull(retrievedEntity);
			Assert.Same(retrievedEntity, entity);
		}

		[Fact]
		public void Get_Native_Entity_By_EntityId_Generic()
		{
			var scriptManager = new ScriptManager();
			ScriptManager.Instance = scriptManager;

			var entity = new NativeEntity(9, IntPtr.Zero);
			scriptManager.AddScriptInstance(entity, ScriptType.Entity);

			var retrievedEntity = Entity.Get<NativeEntity>(9);
			Assert.NotNull(retrievedEntity);
			Assert.Same(retrievedEntity, entity);
		}

		[Fact]
		public void Get_Native_Entity_By_EntityId()
		{
			var scriptManager = new ScriptManager();
			ScriptManager.Instance = scriptManager;

			var entity = new NativeEntity(14, IntPtr.Zero);
			scriptManager.AddScriptInstance(entity, ScriptType.Entity);

			var retrievedEntity = Entity.Get(14);
			Assert.NotNull(retrievedEntity);
			Assert.Same(retrievedEntity, entity);
		}
	}
}
