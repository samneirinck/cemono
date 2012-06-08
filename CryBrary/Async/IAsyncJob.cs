using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CryEngine.Async
{
    public interface IAsyncJob
    {
        bool IsFinished { get; }
        bool Update(float frameTime);
        Task Task { get; }
    }
}
