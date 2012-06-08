using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CryEngine.Async;
using NUnit.Framework;

namespace CryBrary.Tests.Async
{
    [TestFixture]
    public class FrameDelayJobTests : CryBraryTests
    {

        [Test]
        public void FrameDelayJob_NegativeNumberOfFrames_InstantExecution()
        {
            // Arrange
            var job = new FrameDelayJob(-20);

            // Act

            // Assert
            Assert.IsTrue(job.IsFinished);
            Assert.IsTrue(job.Task.IsCompleted);
            Assert.IsFalse(job.Task.IsCanceled);
            Assert.IsFalse(job.Task.IsFaulted);
            Assert.IsTrue(job.Task.Status == TaskStatus.RanToCompletion);
        }

        [Test]
        public void FrameDelayJob_5Frames_DelayedExecution()
        {
            // Arrange
            var job = new FrameDelayJob(5);

            Assert.IsFalse(job.IsFinished);
            Assert.IsFalse(job.Task.IsCompleted);

            // Act
            Assert.IsFalse(job.Update(1));
            Assert.IsFalse(job.Update(1));
            Assert.IsFalse(job.Update(1));
            Assert.IsFalse(job.Update(1));

            // 5th frame should be the final one
            Assert.IsTrue(job.Update(1));

            // Assert
            Assert.IsTrue(job.IsFinished);
            Assert.IsTrue(job.Task.IsCompleted);

        }
    }
}
