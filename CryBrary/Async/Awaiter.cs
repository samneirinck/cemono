using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.Async
{
    public class Awaiter
    {
        public static Awaiter Instance { get; set; }
        private Awaiter()
        {

        }

        static Awaiter()
        {
            Instance = new Awaiter();
        }

        private readonly List<IAsyncJob> _jobs = new List<IAsyncJob>();
        public List<IAsyncJob> Jobs
        {
            get
            {
                return _jobs;
            }
        }

        public void OnUpdate(float frameTime)
        {
            for (int i = 0; i < Jobs.Count; i++)
            {
                var job = Jobs[i];
                // Update the job
                // If the job returns true, it means it has finished, and we can remove it from the updatelist
                if (job.Update(frameTime) == true)
                {
                    Jobs.Remove(job);
                    
                    // We need to decrease i since we have removed an element
                    i--; 
                }
            }
        }
    }
}
