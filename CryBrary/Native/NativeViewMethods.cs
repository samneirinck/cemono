using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal class NativeViewMethods : INativeViewMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static uint _GetView(uint linkedEntityId, bool forceCreate = false);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _RemoveView(uint viewId);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static uint _GetActiveView();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetActiveView(uint viewId);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static ViewParams _GetViewParams(uint viewId);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetViewParams(uint viewId, ViewParams cam);

        public uint GetView(uint linkedEntityId, bool forceCreate = false)
        {
            return _GetView(linkedEntityId, forceCreate);
        }

        public void RemoveView(uint viewId)
        {
            _RemoveView(viewId);
        }

        public uint GetActiveView()
        {
            return _GetActiveView();
        }

        public void SetActiveView(uint viewId)
        {
            _SetActiveView(viewId);
        }

        public ViewParams GetViewParams(uint viewId)
        {
            return _GetViewParams(viewId);
        }

        public void SetViewParams(uint viewId, ViewParams cam)
        {
            _SetViewParams(viewId, cam);
        }
    }
}
