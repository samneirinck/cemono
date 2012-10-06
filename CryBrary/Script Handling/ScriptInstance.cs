using System;

namespace CryEngine
{
	/// <summary>
	/// This interface permits derived classes to be used for script compilation recognition.
	/// </summary>
	public abstract class CryScriptInstance
	{
		public delegate void OnDestroyedDelegate(OnDestroyedEventArgs args);

		public class OnDestroyedEventArgs : EventArgs
		{
			public OnDestroyedEventArgs(int scriptId)
			{
				ScriptId = scriptId;
			}

			public int ScriptId { get; private set; }
		}

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
            if (obj == null)
                return false;

			if(obj is CryScriptInstance)
				return obj.GetHashCode() == GetHashCode();

			return false;
		}

        internal virtual void OnScriptReloadInternal() { OnScriptReload(); }
		public virtual void OnScriptReload() { }

		internal virtual void OnDestroyedInternal()
		{
			IsDestroyed = true;

			Destroyed(new OnDestroyedEventArgs(ScriptId));
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

		/// <summary>
		/// Set to true when the script instance is removed via ScriptManager.RemoveInstances.
		/// </summary>
		public bool IsDestroyed { get; private set; }

		public event OnDestroyedDelegate Destroyed;
	}
}