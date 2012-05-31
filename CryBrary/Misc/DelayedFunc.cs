using System;
using CryEngine.Initialization;

namespace CryEngine
{
	public sealed class DelayedFunc<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> : CryScriptInstance
	{
		/// <summary>
		/// </summary>
		/// <param name="delay">The desired delay, in milliseconds.</param>
		public DelayedFunc(Action<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> action, float delay, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7, T8 param8, T9 param9, T10 param10)
		{
			ScriptManager.AddScriptInstance(this);

			Action = action;
			Delay = delay;
			FrameStartTime = Time.FrameStartTime;

			Param1 = param1;
			Param2 = param2;
			Param3 = param3;
			Param4 = param4;
			Param5 = param5;
			Param6 = param6;
			Param7 = param7;
			Param8 = param8;
			Param9 = param9;
			Param10 = param10;

			ReceiveUpdates = true;
		}

		public void Reset()
		{
			FrameStartTime = Time.FrameStartTime;

			ReceiveUpdates = true;
		}

		public override void OnUpdate()
		{
			if((Time.FrameStartTime - FrameStartTime) > Delay)
			{
				ReceiveUpdates = false;

				Action(Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8, Param9, Param10);
			}
		}

		public float Delay { get; set; }
		public float FrameStartTime { get; set; }
		public Action<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> Action { get; set; }
		public T1 Param1 { get; set; }
		public T2 Param2 { get; set; }
		public T3 Param3 { get; set; }
		public T4 Param4 { get; set; }
		public T5 Param5 { get; set; }
		public T6 Param6 { get; set; }
		public T7 Param7 { get; set; }
		public T8 Param8 { get; set; }
		public T9 Param9 { get; set; }
		public T10 Param10 { get; set; }
	}

	public sealed class DelayedFunc<T1, T2, T3, T4, T5, T6, T7, T8, T9> : CryScriptInstance
	{
		/// <summary>
		/// </summary>
		/// <param name="delay">The desired delay, in milliseconds.</param>
		public DelayedFunc(Action<T1, T2, T3, T4, T5, T6, T7, T8, T9> action, float delay, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7, T8 param8, T9 param9)
		{
			ScriptManager.AddScriptInstance(this);

			Action = action;
			Delay = delay;
			FrameStartTime = Time.FrameStartTime;

			Param1 = param1;
			Param2 = param2;
			Param3 = param3;
			Param4 = param4;
			Param5 = param5;
			Param6 = param6;
			Param7 = param7;
			Param8 = param8;
			Param9 = param9;

			ReceiveUpdates = true;
		}

		public void Reset()
		{
			FrameStartTime = Time.FrameStartTime;

			ReceiveUpdates = true;
		}

		public override void OnUpdate()
		{
			if((Time.FrameStartTime - FrameStartTime) > Delay)
			{
				ReceiveUpdates = false;

				Action(Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8, Param9);
			}
		}

		public float Delay { get; set; }
		public float FrameStartTime { get; set; }
		public Action<T1, T2, T3, T4, T5, T6, T7, T8, T9> Action { get; set; }
		public T1 Param1 { get; set; }
		public T2 Param2 { get; set; }
		public T3 Param3 { get; set; }
		public T4 Param4 { get; set; }
		public T5 Param5 { get; set; }
		public T6 Param6 { get; set; }
		public T7 Param7 { get; set; }
		public T8 Param8 { get; set; }
		public T9 Param9 { get; set; }
	}

	public sealed class DelayedFunc<T1, T2, T3, T4, T5, T6, T7, T8> : CryScriptInstance
	{
		/// <summary>
		/// </summary>
		/// <param name="delay">The desired delay, in milliseconds.</param>
		public DelayedFunc(Action<T1, T2, T3, T4, T5, T6, T7, T8> action, float delay, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7, T8 param8)
		{
			ScriptManager.AddScriptInstance(this);

			Action = action;
			Delay = delay;
			FrameStartTime = Time.FrameStartTime;

			Param1 = param1;
			Param2 = param2;
			Param3 = param3;
			Param4 = param4;
			Param5 = param5;
			Param6 = param6;
			Param7 = param7;
			Param8 = param8;

			ReceiveUpdates = true;
		}

		public void Reset()
		{
			FrameStartTime = Time.FrameStartTime;

			ReceiveUpdates = true;
		}

		public override void OnUpdate()
		{
			if((Time.FrameStartTime - FrameStartTime) > Delay)
			{
				ReceiveUpdates = false;

				Action(Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8);
			}
		}

		public float Delay { get; set; }
		public float FrameStartTime { get; set; }
		public Action<T1, T2, T3, T4, T5, T6, T7, T8> Action { get; set; }
		public T1 Param1 { get; set; }
		public T2 Param2 { get; set; }
		public T3 Param3 { get; set; }
		public T4 Param4 { get; set; }
		public T5 Param5 { get; set; }
		public T6 Param6 { get; set; }
		public T7 Param7 { get; set; }
		public T8 Param8 { get; set; }
	}

