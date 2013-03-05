using System;
using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    public static class NativeGameObjectMethods
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static IntPtr GetGameObject(EntityId id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void EnablePostUpdates(IntPtr gameObjectPtr, IntPtr extensionPtr, bool enable);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void EnablePrePhysicsUpdates(IntPtr gameObjectPtr, PrePhysicsUpdateMode mode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static IntPtr QueryExtension(IntPtr gameObjectPtr, string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static IntPtr AcquireExtension(IntPtr gameObjectPtr, string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void ReleaseExtension(IntPtr gameObjectPtr, string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static bool ActivateExtension(IntPtr gameObjectPtr, string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void DeactivateExtension(IntPtr gameObjectPtr, string name);
    }
}