using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

using CryEngine.Native;

namespace CryEngine
{
    /// <summary>
    /// Represents a CryENGINE material applicable to any ingame object or entity.
    /// </summary>
    public class Material
    {
        static List<Material> materials = new List<Material>();

        internal Material(IntPtr ptr)
        {
            HandleRef = new HandleRef(this, ptr);
        }

        #region Properties
        /// <summary>
        /// Gets or sets the alphatest.
        /// </summary>
        public float AlphaTest { get { return GetParam("alpha"); } set { SetParam("alpha", value); } }

        /// <summary>
        /// Gets or sets the opacity.
        /// </summary>
        public float Opacity { get { return GetParam("opacity"); } set { SetParam("opacity", value); } }

        /// <summary>
        /// Gets or sets the glow.
        /// </summary>
        public float Glow { get { return GetParam("glow"); } set { SetParam("glow", value); } }

        /// <summary>
        /// Gets or sets the shininess.
        /// </summary>
        public float Shininess { get { return GetParam("shininess"); } set { SetParam("shininess", value); } }

        /// <summary>
        /// Gets or sets the diffuse color.
        /// </summary>
        public Color DiffuseColor { get { return GetParamColor("diffuse"); } set { SetParam("diffuse", value); } }

        /// <summary>
        /// Gets or sets the emissive color.
        /// </summary>
        public Color EmissiveColor { get { return GetParamColor("emissive"); } set { SetParam("emissive", value); } }

        /// <summary>
        /// Gets or sets the specular color.
        /// </summary>
        public Color SpecularColor { get { return GetParamColor("specular"); } set { SetParam("specular", value); } }

        /// <summary>
        /// Gets the surface type assigned to this material.
        /// </summary>
        public string SurfaceType { get { return NativeMethods.Material.GetSurfaceTypeName(HandleRef.Handle); } }

        /// <summary>
        /// Gets the amount of shader parameters in this material.
        /// See <see cref="GetShaderParamName(int)"/>
        /// </summary>
        public int ShaderParamCount { get { return NativeMethods.Material.GetShaderParamCount(HandleRef.Handle); } }

        /// <summary>
        /// Gets the amount of submaterials tied to this material.
        /// </summary>
        public int SubmaterialCount { get { return NativeMethods.Material.GetSubmaterialCount(HandleRef.Handle); } }

        /// <summary>
        /// Gets or sets the native IMaterial pointer.
        /// </summary>
        public HandleRef HandleRef { get; set; }
        #endregion

        #region Statics
        public static Material Find(string name)
        {
            var ptr = NativeMethods.Material.FindMaterial(name);

            return TryAdd(ptr);
        }

        public static Material Create(string name, bool makeIfNotFound = true, bool nonRemovable = false)
        {
            var ptr = NativeMethods.Material.CreateMaterial(name);

            return TryAdd(ptr);
        }

        public static Material Load(string name, bool makeIfNotFound = true, bool nonRemovable = false)
        {
            var ptr = NativeMethods.Material.LoadMaterial(name, makeIfNotFound, nonRemovable);

            return TryAdd(ptr);
        }

        public static Material Get(EntityBase entity, int slot = 0)
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (entity == null)
                throw new ArgumentNullException("entity");
#endif

            var ptr = NativeMethods.Material.GetMaterial(entity.GetEntityHandle().Handle, slot);
            return TryAdd(ptr);
        }

        public static void Set(EntityBase entity, Material mat, int slot = 0)
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (entity == null)
                throw new ArgumentNullException("entity");
            if (mat == null)
                throw new ArgumentNullException("mat");
#endif

