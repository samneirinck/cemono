using System;

namespace CryEngine.Async.Jobs
{
    /// <summary>
    /// Delays for a time period
    /// </summary>
    public class TimeDelayJob : AsyncJob<bool>
    {
        private float _timeElapsed;

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
            _timeElapsed = 0;

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
            _timeElapsed += frameTime;
            if (!IsFinished && _timeElapsed >= DelayInMilliseconds)
            {
                source.TrySetResult(true);
                IsFinished = true;
            }
            return IsFinished;
        }
    }
}
