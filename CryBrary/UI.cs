using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CryEngine.Utils;
using System.Reflection;
using System.Runtime.CompilerServices;

namespace CryEngine
{
    public enum EUIParameterType
    {
        eUIPT_Any = 0,
        eUIPT_Bool,
        eUIPT_Int,
        eUIPT_Float,
        eUIPT_String,
    };

    public enum EUIEventDirection
    {
        eUIED_UIToSystem = 0,
        eUIED_SystemToUI,
    };

    public struct SUIParameterDesc
    {
        public EUIParameterType Type;
        public string Name;
        public string DisplayName;
        public string Description;
        public SUIParameterDesc(EUIParameterType type = EUIParameterType.eUIPT_Any)
        {
            Type = EUIParameterType.eUIPT_Any;
            Name = "Undefined";
            DisplayName = "Undefined";
            Description = "Undefined";
        }
        public SUIParameterDesc(string name, string displayname, string description, EUIParameterType type = EUIParameterType.eUIPT_Any)
        {
            Type = type;
            Name = name;
            DisplayName = displayname;
            Description = description;
        }
    };

    public struct SUIEventDesc
    {

        public EUIParameterType Type;
        public string Name;
        public string DisplayName;
        public string Description;

        public Object[] Params;
        public bool IsDynamic;
        public string DynamicName;
        public string DynamicDesc;
        public SUIEventDesc(EUIParameterType type = EUIParameterType.eUIPT_Any)
        {
            Type = EUIParameterType.eUIPT_Any;
            Name = "Undefined";
            DisplayName = "Undefined";
            Description = "Undefined";

            IsDynamic = false;
            DynamicName = "Array";
            DynamicDesc = "";
            Params = null;
        }
        public SUIEventDesc(string name, string displayname, string description, bool isdyn = false, string dynamicname = "Array", string dynamicdesc = "")
        {
            Type = EUIParameterType.eUIPT_Any;
            Name = name;
            DisplayName = displayname;
            Description = description;

            IsDynamic = isdyn;
            DynamicName = dynamicname;
            DynamicDesc = dynamicdesc;
            Params = null;
        }
        public void SetDynamic(string dynamicname, string dynamicdesc){
            IsDynamic = true;
            DynamicName = dynamicname;
            DynamicDesc = dynamicdesc;
        }
    };

    public class UIEventArgs : System.EventArgs
    {
        public int Event;
        public Object[] Args;
        public UIEventArgs()
		{
            Event = 0;
		}
    }

	public class UI
	{
        [MethodImpl(MethodImplOptions.InternalCall)]
	    extern internal static int _RegisterEvent(string eventsystem, int direction, SUIEventDesc desc);
        [MethodImpl(MethodImplOptions.InternalCall)]
	    extern internal static bool _RegisterToEventSystem(string eventsystem, int type);
	    [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _UnregisterFromEventSystem(string eventsystem, int type);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _SendEvent(string eventsystem, int Event, object[] args);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _SendNamedEvent(string eventsystem, string Event, object[] args);


        private static Dictionary<int, string> s_EventMap;

		public static void OnEvent(string EventSystem, string EventName, int EventID, object[] args)
		{
            UIEventArgs e = new UIEventArgs();
            e.Event = EventID;
            e.Args = args;
            Debug.LogAlways("Event: {0}.{1} = {2}", EventSystem, EventName, e.Event);
            int i, c;
            Object o;
            c = e.Args.Length;
            for (i = 0; i < c; i++){
                o = e.Args[i];
                Debug.LogAlways("Arg {0}/{1}: {2} {3}", i+1, c, o.GetType().Name, o);
            }
			SendEvent("MySystemEvent", "TestEvent2", new object[2] { EventName, EventID });

			//Events(null, new UIEventArgs());
		}


        public static int RegisterEvent(string eventsystem, EUIEventDirection direction, SUIEventDesc desc)
        {
            return _RegisterEvent(eventsystem, (int)direction, desc);
        }

        public static bool RegisterToEventSystem(string eventsystem, EUIEventDirection direction)
        {
            return _RegisterToEventSystem(eventsystem, (int)direction);
        }
        public static void UnregisterFromEventSystem(string eventsystem, EUIEventDirection direction)
        {
            _UnregisterFromEventSystem(eventsystem, (int)direction);
        }


        public static void SendEvent(string eventsystem, int Event, object[] args)
        {
            _SendEvent(eventsystem, Event, args);
        }
        public static void SendEvent(string eventsystem, string Event, object[] args)
        {
            _SendNamedEvent(eventsystem, Event, args);
        }

		public delegate void UIEventDelegate(object sender, UIEventArgs e);

		/// <summary>
		/// UI.Events += MyUIEventDelegateMethod;
		/// </summary>
		public static event UIEventDelegate Events;

        public static void TestInit()
        {
            bool b;
            b = RegisterToEventSystem("MenuEvents", EUIEventDirection.eUIED_UIToSystem);
            Debug.LogAlways("RegisterToEventSystem(\"MenuEvents\") == {0}", b);
            SUIEventDesc desc = new SUIEventDesc("TestEvent", "TestEventDName", "TestEventDescription");
            desc.Params = new Object[2];
            desc.Params[0] = new SUIParameterDesc("Param1", "Param1DName", "Param1Desc", EUIParameterType.eUIPT_String);
            desc.Params[1] = new SUIParameterDesc("Param2", "Param2DName", "Param2Desc", EUIParameterType.eUIPT_Int);
            int i = RegisterEvent("MyEvent", EUIEventDirection.eUIED_UIToSystem, desc);
            Debug.LogAlways("RegisterEvent == {0}", i);
            i = RegisterEvent("MyEvent2", EUIEventDirection.eUIED_UIToSystem, desc);
            Debug.LogAlways("RegisterEvent2 == {0}", i);

			desc = new SUIEventDesc("BoidCount", "BoidCount", "Sets the boid count");
			desc.Params = new Object[1];
			desc.Params[0] = new SUIParameterDesc("Count", "Count", "Number of available boids", EUIParameterType.eUIPT_Int);
			i = RegisterEvent("AngryBoids", EUIEventDirection.eUIED_SystemToUI, desc);
        }
	}
}
