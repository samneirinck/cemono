using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using CryEngine.Utilities;
using NUnit.Framework;

namespace CryBrary.Tests.Utilities
{
    // We want to test protected methods, the public interface relies on file IO
    // This is not a good practice, but we don't want to pollute the actual class with IO abstractions
    [TestFixture]
    public class AssemblyReferenceHandlerTests : AssemblyReferenceHandler
    {

        [Test]
        public void GetNamespacesFromStream_EmptyStream_EmptyResult()
        {
            // Arrange
            using (var stream = new MemoryStream())
            {

                // Act
                var namespaces = this.GetNamespacesFromStream(stream);

                // Assert
                Assert.IsNotNull(namespaces, "Namespaces IEnumerable should be empty instead of null");
                Assert.IsTrue(namespaces.Count() == 0);
            }
        }

        [Test]
        public void GetNamespacesFromStream_ScriptWithoutUsingStatements_EmptyResult()
        {
            // Arrange
            using (var stream = GetScriptWithoutUsingStatements())
            {

                // Act
                var namespaces = this.GetNamespacesFromStream(stream);

                // Assert
                Assert.IsNotNull(namespaces, "Namespaces IEnumerable should be empty instead of null");
                Assert.IsTrue(namespaces.Count() == 0);
            }
        }

        [Test]
        public void GetNamespacesFromStream_ScriptWithMultipleUsingStatements_MultipleNamespaces()
        {
            // Arrange
            using (var stream = GetScriptWith5UsingStatements())
            {

                // Act
                var namespaces = this.GetNamespacesFromStream(stream);

                // Assert
                Assert.AreEqual(5, namespaces.Count());
                Assert.AreEqual("System", namespaces.First());
            }
        }

        [Test]
        public void GetNamespacesFromStream_ScriptWithUsingStatementsOnSingleLine_CorrectAmountOfNamespaces()
        {
            // Arrange
            using (var stream = GetScriptWithUsingStatementsOnSingleLine())
            {

                // Act
                var namespaces = this.GetNamespacesFromStream(stream);

                // Assert
                Assert.AreEqual(5, namespaces.Count());
                Assert.AreEqual("System", namespaces.First());
            }
        }


        private Stream GetScriptWithoutUsingStatements()
        {
            return new MemoryStream(Encoding.UTF8.GetBytes(@"
class MyEntity {
    string Name;
}"));
        }

        private Stream GetScriptWith5UsingStatements()
        {
            return new MemoryStream(Encoding.UTF8.GetBytes(@"
using System; 
using CryMono;
using System.Linq;
using MyClassLibrary;
using System.Diagnostics;

class MyClass {
    // TODO: Add implementation
    MyClass() { Console.WriteLine(""Constructed!""); }

}
"));
        }

        private Stream GetScriptWithUsingStatementsOnSingleLine()
        {
            return new MemoryStream(Encoding.UTF8.GetBytes(@"
using System;using CryMono;using System.Linq;
using MyClassLibrary;
using System.Diagnostics;

class MyClass {
    // TODO: Add implementation
    MyClass() { Console.WriteLine(""Constructed!""); }

}
"));
        }

    }
}
