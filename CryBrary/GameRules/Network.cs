
namespace CryEngine
{
	public static partial class CryNetwork
	{
		/// <summary>
		/// Initializes static variables which never change, i.e. IsEditor.
		/// </summary>
		/// <param name="isEditor"></param>
		/// <param name="isDedicated"></param>
		internal static void InitializeNetworkStatics(bool isEditor, bool isDedicated)
		{
			IsEditor = isEditor;
			IsDedicated = isDedicated;
		}

		/// <summary>
		/// Initializes variables which change, i.e. IsServer.
		/// </summary>
		/// <param name="isMultiplayer"></param>
		/// <param name="isClient"></param>
		/// <param name="isServer"></param>
		internal static void InitializeNetwork(bool isMultiplayer, bool isClient, bool isServer)
		{
			IsMultiplayer = isMultiplayer;
			IsClient = isClient;
			IsServer = isServer;
		}

		public static bool IsMultiplayer { private set; get; }
		public static bool IsServer { private set; get; }
		public static bool IsClient { private set; get; }
		// TODO: Find another place for this? - not quite so networky.
		public static bool IsEditor { private set; get; }
		public static bool IsDedicated { private set; get; }
	}
}
