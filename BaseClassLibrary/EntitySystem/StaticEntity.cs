using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Linq;

using System.Reflection;

using CryEngine.Extensions;

namespace CryEngine
{
	/// <summary>
	/// Static entities must inherit this in order to be registered.
	/// It is no longer an interface, due to basic functionality each entity <b>must</b> have.
	/// </summary>
	public class StaticEntity : FlowNode
	{
		#region Externals
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern protected static string _GetPropertyValue(uint entityId, string propertyName);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern protected static void _SetPropertyValue(uint entityId, string property, string value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetWorldPos(uint entityId, Vec3 newPos);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static Vec3 _GetWorldPos(uint entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetWorldAngles(uint entityId, Vec3 newAngles);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static Vec3 _GetWorldAngles(uint entityId);

		/// <summary>
		/// Loads a static model on the object (.cgf).
		/// </summary>
		/// <param name="entityId"></param>
		/// <param name="fileName"></param>
		/// <param name="slot"></param>
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _LoadObject(uint entityId, string fileName, int slot);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static string _GetStaticObjectFilePath(uint entityId, int slot);

		/// <summary>
		/// Loads an non-static model on the object (.chr, .cdf, .cga)
		/// </summary>
		/// <param name="entityId"></param>
		/// <param name="fileName"></param>
		/// <param name="slot"></param>
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _LoadCharacter(uint entityId, string fileName, int slot);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _Physicalize(uint entityId, PhysicalizationParams physicalizationParams);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _CreateGameObjectForEntity(uint entityId);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddMovement(uint entityId, ref EntityMovementRequest request);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static Vec3 _GetVelocity(uint entityId);
		#endregion

		public StaticEntity() { }

		internal StaticEntity(uint entityId)
		{
			Id = entityId;

			MonoEntity = false;
		}

		/// <summary>
		/// Initializes the entity, not recommended to set manually.
		/// </summary>
		/// <param name="entityId"></param>
		public virtual void InternalSpawn(uint entityId)
		{
			SpawnCommon(entityId);
			OnSpawn();
		}

		internal void InitPhysics()
		{
			_physics = new PhysicsParams(Id);
			_physics.Slot = 0;
		}

		internal void SpawnCommon(uint entityId)
		{
			Id = entityId;

			MonoEntity = true;
			EntitySystem.RegisterInternalEntity(entityId, this);
			Spawned = true;

			//Do this before the property overwrites
			InitPhysics();

			//TODO: Make sure that mutators are only called once on startup
			//var storedPropertyNames = storedProperties.Keys.Select(key => key[0]);

			foreach(var property in GetType().GetProperties())
			{
				EditorPropertyAttribute attr;
				if(property.TryGetAttribute(out attr) && attr.DefaultValue != null)// && !storedPropertyNames.Contains(property.Name))
					property.SetValue(this, attr.DefaultValue, null);
			}

			foreach(var field in GetType().GetFields())
			{
				EditorPropertyAttribute attr;
				if(field.TryGetAttribute(out attr) && attr.DefaultValue != null)// && !storedPropertyNames.Contains(field.Name))
					field.SetValue(this, attr.DefaultValue);
			}

			foreach(var storedProperty in storedProperties)
			{
				if(string.IsNullOrEmpty(storedProperty.Key[1]))
					continue;

				SetPropertyValue(storedProperty.Key[0], storedProperty.Value, storedProperty.Key[1]);
				Console.LogAlways("Applying serialised property {0}, value is {1}", storedProperty.Key[0], storedProperty.Key[1]);
			}

			storedProperties.Clear();
			storedProperties = null;
		}

		public static implicit operator StaticEntity(uint id)
		{
			return EntitySystem.GetEntity(id);
		}

		public static implicit operator StaticEntity(int id)
		{
			return EntitySystem.GetEntity((uint)id);
		}

		#region Methods & Fields
		public Vec3 Position { get { return _GetWorldPos(Id); } set { _SetWorldPos(Id, value); } }
		public Vec3 Rotation { get { return _GetWorldAngles(Id); } set { _SetWorldAngles(Id, value); } }

		PhysicsParams _physics;
		public PhysicsParams Physics { get { return _physics; } set { _physics = value; _physics._entity = this; } }

		public uint Id { get; set; }
		public string Name { get; set; }
		public EntityFlags Flags { get; set; }
		internal bool Spawned;

		internal bool MonoEntity;
		#endregion

		#region Callbacks
		/// <summary>
		/// This callback is called when this entity has finished spawning. The entity has been created and added to the list of entities.
		/// </summary>
		public virtual void OnSpawn() { }

		/// <summary>
		/// Called when the entity is being removed.
		/// </summary>
		/// <returns>True to allow removal, false to deny.</returns>
		public virtual bool OnRemove() { return true; }

		/// <summary>
		/// Called when resetting the state of the entity in Editor.
		/// </summary>
		/// <param name="enteringGame">true if currently entering gamemode, false if exiting.</param>
		public virtual void OnReset(bool enteringGame) { }

		/// <summary>
		/// Called when game is started (games may start multiple times)
		/// </summary>
		public virtual void OnStartGame() { }

		/// <summary>
		/// Called when the level is started.
		/// </summary>
		public virtual void OnStartLevel() { }

		/// <summary>
		/// Sent when triggering entity enters to the area proximity.
		/// </summary>
		/// <param name="triggerEntityId"></param>
		/// <param name="areaEntityId"></param>
		public virtual void OnEnterArea(uint triggerEntityId, uint areaEntityId) { }

		/// <summary>
		/// Sent when triggering entity leaves the area proximity.
		/// </summary>
		/// <param name="triggerEntityId"></param>
		/// <param name="areaEntityId"></param>
		public virtual void OnLeaveArea(uint triggerEntityId, uint areaEntityId) { }
		#endregion

		#region Overrides
		// The stash; hide all internal code we don't want anyone to see down here. *sweep sweep*
		public override bool Equals(object obj)
		{
			StaticEntity ent = obj as StaticEntity;

			return (ent == null) ? false : this.Id == ent.Id;
		}

		public override int GetHashCode()
		{
			return base.GetHashCode();
		}
		#endregion

		#region Base Logic

		protected virtual string GetPropertyValue(string propertyName)
		{
			return _GetPropertyValue(Id, propertyName);
		}

		/// <summary>
		/// Temporarily stored so we can set them properly on spawn.
		/// </summary>
		Dictionary<string[], EntityPropertyType> storedProperties;

		public virtual void SetPropertyValue(string propertyName, EntityPropertyType propertyType, string value)
		{
			if(value.Length <= 0 && propertyType != EntityPropertyType.String)
				return;

			if(!memberIsProperty.ContainsKey(propertyName))
			{
				if(GetType().GetProperty(propertyName) != null)
					memberIsProperty.Add(propertyName, true);
				else if(GetType().GetField(propertyName) != null)
					memberIsProperty.Add(propertyName, false);
				else
					throw new Exception("The specified property name does not exist. This really shouldn't happen.");
			}

			var isProperty = memberIsProperty[propertyName];

			// Store properties so we can utilize the get set functionality after opening a saved level.
			if(!Spawned && isProperty)
			{
				if(storedProperties == null)
					storedProperties = new Dictionary<string[], EntityPropertyType>();

				storedProperties.Add(new string[] { propertyName, value }, propertyType);

				return;
			}

			switch(propertyType)
			{
				case EntityPropertyType.Bool:
					{
						if(isProperty)
							GetType().GetProperty(propertyName).SetValue(this, Convert.ToBoolean(value), null);
						else
							GetType().GetField(propertyName).SetValue(this, Convert.ToBoolean(value));
					}
					break;
				case EntityPropertyType.Int:
					{
						if(isProperty)
							GetType().GetProperty(propertyName).SetValue(this, Convert.ToInt32(value), null);
						else
							GetType().GetField(propertyName).SetValue(this, Convert.ToInt32(value));
					}
					break;
				case EntityPropertyType.Float:
					{
						if(isProperty)
							GetType().GetProperty(propertyName).SetValue(this, Convert.ToSingle(value), null);
						else
							GetType().GetField(propertyName).SetValue(this, Convert.ToSingle(value));
					}
					break;
				case EntityPropertyType.Vec3:
					{
						string[] split = value.Split(',');
						var vec = new Vec3(Convert.ToSingle(split[0]), Convert.ToSingle(split[1]), Convert.ToSingle(split[2]));

						if(isProperty)
							GetType().GetProperty(propertyName).SetValue(this, vec, null);
						else
							GetType().GetField(propertyName).SetValue(this, vec);
					}
					break;
				case EntityPropertyType.String:
					{
						if(isProperty)
							GetType().GetProperty(propertyName).SetValue(this, value, null);
						else
							GetType().GetField(propertyName).SetValue(this, value);
					}
					break;
			}
		}

		/// <summary>
		/// Loads a mesh for this entity. Can optionally load multiple meshes using entity slots.
		/// </summary>
		/// <param name="name"></param>
		/// <param name="slotNumber"></param>
		/// <returns></returns>
		public bool LoadObject(string name, int slotNumber = 0)
		{
			if(name.EndsWith("cgf"))
				_LoadObject(Id, name, slotNumber);
			else if(name.EndsWith("cdf") || name.EndsWith("cga") || name.EndsWith("cga"))
				_LoadCharacter(Id, name, slotNumber);
			else
				return false;

			return true;
		}

		public string GetObjectFilePath(int slot = 0)
		{
			return _GetStaticObjectFilePath(Id, slot);
		}

		Dictionary<string, bool> memberIsProperty = new Dictionary<string, bool>();
		internal EntityConfig GetEntityConfig()
		{
			Type type = GetType();
			var properties = type.GetProperties();
			var fields = type.GetFields();
			var entityProperties = new List<object>();

			//Process all properties
			foreach(var property in properties)
			{
				if(property.ContainsAttribute<EditorPropertyAttribute>())
				{
					var attribute = property.GetAttribute<EditorPropertyAttribute>();
					EntityPropertyType propertyType = GetEditorType(property.PropertyType, attribute.Type);
					var limits = new EntityPropertyLimits(attribute.Min, attribute.Max);
					memberIsProperty.Add(property.Name, true);
					entityProperties.Add(new EntityProperty(property.Name, attribute.Description, propertyType, limits, attribute.Flags));
				}
			}

			//Process all fields
			foreach(var field in fields)
			{
				if(field.ContainsAttribute<EditorPropertyAttribute>())
				{
					var attribute = field.GetAttribute<EditorPropertyAttribute>();
					EntityPropertyType propertyType = GetEditorType(field.FieldType, attribute.Type);
					var limits = new EntityPropertyLimits(attribute.Min, attribute.Max);
					memberIsProperty.Add(field.Name, false);
					entityProperties.Add(new EntityProperty(field.Name, attribute.Description, propertyType, limits, attribute.Flags));
				}
			}

			return new EntityConfig(GetRegistrationConfig(type), entityProperties.ToArray());
		}

		internal EntityPropertyType GetEditorType(Type type, EntityPropertyType propertyType)
		{
			//If a special type is needed, do this here.
			switch(propertyType)
			{
				case EntityPropertyType.Object:
				case EntityPropertyType.Texture:
				case EntityPropertyType.File:
				case EntityPropertyType.Sound:
				case EntityPropertyType.Dialogue:
				case EntityPropertyType.Sequence:
					{
						if(type == typeof(string))
							return propertyType;
						else
							throw new EntityException("File selector type was specified, but property was not a string.");
					}
				case EntityPropertyType.Color:
					{
						if(type == typeof(Vec3))
							return propertyType;
						else
							throw new EntityException("Vector type was specified, but property was not a vector.");
					}
			}

			//OH PROGRAMMING GODS, FORGIVE ME
			if(type == typeof(string))
				return EntityPropertyType.String;
			else if(type == typeof(int))
				return EntityPropertyType.Int;
			else if(type == typeof(float) || type == typeof(double))
				return EntityPropertyType.Float;
			else if(type == typeof(bool))
				return EntityPropertyType.Bool;
			else if(type == typeof(Vec3))
				return EntityPropertyType.Vec3;
			else
				throw new EntityException("Invalid property type specified.");
		}

		public override NodeConfig GetNodeConfig()
		{
			return new NodeConfig(FlowNodeCategory.Approved, "", FlowNodeFlags.HideUI | FlowNodeFlags.TargetEntity);
		}

		internal EntityRegisterParams GetRegistrationConfig(Type type)
		{
			EntityAttribute entityAttribute = null;
			if(type.TryGetAttribute<EntityAttribute>(out entityAttribute))
			{
				return new EntityRegisterParams(type.Name, entityAttribute.Category, entityAttribute.EditorHelper,
					entityAttribute.Icon, entityAttribute.Flags);
			}

			return new EntityRegisterParams(type.Name, "Default", "", "", EntityClassFlags.Default);
		}

		public class EntityException : Exception
		{
			public EntityException(string message) { }
		}
		#endregion

	}
}
