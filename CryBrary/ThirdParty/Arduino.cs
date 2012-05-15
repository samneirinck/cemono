using System.IO.Ports;

namespace CryEngine.Arduino
{
	/// <summary>
	/// Helper functions for Arduino communication.
	/// </summary>
	[System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Naming", "CA1704:IdentifiersShouldBeSpelledCorrectly", MessageId = "Arduino")]
	public static class ArduinoHelper
	{
		/// <summary>
		/// The main port used for serial communication.
		/// </summary>
		public static SerialPort Port { get; set; }
	}
}