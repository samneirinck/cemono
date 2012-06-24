using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CryEngine.Async;
using CryEngine.Async.Jobs;
using NUnit.Framework;

namespace CryBrary.Tests.Async
{
    [TestFixture]
    public class TimeDelayJobTests : CryBraryTests
    {
        [Test]
        public void TimeDelayJobTests_NegativeTime_InstantExecution()
        {
            // Arrange
            var job = new TimeDelayJob(TimeSpan.FromSeconds(-1));

            // Act

            // Assert
            Assert.IsTrue(job.IsFinished);
            Assert.IsTrue(job.Task.IsCompleted);
            Assert.IsFalse(job.Task.IsCanceled);
            Assert.IsFalse(job.Task.IsFaulted);
            Assert.IsTrue(job.Task.Status == TaskStatus.RanToCompletion);
        }

        [Test]
        public void TimeDelayJobTests_500ms_UpdatedAtTheCorrectTime()
        {
            // Arrange
            var job = new TimeDelayJob(TimeSpan.FromSeconds(2));

            // Act & assert
            job.Update(500);

            Assert.IsFalse(job.IsFinished);

            job.Update(1499);
            Assert.IsFalse(job.IsFinished);
            job.Update(1);
            Assert.IsTrue(job.IsFinished);
        }

    }
}