	public sealed class DelayedFunc<T1, T2, T3, T4, T5, T6, T7> : CryScriptInstance
	{
		/// <summary>
		/// </summary>
		/// <param name="delay">The desired delay, in milliseconds.</param>
		public DelayedFunc(Action<T1, T2, T3, T4, T5, T6, T7> action, float delay, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7)
		{
			ScriptManager.AddScriptInstance(this);

			Action = action;
			Delay = delay;
			FrameStartTime = Time.FrameStartTime;

			Param1 = param1;
			Param2 = param2;
			Param3 = param3;
			Param4 = param4;
			Param5 = param5;
			Param6 = param6;
			Param7 = param7;

			ReceiveUpdates = true;
		}

		public void Reset()
		{
			FrameStartTime = Time.FrameStartTime;

			ReceiveUpdates = true;
		}

		public override void OnUpdate()
		{
			if((Time.FrameStartTime - FrameStartTime) > Delay)
			{
				ReceiveUpdates = false;

				Action(Param1, Param2, Param3, Param4, Param5, Param6, Param7);
			}
		}

		public float Delay { get; set; }
		public float FrameStartTime { get; set; }
		public Action<T1, T2, T3, T4, T5, T6, T7> Action { get; set; }
		public T1 Param1 { get; set; }
		public T2 Param2 { get; set; }
		public T3 Param3 { get; set; }
		public T4 Param4 { get; set; }
		public T5 Param5 { get; set; }
		public T6 Param6 { get; set; }
		public T7 Param7 { get; set; }
	}

	public sealed class DelayedFunc<T1, T2, T3, T4, T5, T6> : CryScriptInstance
	{
		/// <summary>
		/// </summary>
		/// <param name="delay">The desired delay, in milliseconds.</param>
		public DelayedFunc(Action<T1, T2, T3, T4, T5, T6> action, float delay, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6)
		{
			ScriptManager.AddScriptInstance(this);

			Action = action;
			Delay = delay;
			FrameStartTime = Time.FrameStartTime;

			Param1 = param1;
			Param2 = param2;
			Param3 = param3;
			Param4 = param4;
			Param5 = param5;
			Param6 = param6;

			ReceiveUpdates = true;
		}

		public void Reset()
		{
			FrameStartTime = Time.FrameStartTime;

			ReceiveUpdates = true;
		}

		public override void OnUpdate()
		{
			if((Time.FrameStartTime - FrameStartTime) > Delay)
			{
				ReceiveUpdates = false;

				Action(Param1, Param2, Param3, Param4, Param5, Param6);
			}
		}

		public float Delay { get; set; }
		public float FrameStartTime { get; set; }
		public Action<T1, T2, T3, T4, T5, T6> Action { get; set; }
		public T1 Param1 { get; set; }
		public T2 Param2 { get; set; }
		public T3 Param3 { get; set; }
		public T4 Param4 { get; set; }
		public T5 Param5 { get; set; }
		public T6 Param6 { get; set; }
	}

	public sealed class DelayedFunc<T1, T2, T3, T4, T5> : CryScriptInstance
	{
		/// <summary>
		/// </summary>
		/// <param name="delay">The desired delay, in milliseconds.</param>
		public DelayedFunc(Action<T1, T2, T3, T4, T5> action, float delay, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5)
		{
			ScriptManager.AddScriptInstance(this);

			Action = action;
			Delay = delay;
			FrameStartTime = Time.FrameStartTime;

			Param1 = param1;
			Param2 = param2;
			Param3 = param3;
			Param4 = param4;
			Param5 = param5;

			ReceiveUpdates = true;
		}

		public void Reset()
		{
			FrameStartTime = Time.FrameStartTime;

			ReceiveUpdates = true;
		}

		public override void OnUpdate()
		{
			if((Time.FrameStartTime - FrameStartTime) > Delay)
			{
				ReceiveUpdates = false;

				Action(Param1, Param2, Param3, Param4, Param5);
			}
		}

		public float Delay { get; set; }
		public float FrameStartTime { get; set; }
		public Action<T1, T2, T3, T4, T5> Action { get; set; }
		public T1 Param1 { get; set; }
		public T2 Param2 { get; set; }
		public T3 Param3 { get; set; }
		public T4 Param4 { get; set; }
		public T5 Param5 { get; set; }
	}

	public sealed class DelayedFunc<T1, T2, T3, T4> : CryScriptInstance
	{
		/// <summary>
		/// </summary>
		/// <param name="delay">The desired delay, in milliseconds.</param>
		public DelayedFunc(Action<T1, T2, T3, T4> action, float delay, T1 param1, T2 param2, T3 param3, T4 param4)
		{
			ScriptManager.AddScriptInstance(this);

			Action = action;
			Delay = delay;
			FrameStartTime = Time.FrameStartTime;

			Param1 = param1;
			Param2 = param2;
			Param3 = param3;
			Param4 = param4;

			ReceiveUpdates = true;
		}

