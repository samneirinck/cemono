using System.Collections.Generic;
using CryEngine.Async.Jobs;

namespace CryEngine.Async
{
    /// <summary>
    /// Class that manages all jobs (updating, removing)
    /// </summary>
    public class Awaiter
    {
        /// <summary>
        /// Singleton instance of the awaiter
        /// </summary>
        public static Awaiter Instance { get; set; }

        private Awaiter()
        {

        }

        static Awaiter()
        {
            Instance = new Awaiter();
        }

        private readonly List<IAsyncJob> _jobs = new List<IAsyncJob>();
        /// <summary>
        /// A list of all jobs scheduled to be executed on the next OnUpdate call
        /// </summary>
        public List<IAsyncJob> Jobs
        {
            get
            {
                return _jobs;
            }
        }

        /// <summary>
        /// Updates all scheduled jobs
        /// </summary>
        /// <param name="frameTime"></param>
        public void OnUpdate(float frameTime)
        {
            for (int i = 0; i < Jobs.Count; i++)
            {
                var job = Jobs[i];
                // Update the job
                // If the job returns true, it means it has finished, and we can remove it from the updatelist
                if (job.Update(frameTime))
                {
                    Jobs.Remove(job);
                    
                    // We need to decrease i since we have removed an element
                    i--; 
                }
            }
        }
    }
}
