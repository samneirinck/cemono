using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using CryEngine.Initialization;
using CryEngine.Native;

namespace CryEngine
{
    /// <summary>
    /// Represents an CryENGINE entity
    /// </summary>
    public abstract class EntityBase : CryScriptInstance
    {
        internal PhysicalEntity _physics;

        /// <summary>
        /// Gets this entity's Lua script table, providing it exists.
        /// </summary>
        public Lua.ScriptTable ScriptTable { get { return Lua.ScriptTable.Get(this.GetEntityHandle()); } }

        /// <summary>
        /// Gets or sets a value indicating whether the player is hidden or not.
        /// </summary>
        public bool Hidden { get { return NativeMethods.Entity.IsHidden(this.GetEntityHandle()); } set { NativeMethods.Entity.Hide(this.GetEntityHandle(), value); } }

        /// <summary>
        /// Gets or sets the world space entity position.
        /// </summary>
        public Vec3 Position { get { return NativeMethods.Entity.GetWorldPos(this.GetEntityHandle()); } set { NativeMethods.Entity.SetWorldPos(this.GetEntityHandle(), value); } }

        /// <summary>
        /// Gets or sets the world space entity orientation quaternion.
        /// </summary>
        public Quat Rotation { get { return NativeMethods.Entity.GetWorldRotation(this.GetEntityHandle()); } set { NativeMethods.Entity.SetWorldRotation(this.GetEntityHandle(), value); } }

        /// <summary>
        /// Gets or sets the local space entity position.
        /// </summary>
        public Vec3 LocalPosition { get { return NativeMethods.Entity.GetPos(this.GetEntityHandle()); } set { NativeMethods.Entity.SetPos(this.GetEntityHandle(), value); } }

        /// <summary>
        /// Gets or sets the local space entity orientation quaternion.
        /// </summary>
        public Quat LocalRotation { get { return NativeMethods.Entity.GetRotation(this.GetEntityHandle()); } set { NativeMethods.Entity.SetRotation(this.GetEntityHandle(), value); } }

        /// <summary>
        /// Gets or sets the world space entity transformation matrix.
        /// </summary>
        public Matrix34 Transform { get { return NativeMethods.Entity.GetWorldTM(this.GetEntityHandle()); } set { NativeMethods.Entity.SetWorldTM(this.GetEntityHandle(), value); } }

        /// <summary>
        /// Gets or sets the local space entity transformation matrix.
        /// </summary>
        public Matrix34 LocalTransform { get { return NativeMethods.Entity.GetLocalTM(this.GetEntityHandle()); } set { NativeMethods.Entity.SetLocalTM(this.GetEntityHandle(), value); } }

        /// <summary>
        /// Gets the entity axis aligned bounding box in the world space.
        /// </summary>
        public BoundingBox BoundingBox { get { return NativeMethods.Entity.GetWorldBoundingBox(this.GetEntityHandle()); } }

        /// <summary>
        /// Gets the entity axis aligned bounding box in the world space.
        /// </summary>
        public BoundingBox LocalBoundingBox { get { return NativeMethods.Entity.GetBoundingBox(this.GetEntityHandle()); } }

        /// <summary>
        /// Gets or sets the entity name.
        /// </summary>
        public string Name { get { return NativeMethods.Entity.GetName(this.GetEntityHandle()); } set { NativeMethods.Entity.SetName(this.GetEntityHandle(), value); } }

        /// <summary>
        /// Gets or sets the entity flags.
        /// </summary>
        public EntityFlags Flags { get { return NativeMethods.Entity.GetFlags(this.GetEntityHandle()); } set { NativeMethods.Entity.SetFlags(this.GetEntityHandle(), value); } }

        /// <summary>
        /// Gets or sets the material currently assigned to this entity.
        /// </summary>
        public Material Material { get { return Material.Get(this); } set { Material.Set(this, value); } }

        /// <summary>
        /// Gets  the runtime unique identifier of this entity assigned to it by the Entity System.
        /// EntityId may not be the same when saving/loading entity.
        /// EntityId is mostly used in runtime for fast and unique identification of entities..
        /// </summary>
        public EntityId Id { get; internal set; }

        public EntityUpdatePolicy UpdatePolicy { get { return NativeMethods.Entity.GetUpdatePolicy(this.GetEntityHandle()); } set { NativeMethods.Entity.SetUpdatePolicy(this.GetEntityHandle(), value); } }

        /// <summary>
        /// Gets or sets entity velocity as set by the physics system.
        /// </summary>
        public Vec3 Velocity
        {
            get { return NativeMethods.Physics.GetVelocity(this.GetEntityHandle()); }
            set { NativeMethods.Physics.SetVelocity(this.GetEntityHandle(), value); }
        }

        /// <summary>
        /// Gets the physical entity, contains essential functions for modifying the entitys existing physical state.
        /// </summary>
        public PhysicalEntity Physics
        {
            get { return _physics ?? (_physics = new PhysicalEntity(this)); }
        }

