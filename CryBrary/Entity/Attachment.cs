using System;
using System.Linq;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using CryEngine.Native;

namespace CryEngine
{
	public class Attachment
	{
		#region Statics
		internal static Attachment TryAdd(IntPtr ptr)
		{
			if (ptr == IntPtr.Zero)
				return null;

			var attachment = Attachments.FirstOrDefault(x => x.AttachmentHandleRef.Handle == ptr);
			if (attachment != default(Attachment))
				return attachment;

			attachment = new Attachment(ptr);
			Attachments.Add(attachment);

			return attachment;
		}

		static List<Attachment> Attachments = new List<Attachment>();
		#endregion

		internal Attachment(IntPtr ptr)
		{
			this.SetAttachmentHandle(new HandleRef(this, ptr));
		}

		void SetupAttachmentEntity()
		{
			string attachmentObject = NativeMethods.Entity.GetAttachmentObject(this.GetAttachmentHandle().Handle);

			AttachmentEntity = Entity.Spawn<NativeEntity>("AttachmentEntity");
			NativeMethods.Entity.LinkEntityToAttachment(this.GetAttachmentHandle().Handle, AttachmentEntity.Id);

			AttachmentEntity.LoadObject("objects/tanks/turret_heavy.chr");
		}

		public Vec3 Position 
		{ 
			get 
			{
				if (AttachmentEntity == null || AttachmentEntity.IsDestroyed)
					SetupAttachmentEntity();

				return AttachmentEntity.Position;
			}
			set
			{
				if (AttachmentEntity == null || AttachmentEntity.IsDestroyed)
					SetupAttachmentEntity();

				AttachmentEntity.Position = value;
			}
		}
		public Vec3 LocalPosition 
		{
			get
			{
				if (AttachmentEntity == null || AttachmentEntity.IsDestroyed)
					SetupAttachmentEntity();

				return AttachmentEntity.LocalPosition;
			}
			set
			{
				if (AttachmentEntity == null || AttachmentEntity.IsDestroyed)
					SetupAttachmentEntity();

				AttachmentEntity.LocalPosition = value;
			}
		}
		public Vec3 DefaultPosition { get { return NativeMethods.Entity.GetAttachmentDefaultWorldPosition(this.GetAttachmentHandle().Handle); } }
		public Vec3 DefaultLocalPosition { get { return NativeMethods.Entity.GetAttachmentDefaultLocalPosition(this.GetAttachmentHandle().Handle); } }

		public Quat Rotation 
		{ 
			get 
			{
				if (AttachmentEntity == null || AttachmentEntity.IsDestroyed)
					SetupAttachmentEntity();

				return AttachmentEntity.Rotation;
			}
			set
			{
				if (AttachmentEntity == null || AttachmentEntity.IsDestroyed)
					SetupAttachmentEntity();

				AttachmentEntity.Rotation = value;
			}
		}
		public Quat LocalRotation 
		{ 
			get 
			{
				if (AttachmentEntity == null || AttachmentEntity.IsDestroyed)
					SetupAttachmentEntity();

				return AttachmentEntity.LocalRotation;
			}
			set
			{
				if (AttachmentEntity == null || AttachmentEntity.IsDestroyed)
					SetupAttachmentEntity();

				AttachmentEntity.LocalRotation = value;
			}
		}
		public Quat DefaultRotation { get { return NativeMethods.Entity.GetAttachmentDefaultWorldRotation(this.GetAttachmentHandle().Handle); } }
		public Quat DefaultLocalRotation { get { return NativeMethods.Entity.GetAttachmentDefaultLocalRotation(this.GetAttachmentHandle().Handle); } }

		/// <summary>
		/// Used internally to allow repositioning attachments
		/// </summary>
		Entity AttachmentEntity { get; set; }

		public Material Material
		{
			get { return Material.TryAdd(NativeMethods.Entity.GetAttachmentMaterial(this.GetAttachmentHandle().Handle)); }
			set { NativeMethods.Entity.SetAttachmentMaterial(this.GetAttachmentHandle().Handle, value.HandleRef.Handle); }
		}

		internal HandleRef AttachmentHandleRef { get; set; }
	}
}
