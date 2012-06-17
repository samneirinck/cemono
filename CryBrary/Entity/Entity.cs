using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Runtime.Serialization;
using CryEngine.Extensions;

using CryEngine.Initialization;

namespace CryEngine
{
	/// <summary>
	/// The base class for all entities in the game world.
	/// </summary>
	public abstract partial class Entity : EntityBase
	{
		protected Entity() { }
		
		internal static void Load(CryScript script)
		{
			//LoadFlowNode(ref script, true);

			Methods.RegisterClass(GetEntityConfig(script.Type));
		}

		/// <summary>
		/// Initializes the entity, not recommended to set manually.
		/// </summary>
		/// <param name="entInfo">Struct containing the IEntity pointer and EntityId.</param>
		/// <returns>IsEntityFlowNode</returns>
		[EditorBrowsable(EditorBrowsableState.Never)]
		internal virtual bool InternalSpawn(EntityInfo entInfo)
		{
			EntityPointer = entInfo.IEntityPtr;
			Id = new EntityId(entInfo.Id);

			Spawned = true;

			foreach(var property in GetType().GetProperties())
			{
				EditorPropertyAttribute attr;
				try
				{
					if(property.TryGetAttribute(out attr) && attr.DefaultValue != null && !HasEditorPropertyBeenSet(property.GetValue(this, null), property.PropertyType))// && !storedPropertyNames.Contains(property.Name))
						property.SetValue(this, attr.DefaultValue, null);
				}
				catch(Exception ex)
				{
					Debug.LogException(ex);
				}
			}

			foreach(var field in GetType().GetFields())
			{
				EditorPropertyAttribute attr;
				if(field.TryGetAttribute(out attr) && attr.DefaultValue != null && !HasEditorPropertyBeenSet(field.GetValue(this), field.FieldType))// && !storedPropertyNames.Contains(field.Name))
					field.SetValue(this, attr.DefaultValue);
			}

			OnSpawn();

			return IsEntityFlowNode();
		}

		bool HasEditorPropertyBeenSet(object value, Type type)
		{
			object defaultVal = null;
			if(type.IsValueType)
				defaultVal = Activator.CreateInstance(type);

			return value != defaultVal;
		}

		/// <summary>
		/// Returns true if this entity contains input or output ports.
		/// </summary>
		/// <returns></returns>
		public bool IsEntityFlowNode()
		{
			var members = GetType().GetMembers(BindingFlags.Instance);
			if(members == null || members.Length <= 0)
				return false;

			return members.Any(member => member.ContainsAttribute<PortAttribute>());
		}

		#region Methods & Fields
		internal bool Spawned;
		#endregion

		#region Callbacks
		/// <summary>
		/// Called when the entity is being removed.
		/// </summary>
		/// <returns>True to allow removal, false to deny.</returns>
		protected virtual bool OnRemove() { return true; }

		/// <summary>
		/// Called when resetting the state of the entity in Editor.
		/// </summary>
		/// <param name="enteringGame">true if currently entering gamemode, false if exiting.</param>
		protected virtual void OnReset(bool enteringGame) { }

		/// <summary>
		/// Called when game is started (games may start multiple times)
		/// </summary>
		protected virtual void OnStartGame() { }

		/// <summary>
		/// Called when the level is started.
		/// </summary>
		protected virtual void OnStartLevel() { }

		/// <summary>
		/// Sent when entity enters to the area proximity.
		/// </summary>
		/// <param name="entityId"></param>
		/// <param name="areaEntityId"></param>
		protected virtual void OnEnterArea(EntityId entityId, int areaEntityId) { }

		/// <summary>
		/// Sent when entity moves inside the area proximity.
		/// </summary>
		/// <param name="entityId"></param>
		/// <param name="areaId"></param>
		/// <param name="fade"></param>
		protected virtual void OnMoveInsideArea(EntityId entityId, int areaId, float fade) { }

		/// <summary>
		/// Sent when entity leaves the area proximity.
		/// </summary>
		/// <param name="entityId"></param>
		/// <param name="areaEntityId"></param>
		protected virtual void OnLeaveArea(EntityId entityId, int areaEntityId) { }

		protected virtual void OnEnterNearArea(EntityId entityId, int areaId, float fade) { }
		protected virtual void OnLeaveNearArea(EntityId entityId, int areaId, float fade) { }
		protected virtual void OnMoveNearArea(EntityId entityId, int areaId, float fade) { }

		/// <summary>
		/// Sent on entity collision.
		/// </summary>
		/// <param name="targetEntityId"></param>
		/// <param name="hitPos"></param>
		/// <param name="dir"></param>
		/// <param name="materialId"></param>
		/// <param name="contactNormal"></param>
		protected virtual void OnCollision(EntityId targetEntityId, Vec3 hitPos, Vec3 dir, short materialId, Vec3 contactNormal) { }

		/// <summary>
		/// 
		/// </summary>
		public virtual void OnHit(HitInfo hitInfo) { }