        #region Native handles
        /// <summary>
        /// Gets or sets IEntity handle
        /// </summary>
        internal IntPtr EntityHandle { get; set; }

        /// <summary>
        /// Gets or sets IAnimatedCharacter handle
        /// </summary>
        internal IntPtr AnimatedCharacterHandle { get; set; }
        #endregion

        /// <summary>
        /// Removes the entity from the CryEngine world.
        /// </summary>
        /// <param name="forceRemoveNow">If true, the entity will be removed immediately.</param>
        public virtual void Remove(bool forceRemoveNow = false)
        {
            Entity.Remove(Id, forceRemoveNow);
        }

        /// <summary>
        /// Retrieves the flags of the specified slot.
        /// </summary>
        /// <param name="slot">Index of the slot</param>
        /// <returns>The slot flags, or 0 if specified slot is not valid.</returns>
        public EntitySlotFlags GetSlotFlags(int slot = 0)
        {
            return NativeMethods.Entity.GetSlotFlags(this.GetEntityHandle(), slot);
        }

        /// <summary>
        /// Sets the flags of the specified slot.
        /// </summary>
        /// <param name="flags">Flags to set.</param>
        /// <param name="slot">Index of the slot, if -1 apply to all existing slots.</param>
        public void SetSlotFlags(EntitySlotFlags flags, int slot = 0)
        {
            NativeMethods.Entity.SetSlotFlags(this.GetEntityHandle(), slot, flags);
        }

#region Attachments
        /// <summary>
        /// Gets the attachment at the specified slot and index.
        /// </summary>
        /// <param name="index">Attachment index</param>
        /// <param name="characterSlot">Index of the character slot we wish to get an attachment from</param>
        /// <returns>null if failed, otherwise the attachment.</returns>
        public Attachment GetAttachment(int index, int characterSlot = 0)
        {
            var ptr = NativeMethods.Entity.GetAttachmentByIndex(this.GetEntityHandle(), index, characterSlot);
            if (ptr == IntPtr.Zero)
                return null;

            return Attachment.TryAdd(ptr, this);
        }

        /// <summary>
        /// Gets the attachment by name at the specified slot.
        /// </summary>
        /// <param name="name">Attachment name</param>
        /// <param name="characterSlot">Index of the character slot we wish to get an attachment from</param>
        /// <returns>null if failed, otherwise the attachment.</returns>
        public Attachment GetAttachment(string name, int characterSlot = 0)
        {
            var ptr = NativeMethods.Entity.GetAttachmentByName(this.GetEntityHandle(), name, characterSlot);

            return Attachment.TryAdd(ptr, this);
        }

        /// <summary>
        /// Gets the number of attachments at the specified character slot.
        /// </summary>
        /// <param name="characterSlot">Index of the slot we wish to get the attachment count of</param>
        /// <returns>Number of attachments at the specified slot</returns>
        public int GetAttachmentCount(int characterSlot = 0)
        {
            return NativeMethods.Entity.GetAttachmentCount(this.GetEntityHandle(), characterSlot); 
        }
#endregion

        /// <summary>
        /// Links to another entity, becoming the parent. Any change to the parent object is propagated to all child (linked) objects.
        /// </summary>
        /// <param name="linkName">Name of the link</param>
        /// <param name="otherEntityId">Id of the entity we wish to be linked to</param>
        /// <param name="relativeRot"></param>
        /// <param name="relativePos"></param>
        /// <returns>true if successful, otherwise false.</returns>
        public bool Link(string linkName, EntityId otherEntityId, Quat relativeRot, Vec3 relativePos)
        {
            return NativeMethods.Entity.AddEntityLink(this.GetEntityHandle(), linkName, otherEntityId, relativeRot, relativePos);
        }

        /// <summary>
        /// Removes an entity link, see <see cref="Link"/>.
        /// </summary>
        /// <param name="otherEntityId">Id of the entity we are currently linked to</param>
        public void Unlink(EntityId otherEntityId)
        {
            NativeMethods.Entity.RemoveEntityLink(this.GetEntityHandle(), otherEntityId);
        }

        /// <summary>
        /// Loads a light source to the specified slot, or to the next available slot.
        /// </summary>
        /// <param name="parameters">New params of the light source we wish to load</param>
        /// <param name="slot">Slot we want to load the light into, if -1 chooses the next available slot.</param>
        /// <returns>The slot where the light source was loaded, or -1 if loading failed.</returns>
        public int LoadLight(LightParams parameters, int slot = 1)
        {
            return NativeMethods.Entity.LoadLight(this.GetEntityHandle(), slot, parameters);
        }

        /// <summary>
        /// Loads a mesh for this entity. Can optionally load multiple meshes using entity slots.
        /// </summary>
        /// <param name="name">Path to the object (Relative to the game directory)</param>
        /// <param name="slot"></param>
        /// <returns>true if successful, otherwise false.</returns>
        public bool LoadObject(string name, int slot = 0)
        {
            if (name == null)
                throw new ArgumentNullException("name");

            if (name.EndsWith("cgf"))
                NativeMethods.Entity.LoadObject(this.GetEntityHandle(), name, slot);
            else if (name.EndsWith("cdf") || name.EndsWith("cga") || name.EndsWith("chr"))
                NativeMethods.Entity.LoadCharacter(this.GetEntityHandle(), name, slot);
            else
                return false;

            return true;
        }

