using System.Runtime.InteropServices;

namespace CryEngine.Native
{
	public static class NativeAttachmentExtensions
	{
		public static HandleRef GetAttachmentHandle(this Attachment attachment)
		{
			return attachment.AttachmentHandleRef;
		}

		public static void SetAttachmentHandle(this Attachment attachment, HandleRef newHandle)
		{
			attachment.AttachmentHandleRef = newHandle;
		}
	}
}
