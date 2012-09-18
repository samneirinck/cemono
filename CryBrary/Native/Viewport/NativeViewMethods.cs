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
        extern internal static Vec3 _GetViewPosition(uint viewId);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static Quat _GetViewRotation(uint viewId);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static float _GetViewNearPlane(uint viewId);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static float _GetViewFieldOfView(uint viewId);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetViewPosition(uint viewId, Vec3 pos);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetViewRotation(uint viewId, Quat rot);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetViewNearPlane(uint viewId, float nearPlane);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetViewFieldOfView(uint viewId, float fov);

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

		public Vec3 GetViewPosition(uint viewId)
		{
			return _GetViewPosition(viewId);
		}

		public Quat GetViewRotation(uint viewId)
		{
			return _GetViewRotation(viewId);
		}

		public float GetViewNearPlane(uint viewId)
		{
			return _GetViewNearPlane(viewId);
		}

		public float GetViewFieldOfView(uint viewId)
		{
			return _GetViewFieldOfView(viewId);
		}

		public void SetViewPosition(uint viewId, Vec3 pos)
		{
			_SetViewPosition(viewId, pos);
		}

		public void SetViewRotation(uint viewId, Quat rot)
		{
			_SetViewRotation(viewId, rot);
		}

		public void SetViewNearPlane(uint viewId, float nearPlane)
		{
			_SetViewNearPlane(viewId, nearPlane);
		}

		public void SetViewFieldOfView(uint viewId, float fov)
		{
			_SetViewFieldOfView(viewId, fov);
		}
    }
}
