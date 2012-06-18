using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;

namespace CryEngine
{
	public abstract class CVar
	{
		public string Name { get; protected set; }
		public string Help { get; protected set; }
		public CVarFlags Flags { get; protected set; }

		public virtual string String { get; set; }
		public virtual float FVal { get; set; }
		public virtual int IVal { get; set; }

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void _HandleException(Exception ex);

		#region Externals

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void _RegisterCommand(string name, string description, CVarFlags flags);

		// CVars
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void _RegisterCVarFloat(string name, ref float val, float defaultVal, CVarFlags flags,
		                                               string description);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void _RegisterCVarInt(string name, ref int val, int defaultVal, CVarFlags flags,
		                                             string description);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void _RegisterCVarString(string name, [MarshalAs(UnmanagedType.LPStr)] string val,
		                                                string defaultVal, CVarFlags flags, string description);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float _GetCVarFloat(string name);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern int _GetCVarInt(string name);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern string _GetCVarString(string name);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void _SetCVarFloat(string name, float value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void _SetCVarInt(string name, int value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void _SetCVarString(string name, string value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool _HasCVar(string name);

		#endregion

		#region Statics

		private static readonly List<CVar> CVars = new List<CVar>();

		/// <summary>
		/// Registers a CVar.
		/// </summary>
		/// <param name="name"></param>
		/// <param name="value"></param>
		/// <param name="help"></param>
		/// <param name="flags"></param>
		/// <returns></returns>
		public static CVar Register(string name, int value, string help = "", CVarFlags flags = CVarFlags.None)
		{
			return RegisterInternal(name, value, help, flags);
		}

		/// <summary>
		/// Registers a CVar.
		/// </summary>
		/// <param name="name"></param>
		/// <param name="value"></param>
		/// <param name="help"></param>
		/// <param name="flags"></param>
		/// <returns></returns>
		public static CVar Register(string name, float value, string help = "", CVarFlags flags = CVarFlags.None)
		{
			return RegisterInternal(name, value, help, flags);
		}

		/// <summary>
		/// Registers a CVar.
		/// </summary>
		/// <param name="name"></param>
		/// <param name="value"></param>
		/// <param name="help"></param>
		/// <param name="flags"></param>
		/// <returns></returns>
		public static CVar Register(string name, string value, string help = "", CVarFlags flags = CVarFlags.None)
		{
			return RegisterInternal(name, value, help, flags);
		}

		private static CVar RegisterInternal(string name, object value, string help, CVarFlags flags)
		{
			CVars.Add(new DynamicCVar(name, value, flags, help));

			return CVars.Last();
		}

		public static CVar RegisterInt(string name, ref int value, string help = "", CVarFlags flags = CVarFlags.None)
		{
			_RegisterCVarInt(name, ref value, value, flags, help);

			CVars.Add(new ExternalCVar(name));

			return CVars.Last();
		}

		public static CVar RegisterFloat(string name, ref float value, string help = "", CVarFlags flags = CVarFlags.None)
		{
			_RegisterCVarFloat(name, ref value, value, flags, help);

			CVars.Add(new ExternalCVar(name));

			return CVars.Last();
		}

		internal static CVar Register(CVarAttribute attribute, MemberInfo memberInfo, ref int value)
		{
			if (attribute.Name == null)
				attribute.Name = memberInfo.Name;

			_RegisterCVarInt(attribute.Name, ref value, System.Convert.ToInt32(attribute.DefaultValue), attribute.Flags,
			                 attribute.Help);

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

			_RegisterCVarFloat(attribute.Name, ref value, System.Convert.ToSingle(attribute.DefaultValue), attribute.Flags,
			                   attribute.Help);

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

			_RegisterCVarString(attribute.Name, value, (string) attribute.DefaultValue ?? "", attribute.Flags, attribute.Help);

			if (memberInfo.MemberType == MemberTypes.Field)
				CVars.Add(new StaticCVarField(attribute, memberInfo as FieldInfo));
			else
				CVars.Add(new StaticCVarProperty(attribute, memberInfo as PropertyInfo));

			return CVars.Last();
		}

		/// <summary>
		/// Gets a CVar by name
		/// </summary>
		/// <param name="name">The name of the CVar to retrieve</param>
		/// <returns></returns>
		public static CVar Get(string name)
		{
			CVar cvar = CVars.FirstOrDefault(var => var.Name.Equals(name));
			if (cvar != default(CVar))
				return cvar;

			if (_HasCVar(name))
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

		#endregion
	}

	/// <summary>
	/// CVar created using CVarAttribute, targeting a field.
	/// </summary>
	internal class StaticCVarField : CVar
	{
		private readonly FieldInfo field;

		public StaticCVarField(CVarAttribute attribute, FieldInfo fieldInfo)
		{
			Name = attribute.Name;
			Flags = attribute.Flags;
			Help = attribute.Help;

			fieldInfo.SetValue(null, attribute.DefaultValue);

			field = fieldInfo;
		}

		public override string String
		{
			get { return field.GetValue(null) as string; }
			set { field.SetValue(null, value); }
		}

		public override float FVal
		{
			get { return (float) field.GetValue(null); }
			set { field.SetValue(null, value); }
		}

		public override int IVal
		{
			get { return (int) field.GetValue(null); }
			set { field.SetValue(null, value); }
		}
	}

	/// <summary>
	/// CVar created using CVarAttribute, targeting a property.
	/// </summary>
	internal class StaticCVarProperty : CVar
	{
		private readonly PropertyInfo property;

		public StaticCVarProperty(CVarAttribute attribute, PropertyInfo propertyInfo)
		{
			Name = attribute.Name;
			Flags = attribute.Flags;
			Help = attribute.Help;

			property.SetValue(null, attribute.DefaultValue, null);

			property = propertyInfo;
		}

		public override string String
		{
			get { return property.GetValue(null, null) as string; }
			set { property.SetValue(null, value, null); }
		}

		public override float FVal
		{
			get { return (float) property.GetValue(null, null); }
			set { property.SetValue(null, value, null); }
		}

		public override int IVal
		{
			get { return (int) property.GetValue(null, null); }
			set { property.SetValue(null, value, null); }
		}
	}

	/// <summary>
	/// CVar created at run-time
	/// </summary>
	internal class DynamicCVar : CVar
	{
		/// <summary>
		/// Used by CryConsole.RegisterCVar to construct the CVar.
		/// </summary>
		/// <param name="name"></param>
		/// <param name="value"></param>
		/// <param name="flags"></param>
		/// <param name="help"></param>
		internal DynamicCVar(string name, object value, CVarFlags flags, string help)
		{
			Flags = flags;
			Help = help;
			Name = name;

			if (value is int)
			{
				IntValue = (int)value;

				_RegisterCVarInt(Name, ref IntValue, IntValue, Flags, Help);
			}
			else if (value is float || value is double)
			{
				FloatValue = (float) value;

				_RegisterCVarFloat(Name, ref FloatValue, FloatValue, Flags, Help);
			}
			else if (value is string)
			{
				StringValue = value as string;

				// String CVars are not supported yet.
				_RegisterCVarString(Name, StringValue, StringValue, Flags, Help);
			}
			else
				throw new CVarException(string.Format("Invalid data type ({0}) used in CVar {1}.", value.GetType(), Name));
		}

		#region Properties

		private float FloatValue;
		private int IntValue;
		private string StringValue;

		public override string String
		{
			get { return StringValue; }
			set { StringValue = value; }
		}

		public override float FVal
		{
			get { return FloatValue; }
			set { FloatValue = value; }
		}

		public override int IVal
		{
			get { return IntValue; }
			set { IntValue = value; }
		}

		#endregion

		#region Overrides

		public override bool Equals(object obj)
		{
			if (obj == null)
				return false;

			var cvar = obj as CVar;
			if (cvar == null)
				return false;

			return Name.Equals(cvar.Name);
		}

		public override int GetHashCode()
		{
			unchecked // Overflow is fine, just wrap
			{
				int hash = 17;

				hash = hash * 29 + FloatValue.GetHashCode();
				hash = hash * 29 + IntValue.GetHashCode();
				hash = hash * 29 + StringValue.GetHashCode();
				hash = hash * 29 + Flags.GetHashCode();
				hash = hash * 29 + Name.GetHashCode();

				return hash;
			}
		}
		#endregion
	}

	/// <summary>
	/// CVar created outside CryMono
	/// </summary>
	internal class ExternalCVar : CVar
	{
		internal ExternalCVar(string name)
		{
			Name = name;
		}

		public override string String
		{
			get { return _GetCVarString(Name); }
			set { _SetCVarString(Name, value); }
		}

		public override float FVal
		{
			get { return _GetCVarFloat(Name); }
			set { _SetCVarFloat(Name, value); }
		}

		public override int IVal
		{
			get { return _GetCVarInt(Name); }
			set { _SetCVarInt(Name, value); }
		}
	}

	//[Obsolete("CVars cannot be created using attributes at the moment.")]
	[AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
	public sealed class CVarAttribute : Attribute
	{
		public object DefaultValue;
		public CVarFlags Flags;
		public string Help;
		public string Name;
	}

	public enum CVarType
	{
		Int,
		Float,
		String
	}

	[Flags]
	public enum CVarFlags
	{
		/// <summary>
		/// just to have one recognizable spot where the flags are located in the Register call
		/// </summary>
		None = 0x00000000,

		/// <summary>
		/// stays in the default state when cheats are disabled
		/// </summary>
		Cheat = 0x00000002,
		DumpToDisk = 0x00000100,

		/// <summary>
		/// can not be changed by the user
		/// </summary>
		ReadOnly = 0x00000800,
		RequireLevelReload = 0x00001000,
		RequireAppRestart = 0x00002000,

		/// <summary>
		///  shows warning that this var was not used in config file
		/// </summary>
		WarningNotUsed = 0x00004000,

		/// <summary>
		/// otherwise the const char * to the name will be stored without copying the memory
		/// </summary>
		CopyName = 0x00008000,

		/// <summary>
		/// Set when variable value modified.
		/// </summary>
		Modified = 0x00010000,

		/// <summary>
		/// Set when variable was present in config file.
		/// </summary>
		WasInConfig = 0x00020000,

		/// <summary>
		/// Allow bitfield setting syntax.
		/// </summary>
		Bitfield = 0x00040000,

		/// <summary>
		/// is visible and usable in restricted (normal user) console mode
		/// </summary>
		RestrictedMode = 0x00080000,

		/// <summary>
		/// Invisible to the user in console
		/// </summary>
		Invisible = 0x00100000,

		/// <summary>
		/// Always accept variable value and call on change callback even if variable value didnt change
		/// </summary>
		AlwaysOnChange = 0x00200000,

		/// <summary>
		/// Blocks the execution of console commands for one frame
		/// </summary>
		BlockFrame = 0x00400000,

		/// <summary>
		/// Set if it is a const cvar not to be set inside cfg-files
		/// </summary>
		ConstCVar = 0x00800000,

		/// <summary>
		/// This variable is critical to check in every hash, since it's extremely vulnerable
		/// </summary>
		CheatAlwaysCheck = 0x01000000,

		/// <summary>
		/// This variable is set as VF_CHEAT but doesn't have to be checked/hashed since it's harmless to workaround
		/// </summary>
		CheatNoCheck = 0x02000000,

		// These flags should never be set during cvar creation, and probably never set manually.
		InternalFlagsStart = 0x00000080,

		/// <summary>
		/// can be changed on client and when connecting the var not sent to the client (is set for all vars in Game/scripts/Network/cvars.txt)
		/// </summary>
		NotNetSyncedInternal = InternalFlagsStart,
		InternalFlagsEnd = NotNetSyncedInternal
	}

	/// <summary>
	/// This exception is called when invalid CVar operations are performed.
	/// </summary>
	[Serializable]
	public class CVarException : Exception
	{
		public CVarException()
		{
		}

		public CVarException(string message)
			: base(message)
		{
		}

		public CVarException(string message, Exception inner)
			: base(message, inner)
		{
		}

		protected CVarException(
			SerializationInfo info,
			StreamingContext context)
			: base(info, context)
		{
		}
	}
}