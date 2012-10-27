using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.Misc
{
    public class BoredAtWorkException : Exception
    {
        public BoredAtWorkException()
            : base("Work boredom detected, please carry on with CryMono work")
        {
        }
    }
}
