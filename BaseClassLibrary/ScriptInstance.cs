
namespace CryEngine
{
	/// <summary>
	/// This interface permits derived classes to be used for script compilation recognition.
	/// </summary>
	public class CryScriptInstance 
    {
        public int ScriptId { internal set; get; }

        /// <summary>
        /// Called each frame if script has been set to be regularly updated (See Updated property)
        /// </summary>
        public virtual void OnUpdate() { }

		/// <summary>
		/// Controls whether the entity receives an update per frame.
		/// </summary>
		public bool ReceiveUpdates { get; set; }
    }
}