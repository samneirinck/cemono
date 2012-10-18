using System;
using Xunit;

using CryEngine;
using CryEngine.Initialization;

namespace CryBrary.Tests.Scripts
{
	public class ActorTests : CryBraryTests
	{
		public class MyActor : Actor { }

		[Fact]
		public void Get_Mono_Actor_By_Id_EntityGeneric()
		{
			var scriptManager = new ScriptManager();
			ScriptManager.Instance = scriptManager;

			var actor = new MyActor();
			actor.InternalSpawn(new ActorInfo { Id = 13 }, 3);
			scriptManager.AddScriptInstance(actor, ScriptType.Actor);

			var retrievedActor = Actor.Get<MyActor>((EntityId)13);
			Assert.NotNull(retrievedActor);
			Assert.Same(retrievedActor, actor);
		}

		[Fact]
		public void Get_Mono_Actor_By_EntityId()
		{
			var scriptManager = new ScriptManager();
			ScriptManager.Instance = scriptManager;

			var actor = new MyActor();
			actor.InternalSpawn(new ActorInfo { Id = 21 }, 4);
			scriptManager.AddScriptInstance(actor, ScriptType.Actor);

			var retrievedActor = Actor.Get((EntityId)21);
			Assert.NotNull(retrievedActor);
			Assert.Same(retrievedActor, actor);
		}

		[Fact]
		public void Get_Mono_Actor_By_ChannelId_Generic()
		{
			var scriptManager = new ScriptManager();
			ScriptManager.Instance = scriptManager;

			var actor = new MyActor();
			actor.InternalSpawn(new ActorInfo { Id = 47 }, 2);
			scriptManager.AddScriptInstance(actor, ScriptType.Actor);

			var retrievedActor = Actor.Get<MyActor>(2);
			Assert.NotNull(retrievedActor);
			Assert.Same(retrievedActor, actor);
		}

		[Fact]
		public void Get_Mono_Actor_By_ChannelId()
		{
			var scriptManager = new ScriptManager();
			ScriptManager.Instance = scriptManager;

			var actor = new MyActor();
			actor.InternalSpawn(new ActorInfo { Id = 67 }, 23);
			scriptManager.AddScriptInstance(actor, ScriptType.Actor);

			var retrievedActor = Actor.Get(23);
			Assert.NotNull(retrievedActor);
			Assert.Same(retrievedActor, actor);
		}

		public class MyNativeActor : NativeActor { }

		[Fact]
		public void Get_Native_Actor_By_Id_EntityGeneric()
		{
			var scriptManager = new ScriptManager();
			ScriptManager.Instance = scriptManager;

			var actor = new MyNativeActor();
			actor.InternalSpawn(new ActorInfo { Id = 13 }, 3);
			scriptManager.AddScriptInstance(actor, ScriptType.Actor);

			var retrievedActor = Actor.Get<MyNativeActor>((EntityId)13);
			Assert.NotNull(retrievedActor);
			Assert.Same(retrievedActor, actor);
		}

		[Fact]
		public void Get_Native_Actor_By_EntityId()
		{
			var scriptManager = new ScriptManager();
			ScriptManager.Instance = scriptManager;

			var actor = new MyNativeActor();
			actor.InternalSpawn(new ActorInfo { Id = 21 }, 4);
			scriptManager.AddScriptInstance(actor, ScriptType.Actor);

			var retrievedActor = Actor.Get((EntityId)21);
			Assert.NotNull(retrievedActor);
			Assert.Same(retrievedActor, actor);
		}

		[Fact]
		public void Get_Native_Actor_By_ChannelId_Generic()
		{
			var scriptManager = new ScriptManager();
			ScriptManager.Instance = scriptManager;

			var actor = new MyNativeActor();
			actor.InternalSpawn(new ActorInfo { Id = 47 }, 2);
			scriptManager.AddScriptInstance(actor, ScriptType.Actor);

			var retrievedActor = Actor.Get<MyNativeActor>(2);
			Assert.NotNull(retrievedActor);
			Assert.Same(retrievedActor, actor);
		}

		[Fact]
		public void Get_Native_Actor_By_ChannelId()
		{
			var scriptManager = new ScriptManager();
			ScriptManager.Instance = scriptManager;

			var actor = new MyNativeActor();
			actor.InternalSpawn(new ActorInfo { Id = 67 }, 23);
			scriptManager.AddScriptInstance(actor, ScriptType.Actor);

			var retrievedActor = Actor.Get(23);
			Assert.NotNull(retrievedActor);
			Assert.Same(retrievedActor, actor);
		}
	}
}
