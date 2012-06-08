using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.Async
{
    public class FrameDelayJob : AsyncJob<bool>
    {
        int framesWaited;
        public int FramesToWait { get; set; }

        public FrameDelayJob(int numberOfFramesToWait)
        {
            framesWaited = 0;
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
                framesWaited++;
                if (framesWaited >= FramesToWait)
                {
                    source.TrySetResult(true);
                    IsFinished = true;
                }
            }
            return IsFinished;
        }
    }
}