		public void Reset()
		{
			FrameStartTime = Time.FrameStartTime;

			ReceiveUpdates = true;
		}

		public override void OnUpdate()
		{
			if((Time.FrameStartTime - FrameStartTime) > Delay)
			{
				ReceiveUpdates = false;

				Action(Param1, Param2, Param3, Param4);
			}
		}

		public float Delay { get; set; }
		public float FrameStartTime { get; set; }
		public Action<T1, T2, T3, T4> Action { get; set; }
		public T1 Param1 { get; set; }
		public T2 Param2 { get; set; }
		public T3 Param3 { get; set; }
		public T4 Param4 { get; set; }
	}

	public sealed class DelayedFunc<T1, T2, T3> : CryScriptInstance
	{
		/// <summary>
		/// </summary>
		/// <param name="delay">The desired delay, in milliseconds.</param>
		public DelayedFunc(Action<T1, T2, T3> action, float delay, T1 param1, T2 param2, T3 param3)
		{
			ScriptManager.AddScriptInstance(this);

			Action = action;
			Delay = delay;
			FrameStartTime = Time.FrameStartTime;

			Param1 = param1;
			Param2 = param2;
			Param3 = param3;

			ReceiveUpdates = true;
		}

		public void Reset()
		{
			FrameStartTime = Time.FrameStartTime;

			ReceiveUpdates = true;
		}

		public override void OnUpdate()
		{
			if((Time.FrameStartTime - FrameStartTime) > Delay)
			{
				ReceiveUpdates = false;

				Action(Param1, Param2, Param3);
			}
		}

		public float Delay { get; set; }
		public float FrameStartTime { get; set; }
		public Action<T1, T2, T3> Action { get; set; }
		public T1 Param1 { get; set; }
		public T2 Param2 { get; set; }
		public T3 Param3 { get; set; }
	}

	public sealed class DelayedFunc<T1, T2> : CryScriptInstance
	{
		/// <summary>
		/// </summary>
		/// <param name="delay">The desired delay, in milliseconds.</param>
		public DelayedFunc(Action<T1, T2> action, float delay, T1 param1, T2 param2)
		{
			ScriptManager.AddScriptInstance(this);

			Action = action;
			Delay = delay;
			FrameStartTime = Time.FrameStartTime;

			Param1 = param1;
			Param2 = param2;

			ReceiveUpdates = true;
		}

		public void Reset()
		{
			FrameStartTime = Time.FrameStartTime;

			ReceiveUpdates = true;
		}

		public override void OnUpdate()
		{
			if((Time.FrameStartTime - FrameStartTime) > Delay)
			{
				ReceiveUpdates = false;

				Action(Param1, Param2);
			}
		}

		public float Delay { get; set; }
		public float FrameStartTime { get; set; }
		public Action<T1, T2> Action { get; set; }
		public T1 Param1 { get; set; }
		public T2 Param2 { get; set; }
	}

	public sealed class DelayedFunc<T1> : CryScriptInstance
	{
		/// <summary>
		/// </summary>
		/// <param name="delay">The desired delay, in milliseconds.</param>
		public DelayedFunc(Action<T1> action, float delay, T1 param1)
		{
			ScriptManager.AddScriptInstance(this);

			Action = action;
			Delay = delay;
			FrameStartTime = Time.FrameStartTime;

			Param1 = param1;

			ReceiveUpdates = true;
		}

		public void Reset()
		{
			FrameStartTime = Time.FrameStartTime;

			ReceiveUpdates = true;
		}

		public override void OnUpdate()
		{
			if((Time.FrameStartTime - FrameStartTime) > Delay)
			{
				ReceiveUpdates = false;

				Action(Param1);
			}
		}

		public float Delay { get; set; }
		public float FrameStartTime { get; set; }
		public Action<T1> Action { get; set; }
		public T1 Param1 { get; set; }
	}

	public sealed class DelayedFunc : CryScriptInstance
	{
		/// <summary>
		/// </summary>
		/// <param name="delay">The desired delay, in milliseconds.</param>
		public DelayedFunc(Action action, float delay)
		{
			ScriptManager.AddScriptInstance(this);

			Action = action;
			Delay = delay;
			FrameStartTime = Time.FrameStartTime;

			ReceiveUpdates = true;
		}

		public void Reset()
		{
			FrameStartTime = Time.FrameStartTime;

			ReceiveUpdates = true;
		}

		public override void OnUpdate()
		{
			if((Time.FrameStartTime - FrameStartTime) > Delay)
			{
				ReceiveUpdates = false;

				Action();
			}
		}

		public float Delay { get; set; }
		public float FrameStartTime { get; set; }
		public Action Action { get; set; }
	}
}