using System;
using System.Collections.Generic;
using System.Reflection;

using CryEngine.Initialization;
using CryEngine.Extensions;
using CryEngine.Native;

namespace CryEngine
{
    public abstract class UIEventSystem
    {
        /*internal static void Load(CryScript script)
        {
            bool createdToSystem = false;
            bool createdToUI = false;

            foreach (var member in script.Type.GetMembers())
            {
                switch (member.MemberType)
                {
                    case MemberTypes.Field:
                    case MemberTypes.Property:
                        {
                            if (ToUIPointers == null)
                                ToUIPointers = new Dictionary<Type, IntPtr>();

                            string name = member.Name;
                            string desc = "";

                            if (member.ContainsAttribute<UIEventAttribute>())
                            {
                                var attribute = member.GetAttribute<UIEventAttribute>();

                                if (attribute.Name != null && attribute.Name.Length > 0)
                                    name = attribute.Name;

                                if (attribute.Description != null && attribute.Description.Length > 0)
                                    desc = attribute.Description;
                            }
                            

                            var fieldInfo = member as FieldInfo;
                            var propertyInfo = member as PropertyInfo;

                            var memberType = (fieldInfo != null ? fieldInfo.FieldType : propertyInfo.PropertyType);

                            if (!memberType.Name.StartsWith("UIEvent`"))
                                break;

                            if (!createdToUI)
                            {
                                ToUIPointers.Add(script.Type, UI.CreateEventSystem(script.ScriptName, UI.EventSystemType.ToUI));

                                createdToUI = true;
                            }

                            var args = new List<object>();

                            memberType.GetGenericArguments().ForEach(type =>
                                                                         {
                                                                             args.Add(new OutputPortConfig("Output", "", "desc",
                                                                                                           FlowNode.GetPortType(type)));
                                                                         });
                                

                            var eventId = UI._RegisterEvent(ToUIPointers[script.Type], name, desc, args.ToArray());

                            var uiEvent = Activator.CreateInstance(memberType);

                            memberType.GetProperty("EventId").SetValue(uiEvent, eventId, null);
                            memberType.GetProperty("EventSystemPointer").SetValue(uiEvent, ToUIPointers[script.Type], null);

                            if (fieldInfo != null)
                                fieldInfo.SetValue(null, uiEvent);
                            else
                                propertyInfo.SetValue(null, uiEvent, null);
                        }
                        break;
                    case MemberTypes.Method:
                        {
                            UIFunctionAttribute attribute;
                            if (member.TryGetAttribute(out attribute))
                            {
                                if (ToSystemPointers == null)
                                    ToSystemPointers = new Dictionary<Type, IntPtr>();

                                if (attribute.Name == null || attribute.Name.Length < 0)
                                    attribute.Name = member.Name;

                                if (attribute.Description == null)
                                    attribute.Description = "";

                                if (!createdToSystem)
                                {
                                    ToSystemPointers.Add(script.Type, UI.CreateEventSystem(script.ScriptName, UI.EventSystemType.ToSystem));

                                    createdToSystem = true;
                                }

                                var method = member as MethodInfo;
                                var args = new List<object>();

                                method.GetParameters().ForEach(param =>
                                                                   {
                                                                       args.Add(new InputPortConfig(param.Name,
                                                                                                    FlowNode.GetPortType(param.ParameterType), ""));
                                                                   });
                                    

                                UI.RegisterFunction(ToSystemPointers[script.Type], attribute.Name, attribute.Description, args.ToArray(), method);
                            }
                        }
                        break;
                }
            }
        }*/

        internal static Dictionary<Type, IntPtr> ToSystemPointers { get; set; }
        internal static Dictionary<Type, IntPtr> ToUIPointers { get; set; }

        public struct UIEvent<T1, T2, T3>
        {
            public void Activate(T1 t1, T2 t2, T3 t3)
            {
                object[] args = { t1, t2, t3 };

                NativeUIMethods.SendEvent(EventSystemPointer, EventId, args);
            }

            private uint EventId { get; set; }
            private IntPtr EventSystemPointer { get; set; }
        }
    }

    [AttributeUsage(AttributeTargets.Method)]
    public sealed class UIFunctionAttribute : Attribute
    {
        public string Name { get; set; }
        public string Description { get; set; }
    }

    [AttributeUsage(AttributeTargets.Property | AttributeTargets.Field)]
    public sealed class UIEventAttribute : Attribute
    {
        public string Name { get; set; }
        public string Description { get; set; }
    }
}
