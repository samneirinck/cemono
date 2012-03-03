using System;
using System.Runtime.CompilerServices;
using System.Reflection;

using System.Collections.Generic;

using System.Linq;

namespace CryEngine
{
    public partial class EntitySystem
	{
		/// <summary>
		/// Spawns an entity with the specified parameters.
		/// </summary>
		/// <param name="spawnParams">The <see cref="EntitySpawnParams"/></param>
		/// <param name="autoInit">Should the entity automatically be initialised?</param>
		/// <returns></returns>
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static uint _SpawnEntity(EntitySpawnParams spawnParams, bool autoInit = true);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _RemoveEntity(uint entityId);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static bool _RegisterEntityClass(EntityRegisterParams registerParams, object[] properties);

		/// <summary>
		/// Register a new entity type.
		/// </summary>
		/// <param name="config">The Entity configuration.</param>
		/// <returns>True if registration succeeded, otherwise false.</returns>
        public static bool RegisterEntityClass(EntityConfig config)
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
		public static T SpawnEntity<T>(string name, Vec3 pos, Vec3? rot = null, Vec3? scale = null, bool autoInit = true, EntityFlags flags = EntityFlags.CastShadow) where T : StaticEntity
		{
			spawnedEntities.Add(GetEntity(_SpawnEntity(new EntitySpawnParams { Name = name, Class = typeof(T).Name, Pos = pos, Rot = rot ?? new Vec3(0, 0, 0), Scale = scale ?? new Vec3(1, 1, 1), Flags = flags }, autoInit)));

			return spawnedEntities.Last() as T;
		}

		public static void RemoveEntity(EntityId id)
		{
			_RemoveEntity(id);

			spawnedEntities.RemoveAll(entity => entity.Id == id);
		}

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static uint _FindEntity(string name);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static object[] _GetEntitiesByClass(string className);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static bool _EntityExists(uint entityId);

		/// <summary>
		/// Get an entity by its unique ID.
		/// </summary>
		/// <param name="entityId">The ID as an unsigned integer.</param>
		/// <returns>A reference to the entity.</returns>
		/// <remarks>If the entity does not exist in the managed space, this function will attempt to find
		/// a C++ entity with the specified ID></remarks>
		public static StaticEntity GetEntity(EntityId entityId)
        {
			if (entityId == 0)
				return null;

			StaticEntity ent = spawnedEntities.Find(entity => entity.Id == entityId);
			if (ent != default(StaticEntity))
				return ent;

			if(_EntityExists(entityId))
				return new StaticEntity(entityId);

            return null;
        }

		/// <summary>
		/// Get an entity by name.
		/// </summary>
		/// <param name="name">The name of the entity.</param>
		/// <returns>A reference to the entity.</returns>
		/// <remarks>If multiple entities have the same name, it will return the first found.
		/// Consider using IDs where necessary.</remarks>
        public static StaticEntity GetEntity(string name)
        {
            return GetEntity(_FindEntity(name));
        }

		/// <summary>
		/// Gets an array of entities that are of a given class.
		/// </summary>
		/// <param name="className">The entity class to search for.</param>
		/// <returns>An array of entities.</returns>
        public static IEnumerable<StaticEntity> GetEntities(string className)
        {
            var entitiesByClass = _GetEntitiesByClass(className);
            if (entitiesByClass == null)
                return null;

            var entities = new StaticEntity[entitiesByClass.Length];

            for (int i = 0; i < entitiesByClass.Length; i++)
                entities[i] = GetEntity((uint)entitiesByClass[i]);

		    return entities;
        }

		/// <summary>
		/// Gets an array of entities that are of a given class.
		/// </summary>
		/// <typeparam name="T">The entity class to search for.</typeparam>
		/// <returns>An array of entities of type T.</returns>
		public static IEnumerable<T> GetEntities<T>() where T : StaticEntity
		{
			var results = GetEntities(typeof(T).Name);

			if(results != null && results.Count() > 0)
				return results.Cast<T>();

			return null;
		}

        internal static void RegisterInternalEntity(StaticEntity entity)
        {
			if (!spawnedEntities.Contains(entity))
				spawnedEntities.Add(entity);
			else
				throw new Exception("Attempted to register internal entity twice.");
        }

		internal static void OnUpdate()
		{
			foreach (var entity in spawnedEntities)
			{
				if (entity != null && entity.ReceiveUpdates==true)
					entity.OnUpdate();
			}
		}
        
		/// <summary>
		/// Contains entities spawned using EntitySystem.SpawnEntity.
		/// Necessary to update scripts.
		/// </summary>
		static List<StaticEntity> spawnedEntities = new List<StaticEntity>();
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

    public struct EntityConfig
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

    public struct EntityRegisterParams
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