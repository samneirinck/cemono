namespace CryEngine.FlowSystem
{
    public class InputPortConfig
    {
        public InputPortConfig(string sName, InputPortTypes inType, string sDescription = null, string sHumanName = null, string UIConfig = null)
        {
            this.Name = sName;
            this.Description = sDescription;
            this.HumanName = sHumanName;
            this.UIConfig = UIConfig;
            this.InputType = inType;
        }

        // name of this port
        public string Name { get; set; }
        // Human readable name of this port (default: same as name)
        public string HumanName { get; set; }
        // Human readable description of this port (help)
        public string Description { get; set; }
        // UIConfig: enums for the variable e.g 
        // "enum_string:a,b,c"
        // "enum_string:something=a,somethingelse=b,whatever=c"
        // "enum_int:something=0,somethingelse=10,whatever=20"
        // "enum_float:something=1.0,somethingelse=2.0"
        // "enum_global:GlobalEnumName"
        public string UIConfig { get; set; }

        public InputPortTypes InputType { get; set; }
    }
}
