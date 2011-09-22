using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace CryEngine.API
{
    public class GameSystem
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern private static void _RegisterGameClass(BaseGame game);

        public static void RegisterGameClass(BaseGame game)
        {
            _RegisterGameClass(game);
        }
    }
}
