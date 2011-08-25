namespace CryEngine.FlowSystem
{
    public struct InputPortConfig
    {
        public InputPortConfig(string sName, InputPortTypes inType, string sDescription = null, string sHumanName = null, string UIConfig = null)
        {
            name = sName;
            description = sDescription;
            humanName = sHumanName;
            sUIConfig = UIConfig;

            inputType = inType;
        }

        // name of this port
        public string name;
        // Human readable name of this port (default: same as name)
        public string humanName;
        // Human readable description of this port (help)
        public string description;
        // UIConfig: enums for the variable e.g 
        // "enum_string:a,b,c"
        // "enum_string:something=a,somethingelse=b,whatever=c"
        // "enum_int:something=0,somethingelse=10,whatever=20"
        // "enum_float:something=1.0,somethingelse=2.0"
        // "enum_global:GlobalEnumName"
        public string sUIConfig;

        public InputPortTypes inputType;
    }
}
