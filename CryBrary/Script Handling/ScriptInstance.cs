namespace CryEngine
{
	/// <summary>
	/// This interface permits derived classes to be used for script compilation recognition.
	/// </summary>
	public abstract class CryScriptInstance 
    {
		public override int GetHashCode()
		{
			unchecked // Overflow is fine, just wrap
			{
				int hash = 17;

				hash = hash * 29 + ScriptId.GetHashCode();
				hash = hash * 29 + ReceiveUpdates.GetHashCode();

				return hash;
			}
		}

		public override bool Equals(object obj)
		{
			if(obj is CryScriptInstance)
				return obj.GetHashCode() == GetHashCode();

			return false;
		}

        /// <summary>
        /// Called each frame if script has been set to be regularly updated (See Updated property)
        /// </summary>
        public virtual void OnUpdate() { }

		public int ScriptId { internal set; get; }

		/// <summary>
		/// Controls whether the entity receives an update per frame.
		/// </summary>
		public bool ReceiveUpdates { get; set; }
    }
}