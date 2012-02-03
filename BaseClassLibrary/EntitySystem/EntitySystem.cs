using System.Runtime.CompilerServices;

using System.Collections.Specialized;

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
        extern public static uint SpawnEntity(EntitySpawnParams spawnParams, bool autoInit = true);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public static bool _RegisterEntityClass(EntityRegisterParams registerParams, object[] properties);
		/// <summary>
		/// Register a new entity type.
		/// </summary>
		/// <param name="config">The Entity configuration.</param>
		/// <returns>True if registration succeeded, otherwise false.</returns>
        public static bool RegisterEntityClass(EntityConfig config)
        {
            return _RegisterEntityClass(config.registerParams, config.properties);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern protected static uint _FindEntity(string name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern protected static object[] _GetEntitiesByClass(string className);

		/// <summary>
		/// Get an entity by its unique ID.
		/// </summary>
		/// <param name="entityId">The ID as an unsigned integer.</param>
		/// <returns>A reference to the entity.</returns>
		/// <remarks>If the entity does not exist in the managed space, this function will attempt to find
		/// a C++ entity with the specified ID></remarks>
        public static StaticEntity GetEntity(uint entityId)
        {
            if (m_internalEntities.Contains(entityId))
                return m_internalEntities[entityId] as StaticEntity;

            return new StaticEntity(entityId);
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
        public static StaticEntity[] GetEntities(string className)
        {
            object[] entitiesByClass = _GetEntitiesByClass(className);
            if (entitiesByClass == null)
                return null;

            StaticEntity[] entities = new StaticEntity[entitiesByClass.Length];

            for (int i = 0; i < entitiesByClass.Length; i++)
                entities[i] = GetEntity((uint)entitiesByClass[i]);

		    return entities;
        }

		/// <summary>
		/// Gets an array of entities that are of a given class.
		/// </summary>
		/// <typeparam name="T">The entity class to search for.</typeparam>
		/// <returns>An array of entities of type T.</returns>
		public static T[] GetEntities<T>()
		{
			return GetEntities(typeof(T).Name) as T[];
		}

        internal static void RegisterInternalEntity(uint entityId, StaticEntity entity)
        {
            m_internalEntities.Add(entityId, entity);
        }

        
        /// <summary>
        /// Contains the entities registered with mono.
        /// EntityId, Entity are stored in here. EntityId is also stored within Entity, but storing it seperately here provides for fast lookup and rids of us too many foreach loops.
        /// </summary>
        static OrderedDictionary m_internalEntities = new OrderedDictionary();
    }

	/// <summary>
	/// These flags control entity instance behaviour.
	/// </summary>
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