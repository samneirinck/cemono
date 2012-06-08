using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CryEngine.Async
{

    public abstract class AsyncJob<T> : IAsyncJob
    {
        protected TaskCompletionSource<T> source = new TaskCompletionSource<T>();
        public Task Task
        {
            get
            {
                return source.Task; 
            }
        }
        public bool IsFinished { get; set; }
        public abstract bool Update(float frameTime);
    }
}
