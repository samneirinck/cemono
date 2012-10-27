using System;

namespace CryEngine.Native
{
    [CLSCompliant(false)]
    public interface INativeViewMethods
    {
        uint GetView(uint linkedEntityId, bool forceCreate = false);
        void RemoveView(uint viewId);
        uint GetActiveView();
        void SetActiveView(uint viewId);

        Vec3 GetViewPosition(uint viewId);
        Quat GetViewRotation(uint viewId);
        float GetViewNearPlane(uint viewId);
        float GetViewFieldOfView(uint viewId);
        void SetViewPosition(uint viewId, Vec3 pos);
        void SetViewRotation(uint viewId, Quat rot);
        void SetViewNearPlane(uint viewId, float nearPlane);
        void SetViewFieldOfView(uint viewId, float fov);
    }
}