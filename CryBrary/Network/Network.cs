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

        public static void RemoteInvocation(Action action, NetworkTarget netTarget)
        {
            RemoteInvocation(action.Target as EntityBase, action.Method, netTarget, null);
        }

        #region RemoteInvocation arg generics
        public static void RemoteInvocation<T1>(Action<T1> action, NetworkTarget netTarget, T1 param1)
        {
            object[] args = new object[1];
            args[0] = param1;

            RemoteInvocation(action.Target as EntityBase, action.Method, netTarget, args);
        }

        public static void RemoteInvocation<T1, T2>(Action<T1, T2> action, NetworkTarget netTarget, T1 param1, T2 param2)
        {
            object[] args = new object[2];
            args[0] = param1;
            args[1] = param2;

            RemoteInvocation(action.Target as EntityBase, action.Method, netTarget, args);
        }

        public static void RemoteInvocation<T1, T2, T3>(Action<T1, T2, T3> action, NetworkTarget netTarget, T1 param1, T2 param2, T3 param3)
        {
            object[] args = new object[3];
            args[0] = param1;
            args[1] = param2;
            args[2] = param3;

            RemoteInvocation(action.Target as EntityBase, action.Method, netTarget, args);
        }

        public static void RemoteInvocation<T1, T2, T3, T4>(Action<T1, T2, T3, T4> action, NetworkTarget netTarget, T1 param1, T2 param2, T3 param3, T4 param4)
        {
            object[] args = new object[4];
            args[0] = param1;
            args[1] = param2;
            args[2] = param3;
            args[3] = param4;

            RemoteInvocation(action.Target as EntityBase, action.Method, netTarget, args);
        }

        public static void RemoteInvocation<T1, T2, T3, T4, T5>(Action<T1, T2, T3, T4, T5> action, NetworkTarget netTarget, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5)
        {
            object[] args = new object[5];
            args[0] = param1;
            args[1] = param2;
            args[2] = param3;
            args[3] = param4;
            args[4] = param5;

            RemoteInvocation(action.Target as EntityBase, action.Method, netTarget, args);
        }

        public static void RemoteInvocation<T1, T2, T3, T4, T5, T6>(Action<T1, T2, T3, T4, T5, T6> action, NetworkTarget netTarget, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6)
        {
            object[] args = new object[6];
            args[0] = param1;
            args[1] = param2;
            args[2] = param3;
            args[3] = param4;
            args[4] = param5;
            args[5] = param6;

            RemoteInvocation(action.Target as EntityBase, action.Method, netTarget, args);
        }
        #endregion

        static void RemoteInvocation(EntityBase target, MethodInfo method, NetworkTarget netTarget, params object[] args)
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (!method.ContainsAttribute<RemoteInvocationAttribute>())
                throw new AttributeUsageException("Method did not contain RemoteInvocation attribute");
            if (target == null)
                throw new RemoteInvocationException("Non-static method owner does not derive from CryScriptInstance.");
#endif

            const string networkManagerName = "CryMonoNetworkManager";
            if (Entity.Find(networkManagerName) == null)
                NetworkManager = Entity.Spawn(networkManagerName, typeof(NativeEntity).Name);

            NativeNetworkMethods.RemoteInvocation(NetworkManager.Id, target.Id, method.Name, args, netTarget, -1);
        }

        public static void OnRemoteInvocation(string methodName, object[] args, EntityId targetId)
        {
            var entity = Entity.Get(targetId);

            entity.GetType().InvokeMember(methodName, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.InvokeMethod, null, entity, args);
        }

        internal static Entity NetworkManager { get; set; }

        #region Properties
        public static bool IsMultiplayer { get { return NativeNetworkMethods.IsMultiplayer(); } }
        public static bool IsServer { get { return NativeNetworkMethods.IsServer(); } }
        public static bool IsClient { get { return NativeNetworkMethods.IsClient(); } }

        // TODO: Find another place for this? - not quite so networky.
        public static bool IsEditor { private set; get; }
        public static bool IsDedicated { private set; get; }
        #endregion
    }
}
