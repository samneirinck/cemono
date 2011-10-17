using System;
using System.Collections;

namespace CryEngine
{
    public class BaseGame : IGame
    {
        #region Events
        public event EventHandler GameStarted;
        #endregion

        public virtual bool Init()
        {
            return true;
        }

        public virtual bool CompleteInit()
        {
            return true;
        }

        public virtual bool Shutdown()
        {
            return true;
        }

        public virtual IEnumerator Update(bool haveFocus)
        {
            yield return null;
        }
    }
}
