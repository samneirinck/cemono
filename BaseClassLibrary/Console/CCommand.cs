
namespace CryEngine
{
	public partial class Console
	{
        /// <summary>
        /// Called from C++ when a console command registered via Mono is invoked.
        /// </summary>
        /// <param name="cmdName"></param>
        public static void OnMonoCmd(string cmdName)
        {
        }
	}
    /*
	public class CCommand
	{
		public delegate void CommandDelegate(params object args);
		public CommandDelegate Command { get; set; }

		public CCommand(CommandDelegate command)
		{
			Command = command;
		}

		public void Execute(params object args)
		{
			Command.Invoke(args);
		}
	}*/
}
