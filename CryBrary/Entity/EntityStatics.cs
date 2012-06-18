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
			EntityInfo info;
			if(_SpawnEntity(new EntitySpawnParams { Name = name, Class = typeof(T).Name, Pos = pos, Rot = rot ?? Vec3.Zero, Scale = scale ?? new Vec3(1, 1, 1), Flags = flags }, autoInit, out info))
			{
				var ent = new T();

				ScriptManager.AddScriptInstance(ent, ScriptType.Entity);
				ent.InternalSpawn(info);

				return ent as T;
			}
			else
				Debug.LogAlways("[Entity.Spawn] Failed to spawn entity of class {0} with name {1}", typeof(T).Name, name);

			return null;
		}

		public static void Remove(EntityId id)
		{
			if(id == 0)
				throw new ArgumentException("entityId cannot be 0!");

			_RemoveEntity(id);
		}

		public void Remove()
		{
			Entity.Remove(Id);
		}

		internal static bool InternalRemove(EntityId id)
		{
			int numRemoved = ScriptManager.RemoveInstances(ScriptType.Entity, instance =>
				{
					var entity = instance as Entity;
							if(entity != null && entity.Id == id && entity.OnRemove())
								return true;

							return false;
				});

			return numRemoved > 0;
		}

		/// <summary>
		/// Get an entity by its unique ID.
		/// </summary>
		/// <param name="entityId">The ID as an unsigned integer.</param>
		/// <returns>A reference to the entity.</returns>
		/// <remarks>If the entity does not exist in the managed space, this function will attempt to find
		/// a C++ entity with the specified ID></remarks>
		public static T Get<T>(EntityId entityId) where T : EntityBase
		{
			if(entityId == 0)
				throw new ArgumentException("entityId cannot be 0!");

			return ScriptManager.Find<T>(ScriptType.Entity, x => x.Id == entityId);
		}

		/// <summary>
		/// Get an entity by its unique ID.
		/// </summary>
		/// <param name="entityId">The ID as an unsigned integer.</param>
		/// <returns>A reference to the entity.</returns>
		/// <remarks>If the entity does not exist in the managed space, this function will attempt to find
		/// a C++ entity with the specified ID></remarks>
		public static EntityBase Get(EntityId entityId)
		{
			if(entityId == 0)
				throw new ArgumentException("entityId cannot be 0!");

			var ent = Get<EntityBase>(entityId);
			if(ent != null)
				return ent;

			// Couldn't find a CryMono entity, check if a non-managed one exists.
			var entPointer = _GetEntity(entityId);
			if(entPointer != IntPtr.Zero)
				return CreateNativeEntity(entityId, entPointer);

			return null;
		}

		internal static EntityBase CreateNativeEntity(EntityId id, IntPtr entityPointer)
		{
			// check if actor
			var actorInfo = Actor._GetActorInfoById((uint)id._value);
			if(actorInfo.Id != 0)
				return Actor.CreateNativeActor(actorInfo);

			var nativeEntity = new NativeEntity(id, entityPointer);
			ScriptManager.AddScriptInstance(nativeEntity, ScriptType.Entity);

			return nativeEntity;
		}

		/// <summary>
		/// Get an entity by name.
		/// </summary>
		/// <param name="name">The name of the entity.</param>
		/// <returns>A reference to the entity.</returns>
		/// <remarks>If multiple entities have the same name, it will return the first found.
		/// Consider using IDs where necessary.</remarks>
		public static EntityBase Find(string name)
		{
			var id = _FindEntity(name);
			if(id == 0)
				return null;

			return Get(new EntityId(id));
		}

		/// <summary>
		/// Gets an array of entities that are of a given class.
		/// </summary>
		/// <param name="className">The entity class to search for.</param>
		/// <returns>An array of entities.</returns>
		public static IEnumerable<Entity> GetByClass(string className)
		{
			if(String.IsNullOrEmpty(className))
				throw new ArgumentException("className should not be null or empty", "className");

			return GetEntitiesCommon<Entity>(_GetEntitiesByClass(className));
		}

		/// <summary>
		/// Gets an array of entities that are of a given class.
		/// </summary>
		/// <typeparam name="T">The entity class to search for.</typeparam>
		/// <returns>An array of entities of type T.</returns>
		public static IEnumerable<T> GetByClass<T>() where T : Entity
		{
			return GetEntitiesCommon<T>(_GetEntitiesByClass(typeof(T).Name));
		}

		public static IEnumerable<Entity> GetInBox(BoundingBox bbox, EntityQueryFlags flags = EntityQueryFlags.All)
		{
			return GetEntitiesCommon<Entity>(_GetEntitiesInBox(bbox, flags));
		}

		public static IEnumerable<T> GetInBox<T>(BoundingBox bbox, EntityQueryFlags flags = EntityQueryFlags.All) where T : Entity
		{
			return GetEntitiesCommon<T>(_GetEntitiesInBox(bbox, flags));
		}

		internal static IEnumerable<T> GetEntitiesCommon<T>(object[] ents) where T : Entity
		{
			if(ents == null || ents.Length <= 0)
				yield break;

			foreach(EntityId id in ents)
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
		public uint Id;
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

		/// <summary>
		/// Entity will trigger areas when it enters them.
		/// </summary>
		TriggerAreas = (1 << 14),

		/// <summary>
		/// Entity was spawned dynamically without a class.
		/// </summary>
		Spawned = (1 << 24),
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