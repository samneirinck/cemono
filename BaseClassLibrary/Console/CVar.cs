using System;
using System.Collections.Generic;
using System.Linq;

namespace CryEngine
{
	public partial class CryConsole
	{
        static List<CVar> m_internalCVars = new List<CVar>();

        /// <summary>
        /// Invoked by a CVar after its creation. Should <b>not</b> be visible outside this assembly.
        /// </summary>
        /// <param name="cvar"></param>
        public static void RegisterCVar(string name, object value, CVarFlags flags, string help)
        {
            m_internalCVars.Add(new CVar(name));
            CVar cvar = m_internalCVars.Last();

            switch(cvar.Type)
            {
                case CVarType.Float:
                    {
                        _RegisterCVarFloat(cvar.Name, ref cvar.FVal, cvar.FVal, cvar.Flags, cvar.Help);
                    }
                    break;
                case CVarType.Int:
                    {
                        _RegisterCVarInt(cvar.Name, ref cvar.IVal, cvar.IVal, cvar.Flags, cvar.Help);
                    }
                    break;
                case CVarType.String:
                    {
                        //_RegisterCVarString(cvar.Name, ref cvar.FVal, cvar.FVal, cvar.Flags, cvar.Help);
                    }
                    break;
            }
        }

        public static CVar GetCVar(string name)
        {
            CVar cvar = m_internalCVars.Where(var => var.Name.Equals(name)).FirstOrDefault();
            if(cvar!=null && cvar!=default(CVar))
                return cvar;

            if (CryConsole._HasCVar(name))
                return new CVar(name);

            return null;
        }

		/// <summary>
		/// When true, accessing a CVar that has not yet been registered will not throw an exception but instead return -1 or string.Empty;
		/// Attempting an invalid set will simply fail.
		/// </summary>
		public static bool IgnoreNonExistentCVars { get; set; }
	}

	public class CVar
    {
        /// <summary>
        /// Used by CryConsole.RegisterCVar to construct the CVar.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        /// <param name="flags"></param>
        /// <param name="help"></param>
        internal CVar(string name, object value, CVarFlags flags, string help)
        {
            Flags = flags;
            Help = help;
            Name = name;

            if (value is int)
            {
                Type = CVarType.Int;
                IVal = (int)value;
            }
            else if (value is float || value is double)
            {
                Type = CVarType.Float;
                FVal = (float)value;
            }
            else if (value is string)
            {
                Type = CVarType.String;
                String = (string)value;
            }
            else
                throw new CVarException(string.Format("Invalid data type ({0}) used in CVar {1}.", value.GetType().ToString(), Name));
        }

        /// <summary>
        /// Used to construct an "empty" CVar used to handle C++ registered CVars.
        /// </summary>
        /// <param name="name"></param>
        internal CVar(string name)
        {
            Name = name;

            ExternallyRegistered = true;
        }

        public void Set(float value)
        {
            // This was most likely registered in C++
            if (ExternallyRegistered)
                CryConsole._SetCVarFloat(Name, value);

            FVal = value;
        }

        public void Set(int value)
        {
            // This was most likely registered in C++
            if (ExternallyRegistered)
                CryConsole._SetCVarInt(Name, value);

            IVal = value;
        }

        public void Set(string value)
        {
            // This was most likely registered in C++
            if (ExternallyRegistered)
                CryConsole._SetCVarString(Name, value);

            String = value;
        }

        public float GetFVal()
        {
            // This was most likely registered in C++
            if (ExternallyRegistered)
                return CryConsole._GetCVarFloat(Name);

            return FVal;
        }

        public int GetIVal()
        {
            // This was most likely registered in C++
            if (ExternallyRegistered)
                return CryConsole._GetCVarInt(Name);

            return IVal;
        }

        public string GetString()
        {
            // This was most likely registered in C++
            if (ExternallyRegistered)
                return CryConsole._GetCVarString(Name);

            return String;
        }

		public string Name { get; set; }
		public string Help { get; private set; }
		public CVarFlags Flags { get; private set; }
		public CVarType Type { get; private set; }

        internal string String;
        internal float FVal;
        internal int IVal;

        internal bool ExternallyRegistered;

        public override bool Equals(object obj)
        {
            if (obj == null)
                return false;

            CVar cvar = obj as CVar;
            if ((object)cvar == null)
                return false;

            return Name.Equals(cvar.Name);
        }
	}

	public enum CVarType
	{
        Int,
        Float,
		String
	}

    public enum CVarFlags
    {
        /// <summary>
        /// just to have one recognizable spot where the flags are located in the Register call
        /// </summary>
        Null = 0x00000000,
        /// <summary>
        /// stays in the default state when cheats are disabled
        /// </summary>
        Cheat = 0x00000002,
        DumpToDisk = 0x00000100,
        /// <summary>
        /// can not be changed by the user
        /// </summary>
        ReadOnly = 0x00000800,
        RequireLevelReload = 0x00001000,
        RequireAppRestart = 0x00002000,
        /// <summary>
        ///  shows warning that this var was not used in config file
        /// </summary>
        WarningNotUsed = 0x00004000,
        /// <summary>
        /// otherwise the const char * to the name will be stored without copying the memory
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
        Bitfield = 0x00040000,
        /// <summary>
        /// is visible and usable in restricted (normal user) console mode
        /// </summary>
        RestrictedMode = 0x00080000,
        /// <summary>
        /// Invisible to the user in console
        /// </summary>
        Invisible = 0x00100000,
        /// <summary>
        /// Always accept variable value and call on change callback even if variable value didnt change
        /// </summary>
        AlwaysOnChange = 0x00200000,
        /// <summary>
        /// Blocks the execution of console commands for one frame
        /// </summary>
        BlockFrame = 0x00400000,
        /// <summary>
        /// Set if it is a const cvar not to be set inside cfg-files
        /// </summary>
        ConstCVar = 0x00800000,
        /// <summary>
        /// This variable is critical to check in every hash, since it's extremely vulnerable
        /// </summary>
        CheatAlwaysCheck = 0x01000000,
        /// <summary>
        /// This variable is set as VF_CHEAT but doesn't have to be checked/hashed since it's harmless to workaround
        /// </summary>
        CheatNoCheck = 0x02000000,

        // These flags should never be set during cvar creation, and probably never set manually.
        InternalFlagsStart = 0x00000080,
        /// <summary>
        /// can be changed on client and when connecting the var not sent to the client (is set for all vars in Game/scripts/Network/cvars.txt)
        /// </summary>
        NotNetSyncedInternal = InternalFlagsStart,
        InternalFlagsEnd = NotNetSyncedInternal
    }

	/// <summary>
	/// This exception is called when invalid CVar operations are performed.
	/// </summary>
	public class CVarException : Exception
	{
		public CVarException(string message) { }
	}
}
