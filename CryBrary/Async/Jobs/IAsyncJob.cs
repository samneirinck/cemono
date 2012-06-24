using System.Threading.Tasks;

namespace CryEngine.Async.Jobs
{
    /// <summary>
    /// An asynchronous job
    /// </summary>
    public interface IAsyncJob
    {
        /// <summary>
        /// Indicates if the job has finished processing
        /// </summary>
        bool IsFinished { get; }

        /// <summary>
        /// Updates the job
        /// </summary>
        /// <param name="frameTime"></param>
        /// <returns></returns>
        bool Update(float frameTime);

        /// <summary>
        /// Gets the internal Task object associated with the job
        /// </summary>
        Task Task { get; }
    }
}
