using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;

using CryEngine;
using System.Diagnostics;

namespace CryMono
{
	public class Manager
	{
		Manager()
		{
            CryConsole.LogAlways("Initializing CryMono Manager ...");
			CryConsole.IgnoreNonExistentCVars = false;

			// Set up console redirect (Console.* methods) & Catch unhandled exceptions
			console = new CryConsole();
			scriptCompiler = new CryScriptCompiler(this);
			//Enable script reloading
			//CryFileMonitor.Enabled = true;

            updateCallback = new CallbackHandler.Callback(Update); 
            CallbackHandler.RegisterCallback("Update", "Manager", updateCallback);

            CryConsole.LogAlways("Initializing CryMono Manager done, MemUsage={0}Kb ", CryStats.MemoryUsageKB.ToString());
		}

        /// <summar>y
        /// Called once per frame.
        /// </summary>
        void Update()
        {
            scriptCompiler.UpdateScriptsOfType(MonoScriptType.Entity);
        }

        public int InstantiateClass(MonoScriptType scriptType, string scriptName)
		{
            int scriptId = -1;

			switch(scriptType)
			{
				case MonoScriptType.GameRules:
					{
                        scriptId = scriptCompiler.Instantiate<IGameRules>(scriptName);
					}
                    break;
				case MonoScriptType.FlowNode:
					{
						//return scriptCompiler.Instantiate<IFlowNode>(scriptName);
					}
					break;
				case MonoScriptType.Entity:
					{
                        scriptId = scriptCompiler.Instantiate<Entity>(scriptName);
					}
                    break;
			}

            return scriptId;
		}

        public void RemoveScriptInstance(int scriptId, string scriptName)
        {
            scriptCompiler.RemoveInstance(scriptId, scriptName);
        }

        public object InvokeScriptFunctionById(int id, string func, object[] args = null)
        {
            return InvokeScriptFunction(scriptCompiler.GetScriptInstanceById(id).Instance, func, args);
        }

        public object InvokeScriptFunction(object scriptInstance, string func, object[] args = null)
        {
            if (scriptInstance == null)
            {
                CryConsole.LogAlways("Attempted to invoke method {0} with an invalid instance.", func);
                return null;
            }

            MethodInfo methodInfo = scriptInstance.GetType().GetMethod(func);
            if (methodInfo == null)
            {
                CryConsole.LogAlways("Could not find method {0} in type {1}", func, scriptInstance.GetType().ToString());
                return null;
            }

            // Sort out optional parameters
            ParameterInfo[] info = methodInfo.GetParameters();

            if (info.Length > 0)
            {
                object[] tempArgs;
                tempArgs = new object[info.Length];
                int argIndexLength = args.Length - 1;

                for (int i = 0; i < info.Length; i++)
                {
                    if (i <= argIndexLength)
                        tempArgs.SetValue(args[i], i);
                    else if (i > argIndexLength && info[i].IsOptional)
                        tempArgs[i] = info[i].DefaultValue;
                }

                args = null;
                args = tempArgs;
                tempArgs = null;
            }
            else
                args = null;

            object result =  methodInfo.Invoke(scriptInstance, args);

            args = null;
            methodInfo = null;
            info = null;

            return result;
        }

        CallbackHandler.Callback updateCallback;
		CryConsole console;
		public static CryScriptCompiler scriptCompiler;
	}
}