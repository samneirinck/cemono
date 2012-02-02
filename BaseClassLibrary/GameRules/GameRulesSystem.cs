using System.Runtime.CompilerServices;

namespace CryEngine
{
	/// <summary>
	/// GameRules contains scriptbinds used in relation to the game rules system.
	/// </summary>
    public partial class GameRulesSystem
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _RegisterGameMode(string gamemode);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _AddGameModeAlias(string gamemode, string alias);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _AddGameModeLevelLocation(string gamemode, string location);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetDefaultGameMode(string gamemode);
    }
}