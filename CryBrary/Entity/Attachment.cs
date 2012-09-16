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

		public Vec3 Position { get { return NativeMethods.Entity.GetAttachmentWorldPosition(this.GetAttachmentHandle().Handle); } set { NativeMethods.Entity.SetAttachmentWorldPosition(this.GetAttachmentHandle().Handle, value); } }
		public Vec3 LocalPosition { get { return NativeMethods.Entity.GetAttachmentLocalPosition(this.GetAttachmentHandle().Handle); } set { NativeMethods.Entity.SetAttachmentLocalPosition(this.GetAttachmentHandle().Handle, value); } }
		public Vec3 DefaultPosition { get { return NativeMethods.Entity.GetAttachmentDefaultWorldPosition(this.GetAttachmentHandle().Handle); } }
		public Vec3 DefaultLocalPosition { get { return NativeMethods.Entity.GetAttachmentDefaultLocalPosition(this.GetAttachmentHandle().Handle); } }

		public Quat Rotation { get { return NativeMethods.Entity.GetAttachmentWorldRotation(this.GetAttachmentHandle().Handle); } set { NativeMethods.Entity.SetAttachmentWorldRotation(this.GetAttachmentHandle().Handle, value); } }
		public Quat LocalRotation { get { return NativeMethods.Entity.GetAttachmentLocalRotation(this.GetAttachmentHandle().Handle); } set { NativeMethods.Entity.SetAttachmentLocalRotation(this.GetAttachmentHandle().Handle, value); } }
		public Quat DefaultRotation { get { return NativeMethods.Entity.GetAttachmentDefaultWorldRotation(this.GetAttachmentHandle().Handle); } }
		public Quat DefaultLocalRotation { get { return NativeMethods.Entity.GetAttachmentDefaultLocalRotation(this.GetAttachmentHandle().Handle); } }

		public Material Material
		{
			get { return Material.TryAdd(NativeMethods.Entity.GetAttachmentMaterial(this.GetAttachmentHandle().Handle)); }
			set { NativeMethods.Entity.SetAttachmentMaterial(this.GetAttachmentHandle().Handle, value.HandleRef.Handle); }
		}

		internal HandleRef AttachmentHandleRef { get; set; }
	}
}
