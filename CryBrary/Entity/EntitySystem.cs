using System;

using System.Collections.Generic;

using System.Linq;

using CryEngine.Initialization;

namespace CryEngine
{
	public partial class Entity
	{
		/// <summary>
		/// Spawn a new instance of entity type T.
		/// </summary>
		/// <typeparam name="T">The entity type</typeparam>
		/// <param name="name"></param>
		/// <param name="pos"></param>
		/// <param name="rot"></param>
		/// <param name="scale"></param>
		/// <param name="autoInit"></param>
		/// <returns></returns>
		public static T Spawn<T>(string name, Vec3 pos, Vec3? rot = null, Vec3? scale = null, bool autoInit = true, EntityFlags flags = EntityFlags.CastShadow) where T : Entity, new()
		{
			var entInfo = _SpawnEntity(new EntitySpawnParams { Name = name, Class = typeof(T).Name, Pos = pos, Rot = rot ?? Vec3.Zero, Scale = scale ?? new Vec3(1, 1, 1), Flags = flags }, autoInit);
			if(entInfo.Id != 0)
			{
				var ent = new T();
				if(ent == null)
				{
					Debug.LogAlways("[Entity.Spawn] Failed to add script instance");
					return null;
				}

				ScriptManager.AddScriptInstance(ent, ScriptType.Entity);
				ent.InternalSpawn(entInfo);

				return ent as T;
			}

			return null;
		}

		public static void Remove(EntityId id)
		{
			_RemoveEntity(id);

			InternalRemove(id);
		}

		public void Remove()
		{
			Entity.Remove(Id);
		}

		internal static void InternalRemove(EntityId id)
		{
			foreach(var script in ScriptManager.CompiledScripts[ScriptType.Entity])
			{
				if(script.ScriptInstances != null)
					script.ScriptInstances.RemoveAll(instance => instance is Entity && (instance as Entity).Id == id);
			}
		}

		/// <summary>
		/// Get an entity by its unique ID.
		/// </summary>
		/// <param name="entityId">The ID as an unsigned integer.</param>
		/// <returns>A reference to the entity.</returns>
		/// <remarks>If the entity does not exist in the managed space, this function will attempt to find
		/// a C++ entity with the specified ID></remarks>
		public static T Get<T>(EntityId entityId) where T : Entity
		{
			if(entityId == 0)
				throw new ArgumentException("entityId cannot be 0!");

			return ScriptManager.FindScriptInstance<T>(x => x.Id == entityId);
		}

		/// <summary>
		/// Get an entity by its unique ID.
		/// </summary>
		/// <param name="entityId">The ID as an unsigned integer.</param>
		/// <returns>A reference to the entity.</returns>
		/// <remarks>If the entity does not exist in the managed space, this function will attempt to find
		/// a C++ entity with the specified ID></remarks>
		public static Entity Get(EntityId entityId)
		{
			var ent = Get<Entity>(entityId);
			if(ent != null)
				return ent;

			// Couldn't find a CryMono entity, check if a non-managed one exists.
			var entPointer = _GetEntity(entityId);
			if(entPointer != null)
			{
				var script = ScriptManager.CompiledScripts[ScriptType.Entity].First(x => x.Type == typeof(NativeEntity));
				if(script == null)
					throw new TypeLoadException("Failed to locate NativeEntity type");

				var nativeEntity = new NativeEntity(entityId, entPointer);
				ScriptManager.AddScriptInstance(nativeEntity, script);

				return nativeEntity;
			}

			return null;
		}

		/// <summary>
		/// Get an entity by name.
		/// </summary>
		/// <param name="name">The name of the entity.</param>
		/// <returns>A reference to the entity.</returns>
		/// <remarks>If multiple entities have the same name, it will return the first found.
		/// Consider using IDs where necessary.</remarks>
		public static Entity Find(string name)
		{
			return Get(new EntityId(_FindEntity(name)));
		}

		/// <summary>
		/// Gets an array of entities that are of a given class.
		/// </summary>
		/// <param name="className">The entity class to search for.</param>
		/// <returns>An array of entities.</returns>
		public static IEnumerable<Entity> GetEntities(string className)
		{
			return GetEntitiesCommon<Entity>(className);
		}

		/// <summary>
		/// Gets an array of entities that are of a given class.
		/// </summary>
		/// <typeparam name="T">The entity class to search for.</typeparam>
		/// <returns>An array of entities of type T.</returns>
		public static IEnumerable<T> GetEntities<T>() where T : Entity
		{
			return GetEntitiesCommon<T>(typeof(T).Name);
		}

		internal static IEnumerable<T> GetEntitiesCommon<T>(string className) where T : Entity
		{
			if(String.IsNullOrEmpty(className))
				throw new ArgumentException("className should not be null or empty", "className");

			var entitiesByClass = _GetEntitiesByClass(className);
			if(entitiesByClass == null || entitiesByClass.Length <= 0)
				yield break;

			foreach(EntityId id in entitiesByClass)
			{
				var ent = Get<T>(id);
				if(ent != null)
					yield return ent;
			}
		}
	}

	internal struct EntityInfo
	{
		public EntityInfo(IntPtr ptr, EntityId id)
		{
			IEntityPtr = ptr;
			Id = id;
		}

		public IntPtr IEntityPtr;
		public EntityId Id;
	}

	/// <summary>
	/// These flags control entity instance behaviour.
	/// </summary>
	[Flags]
	public enum EntityFlags
	{
		CastShadow = (1 << 1),
		Unremovable = (1 << 2),

		ClientOnly = (1 << 8),
		ServerOnly = (1 << 9),
	}

	public struct EntitySpawnParams
	{
		public string Name;
		public string Class;

		public Vec3 Pos;
		public Vec3 Rot;
		public Vec3 Scale;

		public EntityFlags Flags;
	}

	/// <summary>
	/// These flags define behaviour for entity classes.
	/// </summary>
	[Flags]
	public enum EntityClassFlags
	{
		/// <summary>
		/// If set this class will not be visible in editor,and entity of this class cannot be placed manually in editor.
		/// </summary>
		Invisible = 0x0001,
		/// <summary>
		/// If this is default entity class.
		/// </summary>
		Default = 0x0002,
	}

	struct EntityConfig
	{
		public EntityConfig(EntityRegisterParams _params, object[] props)
			: this()
		{
			registerParams = _params;
			properties = props;
		}

		/// <summary>
		/// The registration information.
		/// </summary>
		public EntityRegisterParams registerParams;
		/// <summary>
		/// The properties that will be displayed inside Sandbox.
		/// </summary>
		public object[] properties;
	}

	struct EntityRegisterParams
	{
		public EntityRegisterParams(string helper, string icon, EntityClassFlags flags)
			: this()
		{
			EditorHelper = helper;
			EditorIcon = icon;

			Flags = flags;
		}

		public EntityRegisterParams(string name, string category, string helper, string icon, EntityClassFlags flags)
			: this()
		{
			Name = name;
			Category = category;

			EditorHelper = helper;
			EditorIcon = icon;

			Flags = flags;
		}

		public string Name;
		public string Category;

		public string EditorHelper;
		public string EditorIcon;

		public EntityClassFlags Flags;
	}
}