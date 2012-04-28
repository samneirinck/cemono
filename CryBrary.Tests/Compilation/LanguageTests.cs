using System;
using System.Linq;
using CryEngine.Initialization;

using NUnit.Framework;

namespace CryBrary.Tests.Compilation
{
	[TestFixture]
	public class LanguageTests
	{
		[Test]
		public void Compile_CSharp_FromSource()
		{
			var compilationParams = new CompilationParameters();

			compilationParams.Language = ScriptLanguage.CSharp;
			compilationParams.Sources = new string[] {
				@"public class Foo
				{
					public int MyProperty { get; set; }
				}
 
				public static class Bar
				{
					private static Foo myFooValue = new Foo { MyProperty = 42 };
 
					public static Foo MyFoo { get { return myFooValue; } }
				}" };

			ScriptCompiler scriptCompiler = new ScriptCompiler();
			var types = scriptCompiler.CompileScripts(ref compilationParams);

			Assert.IsNotNull(types);
			Assert.IsNotEmpty(types);

			var foo = types.ElementAt(0);
			Assert.IsNotNull(foo);
			Assert.AreEqual(foo.Name, "Foo");

			var bar = types.ElementAt(1);
			Assert.IsNotNull(bar);
			Assert.AreEqual(bar.Name, "Bar");
		}
	}
}
