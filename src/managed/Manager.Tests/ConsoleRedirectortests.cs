using NUnit.Framework;
using Rhino.Mocks;
using Cemono;

namespace Manager.Tests
{
    [TestFixture]
    public class ConsoleRedirectorTests
    {

        [Test]
        public void Log_SimpleMessage_Success()
        {
            string msg = "Hello log!";

            var mocks = new MockRepository();
            var logging = mocks.StrictMock<ILogging>();
            logging.Log(msg);

            mocks.ReplayAll();

            var redirector = new ConsoleRedirector();
            redirector.Logging = logging;
            redirector.WriteLine(msg);

            mocks.VerifyAll();
        }
    }
}
