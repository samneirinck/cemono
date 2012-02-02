using System;
using System.Collections.Generic;
using System.IO.Ports;

/// <summary>
/// CryENGINE/Arduino communication via C# serial ports.
/// </summary>
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