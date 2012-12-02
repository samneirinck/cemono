namespace CryEngine.Initialization
{
    public struct EntityRegistrationParams : IScriptRegistrationParams
    {
        public string name;
        public string category;

        public string editorHelper;
        public string editorIcon;

        public EntityClassFlags flags;

        public object[] properties;
    }
}
