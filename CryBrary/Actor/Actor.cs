using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using CryEngine.Initialization;
using CryEngine.Extensions;
using CryEngine.Native;

namespace CryEngine
{
    /// <summary>
    /// WIP Player class. TODO: Redo, currently very limited in terms of callbacks + interoperability with C++ backend
    /// </summary>
    public abstract partial class Actor : ActorBase
    {
        /// <summary>
        /// Sets / gets the current health of this actor.
        /// </summary>
        public override float Health { get; set; }
        /// <summary>
        /// Sets / gets the max health value for this actor.
        /// </summary>
        public override float MaxHealth { get; set; }

        #region Callbacks
        /// <summary>
        /// Called when resetting the state of the entity in Editor.
        /// </summary>
        /// <param name="enteringGame">true if currently entering gamemode, false if exiting.</param>
        protected virtual void OnEditorReset(bool enteringGame) { }

        /// <summary>
        /// Called to update the view associated to this actor.
        /// </summary>
        /// <param name="viewParams"></param>
        protected virtual void UpdateView(ref ViewParams viewParams) { }

        /// <summary>
        /// Called prior to updating physics, useful for requesting movement.
        /// </summary>
        protected virtual void OnPrePhysicsUpdate() { }
        #endregion
    }
}
