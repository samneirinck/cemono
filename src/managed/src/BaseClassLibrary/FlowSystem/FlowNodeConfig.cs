namespace CryEngine.FlowSystem
{
    public struct FlowNodeConfig
    {
        public InputPortConfig[] inputs
        {
            get { return inputPorts; }
            set { inputPorts = value; inputSize = inputPorts.Length; }
        }

        public OutputPortConfig[] outputs
        {
            get { return outputPorts; }
            set { outputPorts = value; outputSize = outputPorts.Length; }
        }

        public string description
        {
            get { return sDescription; }
            set { sDescription = value; }
        }

        public FlowNodeFlags category
        {
            get { return eCategory; }
            set { eCategory = value; }
        }

        private InputPortConfig[] inputPorts;
        private OutputPortConfig[] outputPorts;

        private int inputSize;
        private int outputSize;

        private string sDescription;
        private string sUIClassName;

        private FlowNodeFlags eCategory;
    }
}
