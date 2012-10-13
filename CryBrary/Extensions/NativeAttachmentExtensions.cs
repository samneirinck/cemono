using System.Runtime.InteropServices;

namespace CryEngine.Native
{
	public static class NativeAttachmentExtensions
	{
		public static HandleRef GetAttachmentHandle(this Attachment attachment)
		{
			if (attachment.IsDestroyed)
				throw new ScriptInstanceDestroyedException("Attempted to access native attachment handle on a destroyed script");
			return attachment.AttachmentHandleRef;
		}

		public static void SetAttachmentHandle(this Attachment attachment, HandleRef newHandle)
		{
			attachment.AttachmentHandleRef = newHandle;
		}

		public static HandleRef GetEntityAttachmentHandle(this Attachment attachment)
		{
			return attachment.EntityAttachmentHandleRef;
		}

		public static void SetEntityAttachmentHandle(this Attachment attachment, HandleRef newHandle)
		{
			attachment.EntityAttachmentHandleRef = newHandle;
		}
	}
}