        /// <summary>
        /// Gets the path to the currently loaded object.
        /// </summary>
        /// <param name="slot">Slot containing the object we want to know the path of.</param>
        /// <returns>Path to the currently loaded object at the specified slot.</returns>
        public string GetObjectFilePath(int slot = 0)
        {
            return NativeMethods.Entity.GetStaticObjectFilePath(this.GetEntityHandle(), slot);
        }

        /// <summary>
        /// Plays a raw animation.
        /// </summary>
        /// <param name="animationName">Name of the animation we wish to play</param>
        /// <param name="flags"></param>
        /// <param name="slot">Slot on which to play the animation</param>
        /// <param name="layer">Animation layer to play the animation in.</param>
        /// <param name="blend">Transition time between two animations.</param>
        /// <param name="speed">Animation playback speed</param>
        public void PlayAnimation(string animationName, AnimationFlags flags = 0, int slot = 0, int layer = 0, float blend = 0.175f, float speed = 1.0f)
        {
            NativeMethods.Entity.PlayAnimation(this.GetEntityHandle(), animationName, slot, layer, blend, speed, flags);
        }

        /// <summary>
        /// Stops the currently playing animation.
        /// </summary>
        /// <param name="slot">The character slot.</param>
        /// <param name="layer">The animation layer which we want to stop. If -1, stops all layers.</param>
        /// <param name="blendOutTime"></param>
        public void StopAnimation(int slot = 0, int layer = 0, float blendOutTime = 0)
        {
            if (layer == -1)
                NativeMethods.Entity.StopAnimationsInAllLayers(this.GetEntityHandle(), slot);
            else
                NativeMethods.Entity.StopAnimationInLayer(this.GetEntityHandle(), slot, layer, blendOutTime);
        }

        /// <summary>
        /// Frees the specified slot of all objects.
        /// </summary>
        /// <param name="slot"></param>
        public void FreeSlot(int slot)
        {
            NativeMethods.Entity.FreeSlot(this.GetEntityHandle(), slot);
        }

        /// <summary>
        /// Requests movement at the specified slot, providing an animated character is currently loaded.
        /// </summary>
        /// <param name="request"></param>
        public void AddMovement(ref EntityMovementRequest request)
        {
            NativeMethods.Entity.AddMovement(this.GetAnimatedCharacterHandle(), ref request);
        }

        /// <summary>
        /// Gets the absolute of the specified joint
        /// </summary>
        /// <param name="jointName">Name of the joint</param>
        /// <param name="characterSlot">Slot containing the character</param>
        /// <returns>Absolute of the specified joint</returns>
        public QuatT GetJointAbsolute(string jointName, int characterSlot = 0)
        {
            return NativeMethods.Entity.GetJointAbsolute(this.GetEntityHandle(), jointName, characterSlot);
        }

        /// <summary>
        /// Gets the default absolute of the specified joint
        /// </summary>
        /// <param name="jointName">Name of the joint</param>
        /// <param name="characterSlot">Slot containing the character</param>
        /// <returns>Default absolute of the specified joint</returns>
        public QuatT GetJointAbsoluteDefault(string jointName, int characterSlot = 0)
        {
            return NativeMethods.Entity.GetJointAbsoluteDefault(this.GetEntityHandle(), jointName, characterSlot);
        }

        /// <summary>
        /// Sets the absolute of the specified joint
        /// </summary>
        /// <param name="jointName">Name of the joint</param>
        /// <param name="absolute">New absolute</param>
        /// <param name="characterSlot">Slot containing the character</param>
        public void SetJointAbsolute(string jointName, QuatT absolute, int characterSlot = 0)
        {
            NativeMethods.Entity.SetJointAbsolute(this.GetEntityHandle(), jointName, characterSlot, absolute);
        }

        /// <summary>
        /// Gets the relative of the specified joint
        /// </summary>
        /// <param name="jointName">Name of the joint</param>
        /// <param name="characterSlot">Slot containing the character</param>
        /// <returns>Relative of the specified joint</returns>
        public QuatT GetJointRelative(string jointName, int characterSlot = 0)
        {
            return NativeMethods.Entity.GetJointRelative(this.GetEntityHandle(), jointName, characterSlot);
        }

        /// <summary>
        /// Gets the default relative of the specified joint
        /// </summary>
        /// <param name="jointName">Name of the joint</param>
        /// <param name="characterSlot">Slot containing the character</param>
        /// <returns>Default relative of the specified joint</returns>
        public QuatT GetJointRelativeDefault(string jointName, int characterSlot = 0)
        {
            return NativeMethods.Entity.GetJointRelativeDefault(this.GetEntityHandle(), jointName, characterSlot);
        }
    }
}