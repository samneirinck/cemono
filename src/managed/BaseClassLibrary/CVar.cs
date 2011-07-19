using System;
namespace Cemono
{
    public enum CVarFlags
    {
        /// <summary>
        /// Stays in the default state when cheats are disabled
        /// </summary>
        VF_CHEAT = 0x00000002,
        /// <summary>
        /// Can be changed on client and when connecting the var not sent to the client (should not be used during CVar creation, is set for all vars in Game/scripts/cvars.tx)
        /// </summary>
        VF_NOT_NET_SYNCED = 0x00000080,
        VF_DUMPTODISK = 0x00000100,
        /// <summary>
        /// Stored when saving a savegame
        /// </summary>
        VF_SAVEGAME = 0x00000200,
        VF_NOHELP = 0x00000400,
        /// <summary>
        /// Can not be changed by the user
        /// </summary>
        VF_READONLY = 0x00000800,
        VF_REQUIRE_LEVEL_RELOAD = 0x00001000,
        VF_REQUIRE_APP_RESTART = 0x00002000,
        /// <summary>
        /// Shows warning that this var was not used in config file
        /// </summary>
        VF_WARNING_NOTUSED = 0x00004000,
        /// <summary>
        /// Otherwise the const char * to the name will be stored without copying the memory
        /// </summary>
        VF_COPYNAME = 0x00008000,
        /// <summary>
        /// Set when variable value modified.
        /// </summary>
        VF_MODIFIED = 0x00010000,
        /// <summary>
        /// Set when variable was present in config file.
        /// </summary>
        VF_WASINCONFIG = 0x00020000,
        /// <summary>
        /// Allow bitfield setting syntax.
        /// </summary>
        VF_BITFIELD = 0x00040000,
        /// <summary>
        /// Is visible and usable in restricted (normal user) console mode
        /// </summary>
        VF_RESTRICTEDMODE = 0x00080000,
    }

    public class CVar<T> where T : IConvertible
    {
        public CVar(string name)
        {
            // Disallow certain types
            var type = typeof(T);

            if (type != typeof(float) && type != typeof(string) && type != typeof(int))
            {
                throw new ArgumentException("Tried creating a CVar of type {0}, only int/float/string is allowed.", type.Name);
            }
            _name = name;
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
