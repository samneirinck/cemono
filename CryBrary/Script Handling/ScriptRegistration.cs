using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.Initialization
{
	public struct ActorRegistrationParams
	{
		public bool isAI;
		public bool useMonoActor;
	}

	public struct EntityRegistrationParams
	{
		public string name;
		public string category;

		public string editorHelper;
		public string editorIcon;

		public EntityClassFlags flags;

		public object[] properties;
	}

    public struct GameRulesRegistrationParams
    {
        public string name;
        public bool defaultGamemode;
    }

    public struct FlowNodeRegistrationParams
    {
        public string name;
        public string category;
    }

	public static class ScriptRegistration
	{
		public static void Register(CryScript script, ActorRegistrationParams registrationParams)
		{
			if(registrationParams.useMonoActor)
				Actor.Actormethods.RegisterClass(script.ScriptName, registrationParams.isAI);
		}

		public static void Register(CryScript script, EntityRegistrationParams registrationParams)
		{
            if (registrationParams.name == null)
                registrationParams.name = script.ScriptName;

            Entity.Methods.RegisterClass(registrationParams);
		}

        public static void Register(CryScript script, GameRulesRegistrationParams registrationParams)
        {
            if (registrationParams.name == null)
                registrationParams.name = script.ScriptName;

            GameRules._RegisterGameMode(registrationParams.name);

            if (registrationParams.defaultGamemode)
                GameRules._SetDefaultGameMode(registrationParams.name);
        }

        public static void Register(ref CryScript script, FlowNodeRegistrationParams registrationParams)
        {
            if (registrationParams.name == null)
                registrationParams.name = script.ScriptName;
            if (registrationParams.category == null)
                registrationParams.category = script.Type.Namespace;

            script.ScriptName = registrationParams.category + ":" + registrationParams.name;

            ScriptManager.FlowNodes.Add(script.ScriptName);
        }
	}
}
