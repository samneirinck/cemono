using System;
using System.Collections.Generic;
using System.Reflection;
using System.Linq;
using System.Text;

using CryEngine.Initialization;
using CryEngine.Extensions;

namespace CryEngine
{
	public abstract class UIEventSystem
	{
		internal static void Load(ref CryScript script)
		{
			if(Pointers == null)
				Pointers = new Dictionary<Type, IntPtr>();

			bool createdToSystem = false;
			bool createdToUI = false;

			foreach(var member in script.Type.GetMembers())
			{
				PortAttribute attribute;
				if(member.TryGetAttribute<PortAttribute>(out attribute))
				{
					if(attribute.Name == null || attribute.Name.Length < 1)
						attribute.Name = member.Name;

					if(attribute.Description == null)
						attribute.Description = "";

					switch(member.MemberType)
					{
						case MemberTypes.Field:
						case MemberTypes.Property:
							{
								if(!createdToUI)
								{
									Pointers.Add(script.Type, UI.CreateEventSystem(script.ScriptName, UI.EventSystemType.ToUI));

									createdToUI = true;
								}

								var args = new List<object>();

								UI._RegisterEvent(Pointers[script.Type], attribute.Name, attribute.Description, args.ToArray());
							}
							break;
						case MemberTypes.Method:
							{
								if(!createdToSystem)
								{
									Pointers.Add(script.Type, UI.CreateEventSystem(script.ScriptName, UI.EventSystemType.ToSystem));

									createdToSystem = true;
								}

								var method = member as MethodInfo;
								var args = new List<object>();

								foreach(var param in method.GetParameters())
									args.Add(new InputPortConfig(param.Name, FlowNode.GetPortType(param.ParameterType), "desc"));

								UI.RegisterFunction(Pointers[script.Type], attribute.Name, attribute.Description, args.ToArray(), method);
							}
							break;
					}
				}
			}
		}

		internal static Dictionary<Type, IntPtr> Pointers { get; set; }
	}
}
