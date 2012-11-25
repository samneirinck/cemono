using System;
using System.Linq;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using CryEngine.Native;
using CryEngine.Initialization;

namespace CryEngine
{
    /// <summary>
    /// Represents a character attachment, obtained via <see cref="CryEngine.EntityBase.GetAttachment(int, int)"/> and <see cref="CryEngine.EntityBase.GetAttachment(string, int)"/>.
    /// </summary>
    [Entity(Flags = EntityClassFlags.Invisible)]
    public sealed class Attachment : Entity
    {
        #region Statics
        internal static Attachment TryAdd(IntPtr ptr, EntityBase owner)
        {
            if (ptr == IntPtr.Zero)
                return null;

            var attachment = ScriptManager.Instance.Find<Attachment>(ScriptType.Entity, x => x.AttachmentHandle == ptr);
            if (attachment != null)
                return attachment;

            attachment = new Attachment(ptr, owner);

            return attachment;
        }
        #endregion

        private Attachment() { }

        internal Attachment(IntPtr ptr, EntityBase owner)
        {
            Owner = owner;
            Owner.OnDestroyed += (instance) => { NativeEntityMethods.RemoveEntity(Id, false); };

            this.SetAttachmentHandle(ptr);

            string attachmentObject = NativeEntityMethods.GetAttachmentObject(this.GetAttachmentHandle());

            var tempEntity = Entity.Spawn("AttachmentEntity", typeof(NativeEntity).Name);

            Id = tempEntity.Id;
            this.SetEntityHandle(tempEntity.GetEntityHandle());

            ScriptManager.Instance.ReplaceScriptInstance(this, tempEntity.ScriptId, ScriptType.Entity);

            this.SetEntityAttachmentHandle(NativeEntityMethods.LinkEntityToAttachment(this.GetAttachmentHandle(), Id));

            if (!String.IsNullOrEmpty(attachmentObject)) // Just in case it had a model loaded by default
                LoadObject(attachmentObject);
        }

        public QuatT Absolute { get { return NativeEntityMethods.GetAttachmentAbsolute(this.GetAttachmentHandle()); } }

        public QuatT Relative { get { return NativeEntityMethods.GetAttachmentRelative(this.GetAttachmentHandle()); } }

        public QuatT DefaultAbsolute { get { return NativeEntityMethods.GetAttachmentDefaultAbsolute(this.GetAttachmentHandle()); } }

        public QuatT DefaultRelative { get { return NativeEntityMethods.GetAttachmentDefaultRelative(this.GetAttachmentHandle()); } }

        /// <summary>
        /// Gets the entity this attachment is attached to.
        /// </summary>
        public EntityBase Owner { get; private set; }

        bool useEntityPos;

        /// <summary>
        /// Gets or sets a value indicating whether to automatically position the attachment object.
        /// </summary>
        public bool UseEntityPosition 
        { 
            get { return useEntityPos; }
            set
            {
                useEntityPos = value;

                NativeEntityMethods.AttachmentUseEntityPosition(this.GetEntityAttachmentHandle(), value);
            }
        }

        bool useEntityRot;

        /// <summary>
        /// Gets or sets a value indicating whether to automatically rotate the attachment object.
        /// </summary>
        public bool UseEntityRotation
        {
            get { return useEntityPos; }
            set
            {
                useEntityRot = value;

                NativeEntityMethods.AttachmentUseEntityRotation(this.GetEntityAttachmentHandle(), value);
            }
        }

        /// <summary>
        /// Gets or sets CMonoEntityAttachment *
        /// </summary>
        internal IntPtr EntityAttachmentHandle { get; set; }

        /// <summary>
        /// Gets or sets IAttachment *
        /// </summary>
        internal IntPtr AttachmentHandle { get; set; }
    }
}
