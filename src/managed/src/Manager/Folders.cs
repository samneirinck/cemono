namespace Cemono
{
    public class Folders : System.MarshalByRefObject
    {
        public string RootFolder { get; set; }
        public string GameFolder { get; set; }
        public string NodeFolder { get; set; }
        public string LogicFolder { get; set; }
        public string EntitiesFolder { get; set; }
    }
}
