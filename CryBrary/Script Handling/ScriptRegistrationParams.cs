using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.Initialization
{
    public interface ScriptRegistrationParams { }

    public struct ActorRegistrationParams : ScriptRegistrationParams
    {
        public bool isAI;
        public bool useMonoActor;
    }

    public struct EntityRegistrationParams : ScriptRegistrationParams
    {
        public string name;
        public string category;

        public string editorHelper;
        public string editorIcon;

        public EntityClassFlags flags;

        public object[] properties;
    }

    public struct GameRulesRegistrationParams : ScriptRegistrationParams
    {
        public string name;
        public bool defaultGamemode;
    }

    public struct FlowNodeRegistrationParams : ScriptRegistrationParams
    {
        public string name;
        public string category;
    }
}
