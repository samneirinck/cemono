using System;
using System.Collections.Generic;
using System.IO.Ports;

namespace CryEngine.Arduino
{
	/// <summary>
	/// Helper functions for Arduino communication.
	/// </summary>
	public static class ArduinoHelper
	{
		/// <summary>
		/// The main port used for serial communication.
		/// </summary>
		public static SerialPort Port { get; set; }
	}
}