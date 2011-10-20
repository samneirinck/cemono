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

        /// <summary>
        /// Registers the game class to the engine
        /// </summary>
        /// <param name="game">Implementation of the BaseGame abstract class</param>
        public static void RegisterGameClass(BaseGame game)
        {
            _RegisterGameClass(game);
        }
    }
}
