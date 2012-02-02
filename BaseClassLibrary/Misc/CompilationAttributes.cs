using System;

namespace CryEngine
{
	/// <summary>
	/// If this attribute is attached to a class, it will be excluded from compilation.
	/// </summary>
    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Interface, Inherited = false)]
    public class ExcludeFromCompilationAttribute : Attribute { }

    /// <summary>
    /// Using this in a class will make it be used as the default game mode.
    /// </summary>
    [AttributeUsage(AttributeTargets.Class, Inherited = false)]
    public class DefaultGamemodeAttribute : Attribute { }
}
