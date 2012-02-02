using CryEngine;
using CryEngine.Arduino;

using System.IO.Ports;

[FlowNode(UICategory = "Arduino", Description = "Controls the global port settings", Category = FlowNodeCategory.Advanced)] 
public class PortController : FlowNode
{
	[InputPort(Name = "Create Port", Description = "")]
	public void CreatePort()
	{
		if(ArduinoHelper.Port != null && ArduinoHelper.Port.IsOpen)
			ArduinoHelper.Port.Close();

		ArduinoHelper.Port = new SerialPort(GetStringValue(PortName), GetIntValue(BaudRate));
		ArduinoHelper.Port.Open();
		ActivateOutput(createdOutput);
	}

	[InputPort(Name = "Destroy Port", Description = "")]
	public void DestroyPort()
	{
		if(ArduinoHelper.Port != null && ArduinoHelper.Port.IsOpen)
			ArduinoHelper.Port.Close();

		ArduinoHelper.Port = null;
		ActivateOutput(destroyedOutput);
	}

	[InputPort(Name = "Baud Rate", Description = "")]
	public void BaudRate(int rate) { }

	[InputPort(Name = "Port Name", Description = "")]
	public void PortName(string name) { }

	[OutputPort(Name = "Created", Description = "", Type = NodePortType.Void)]
	public static int createdOutput;

	[OutputPort(Name = "Destroyed", Description = "", Type = NodePortType.Void)]
	public static int destroyedOutput;	
}