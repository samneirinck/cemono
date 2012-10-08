using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Runtime.Serialization;
using System.Runtime.InteropServices;

using CryEngine.Extensions;
using CryEngine.Initialization;
using CryEngine.Native;

namespace CryEngine
{
	/// <summary>
	/// The base class for all entities in the game world.
	/// </summary>
	public abstract partial class Entity : EntityBase
	{
		/// <summary>
		/// Initializes the entity, not recommended to set manually.
		/// </summary>
		/// <param name="entInfo">Struct containing the IEntity pointer and EntityId.</param>
		/// <returns>IsEntityFlowNode</returns>
		[EditorBrowsable(EditorBrowsableState.Never)]
		internal virtual void InternalSpawn(EntityInfo entInfo)
		{
			this.SetEntityHandle(new HandleRef(this, entInfo.IEntityPtr));
			this.SetAnimatedCharacterHandle(new HandleRef(this, entInfo.IAnimatedCharacterPtr));
			Id = entInfo.Id;

			foreach(var property in GetType().GetProperties())
			{
				EditorPropertyAttribute attr;
				if(property.TryGetAttribute(out attr) && attr.DefaultValue != null && !HasEditorPropertyBeenSet(property.GetValue(this, null), property.PropertyType))
					property.SetValue(this, attr.DefaultValue, null);
			}

			foreach(var field in GetType().GetFields())
			{
				EditorPropertyAttribute attr;
				if(field.TryGetAttribute(out attr) && attr.DefaultValue != null && !HasEditorPropertyBeenSet(field.GetValue(this), field.FieldType))
					field.SetValue(this, attr.DefaultValue);
			}

			OnSpawn();
		}

		bool HasEditorPropertyBeenSet(object value, Type type)
		{
			object defaultVal = null;
			if(type.IsValueType)
				defaultVal = Activator.CreateInstance(type);

			return value != defaultVal;
		}

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
		protected virtual void OnEnterArea(EntityId entityId, int areaEntityId, float fade) { }

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
		protected virtual void OnLeaveArea(EntityId entityId, int areaEntityId, float fade) { }

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
		protected virtual void OnCollision(EntityId colliderId, Vec3 hitPos, Vec3 dir, short materialId, Vec3 contactNormal) { }

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

		/// <summary>
		/// Called when the user changes a property from within the Editor.
		/// </summary>
        /// <param name="memberInfo"></param>
		/// <param name="propertyType"></param>
		/// <param name="newValue"></param>
		protected virtual void OnPropertyChanged(MemberInfo memberInfo, EntityPropertyType propertyType, object newValue) { }
		
		protected virtual void OnPrePhysicsUpdate() {}
		#endregion

		#region Base Logic
		internal virtual string GetPropertyValue(string propertyName)
		{
#if !((RELEASE && RELEASE_DISABLE_CHECKS))
			if (propertyName == null)
				throw new ArgumentNullException("propertyName");
			if(propertyName.Length < 1)
				throw new ArgumentException("propertyName was empty!");
#endif

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
#if !((RELEASE && RELEASE_DISABLE_CHECKS))
			if(valueString == null)
				throw new ArgumentNullException("valueString");
			if(propertyName == null)
				throw new ArgumentNullException("propertyName");
			if(valueString.Length < 1 && propertyType != EntityPropertyType.String)
				throw new ArgumentException("value was empty!");
			if(propertyName.Length < 1)
				throw new ArgumentException("propertyName was empty!");
#endif

			var value = Convert.FromString(propertyType, valueString);

			var member = GetType().GetMember(propertyName).First(x => x.MemberType == MemberTypes.Field || x.MemberType == MemberTypes.Property);
			if(member == null)
				throw new ArgumentException(string.Format("member {0} could not be located", propertyName));

			if (member.MemberType == MemberTypes.Property)
				(member as PropertyInfo).SetValue(this, value, null);
			else
				(member as FieldInfo).SetValue(this, value);

			OnPropertyChanged(member, propertyType, value);
		}

		public static EntityPropertyType GetEditorType(Type type, EntityPropertyType propertyType)
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

						throw new EntityException("File selector type was specified, but property was not a string.");
					}
				case EntityPropertyType.Color:
					{
						if(type == typeof(Vec3))
							return propertyType;

						throw new EntityException("Vector type was specified, but property was not a vector.");
					}
			}

			//OH PROGRAMMING GODS, FORGIVE ME
			if(type == typeof(string))
				return EntityPropertyType.String;
			if(type == typeof(int))
				return EntityPropertyType.Int;
			if(type == typeof(float) || type == typeof(double))
				return EntityPropertyType.Float;
			if(type == typeof(bool))
				return EntityPropertyType.Bool;
			if(type == typeof(Vec3))
				return EntityPropertyType.Vec3;
			
			throw new EntityException("Invalid property type specified.");
		}

		/// <summary>
		/// Set to detect movement within an area. 
		/// See OnEnterArea, OnMoveInsideArea, OnLeaveArea, OnEnterNearArea, OnLeaveNearArea & OnMoveNearArea
		/// </summary>
		public BoundingBox TriggerBounds
		{
			get { return NativeMethods.Entity.GetTriggerBBox(this.GetEntityHandle().Handle); }
			set
			{
				if (value.Minimum == Vec3.Zero && value.Maximum == Vec3.Zero)
					NativeMethods.Entity.InvalidateTrigger(this.GetEntityHandle().Handle);

				NativeMethods.Entity.SetTriggerBBox(this.GetEntityHandle().Handle, value);
			}
		}

		/*
		internal override NodeConfig GetNodeConfig()
		{
			return new NodeConfig(FlowNodeCategory.Approved, "", FlowNodeFlags.HideUI | FlowNodeFlags.TargetEntity);
		}*/
		#endregion

        #region Overrides
        public override int GetHashCode()
        {
            unchecked // Overflow is fine, just wrap
            {
                int hash = 17;

                hash = hash * 29 + ScriptId.GetHashCode();
                hash = hash * 29 + Id.GetHashCode();
				hash = hash * 29 + this.GetEntityHandle().Handle.GetHashCode();

                return hash;
            }
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
