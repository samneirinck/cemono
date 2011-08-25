namespace CryEngine.FlowSystem
{
    public class OutputPortConfig
    {
        public OutputPortConfig(string sName, OutputPortTypes outType, string sDescription = null, string sHumanName = null)
        {
            this.Name = sName;
            this.Description = sDescription;
            this.HumanName = sHumanName;
            this.OutputType = outType;
        }

        /// <summary>
        /// Name of the port
        /// </summary>
        public string Name { get; set; }

        /// <summary>
        ///  Human readable name of this port (default: same as name)
        /// </summary>
        public string HumanName { get; set; }

        /// <summary>
        ///  Human readable description of this port (help)
        /// </summary>
        public string Description { get; set; }

        /// <summary>
        /// Output port type
        /// </summary>
        public OutputPortTypes OutputType;
    }
}
