using System;
using System.Runtime.InteropServices;

using CryEngine.Initialization;

namespace CryEngine
{
    public delegate void OnCryScriptInstanceDestroyedDelegate(CryScriptInstance scriptInstance);

    /// <summary>
    /// This interface permits derived classes to be used for script compilation recognition.
    /// </summary>
    public abstract class CryScriptInstance
    {
        #region Overrides
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

            if (obj is CryScriptInstance)
                return this == obj;

            return false;
        }
        #endregion

        internal virtual void OnDestroyedInternal()
        {
            IsDestroyed = true;
            ReceiveUpdates = false;

            if (OnDestroyed != null)
                OnDestroyed(this);
        }

        #region Callbacks
        /// <summary>
        /// Called each frame if script has been set to be regularly updated (See Updated property)
        /// </summary>
        public virtual void OnUpdate() { }
        #endregion


        #region Properties
        /// <summary>
        /// This script instance's id, used to keep track of instances in <see cref="CryEngine.Initialization.ScriptManager"/>.
        /// </summary>
        public int ScriptId { get; internal set; }

        /// <summary>
        /// Controls whether the entity receives an update per frame.
        /// </summary>
        public bool ReceiveUpdates { get; set; }

        /// <summary>
        /// Set to true when the script instance is removed via ScriptManager.RemoveInstances.
        /// </summary>
        public bool IsDestroyed { get; private set; }

        /// <summary>
        /// Gets the instance script, set in <see cref="CryEngine.Initialization.ScriptManager.CreateScriptInstance(CryEngine.Initialization.CryScript, object[])"/>.
        /// </summary>
        public CryScript Script { get; internal set; }
        #endregion

        #region Events
        /// <summary>
        /// Event that is invoked when this script is destroyed from <see cref="CryEngine.Initialization.ScriptManager"/>.
        /// </summary>
        public event OnCryScriptInstanceDestroyedDelegate OnDestroyed;
        #endregion
    }
}
