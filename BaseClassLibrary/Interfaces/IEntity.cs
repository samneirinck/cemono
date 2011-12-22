using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Reflection;

using CryEngine;
using CryEngine.Extensions;
using System.Text;

namespace CryEngine
{
	/// <summary>
	/// Entities must inherit this in order to be registered.
	/// It is no longer an interface, due to basic functionality each entity <b>must</b> have.
	/// </summary>
    public class Entity : ICryScriptType
	{
        public Entity()
        {
            MonoEntity = true;
        }

        // Only called for non-mono entities
        internal Entity(UInt32 entityId)
        {
            MonoEntity = false;

            Id = entityId;
        }

        #region Methods & Fields
        public Vec3 Position { get { return EntitySystem._GetWorldPos(Id); } set { EntitySystem._SetWorldPos(Id, value); } }
        public Vec3 Rotation { get { return EntitySystem._GetWorldAngles(Id); } set { EntitySystem._SetWorldAngles(Id, value); } }

        public UInt32 Id { get; set; }
        public string Name { get; set; }
        public EntityFlags Flags { get; set; }

        internal bool MonoEntity;
        #endregion

        #region Callbacks
        /// <summary>
        /// Called once per frame.
        /// </summary>
        public virtual void OnUpdate() { }

        /// <summary>
        /// This callback is called when this entity has finished spawning. The entity has been created and added to the list of entities.
        /// </summary>
        public virtual void OnSpawn() { }

        /// <summary>
        /// Called when the entity is being removed.
        /// </summary>
        /// <returns>True to allow removal, false to deny.</returns>
        public virtual bool OnRemove() { return true; }
        #endregion

        #region Overrides
        // The stash; hide all internal code we don't want anyone to see down here. *sweep sweep*
        public override bool Equals(object obj)
        {
            Entity ent = obj as Entity;

            return (ent == null) ? false : this.Id == ent.Id;
        }
        #endregion

        #region Base Logic

        /// <summary>
        /// Sets up entity and invokes OnSpawn();
        /// </summary>
        /// <param name="entityId"></param>
		internal void Spawn(UInt32 entityId)
		{
            EntitySystem.RegisterInternalEntity(entityId, this);

			Id = entityId;
			OnSpawn();
		}

        public virtual void SetProperty(string propertyName, EntityPropertyType propertyType, string value)
        {
            if (value.Length <= 0 && propertyType != EntityPropertyType.String)
                return;

            switch (propertyType)
            {
                case EntityPropertyType.Bool:
                    {
                        GetType().GetField(propertyName).SetValue(this, Convert.ToBoolean(value));
                    }
                    break;
                case EntityPropertyType.Int:
                    {
                        GetType().GetField(propertyName).SetValue(this, Convert.ToInt32(value));
                    }
                    break;
                case EntityPropertyType.Float:
                    {
                        GetType().GetField(propertyName).SetValue(this, Convert.ToSingle(value));
                    }
                    break;
                case EntityPropertyType.Vec3:
                    {
                        string[] split = value.Split(',');

                        GetType().GetField(propertyName).SetValue(this, new Vec3(Convert.ToSingle(split[0]), Convert.ToSingle(split[1]), Convert.ToSingle(split[2])));
                    }
                    break;
                case EntityPropertyType.String:
                    {
                        GetType().GetField(propertyName).SetValue(this, value);
                    }
                    break;
            }
        }

        public virtual string GetProperty(string propertyName)
        {
            CryConsole.LogAlways("GetProperty");

            return "";
        }

		internal EntityConfig GetConfig()
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

		internal EntityRegisterParams GetRegistrationConfig(Type type)
		{
			var entityAttribute = type.GetAttribute<EntityAttribute>();
			return new EntityRegisterParams(type.Name, entityAttribute.Category, entityAttribute.EditorHelper,
				entityAttribute.Icon, entityAttribute.Flags);
		}
	}

	public class EntityException : Exception
	{
		public EntityException(string message) { }
	}
#endregion
}
