using System;

namespace CryEngine.Testing
{
	/// <summary>
	/// Indicates that a class should be loaded by the unit tester.
	/// </summary>
	[AttributeUsage(AttributeTargets.Class)]
	public sealed class TestCollectionAttribute : Attribute
	{
	}

	/// <summary>
	/// Indicates that a method should be run as a test.
	/// Requires that the method belong to a class decorated with a TestCollectionAttribute.
	/// </summary>
	[AttributeUsage(AttributeTargets.Method)]
	public sealed class TestAttribute : Attribute
	{
	}
}
