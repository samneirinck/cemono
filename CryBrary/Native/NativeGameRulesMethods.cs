using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal class NativeGameRulesMethods : INativeGameRulesMethods
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _RegisterGameMode(string gamemode);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _AddGameModeAlias(string gamemode, string alias);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _AddGameModeLevelLocation(string gamemode, string location);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetDefaultGameMode(string gamemode);


        public void RegisterGameMode(string gamemode)
        {
            _RegisterGameMode(gamemode);
        }

        public void AddGameModeAlias(string gamemode, string alias)
        {
            _AddGameModeAlias(gamemode, alias);
        }

        public void AddGameModeLevelLocation(string gamemode, string location)
        {
            _AddGameModeLevelLocation(gamemode, location);
        }

        public void SetDefaultGameMode(string gamemode)
        {
            _SetDefaultGameMode(gamemode);
        }
    }
}
