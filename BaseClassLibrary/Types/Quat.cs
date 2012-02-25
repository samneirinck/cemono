namespace CryEngine
{
	public struct Quat
	{
		#region Methods
		public Quat(Vec3 axis)
			: this()
		{
			Axis = axis;
		}

		public void Normalize()
		{
			float f = System.Convert.ToSingle(Math.ISqrt(Angle * Angle + Axis.X * Axis.X + Axis.Y * Axis.Y + Axis.Z * Axis.Z));

			Angle *= f;
			axis.X *= f;
			axis.Y *= f;
			axis.Z *= f;
		}
		#endregion

		#region Operators
		public static Vec3 operator *(Quat quat, Vec3 vec)
		{
			Vec3 retVec = new Vec3();
			Vec3 tempVec = new Vec3();

			tempVec.X = (quat.Axis.Y * vec.Z - quat.Axis.Z * vec.Y) + quat.Angle * vec.X;
			tempVec.Y = (quat.Axis.Z * vec.X - quat.Axis.X * vec.Z) + quat.Angle * vec.Y;
			tempVec.Z = (quat.Axis.X * vec.Y - quat.Axis.Y * vec.X) + quat.Angle * vec.Z;

			retVec.X = (tempVec.Z * quat.Axis.Y - tempVec.Y * quat.Axis.Z);
			retVec.X += retVec.X + vec.X;

			retVec.Y = (tempVec.X * quat.Axis.Z - tempVec.Z * quat.Axis.X);
			retVec.Y += retVec.Y + vec.Y;

			retVec.Z = (tempVec.Y * quat.Axis.X - tempVec.X* quat.Axis.Y);
			retVec.Z += retVec.Z + vec.Z;

			tempVec.Zero();

			return retVec;
		}
		#endregion

		private Vec3 axis;
		public Vec3 Axis 
		{ 
			get { return axis; } 
			set 
			{
				double sx, cx; Math.SinCos(value.X * 0.5, out sx, out cx);
				double sy, cy; Math.SinCos(value.Y * 0.5, out sy, out cy);
				double sz, cz; Math.SinCos(value.Z * 0.5, out sz, out cz);
				Angle = System.Convert.ToSingle(cx * cy * cz + sx * sy * sz);
				axis.X = System.Convert.ToSingle(cz * cy * sx - sz * sy * cx);
				axis.Y = System.Convert.ToSingle(cz * sy * cx + sz * cy * sx);
				axis.Z = System.Convert.ToSingle(sz * cy * cx - cz * sy * sx);
			}
		}

		public float Angle;
	}
}
