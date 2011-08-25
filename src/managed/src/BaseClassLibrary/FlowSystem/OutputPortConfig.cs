namespace CryEngine.FlowSystem
{
    public struct OutputPortConfig
    {
        public OutputPortConfig(string sName, OutputPortTypes outType, string sDescription = null, string sHumanName = null)
        {
            this.Name = sName;
            this.Description = sDescription;
            this.HumanName = sHumanName;

            outputType = outType;
        }

        // name of this port
        public string Name { get; set; }
        // Human readable name of this port (default: same as name)
        public string HumanName {                 g  et  ;   set; }
        // Human readable description of this port (help)
        public string Description {get;set;}

        public OutputPortTypes outputType;
    }
}
