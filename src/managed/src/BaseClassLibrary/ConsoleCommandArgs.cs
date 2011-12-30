using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;

namespace CryEngine.API
{
    public class ConsoleCommandArgs
    {
        public ConsoleCommandArgs(IEnumerable<string> args)
        {
            Args = new List<string>(args);
        }
        
        public IList<string> Args { get; protected set; }
    }
}
