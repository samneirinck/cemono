namespace CryEngine.Native
{
    internal interface INativeCVarMethods
    {
		void Execute(string command, bool silent);

        void RegisterCommand(string name, string description, CVarFlags flags);

        void RegisterCVarFloat(string name, ref float val, float defaultVal, CVarFlags flags,
                                               string description);

        void RegisterCVarInt(string name, ref int val, int defaultVal, CVarFlags flags,
                                             string description);

        void RegisterCVarString(string name, string val,
                                                string defaultVal, CVarFlags flags, string description);

        float GetCVarFloat(string name);
        int GetCVarInt(string name);
        string GetCVarString(string name);
        void SetCVarFloat(string name, float value);
        void SetCVarInt(string name, int value);
        void SetCVarString(string name, string value);
        bool HasCVar(string name);

        void HandleException(System.Exception ex);
    }
}