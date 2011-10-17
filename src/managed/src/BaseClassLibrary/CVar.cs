using System;
namespace CryEngine
{
    [Flags]
    public enum CVarFlags
    {
        /// <summary>
        /// Stays in the default state when cheats are disabled
        /// </summary>
        Cheat = 0x00000002,
        /// <summary>
        /// Can be changed on client and when connecting the var not sent to the client (should not be used during CVar creation, is set for all vars in Game/scripts/cvars.tx)
        /// </summary>
        NotNetSynced = 0x00000080,
        DumptoDisk = 0x00000100,
        /// <summary>
        /// Stored when saving a savegame
        /// </summary>
        SaveGame = 0x00000200,
        NoHelp = 0x00000400,
        /// <summary>
        /// Can not be changed by the user
        /// </summary>
        ReadOnly = 0x00000800,
        RequireLevelReload = 0x00001000,
        RequireAppRestart = 0x00002000,
        /// <summary>
        /// Shows warning that this var was not used in config file
        /// </summary>
        WarningNogUsed = 0x00004000,
        /// <summary>
        /// Otherwise the const char * to the name will be stored without copying the memory
        /// </summary>
        CopyName = 0x00008000,
        /// <summary>
        /// Set when variable value modified.
        /// </summary>
        Modified = 0x00010000,
        /// <summary>
        /// Set when variable was present in config file.
        /// </summary>
        WasInConfig = 0x00020000,
        /// <summary>
        /// Allow bitfield setting syntax.
        /// </summary>
        BitField = 0x00040000,
        /// <summary>
        /// Is visible and usable in restricted (normal user) console mode
        /// </summary>
        RestrictedMode = 0x00080000,
    }

    public class CVar<T> where T : IConvertible
    {
        internal CVar(string name)
        {
            // Disallow certain types
            var type = typeof(T);

            if (type != typeof(float) && type != typeof(string) && type != typeof(int))
            {
                throw new ArgumentException("Tried creating a CVar of type {0}, only int/float/string is allowed.", type.Name);
            }
            _name = name;
        }

        public static CVar<T> Create(string name)
        {
            return Create(name, default(T));
        }

        public static CVar<T> Create(string name, T defaultValue)
        {
            return Create(name, defaultValue, 0);
        }

        public static CVar<T> Create(string name, T defaultValue, CVarFlags flags)
        {
            return Create(name, defaultValue, flags, String.Empty);
        }

        public static CVar<T> Create(string name, T defaultValue, CVarFlags flags, string help)
        {
            API.Console.RegisterCVar<T>(name, defaultValue, flags, help);

            return new CVar<T>(name);
        }

        public T Value
        {
            get
            {
                return API.Console.GetCVarValue<T>(Name);
            }
            set
            {
                API.Console.SetCVarValue<T>(Name, value);
            }
        }

        public CVarFlags Flags
        {
            get
            {
                return API.Console.GetCVarFlags(Name);
            }
            set
            {
                API.Console.SetCVarFlags(Name, value);
            }
        }

        /// <summary>
        /// Variable name
        /// </summary>
        private string _name;
        public string Name
        {
            get
            {
                return _name;
            }
        }

        /// <summary>
        /// The variable's help text
        /// </summary>
        public string Help
        {
            get
            {
                return API.Console.GetCVarHelpText(Name);
            }
        }
    }
}
