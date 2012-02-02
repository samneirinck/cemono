using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
	/// <summary>
	/// Renderer functionality; TODO
	/// </summary>
	public class Renderer
	{
		public static Camera Camera;
	}

	public class Camera
	{
		public Vec3 ViewDir { get { return new Vec3(0, 0, 0); } }
		public Vec3 Position { get { return new Vec3(0, 0, 0); } set { } }
	}
}
