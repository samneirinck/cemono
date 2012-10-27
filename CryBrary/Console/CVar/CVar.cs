using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using CryEngine.Native;

namespace CryEngine
{
    public abstract class CVar
    {
        private static readonly List<CVar> CVars = new List<CVar>();

        public string Name { get; protected set; }

        public string Help { get; protected set; }

        public CVarFlags Flags { get; protected set; }

        public virtual string String { get; set; }

        public virtual float FVal { get; set; }

        public virtual int IVal { get; set; }

        #region Statics
        /// <summary>
        /// Registers a new console variable with the specified default value.
        /// </summary>
        /// <param name="name">console variable name</param>
        /// <param name="value">default value of the console variable</param>
        /// <param name="help">help text that is shown when you use <paramref name="name"/> ? in the console</param>
        /// <param name="flags"></param>
        /// <returns>Newly null if failed, new CVar instance if successful</returns>
        public static CVar Register(string name, int value, string help = "", CVarFlags flags = CVarFlags.None)
        {
            return RegisterInternal(name, value, help, flags);
        }

        /// <summary>
        /// Registers a new console variable with the specified default value.
        /// </summary>
        /// <param name="name">console variable name</param>
        /// <param name="value">default value of the console variable</param>
        /// <param name="help">help text that is shown when you use <paramref name="name"/> ? in the console</param>
        /// <param name="flags"></param>
        /// <returns>Newly null if failed, new CVar instance if successful</returns>
        public static CVar Register(string name, float value, string help = "", CVarFlags flags = CVarFlags.None)
        {
            return RegisterInternal(name, value, help, flags);
        }

        /// <summary>
        /// Registers a new console variable with the specified default value.
        /// </summary>
        /// <param name="name">console variable name</param>
        /// <param name="value">default value of the console variable</param>
        /// <param name="help">help text that is shown when you use <paramref name="name"/> ? in the console</param>
        /// <param name="flags"></param>
        /// <returns>Newly null if failed, new CVar instance if successful</returns>
        public static CVar Register(string name, string value, string help = "", CVarFlags flags = CVarFlags.None)
        {
            return RegisterInternal(name, value, help, flags);
        }

        /// <summary>
        /// Registers a new console variable that will update the user defined integer.
        /// </summary>
        /// <param name="name">console variable name</param>
        /// <param name="value">reference to the memory that will be updated</param>
        /// <param name="help">help text that is shown when you use <paramref name="name"/> ? in the console</param>
        /// <param name="flags"></param>
        /// <returns>Newly null if failed, new CVar instance if successful</returns>
        public static CVar RegisterInt(string name, ref int value, string help = "", CVarFlags flags = CVarFlags.None)
        {
            NativeMethods.CVar.RegisterCVarInt(name, ref value, value, flags, help);

            CVars.Add(new ExternalCVar(name));

            return CVars.Last();
        }

        /// <summary>
        /// Registers a new console variable that will update the user defined float.
        /// </summary>
        /// <param name="name">console variable name</param>
        /// <param name="value">reference to the memory that will be updated</param>
        /// <param name="help">help text that is shown when you use <paramref name="name"/> ? in the console</param>
        /// <param name="flags"></param>
        /// <returns>Newly null if failed, new CVar instance if successful</returns>
        public static CVar RegisterFloat(string name, ref float value, string help = "", CVarFlags flags = CVarFlags.None)
        {
            NativeMethods.CVar.RegisterCVarFloat(name, ref value, value, flags, help);

            CVars.Add(new ExternalCVar(name));

            return CVars.Last();
        }

        /// <summary>
        /// Retrieve a console variable by name - not case sensitive
        /// </summary>
        /// <param name="name">The name of the CVar to retrieve</param>
        /// <returns>null if not found, CVar instance if successful</returns>
        public static CVar Get(string name)
        {
            CVar cvar = CVars.FirstOrDefault(var => var.Name.Equals(name));
            if (cvar != default(CVar))
                return cvar;

            if (NativeMethods.CVar.HasCVar(name))
            {
                CVars.Add(new ExternalCVar(name));

                return CVars.Last();
            }

            return null;
        }

        /// <summary>
        /// Firstly checks whether a specified CVar is valid, then if so, modifies the cvar reference
        /// </summary>
        /// <param name="name">The name of the CVar to retrieve</param>
        /// <param name="cvar">The CVar object to modify (usually blank)</param>
        /// <returns>True if the CVar exists, otherwise false</returns>
        public static bool TryGet(string name, out CVar cvar)
        {
            if ((cvar = Get(name)) != null)
                return true;

            return false;
        }

        internal static CVar Register(CVarAttribute attribute, MemberInfo memberInfo, ref int value)
        {
            if (attribute.Name == null)
                attribute.Name = memberInfo.Name;

            NativeMethods.CVar.RegisterCVarInt(attribute.Name, ref value, System.Convert.ToInt32(attribute.DefaultValue), attribute.Flags, attribute.Help);

            if (memberInfo.MemberType == MemberTypes.Field)
                CVars.Add(new StaticCVarField(attribute, memberInfo as FieldInfo));
            else
                CVars.Add(new StaticCVarProperty(attribute, memberInfo as PropertyInfo));

            return CVars.Last();
        }

        internal static CVar Register(CVarAttribute attribute, MemberInfo memberInfo, ref float value)
        {
            if (attribute.Name == null)
                attribute.Name = memberInfo.Name;

            NativeMethods.CVar.RegisterCVarFloat(attribute.Name, ref value, System.Convert.ToSingle(attribute.DefaultValue), attribute.Flags, attribute.Help);

            if (memberInfo.MemberType == MemberTypes.Field)
                CVars.Add(new StaticCVarField(attribute, memberInfo as FieldInfo));
            else
                CVars.Add(new StaticCVarProperty(attribute, memberInfo as PropertyInfo));

            return CVars.Last();
        }

        internal static CVar Register(CVarAttribute attribute, MemberInfo memberInfo, string value)
        {
            if (attribute.Name == null)
                attribute.Name = memberInfo.Name;

            NativeMethods.CVar.RegisterCVarString(attribute.Name, value, (string)attribute.DefaultValue ?? string.Empty, attribute.Flags, attribute.Help);

            if (memberInfo.MemberType == MemberTypes.Field)
                CVars.Add(new StaticCVarField(attribute, memberInfo as FieldInfo));
            else
                CVars.Add(new StaticCVarProperty(attribute, memberInfo as PropertyInfo));

            return CVars.Last();
        }

        internal static CVar RegisterInternal(string name, object value, string help, CVarFlags flags)
        {
            CVars.Add(new DynamicCVar(name, value, flags, help));

            return CVars.Last();
        }
        #endregion
    }
}