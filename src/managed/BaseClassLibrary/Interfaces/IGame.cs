using System.Collections;
namespace Cemono
{
    public interface IGame
    {
        bool Init();
        bool CompleteInit();
        bool Shutdown();
        IEnumerator Update(bool haveFocus);
    }
}
