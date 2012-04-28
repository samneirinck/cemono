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
			Assert.AreEqual(types.Count(), 2);

			var foo = types.ElementAt(0);
			Assert.IsNotNull(foo);
			Assert.AreEqual("Foo", foo.Name);

			var bar = types.ElementAt(1);
			Assert.IsNotNull(bar);
			Assert.AreEqual("Bar", bar.Name);
		}

		[Test]
		public void Compile_VB_FromSource()
		{
			var compilationParams = new CompilationParameters();

			compilationParams.Language = ScriptLanguage.VisualBasic;
			compilationParams.Sources = new string[] {
				@"Public Class Foo
				End Class
 
				Public Class Bar
				End Class" };

			ScriptCompiler scriptCompiler = new ScriptCompiler();
			var types = scriptCompiler.CompileScripts(ref compilationParams);

			Assert.IsNotNull(types);
			// When compiling VB we get thrown 5 other types, MyApplication, MyComputer, ThreadSafeObjectProvider`1, MyWebServices & MyProject. Look into this later.

			Assert.AreEqual(types.Count(), 7);

			var foo = types.ElementAt(3);
			Assert.IsNotNull(foo);
			Assert.AreEqual("Foo", foo.Name);

			var bar = types.ElementAt(4);
			Assert.IsNotNull(bar);
			Assert.AreEqual("Bar", bar.Name);
		}
	}
}
