using CryEngine;
using CryEngine.Arduino;

using System.IO.Ports;

[FlowNode(UICategory = "Arduino", Description = "Test script", Category = FlowNodeCategory.Advanced)]
public class GenericMessage : FlowNode
{
	[InputPort(Name = "Send", Description = "")]
	public void Send()
	{
		ArduinoHelper.Port.WriteLine(GetPortString(MessageSelector));
	}

	[InputPort(Name = "Message", Description = "")]
	public void MessageSelector(string pin) { }
}

/* REFERENCE ARDUINO SCRIPT

const int led = 13;

void setup()
{
  pinMode(led, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  if(Serial.available() > 0)
  {
    char incoming = (char)Serial.read();

    switch(incoming)
    {
    case '1':
      digitalWrite(led, HIGH);
      break;
    case '0':
      digitalWrite(led, LOW);
      break;
    }
  }
}
*/