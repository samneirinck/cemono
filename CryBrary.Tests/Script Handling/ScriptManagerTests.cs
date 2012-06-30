using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CryEngine.Initialization;
using NUnit.Framework;

namespace CryBrary.Tests.Script_Handling
{
    [TestFixture]
    public class ScriptManagerTests : CryBraryTests
    {
        [Test]
        public void ConstructAndInitialize_ScriptManager_WithoutExceptions()
        {
            ScriptManager manager = new ScriptManager();

            manager.PostInit();
        }
    }
}
