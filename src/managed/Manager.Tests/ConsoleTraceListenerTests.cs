using NUnit.Framework;
using Rhino.Mocks;
using Cemono;
namespace Manager.Tests
{
    [TestFixture]
    public class ConsoleTraceListenerTests
    {

        [Test]
        public void Log_SimpleMessage_Success()
        {
            string warningMessage = "Warning message!";
            string informationMessage = "Information message";
            string errorMessage = "Error message";

            var mocks = new MockRepository();
            var logging = mocks.CreateMock<ILogging>();
            logging.Log(msg);

            mocks.ReplayAll();

            var redirector = new ConsoleRedirector();
            redirector.Logging = logging;
            redirector.WriteLine(msg);

            mocks.VerifyAll();
        }
    }
}
