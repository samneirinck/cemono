using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.Native
{
    internal interface INativeMaterialMethods
    {
       IntPtr FindMaterial(string name);
       IntPtr CreateMaterial(string name);
       IntPtr LoadMaterial(string name, bool makeIfNotFound = true, bool nonRemovable = false);
       IntPtr GetSubMaterial(IntPtr materialPtr, int slot);
       int GetSubmaterialCount(IntPtr materialPtr);
       IntPtr GetMaterial(IntPtr entityPtr, int slot);
       void SetMaterial(IntPtr entityPtr, IntPtr materialPtr, int slot);
       IntPtr CloneMaterial(IntPtr materialPtr, int subMtl);
       string GetSurfaceTypeName(IntPtr ptr);
       bool SetGetMaterialParamFloat(IntPtr ptr, string paramName, ref float v, bool get);
       bool SetGetMaterialParamVec3(IntPtr ptr, string paramName, ref Vec3 v, bool get);
       int GetShaderParamCount(IntPtr ptr);
       string GetShaderParamName(IntPtr ptr, int index);
       void SetShaderParam(IntPtr ptr, string paramName, float newVal);
       void SetShaderParam(IntPtr ptr, string paramName, Color newVal);
       MaterialFlags GetFlags(IntPtr ptr);
       void SetFlags(IntPtr ptr, MaterialFlags flags);

    }
}
