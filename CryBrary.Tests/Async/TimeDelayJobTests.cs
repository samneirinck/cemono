using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CryEngine.Async;
using CryEngine.Async.Jobs;
using Xunit;

namespace CryBrary.Tests.Async
{
    public class TimeDelayJobTests : CryBraryTests
    {
        [Fact]
        public void TimeDelayJobTests_NegativeTime_InstantExecution()
        {
            // Arrange
            var job = new TimeDelayJob(TimeSpan.FromSeconds(-1));

            // Act

            // Assert
            Assert.True(job.IsFinished);
            Assert.True(job.Task.IsCompleted);
            Assert.False(job.Task.IsCanceled);
            Assert.False(job.Task.IsFaulted);
            Assert.True(job.Task.Status == TaskStatus.RanToCompletion);
        }

        [Fact]
        public void TimeDelayJobTests_2000ms_UpdatedAtTheCorrectTime()
        {
            // Arrange
            var job = new TimeDelayJob(TimeSpan.FromSeconds(2));

            // Act & assert
            job.Update(500);

            Assert.False(job.IsFinished);

            job.Update(1499);
            Assert.False(job.IsFinished);
            job.Update(1);
            Assert.True(job.IsFinished);
        }

    }
}
