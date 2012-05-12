using System;
using System.Runtime.CompilerServices;
using System.Reflection;

using System.Collections.Generic;
using System.Collections.ObjectModel;

using System.Linq;

using CryEngine.Initialization;
using CryEngine.Extensions;

namespace CryEngine
{
    public partial class Entity
	{
		/// <summary>
		/// Register a new entity type.
		/// </summary>
		/// <param name="config">The Entity configuration.</param>
		/// <returns>True if registration succeeded, otherwise false.</returns>
        internal static bool RegisterClass(EntityConfig config)
        {
            return _RegisterEntityClass(config.registerParams, config.properties);
        }

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
		public static T Spawn<T>(string name, Vec3 pos, Vec3? rot = null, Vec3? scale = null, bool autoInit = true, EntityFlags flags = EntityFlags.CastShadow) where T : Entity
		{
			var entId = new EntityId(_SpawnEntity(new EntitySpawnParams { Name = name, Class = typeof(T).Name, Pos = pos, Rot = rot ?? Vec3.Zero, Scale = scale ?? new Vec3(1, 1, 1), Flags = flags }, autoInit));

			return ScriptManager.AddScriptInstance(Get(entId)) as T;
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
			foreach(var script in ScriptManager.CompiledScripts)
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

			return Get(ent => ent is T && ent.Id == entityId) as T;
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
			if(_EntityExists(entityId))
			{
				int scriptIndex;
				var script = ScriptManager.GetScriptByType(typeof(NativeEntity), out scriptIndex);

				if(script.ScriptInstances == null)
					script.ScriptInstances = new List<CryScriptInstance>();

				script.ScriptInstances.Add(new NativeEntity(entityId));

				ScriptManager.CompiledScripts[scriptIndex] = script;

				return script.ScriptInstances.Last() as Entity;
			}

			return null;
		}

		/// <summary>
		/// Searches for an entity that matches the conditions defined by the specified predicate.
		/// </summary>
		/// <param name="match">The System.Predicate<Entity> that defines the conditions of the element to search for.</param>
		/// <returns>The first element matching the specified predicate.</returns>
		public static Entity Get(Predicate<Entity> match)
		{
			Entity actor = null;
			for(int i = 0; i < ScriptManager.CompiledScripts.Count; i++)
			{
				var script = ScriptManager.CompiledScripts[i];
				if(script.Type.Implements(typeof(Actor)) && script.ScriptInstances != null)
				{
					actor = script.ScriptInstances.Find(x => match(x as Entity)) as Entity;
					if(actor != null)
						return actor;
				}
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
				yield return Get<T>(id);
		}

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