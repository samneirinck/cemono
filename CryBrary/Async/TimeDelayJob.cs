using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.Async
{
    public class TimeDelayJob : AsyncJob<bool>
    {
        // TODO: Refactor to use decent timing instead of DateTime

        public float DelayInMilliseconds { get; set; }

        private DateTime beginTime;

        public TimeDelayJob(float milliseconds)
            : base()
        {
            DelayInMilliseconds = milliseconds;
            beginTime = DateTime.Now;

            if (milliseconds <= 0)
            {
                source.TrySetResult(false);
                IsFinished = true;
            }
        }

        public TimeDelayJob(TimeSpan delay)
            : this(System.Convert.ToSingle(delay.TotalMilliseconds))
        {
        }

        public override bool Update(float frameTime)
        {
            if (!IsFinished && DateTime.Now - beginTime > TimeSpan.FromMilliseconds(DelayInMilliseconds))
            {
                source.TrySetResult(true);
                IsFinished = true;
            }
            return IsFinished;
        }
    }
}