		/// <summary>
		/// Called after level has been loaded, is not called on serialization.
		/// Note that this is called prior to GameRules.OnClientConnect and OnClientEnteredGame!
		/// </summary>
		protected virtual void OnInit()
		{
		}

		/// <summary>
		/// Called when the entities local or world transformation matrix changes. (Position / Rotation / Scale)
		/// </summary>
		protected virtual void OnMove() { }

		/// <summary>
		/// Called whenever another entity has been linked to this entity.
		/// </summary>
		/// <param name="child"></param>
		protected virtual void OnAttach(EntityId child) { }
		/// <summary>
		/// Called whenever another entity has been unlinked from this entity.
		/// </summary>
		/// <param name="child"></param>
		protected virtual void OnDetach(EntityId child) { }
		/// <summary>
		/// Called whenever this entity is unliked from another entity.
		/// </summary>
		/// <param name="parent"></param>
		protected virtual void OnDetachThis(EntityId parent) { }
		#endregion

		#region Base Logic

		internal virtual string GetPropertyValue(string propertyName)
		{
			if(propertyName == null)
				throw new ArgumentNullException("propertyName");
			else if(propertyName.Length < 1)
				throw new ArgumentException("propertyName was empty!");

			var field = GetType().GetField(propertyName);
			if(field != null)
				return field.GetValue(this).ToString();

			var property = GetType().GetProperty(propertyName);
			if(property != null)
				return property.GetValue(this, null).ToString();

			return null;
		}

		internal virtual void SetPropertyValue(string propertyName, EntityPropertyType propertyType, string valueString)
		{
			if(valueString == null)
				throw new ArgumentNullException("value");
			else if(propertyName == null)
				throw new ArgumentNullException("propertyName");
			else if(valueString.Length < 1 && propertyType != EntityPropertyType.String)
				throw new ArgumentException("value was empty!");
			else if(propertyName.Length < 1)
				throw new ArgumentException("propertyName was empty!");

			var value = Convert.FromString(propertyType, valueString);

			// Perhaps we should exclude properties entirely, and just utilize fields (including backing fields)
			var property = GetType().GetProperty(propertyName);
			if(property != null)
			{
				property.SetValue(this, value, null);

				return;
			}

			var field = GetType().GetField(propertyName);
			if(field != null)
				field.SetValue(this, value);
			else
				throw new ArgumentException(string.Format("member {0} could not be located", propertyName));
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
				_LoadObject(EntityPointer, name, slotNumber);
			else if(name.EndsWith("cdf") || name.EndsWith("cga") || name.EndsWith("chr"))
				_LoadCharacter(EntityPointer, name, slotNumber);
			else
				return false;

			return true;
		}

		protected string GetObjectFilePath(int slot = 0)
		{
			return _GetStaticObjectFilePath(EntityPointer, slot);
		}

		internal static EntityConfig GetEntityConfig(Type type)
		{
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

					entityProperties.Add(new EntityProperty(field.Name, attribute.Description, propertyType, limits, attribute.Flags));
				}
			}

			return new EntityConfig(GetRegistrationConfig(type), entityProperties.ToArray());
		}

		internal static EntityPropertyType GetEditorType(Type type, EntityPropertyType propertyType)
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
		/*
		internal override NodeConfig GetNodeConfig()
		{
			return new NodeConfig(FlowNodeCategory.Approved, "", FlowNodeFlags.HideUI | FlowNodeFlags.TargetEntity);
		}*/

		internal static EntityRegisterParams GetRegistrationConfig(Type type)
		{
			EntityAttribute entityAttribute = type.ContainsAttribute<EntityAttribute>() ? type.GetAttribute<EntityAttribute>() : new EntityAttribute();

			return new EntityRegisterParams(entityAttribute.Name ?? type.Name, entityAttribute.Category, entityAttribute.EditorHelper,
					entityAttribute.Icon, entityAttribute.Flags);
		}
		#endregion
	}

	[Serializable]
	public class EntityException : Exception
	{
		public EntityException()
		{
		}

		public EntityException(string message)
			: base(message)
		{
		}

		public EntityException(string message, Exception inner)
			: base(message, inner)
		{
		}

		protected EntityException(
			SerializationInfo info,
			StreamingContext context)
			: base(info, context)
		{
		}
	}

	public enum EntitySlotFlags
	{
		Render = 0x0001,  // Draw this slot.
		RenderNearest = 0x0002,  // Draw this slot as nearest.
		RenderWithCustomCamera = 0x0004,  // Draw this slot using custom camera passed as a Public ShaderParameter to the entity.
		IgnorePhysics = 0x0010,  // This slot will ignore physics events sent to it.
		BreakAsEntity = 0x020,
		RenderInCameraSpace = 0x0040, // This slot position is in camera space 
		RenderAfterPostProcessing = 0x0080, // This slot position is in camera space 
		BreakAsEntityMP = 0x0100, // In MP this an entity that shouldn't fade or participate in network breakage
	}
}
