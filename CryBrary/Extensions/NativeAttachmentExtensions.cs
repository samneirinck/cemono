using System;

namespace CryEngine.Native
{
    public static class NativeAttachmentExtensions
    {
        public static IntPtr GetAttachmentHandle(this Attachment attachment)
        {
            if (attachment.IsDestroyed)
                throw new ScriptInstanceDestroyedException("Attempted to access native attachment handle on a destroyed script");

            return attachment.AttachmentHandle;
        }

        public static void SetAttachmentHandle(this Attachment attachment, IntPtr newHandle)
        {
            attachment.AttachmentHandle = newHandle;
        }

        public static IntPtr GetEntityAttachmentHandle(this Attachment attachment)
        {
            return attachment.EntityAttachmentHandle;
        }

        public static void SetEntityAttachmentHandle(this Attachment attachment, IntPtr newHandle)
        {
            attachment.EntityAttachmentHandle = newHandle;
        }
    }
}
