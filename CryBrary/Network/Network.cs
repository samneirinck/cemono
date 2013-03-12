using System;
using System.IO;
using System.Reflection;

using CryEngine.Serialization;
using CryEngine.Extensions;
using CryEngine.Native;

namespace CryEngine
{
    public static class Network
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

        #region Properties
        public static bool IsMultiplayer { get { return NativeNetworkMethods.IsMultiplayer(); } }
        public static bool IsServer { get { return NativeNetworkMethods.IsServer(); } }
        public static bool IsClient { get { return NativeNetworkMethods.IsClient(); } }
		public static bool IsPureClient { get { return NativeNetworkMethods.IsClient() && !NativeNetworkMethods.IsServer(); } }

        // TODO: Find another place for this? - not quite so networky.
        public static bool IsEditor { private set; get; }
        public static bool IsDedicated { private set; get; }
        #endregion
    }
}
