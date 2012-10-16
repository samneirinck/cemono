using System;
using System.Collections.Generic;

using CryEngine.Initialization;
using CryEngine.Extensions;
using CryEngine.Native;

namespace CryEngine
{
	public partial class Entity
	{
		/// <summary>
		/// Spawns a new entity
		/// </summary>
		/// <param name="entityName"></param>
		/// <param name="type"></param>
		/// <param name="pos"></param>
		/// <param name="rot"></param>
		/// <param name="scale"></param>
		/// <param name="autoInit"></param>
		/// <param name="flags"></param>
		/// <returns></returns>
		public static Entity Spawn(string entityName, Type type, Vec3? pos = null, Quat? rot = null, Vec3? scale = null, bool autoInit = true, EntityFlags flags = EntityFlags.CastShadow)
		{
			return Spawn(entityName, type.Name, pos, rot, scale, autoInit, flags);
		}

		/// <summary>
		/// Spawns a new entity
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="entityName"></param>
		/// <param name="pos"></param>
		/// <param name="rot"></param>
		/// <param name="scale"></param>
		/// <param name="autoInit"></param>
		/// <param name="flags"></param>
		/// <returns></returns>
		public static T Spawn<T>(string entityName, Vec3? pos = null, Quat? rot = null, Vec3? scale = null, bool autoInit = true, EntityFlags flags = EntityFlags.CastShadow) where T : Entity, new()
		{
			return Spawn(entityName, typeof(T).Name, pos, rot, scale, autoInit, flags) as T;
		}

		/// <summary>
		/// Spawns a new entity
		/// </summary>
		/// <param name="entityName"></param>
		/// <param name="className"></param>
		/// <param name="pos"></param>
		/// <param name="rot"></param>
		/// <param name="scale"></param>
		/// <param name="autoInit"></param>
		/// <param name="flags"></param>
		/// <returns></returns>
		public static Entity Spawn(string entityName, string className, Vec3? pos = null, Quat? rot = null, Vec3? scale = null, bool autoInit = true, EntityFlags flags = EntityFlags.CastShadow)
		{
			EntityInfo info;

			var ent = NativeMethods.Entity.SpawnEntity(new EntitySpawnParams { Name = entityName, Class = className, Pos = pos ?? new Vec3(1, 1, 1), Rot = rot ?? Quat.Identity, Scale = scale ?? new Vec3(1, 1, 1), Flags = flags }, autoInit, out info) as Entity;
			if (ent != null)
				return ent;
			else if (info.Id != 0)
				return CreateNativeEntity(info.Id, info.IEntityPtr) as Entity;

			Debug.LogAlways("[Entity.Spawn] Failed to spawn entity of class {0} with name {1}", className, entityName);
			return null;
		}

		/// <summary>
		/// Removes the entity with the specified id.
		/// </summary>
		/// <param name="id"></param>
		/// <param name="forceRemoveNow"></param>
		public static void Remove(EntityId id, bool forceRemoveNow = false)
		{
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
			if(id == 0)
				throw new ArgumentException("entityId cannot be 0!");
#endif
			if (!NativeMethods.Entity.GetFlags(NativeMethods.Entity.GetEntity(id)).HasFlag(EntityFlags.NoSave))
				throw new EntityRemovalException("Attempted to remove an entity placed via Editor");

			NativeMethods.Entity.RemoveEntity(id, forceRemoveNow);
		}

		internal static bool InternalRemove(EntityId id)
		{
			int numRemoved = ScriptManager.Instance.RemoveInstances(ScriptType.Entity, instance =>
				{
					var entity = instance as EntityBase;
					if (entity != null && entity.Id == id)
					{
						if (entity is Entity)
							return (entity as Entity).OnRemove();
						else
							return true;
					}

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
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
			if(entityId == 0)
				throw new ArgumentException("entityId cannot be 0!");
#endif

			return ScriptManager.Instance.Find<T>(ScriptType.Entity, x => x.Id == entityId);
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
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
			if(entityId == 0)
				throw new ArgumentException("entityId cannot be 0!");
#endif

			var ent = Get<EntityBase>(entityId);
			if(ent != null)
				return ent;

			// Couldn't find a CryMono entity, check if a non-managed one exists.
            var entPointer = NativeMethods.Entity.GetEntity(entityId);
			if(entPointer != IntPtr.Zero)
				return CreateNativeEntity(entityId, entPointer);

			return null;
		}

		internal static EntityBase CreateNativeEntity(EntityId id, IntPtr entityPointer)
		{
			// check if actor
            var actorInfo = NativeMethods.Actor.GetActorInfoById((uint)id._value);
			if(actorInfo.Id != 0)
				return Actor.CreateNativeActor(actorInfo);

			var nativeEntity = new NativeEntity(id, entityPointer);
			ScriptManager.Instance.AddScriptInstance(nativeEntity, ScriptType.Entity);

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
            var id = NativeMethods.Entity.FindEntity(name);
			if(id == 0)
				return null;

			return Get(new EntityId(id));
		}

		/// <summary>
		/// Gets an array of entities that are of a given class.
		/// </summary>
		/// <param name="className">The entity class to search for.</param>
		/// <returns>An array of entities.</returns>
		public static IEnumerable<EntityBase> GetByClass(string className)
		{
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
			if(String.IsNullOrEmpty(className))
				throw new ArgumentException("className should not be null or empty", "className");
#endif

            return GetEntitiesCommon<Entity>(NativeMethods.Entity.GetEntitiesByClass(className));
		}

		/// <summary>
		/// Gets an array of entities that are of a given class.
		/// </summary>
		/// <typeparam name="T">The entity class to search for.</typeparam>
		/// <returns>An array of entities of type T.</returns>
		public static IEnumerable<T> GetByClass<T>() where T : EntityBase
		{
            return GetEntitiesCommon<T>(NativeMethods.Entity.GetEntitiesByClass(typeof(T).Name));
		}

		/// <summary>
		/// Gets a list of entities within the specified area.
		/// </summary>
		/// <param name="bbox"></param>
		/// <param name="flags"></param>
		/// <returns></returns>
		public static IEnumerable<EntityBase> GetInBox(BoundingBox bbox, EntityQueryFlags flags = EntityQueryFlags.All)
		{
			return GetEntitiesCommon<EntityBase>(NativeMethods.Entity.GetEntitiesInBox(bbox, flags));
		}

		/// <summary>
		/// Gets a list of entities within the specified area.
		/// </summary>
		/// <param name="bbox"></param>
		/// <param name="flags"></param>
		/// <returns></returns>
		public static IEnumerable<T> GetInBox<T>(BoundingBox bbox, EntityQueryFlags flags = EntityQueryFlags.All) where T : EntityBase
		{
            return GetEntitiesCommon<T>(NativeMethods.Entity.GetEntitiesInBox(bbox, flags));
		}

		internal static IEnumerable<T> GetEntitiesCommon<T>(object[] ents) where T : EntityBase
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
			IAnimatedCharacterPtr = IntPtr.Zero;
		}

		public IntPtr IEntityPtr;
		public IntPtr IAnimatedCharacterPtr;
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
		/// This entity will not be saved.
		/// </summary>
		NoSave = (1 << 15),

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
		public Quat Rot;
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
}