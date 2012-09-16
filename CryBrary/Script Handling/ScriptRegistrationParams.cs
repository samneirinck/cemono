using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.Initialization
{
    public interface IScriptRegistrationParams { }

    public struct ActorRegistrationParams : IScriptRegistrationParams
    {
    }

    public struct EntityRegistrationParams : IScriptRegistrationParams
    {
        public string name;
        public string category;

        public string editorHelper;
        public string editorIcon;

        public EntityClassFlags flags;

        public object[] properties;
    }

    public struct GameRulesRegistrationParams : IScriptRegistrationParams
    {
        public string name;
        public bool defaultGamemode;
    }

    public struct FlowNodeRegistrationParams : IScriptRegistrationParams
    {
        public string name;
        public string category;
    }
}
