using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CryEngine.Async;

namespace CryEngine
{
    public class Delay
    {
        private static Task CreateTaskFromJob(IAsyncJob job)
        {
            job.Task.ConfigureAwait(false).GetAwaiter();
            Awaiter.Instance.Jobs.Add(job);
            return job.Task;
        }


        public static Task FrameDelay(int numFrames)
        {
            var frameDelayJob = new FrameDelayJob(numFrames);

            return CreateTaskFromJob(frameDelayJob);
        }

        public static Task TimeDelay(TimeSpan delay)
        {
            var timeDelayJob = new TimeDelayJob(delay);

            return CreateTaskFromJob(timeDelayJob);
        }
    }
}
