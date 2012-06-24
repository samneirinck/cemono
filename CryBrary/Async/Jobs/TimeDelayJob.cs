using System;

namespace CryEngine.Async.Jobs
{
    /// <summary>
    /// Delays for a time period
    /// </summary>
    public class TimeDelayJob : AsyncJob<bool>
    {
        private readonly DateTime _beginTime;

        /// <summary>
        /// Delay in milliseconds
        /// </summary>
        public float DelayInMilliseconds { get; protected set; }


        /// <summary>
        /// Creates an instance of the TimeDelayJob
        /// </summary>
        /// <param name="milliseconds"></param>
        public TimeDelayJob(float milliseconds)
        {
            DelayInMilliseconds = milliseconds;
            _beginTime = DateTime.Now;

            if (milliseconds <= 0)
            {
                source.TrySetResult(false);
                IsFinished = true;
            }
        }

        /// <summary>
        /// Creates an instance of the TimeDelayJob
        /// </summary>
        /// <param name="delay"></param>
        public TimeDelayJob(TimeSpan delay)
            : this(System.Convert.ToSingle(delay.TotalMilliseconds))
        {
        }

        public override bool Update(float frameTime)
        {
            if (!IsFinished && DateTime.Now - _beginTime > TimeSpan.FromMilliseconds(DelayInMilliseconds))
            {
                source.TrySetResult(true);
                IsFinished = true;
            }
            return IsFinished;
        }
    }
}