            NativeMethods.Material.SetMaterial(entity.GetEntityHandle().Handle, mat.HandleRef.Handle, slot);
        }
        #endregion

        /// <summary>
        /// Gets a submaterial by slot.
        /// </summary>
        /// <param name="slot"></param>
        /// <returns>The submaterial, or null if failed.</returns>
        public Material GetSubmaterial(int slot)
        {
            var ptr = NativeMethods.Material.GetSubMaterial(HandleRef.Handle, slot);

            return TryAdd(ptr);
        }

        /// <summary>
        /// Clones a material
        /// </summary>
        /// <param name="subMaterial">If negative, all sub materials are cloned, otherwise only the specified slot is</param>
        /// <returns>The new clone.</returns>
        public Material Clone(int subMaterial = -1)
        {
            var ptr = NativeMethods.Material.CloneMaterial(HandleRef.Handle, subMaterial);

            return TryAdd(ptr);
        }

        /// <summary>
        /// Sets a material parameter value by name.
        /// </summary>
        /// <param name="paramName"></param>
        /// <param name="value"></param>
        /// <returns>true if successful, otherwise false.</returns>
        public bool SetParam(string paramName, float value)
        {
            return NativeMethods.Material.SetGetMaterialParamFloat(HandleRef.Handle, paramName, ref value, false);
        }

        /// <summary>
        /// Gets a material's parameter value by name.
        /// </summary>
        /// <param name="paramName"></param>
        /// <returns>The param value</returns>
        public float GetParam(string paramName)
        {
            float value;
            TryGetParam(paramName, out value);

            return value;
        }

        /// <summary>
        /// Attempts to get parameter value by name.
        /// </summary>
        /// <param name="paramName"></param>
        /// <param name="value"></param>
        /// <returns>true if successful, otherwise false.</returns>
        public bool TryGetParam(string paramName, out float value)
        {
            value = 0;

            return NativeMethods.Material.SetGetMaterialParamFloat(HandleRef.Handle, paramName, ref value, true);
        }

        /// <summary>
        /// Sets a material parameter value by name.
        /// </summary>
        /// <param name="paramName"></param>
        /// <param name="value"></param>
        /// <returns>true if successful, otherwise false.</returns>
        public bool SetParam(string paramName, Color value)
        {
            Vec3 vecValue = new Vec3(value.R, value.G, value.B);
            var result = NativeMethods.Material.SetGetMaterialParamVec3(HandleRef.Handle, paramName, ref vecValue, false);

            Opacity = value.A;

            return result;
        }

        /// <summary>
        /// Gets a material's parameter value by name.
        /// </summary>
        /// <param name="paramName"></param>
        /// <returns>The color value</returns>
        public Color GetParamColor(string paramName)
        {
            Color value;
            TryGetParam(paramName, out value);

            return value;
        }

        /// <summary>
        /// Attempts to get parameter value by name.
        /// </summary>
        /// <param name="paramName"></param>
        /// <param name="value"></param>
        /// <returns>true if successful, otherwise false.</returns>
        public bool TryGetParam(string paramName, out Color value)
        {
            Vec3 vecVal = Vec3.Zero;
            bool result = NativeMethods.Material.SetGetMaterialParamVec3(HandleRef.Handle, paramName, ref vecVal, true);

            value = new Color();
            value.R = vecVal.X;
            value.G = vecVal.Y;
            value.B = vecVal.Z;
            value.A = Opacity;

            return result;
        }

        /// <summary>
        /// Sets a shader parameter value by name.
        /// </summary>
        /// <param name="paramName"></param>
        /// <param name="newVal"></param>
        public void SetShaderParam(string paramName, float newVal)
        {
            NativeMethods.Material.SetShaderParam(HandleRef.Handle, paramName, newVal);
        }

        /// <summary>
        /// Sets a shader parameter value by name.
        /// </summary>
        /// <param name="param"></param>
        /// <param name="value"></param>
        public void SetShaderParam(ShaderFloatParameter param, float value)
        {
            SetShaderParam(param.GetEngineName(), value);
        }

        /// <summary>
        /// Sets a shader parameter value by name.
        /// </summary>
        /// <param name="paramName"></param>
        /// <param name="newVal"></param>
        public void SetShaderParam(string paramName, Color newVal)
        {
            NativeMethods.Material.SetShaderParam(HandleRef.Handle, paramName, newVal);
        }

        /// <summary>
        /// Sets a shader parameter value by name.
        /// </summary>
        /// <param name="param"></param>
        /// <param name="value"></param>
        public void SetShaderParam(ShaderColorParameter param, Color value)
        {
            SetShaderParam(param.GetEngineName(), value);
        }

        /// <summary>
        /// Sets a shader parameter value by name.
        /// </summary>
        /// <param name="param"></param>
        /// <param name="value"></param>
        public void SetShaderParam(ShaderColorParameter param, Vec3 value)
        {
            SetShaderParam(param.GetEngineName(), new Color(value.X, value.Y, value.Z));
        }

        /// <summary>
        /// Gets a shader parameter name by index.
        /// See <see cref="ShaderParamCount"/>
        /// </summary>
        /// <param name="index"></param>
        /// <returns>The shader parameter name.</returns>
        public string GetShaderParamName(int index)
        {
            return NativeMethods.Material.GetShaderParamName(HandleRef.Handle, index);
        }

        private static Material TryAdd(IntPtr ptr)
        {
            if (ptr == IntPtr.Zero)
                return null;

            var mat = materials.FirstOrDefault(x => x.HandleRef.Handle == ptr);
            if (mat != default(Material))
                return mat;

            mat = new Material(ptr);
            materials.Add(mat);

            return mat;
        }
    }
}