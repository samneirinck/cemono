using System;
using System.Linq;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using CryEngine.Native;
using CryEngine.Initialization;

namespace CryEngine
{
	public sealed class Attachment : Entity
	{
		#region Statics
		internal static Attachment TryAdd(IntPtr ptr, EntityBase owner)
		{
			if (ptr == IntPtr.Zero)
				return null;

			var attachment = ScriptManager.Instance.Find<Attachment>(ScriptType.Entity, x => x.AttachmentHandleRef.Handle == ptr);
			if (attachment != null)
				return attachment;

			attachment = new Attachment(ptr, owner);

			return attachment;
		}
		#endregion

		internal Attachment(IntPtr ptr, EntityBase owner)
		{
			Owner = owner;
			this.SetAttachmentHandle(new HandleRef(this, ptr));

			string attachmentObject = NativeMethods.Entity.GetAttachmentObject(this.GetAttachmentHandle().Handle);

			var tempEntity = Entity.Spawn<NativeEntity>("AttachmentEntity");

			Id = tempEntity.Id;
			this.SetEntityHandle(tempEntity.GetEntityHandle());

			ScriptManager.Instance.RemoveInstance(tempEntity.ScriptId, ScriptType.Entity);
			ScriptManager.Instance.AddScriptInstance(this, ScriptType.Entity);

			this.SetEntityAttachmentHandle(new HandleRef(this, NativeMethods.Entity.LinkEntityToAttachment(this.GetAttachmentHandle().Handle, Id)));

			if(!String.IsNullOrEmpty(attachmentObject)) // Just in case it had a model loaded by default
				LoadObject(attachmentObject);
		}

		public QuatT Absolute { get { return NativeMethods.Entity.GetAttachmentAbsolute(this.GetAttachmentHandle().Handle); } }
		public QuatT Relative { get { return NativeMethods.Entity.GetAttachmentRelative(this.GetAttachmentHandle().Handle); } }
		public QuatT DefaultAbsolute { get { return NativeMethods.Entity.GetAttachmentDefaultAbsolute(this.GetAttachmentHandle().Handle); } }
		public QuatT DefaultRelative { get { return NativeMethods.Entity.GetAttachmentDefaultRelative(this.GetAttachmentHandle().Handle); } }

		public EntityBase Owner { get; private set; }

		bool useEntityPos;
		public bool UseEntityPosition 
		{ 
			get { return useEntityPos; }
			set
			{
				useEntityPos = value;

				NativeMethods.Entity.AttachmentUseEntityPosition(this.GetEntityAttachmentHandle().Handle, value);
			}
		}
		bool useEntityRot;
		public bool UseEntityRotation
		{
			get { return useEntityPos; }
			set
			{
				useEntityRot = value;

				NativeMethods.Entity.AttachmentUseEntityRotation(this.GetEntityAttachmentHandle().Handle, value);
			}
		}

		/// <summary>
		/// CMonoEntityAttachment *
		/// </summary>
		internal HandleRef EntityAttachmentHandleRef { get; set; }
		internal HandleRef AttachmentHandleRef { get; set; }
	}
}
