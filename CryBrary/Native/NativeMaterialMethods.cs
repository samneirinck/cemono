using System;
using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal class NativeMaterialMethods : INativeMaterialMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static IntPtr _FindMaterial(string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static IntPtr _CreateMaterial(string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static IntPtr _LoadMaterial(string name, bool makeIfNotFound = true, bool nonRemovable = false);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static IntPtr _GetSubMaterial(IntPtr materialPtr, int slot);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static IntPtr _GetMaterial(IntPtr entityPtr, int slot);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetMaterial(IntPtr entityPtr, IntPtr materialPtr, int slot);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static IntPtr _CloneMaterial(IntPtr materialPtr, int subMtl);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static string _GetSurfaceTypeName(IntPtr ptr);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static bool _SetGetMaterialParamFloat(IntPtr ptr, string paramName, ref float v, bool get);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static bool _SetGetMaterialParamVec3(IntPtr ptr, string paramName, ref Vec3 v, bool get);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static int _GetShaderParamCount(IntPtr ptr);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static string _GetShaderParamName(IntPtr ptr, int index);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetShaderParam(IntPtr ptr, string paramName, float newVal);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetShaderParam(IntPtr ptr, string paramName, Color newVal);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static MaterialFlags _GetFlags(IntPtr ptr);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetFlags(IntPtr ptr, MaterialFlags flags);

        public IntPtr FindMaterial(string name)
        {
            return _FindMaterial(name);
        }

        public IntPtr CreateMaterial(string name)
        {
            return _CreateMaterial(name);
        }

        public IntPtr LoadMaterial(string name, bool makeIfNotFound = true, bool nonRemovable = false)
        {
            return _LoadMaterial(name, makeIfNotFound, nonRemovable);
        }

        public IntPtr GetSubMaterial(IntPtr materialPtr, int slot)
        {
            return _GetSubMaterial(materialPtr, slot);
        }

        public IntPtr GetMaterial(IntPtr entityPtr, int slot)
        {
            return _GetMaterial(entityPtr, slot);
        }

        public void SetMaterial(IntPtr entityPtr, IntPtr materialPtr, int slot)
        {
            _SetMaterial(entityPtr, materialPtr, slot);
        }

        public IntPtr CloneMaterial(IntPtr materialPtr, int subMtl)
        {
            return _CloneMaterial(materialPtr, subMtl);
        }

        public string GetSurfaceTypeName(IntPtr ptr)
        {
            return _GetSurfaceTypeName(ptr);
        }

        public bool SetGetMaterialParamFloat(IntPtr ptr, string paramName, ref float v, bool get)
        {
            return _SetGetMaterialParamFloat(ptr, paramName, ref v, get);
        }

        public bool SetGetMaterialParamVec3(IntPtr ptr, string paramName, ref Vec3 v, bool get)
        {
            return _SetGetMaterialParamVec3(ptr, paramName, ref v, get);
        }

        public int GetShaderParamCount(IntPtr ptr)
        {
            return _GetShaderParamCount(ptr);
        }

        public string GetShaderParamName(IntPtr ptr, int index)
        {
            return _GetShaderParamName(ptr, index);
        }

        public void SetShaderParam(IntPtr ptr, string paramName, float newVal)
        {
            _SetShaderParam(ptr, paramName, newVal);
        }

        public void SetShaderParam(IntPtr ptr, string paramName, Color newVal)
        {
            _SetShaderParam(ptr,paramName,newVal);
        }

        public MaterialFlags GetFlags(IntPtr ptr)
        {
            return _GetFlags(ptr);
        }

        public void SetFlags(IntPtr ptr, MaterialFlags flags)
        {
            _SetFlags(ptr,flags);
        }

    }
}
