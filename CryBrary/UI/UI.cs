using System.Collections.ObjectModel;
using System.Reflection;
using System.Runtime.CompilerServices;

using System.Linq;

using CryEngine.Extensions;

namespace CryEngine
{
	public class UI
	{
		/// <summary>
		/// Registers an event to the system
		/// </summary>
		/// <param name="eventsystem">The name of the eventsystem this is registered to</param>
		/// <param name="direction">Event direction (system -> UI or UI -> system)</param>
		/// <param name="desc">Descriptor of the event</param>
		/// <returns>-1 if registration failed, event ID otherwise</returns>
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static int _RegisterEvent(string eventsystem, UIEventDirection direction, UIEventDescription desc);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static bool _RegisterToEventSystem(string eventsystem, UIEventDirection direction);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _UnregisterFromEventSystem(string eventsystem, UIEventDirection direction);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SendEvent(string eventsystem, int Event, object[] args);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SendNamedEvent(string eventsystem, string Event, object[] args);

		public static void OnEvent(string EventSystem, string EventName, int EventID, object[] args)
		{
		}

		public static void LoadEvent(System.Type type)
		{
			UINodeAttribute attribute;
			if(type.TryGetAttribute(out attribute))
			{
				UIEventDescription eventDesc = new UIEventDescription(attribute.Name, attribute.Name, attribute.Description);
				UIEventDirection eventDirection = UIEventDirection.ToSystem;

				Collection<UIParameterDescription> parameterDescriptions = new Collection<UIParameterDescription>();

				foreach(var member in type.GetMembers())
					ProcessMember(member, attribute, ref parameterDescriptions, ref eventDirection);

				eventDesc.Params = parameterDescriptions.Cast<object>().ToArray();

				_RegisterEvent(attribute.Category, eventDirection, eventDesc);
			}
		}

		static UIParameterType GetParameterType(System.Type type)
		{
			if(type == typeof(bool))
				return UIParameterType.Bool;
			else if(type == typeof(float))
				return UIParameterType.Float;
			else if(type == typeof(int))
				return UIParameterType.Int;
			else if(type == typeof(string))
				return UIParameterType.String;

			return UIParameterType.Any;
		}

		static void ProcessMember(MemberInfo member, UINodeAttribute parentAttribute, ref Collection<UIParameterDescription> parameterDescriptions, ref UIEventDirection eventDirection)
		{
			PortAttribute attribute;
			if(member.TryGetAttribute(out attribute))
			{
				switch(member.MemberType)
				{
					case MemberTypes.Field:
					case MemberTypes.Property:
						{
							UIParameterType parameterType = UIParameterType.Any;

							var memberType = member.MemberType == MemberTypes.Field ? ((FieldInfo)member).FieldType : ((PropertyInfo)member).PropertyType;
							if(memberType.IsGenericType ? memberType.GetGenericTypeDefinition() == typeof(OutputPort<>) : memberType == typeof(OutputPort))
							{
								parameterType = GetParameterType(memberType.IsGenericType ? memberType.GetGenericArguments()[0] : memberType);

								eventDirection = UIEventDirection.ToUI;
								if(!toUIEventSystems.Contains(parentAttribute.Category))
								{
									_RegisterToEventSystem(parentAttribute.Category, UIEventDirection.ToUI);

									toUIEventSystems.Add(parentAttribute.Category);
								}
							}
							else
							{
								parameterType = GetParameterType(memberType);

								eventDirection = UIEventDirection.ToSystem;
								if(!toSystemEventSystems.Contains(parentAttribute.Category))
								{
									_RegisterToEventSystem(parentAttribute.Category, UIEventDirection.ToSystem);

									toSystemEventSystems.Add(parentAttribute.Category);
								}
							}

							parameterDescriptions.Add(new UIParameterDescription(attribute.Name, attribute.Name, attribute.Description, parameterType));
						}
						break;
				}
			}
		}

		/// <summary>
		/// Event systems handling UI -> System communication.
		/// </summary>
		static Collection<string> toSystemEventSystems = new Collection<string>();
		/// <summary>
		/// Event systems handling System -> UI communication.
		/// </summary>
		static Collection<string> toUIEventSystems = new Collection<string>();
	}

	public class UINodeAttribute : System.Attribute
	{
		public string Name { get; set; }
		public string Description { get; set; }

		public string Category { get; set; }
	}

	public class UIEvent
	{
		static int EventId { get; set; }
	}

	public class UIFunction
	{
		public void Send()
		{
			string eventSystem = GetType().GetAttribute<UINodeAttribute>().Category;
		}

		static int EventId { get; set; }
	}

	internal enum UIParameterType
	{
		Any = 0,
		Bool,
		Int,
		Float,
		String,
	};

	internal enum UIEventDirection
	{
		ToSystem = 0,
		ToUI,
	};

	internal struct UIParameterDescription
	{
		public UIParameterType Type;
		public string Name;
		public string DisplayName;
		public string Description;
		public UIParameterDescription(UIParameterType type = UIParameterType.Any)
		{
			Type = UIParameterType.Any;
			Name = "Undefined";
			DisplayName = "Undefined";
			Description = "Undefined";
		}
		public UIParameterDescription(string name, string displayname, string description, UIParameterType type = UIParameterType.Any)
		{
			Type = type;
			Name = name;
			DisplayName = displayname;
			Description = description;
		}
	};

	internal struct UIEventDescription
	{
		public UIEventDescription(string name, string displayname, string description, bool isdyn = false, string dynamicname = "Array", string dynamicdesc = "")
		{
			Type = UIParameterType.Any;
			Name = name;
			DisplayName = displayname;
			Description = description;

			IsDynamic = isdyn;
			DynamicName = dynamicname;
			DynamicDesc = dynamicdesc;
			Params = null;
		}

		public void SetDynamic(string dynamicname, string dynamicdesc)
		{
			IsDynamic = true;
			DynamicName = dynamicname;
			DynamicDesc = dynamicdesc;
		}

		public UIParameterType Type;
		public string Name;
		public string DisplayName;
		public string Description;

		public object[] Params;
		public bool IsDynamic;
		public string DynamicName;
		public string DynamicDesc;
	};

	internal class UIEventArgs : System.EventArgs
	{
		public UIEventArgs(string eventSystem, string eventName, int eventID, object[] args)
		{
			EventName = eventName;
			EventSystem = eventSystem;
			EventID = eventID;
			Args = args;
		}

		public string EventName { get; private set; }
		public string EventSystem { get; private set; }
		public int EventID { get; private set; }
		public object[] Args { get; private set; }
	}
}
