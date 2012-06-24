namespace CryEngine.Async.Jobs
{
    /// <summary>
    /// Delays execution for a number of frames
    /// </summary>
    public class FrameDelayJob : AsyncJob<bool>
    {
        private int _framesWaited;

        /// <summary>
        /// Number of frames to wait
        /// </summary>
        public int FramesToWait { get; protected set; }

        /// <summary>
        /// Creates an instance of the job
        /// </summary>
        /// <param name="numberOfFramesToWait"></param>
        public FrameDelayJob(int numberOfFramesToWait)
        {
            _framesWaited = 0;
            FramesToWait = numberOfFramesToWait;

            if (numberOfFramesToWait <= 0)
            {
                source.TrySetResult(false);
                IsFinished = true;
            }
        }


        public override bool Update(float frameTime)
        {
            if (!IsFinished)
            {
                _framesWaited++;
                if (_framesWaited >= FramesToWait)
                {
                    source.TrySetResult(true);
                    IsFinished = true;
                }
            }
            return IsFinished;
        }
    }
}
