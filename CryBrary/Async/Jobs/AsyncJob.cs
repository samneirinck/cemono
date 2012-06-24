using System.Threading.Tasks;

namespace CryEngine.Async.Jobs
{
    /// <summary>
    /// Base class for any async job
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public abstract class AsyncJob<T> : IAsyncJob
    {
        protected TaskCompletionSource<T> source = new TaskCompletionSource<T>();
        /// <summary>
        /// The underlying System.Threading.Tasks.Task object
        /// </summary>
        public Task Task
        {
            get
            {
                return source.Task; 
            }
        }

        /// <summary>
        /// Indicates if the job has finished executing
        /// </summary>
        public bool IsFinished { get; set; }

        /// <summary>
        /// Updates the job
        /// </summary>
        /// <param name="frameTime"></param>
        /// <returns></returns>
        public abstract bool Update(float frameTime);
    }
}
