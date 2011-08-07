using System.Collections;
namespace CryEngine
{
    public interface IGame
    {
        bool Init();
        bool CompleteInit();
        bool Shutdown();
        IEnumerator Update(bool haveFocus);
    }
}
