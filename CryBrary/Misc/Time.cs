using System.Runtime.CompilerServices;

namespace CryEngine
{
	/// <summary>
	/// Contains useful functions related to game timing.
	/// </summary>
	public static class Time
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static float _GetFrameStartTime();

        internal static void Set(float frameTime)
        {
            DeltaTime = frameTime;

            FrameStartTime = _GetFrameStartTime();
        }

		/// <summary>
		/// Returns the absolute time of the last frame start in milliseconds.
		/// </summary>
        public static float FrameStartTime { get; private set; }

		/// <summary>
		/// The time used to render the current frame. Useful for creating framerate independent operations.
		/// </summary>
		/// <example>public override void OnUpdate() { this.Position.X += 10 * CryTime.DeltaTime; }</example>
		public static float DeltaTime { get; private set; }
	}
}