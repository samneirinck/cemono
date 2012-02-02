
namespace CryEngine
{
	/// <summary>
	/// Contains useful functions related to game timing.
	/// </summary>
	public static class Time
	{
		/// <summary>
		/// The time used to render the current frame. Useful for creating framerate independent operations.
		/// </summary>
		/// <example>public override void OnUpdate() { this.Position.X += 10 * CryTime.DeltaTime; }</example>
		public static float DeltaTime { get; internal set; }
	}
}