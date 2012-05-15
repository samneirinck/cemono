/*
* Copyright (c) 2007-2010 SlimDX Group
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
using System;
using System.Globalization;
using System.Runtime.InteropServices;

namespace CryEngine
{
	/// <summary>
	/// Represents a four dimensional mathematical quaternion.
	/// </summary>
	[Serializable]
	[StructLayout(LayoutKind.Sequential, Pack = 4)]
	public struct Quat : IEquatable<Quat>, IFormattable
	{
		/// <summary>
		/// The identity <see cref="CryEngine.Quat"/> (0, 0, 0, 1).
		/// </summary>
		public static readonly Quat Identity = new Quat(0.0f, 0.0f, 0.0f, 1.0f);

		/// <summary>
		/// The X, Y and Z components of the quaternion.
		/// </summary>
		public Vec3 V;

		/// <summary>
		/// The W component of the quaternion.
		/// </summary>
		public float W;

		/// <summary>
		/// Initializes a new instance of the <see cref="CryEngine.Quat"/> struct.
		/// </summary>
		/// <param name="value">The value that will be assigned to all components.</param>
		public Quat(float value)
		{
			V.X = value;
			V.Y = value;
			V.Z = value;
			W = value;
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="CryEngine.Quat"/> struct.
		/// </summary>
		/// <param name="value">A vector containing the values with which to initialize the components.</param>
		public Quat(Vec4 value)
		{
			V.X = value.X;
			V.Y = value.Y;
			V.Z = value.Z;
			W = value.W;
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="CryEngine.Quat"/> struct.
		/// </summary>
		/// <param name="value">A vector containing the values with which to initialize the X, Y, and Z components.</param>
		/// <param name="w">Initial value for the W component of the quaternion.</param>
		public Quat(Vec3 value, float w)
		{
			V.X = value.X;
			V.Y = value.Y;
			V.Z = value.Z;
			W = w;
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="CryEngine.Quat"/> struct.
		/// </summary>
		/// <param name="value">A vector containing the values with which to initialize the X and Y components.</param>
		/// <param name="z">Initial value for the Z component of the quaternion.</param>
		/// <param name="w">Initial value for the W component of the quaternion.</param>
		public Quat(Vec2 value, float z, float w)
		{
			V.X = value.X;
			V.Y = value.Y;
			V.Z = z;
			W = w;
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="CryEngine.Quat"/> struct.
		/// </summary>
		/// <param name="x">Initial value for the X component of the quaternion.</param>
		/// <param name="y">Initial value for the Y component of the quaternion.</param>
		/// <param name="z">Initial value for the Z component of the quaternion.</param>
		/// <param name="w">Initial value for the W component of the quaternion.</param>
		public Quat(float x, float y, float z, float w)
		{
			V.X = x;
			V.Y = y;
			V.Z = z;
			W = w;
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="CryEngine.Quat"/> struct.
		/// </summary>
		/// <param name="values">The values to assign to the X, Y, Z, and W components of the quaternion. This must be an array with four elements.</param>
		/// <exception cref="ArgumentNullException">Thrown when <paramref name="values"/> is <c>null</c>.</exception>
		/// <exception cref="ArgumentOutOfRangeException">Thrown when <paramref name="values"/> contains more or less than four elements.</exception>
		public Quat(float[] values)
		{
			if(values == null)
				throw new ArgumentNullException("values");
			if(values.Length != 4)
				throw new ArgumentOutOfRangeException("values", "There must be four and only four input values for Quaternion.");

			V.X = values[0];
			V.Y = values[1];
			V.Z = values[2];
			W = values[3];
		}

		/// <summary>
		/// Gets a value indicating whether this instance is equivalent to the identity quaternion.
		/// </summary>
		/// <value>
		/// <c>true</c> if this instance is an identity quaternion; otherwise, <c>false</c>.
		/// </value>
		public bool IsIdentity
		{
			get { return this.Equals(Identity); }
		}

		/// <summary>
		/// Gets a value indicting whether this instance is normalized.
		/// </summary>
		public bool IsNormalized
		{
			get { return Math.Abs((V.X * V.X) + (V.Y * V.Y) + (V.Z * V.Z) + (W * W) - 1f) < Math.ZeroTolerance; }
		}

		/// <summary>
		/// Gets the angle of the quaternion.
		/// </summary>
		/// <value>The quaternion's angle.</value>
		public float Angle
		{
			get
			{
				float length = (V.X * V.X) + (V.Y * V.Y) + (V.Z * V.Z);
				if(length < Math.ZeroTolerance)
					return 0.0f;

				return (float)(2.0 * Math.Acos(W));
			}
		}

		/// <summary>
		/// Gets the axis components of the quaternion.
		/// </summary>
		/// <value>The axis components of the quaternion.</value>
		public Vec3 Axis
		{
			get
			{
				float length = (V.X * V.X) + (V.Y * V.Y) + (V.Z * V.Z);
				if(length < Math.ZeroTolerance)
					return new Vec3(1, 0, 0);

				float inv = 1.0f / length;
				return new Vec3(V.X * inv, V.Y * inv, V.Z * inv);
			}
		}

		/// <summary>
		/// Gets or sets the component at the specified index.
		/// </summary>
		/// <value>The value of the X, Y, Z, or W component, depending on the index.</value>
		/// <param name="index">The index of the component to access. Use 0 for the X component, 1 for the Y component, 2 for the Z component, and 3 for the W component.</param>
		/// <returns>The value of the component at the specified index.</returns>
		/// <exception cref="System.ArgumentOutOfRangeException">Thrown when the <paramref name="index"/> is out of the range [0, 3].</exception>
		public float this[int index]
		{
			get
			{
				switch(index)
				{
					case 0: return V.X;
					case 1: return V.Y;
					case 2: return V.Z;
					case 3: return W;
				}

				throw new ArgumentOutOfRangeException("index", "Indices for Quaternion run from 0 to 3, inclusive.");
			}

			set
			{
				switch(index)
				{
					case 0: V.X = value; break;
					case 1: V.Y = value; break;
					case 2: V.Z = value; break;
					case 3: W = value; break;
					default: throw new ArgumentOutOfRangeException("index", "Indices for Quaternion run from 0 to 3, inclusive.");
				}
			}
		}

		/// <summary>
		/// Conjugates the quaternion.
		/// </summary>
		public void Conjugate()
		{
			V.X = -V.X;
			V.Y = -V.Y;
			V.Z = -V.Z;
		}

		public static Quat operator !(Quat quat)
		{
			return new Quat(quat.Axis, -quat.Angle);
		}
		/// <summary>
		/// Conjugates and renormalizes the quaternion.
		/// </summary>
		public void Invert()
		{
			this = !this;
		}

		/// <summary>
		/// Calculates the length of the quaternion.
		/// </summary>
		/// <returns>The length of the quaternion.</returns>
		/// <remarks>
		/// <see cref="CryEngine.Quat.LengthSquared"/> may be preferred when only the relative length is needed
		/// and speed is of the essence.
		/// </remarks>
		public float Length()
		{
			return (float)Math.Sqrt((V.X * V.X) + (V.Y * V.Y) + (V.Z * V.Z) + (W * W));
		}

		/// <summary>
		/// Calculates the squared length of the quaternion.
		/// </summary>
		/// <returns>The squared length of the quaternion.</returns>
		/// <remarks>
		/// This method may be preferred to <see cref="CryEngine.Quat.Length"/> when only a relative length is needed
		/// and speed is of the essence.
		/// </remarks>
		public float LengthSquared()
		{
			return (V.X * V.X) + (V.Y * V.Y) + (V.Z * V.Z) + (W * W);
		}

		/// <summary>
		/// Converts the quaternion into a unit quaternion.
		/// </summary>
		public void Normalize()
		{
			float length = Length();
			if(length > Math.ZeroTolerance)
			{
				float inverse = 1.0f / length;
				V.X *= inverse;
				V.Y *= inverse;
				V.Z *= inverse;
				W *= inverse;
			}
		}

		/// <summary>
		/// Creates an array containing the elements of the quaternion.
		/// </summary>
		/// <returns>A four-element array containing the components of the quaternion.</returns>
		public float[] ToArray()
		{
			return new float[] { V.X, V.Y, V.Z, W };
		}

		/// <summary>
		/// Returns a <see cref="CryEngine.Quat"/> containing the 4D Cartesian coordinates of a point specified in Barycentric coordinates relative to a 2D triangle.
		/// </summary>
		/// <param name="value1">A <see cref="CryEngine.Quat"/> containing the 4D Cartesian coordinates of vertex 1 of the triangle.</param>
		/// <param name="value2">A <see cref="CryEngine.Quat"/> containing the 4D Cartesian coordinates of vertex 2 of the triangle.</param>
		/// <param name="value3">A <see cref="CryEngine.Quat"/> containing the 4D Cartesian coordinates of vertex 3 of the triangle.</param>
		/// <param name="amount1">Barycentric coordinate b2, which expresses the weighting factor toward vertex 2 (specified in <paramref name="value2"/>).</param>
		/// <param name="amount2">Barycentric coordinate b3, which expresses the weighting factor toward vertex 3 (specified in <paramref name="value3"/>).</param>
		/// <param name="result">When the method completes, contains a new <see cref="CryEngine.Quat"/> containing the 4D Cartesian coordinates of the specified point.</param>
		public static void Barycentric(ref Quat value1, ref Quat value2, ref Quat value3, float amount1, float amount2, out Quat result)
		{
			Quat start, end;
			Slerp(ref value1, ref value2, amount1 + amount2, out start);
			Slerp(ref value1, ref value3, amount1 + amount2, out end);
			Slerp(ref start, ref end, amount2 / (amount1 + amount2), out result);
		}

		/// <summary>
		/// Returns a <see cref="CryEngine.Quat"/> containing the 4D Cartesian coordinates of a point specified in Barycentric coordinates relative to a 2D triangle.
		/// </summary>
		/// <param name="value1">A <see cref="CryEngine.Quat"/> containing the 4D Cartesian coordinates of vertex 1 of the triangle.</param>
		/// <param name="value2">A <see cref="CryEngine.Quat"/> containing the 4D Cartesian coordinates of vertex 2 of the triangle.</param>
		/// <param name="value3">A <see cref="CryEngine.Quat"/> containing the 4D Cartesian coordinates of vertex 3 of the triangle.</param>
		/// <param name="amount1">Barycentric coordinate b2, which expresses the weighting factor toward vertex 2 (specified in <paramref name="value2"/>).</param>
		/// <param name="amount2">Barycentric coordinate b3, which expresses the weighting factor toward vertex 3 (specified in <paramref name="value3"/>).</param>
		/// <returns>A new <see cref="CryEngine.Quat"/> containing the 4D Cartesian coordinates of the specified point.</returns>
		public static Quat Barycentric(Quat value1, Quat value2, Quat value3, float amount1, float amount2)
		{
			Quat result;
			Barycentric(ref value1, ref value2, ref value3, amount1, amount2, out result);
			return result;
		}

		/// <summary>
		/// Conjugates a quaternion.
		/// </summary>
		/// <param name="value">The quaternion to conjugate.</param>
		/// <param name="result">When the method completes, contains the conjugated quaternion.</param>
		public static void Conjugate(ref Quat value, out Quat result)
		{
			result.V.X = -value.V.X;
			result.V.Y = -value.V.Y;
			result.V.Z = -value.V.Z;
			result.W = value.W;
		}

		/// <summary>
		/// Conjugates a quaternion.
		/// </summary>
		/// <param name="value">The quaternion to conjugate.</param>
		/// <returns>The conjugated quaternion.</returns>
		public static Quat Conjugate(Quat value)
		{
			Quat result;
			Conjugate(ref value, out result);
			return result;
		}

		/// <summary>
		/// Calculates the dot product of two quaternions.
		/// </summary>
		/// <param name="left">First source quaternion.</param>
		/// <param name="right">Second source quaternion.</param>
		/// <param name="result">When the method completes, contains the dot product of the two quaternions.</param>
		public static void Dot(ref Quat left, ref Quat right, out float result)
		{
			result = (left.V.X * right.V.X) + (left.V.Y * right.V.Y) + (left.V.Z * right.V.Z) + (left.W * right.W);
		}

		/// <summary>
		/// Calculates the dot product of two quaternions.
		/// </summary>
		/// <param name="left">First source quaternion.</param>
		/// <param name="right">Second source quaternion.</param>
		/// <returns>The dot product of the two quaternions.</returns>
		public static float Dot(Quat left, Quat right)
		{
			return (left.V.X * right.V.X) + (left.V.Y * right.V.Y) + (left.V.Z * right.V.Z) + (left.W * right.W);
		}

		/// <summary>
		/// Exponentiates a quaternion.
		/// </summary>
		/// <param name="value">The quaternion to exponentiate.</param>
		/// <param name="result">When the method completes, contains the exponentiated quaternion.</param>
		public static void Exponential(ref Quat value, out Quat result)
		{
			float angle = (float)Math.Sqrt((value.V.X * value.V.X) + (value.V.Y * value.V.Y) + (value.V.Z * value.V.Z));
			float sin = (float)Math.Sin(angle);

			if(Math.Abs(sin) >= Math.ZeroTolerance)
			{
				float coeff = sin / angle;
				result.V.X = coeff * value.V.X;
				result.V.Y = coeff * value.V.Y;
				result.V.Z = coeff * value.V.Z;
			}
			else
			{
				result = value;
			}

			result.W = (float)Math.Cos(angle);
		}

		/// <summary>
		/// Exponentiates a quaternion.
		/// </summary>
		/// <param name="value">The quaternion to exponentiate.</param>
		/// <returns>The exponentiated quaternion.</returns>
		public static Quat Exponential(Quat value)
		{
			Quat result;
			Exponential(ref value, out result);
			return result;
		}

		/// <summary>
		/// Conjugates and renormalizes the quaternion.
		/// </summary>
		/// <param name="value">The quaternion to conjugate and renormalize.</param>
		/// <param name="result">When the method completes, contains the conjugated and renormalized quaternion.</param>
		public static void Invert(ref Quat value, out Quat result)
		{
			result = value;
			result.Invert();
		}

		/// <summary>
		/// Conjugates and renormalizes the quaternion.
		/// </summary>
		/// <param name="value">The quaternion to conjugate and renormalize.</param>
		/// <returns>The conjugated and renormalized quaternion.</returns>
		public static Quat Invert(Quat value)
		{
			Quat result;
			Invert(ref value, out result);
			return result;
		}

		/// <summary>
		/// Performs a linear interpolation between two quaternions.
		/// </summary>
		/// <param name="start">Start quaternion.</param>
		/// <param name="end">End quaternion.</param>
		/// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
		/// <param name="result">When the method completes, contains the linear interpolation of the two quaternions.</param>
		/// <remarks>
		/// This method performs the linear interpolation based on the following formula.
		/// <code>start + (end - start) * amount</code>
		/// Passing <paramref name="amount"/> a value of 0 will cause <paramref name="start"/> to be returned; a value of 1 will cause <paramref name="end"/> to be returned. 
		/// </remarks>
		public static void Lerp(ref Quat start, ref Quat end, float amount, out Quat result)
		{
			float inverse = 1.0f - amount;

			if(Dot(start, end) >= 0.0f)
			{
				result.V.X = (inverse * start.V.X) + (amount * end.V.X);
				result.V.Y = (inverse * start.V.Y) + (amount * end.V.Y);
				result.V.Z = (inverse * start.V.Z) + (amount * end.V.Z);
				result.W = (inverse * start.W) + (amount * end.W);
			}
			else
			{
				result.V.X = (inverse * start.V.X) - (amount * end.V.X);
				result.V.Y = (inverse * start.V.Y) - (amount * end.V.Y);
				result.V.Z = (inverse * start.V.Z) - (amount * end.V.Z);
				result.W = (inverse * start.W) - (amount * end.W);
			}

			result.Normalize();
		}

		/// <summary>
		/// Performs a linear interpolation between two quaternion.
		/// </summary>
		/// <param name="start">Start quaternion.</param>
		/// <param name="end">End quaternion.</param>
		/// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
		/// <returns>The linear interpolation of the two quaternions.</returns>
		/// <remarks>
		/// This method performs the linear interpolation based on the following formula.
		/// <code>start + (end - start) * amount</code>
		/// Passing <paramref name="amount"/> a value of 0 will cause <paramref name="start"/> to be returned; a value of 1 will cause <paramref name="end"/> to be returned. 
		/// </remarks>
		public static Quat Lerp(Quat start, Quat end, float amount)
		{
			Quat result;
			Lerp(ref start, ref end, amount, out result);
			return result;
		}

		/// <summary>
		/// Calculates the natural logarithm of the specified quaternion.
		/// </summary>
		/// <param name="value">The quaternion whose logarithm will be calculated.</param>
		/// <param name="result">When the method completes, contains the natural logarithm of the quaternion.</param>
		public static void Logarithm(ref Quat value, out Quat result)
		{
			if(Math.Abs(value.W) < 1.0)
			{
				float angle = (float)Math.Acos(value.W);
				float sin = (float)Math.Sin(angle);

				if(Math.Abs(sin) >= Math.ZeroTolerance)
				{
					float coeff = angle / sin;
					result.V.X = value.V.X * coeff;
					result.V.Y = value.V.Y * coeff;
					result.V.Z = value.V.Z * coeff;
				}
				else
				{
					result = value;
				}
			}
			else
			{
				result = value;
			}

			result.W = 0.0f;
		}

		/// <summary>
		/// Calculates the natural logarithm of the specified quaternion.
		/// </summary>
		/// <param name="value">The quaternion whose logarithm will be calculated.</param>
		/// <returns>The natural logarithm of the quaternion.</returns>
		public static Quat Logarithm(Quat value)
		{
			Quat result;
			Logarithm(ref value, out result);
			return result;
		}

		/// <summary>
		/// Converts the quaternion into a unit quaternion.
		/// </summary>
		/// <param name="value">The quaternion to normalize.</param>
		/// <param name="result">When the method completes, contains the normalized quaternion.</param>
		public static void Normalize(ref Quat value, out Quat result)
		{
			Quat temp = value;
			result = temp;
			result.Normalize();
		}

		/// <summary>
		/// Converts the quaternion into a unit quaternion.
		/// </summary>
		/// <param name="value">The quaternion to normalize.</param>
		/// <returns>The normalized quaternion.</returns>
		public static Quat Normalize(Quat value)
		{
			value.Normalize();
			return value;
		}

		/// <summary>
		/// Creates a quaternion given a rotation and an axis.
		/// </summary>
		/// <param name="axis">The axis of rotation.</param>
		/// <param name="angle">The angle of rotation.</param>
		/// <param name="result">When the method completes, contains the newly created quaternion.</param>
		public static void RotationAxis(ref Vec3 axis, float angle, out Quat result)
		{
			Vec3 normalized;
			Vec3.Normalize(ref axis, out normalized);

			float half = angle * 0.5f;
			float sin = (float)Math.Sin(half);
			float cos = (float)Math.Cos(half);

			result.V.X = normalized.X * sin;
			result.V.Y = normalized.Y * sin;
			result.V.Z = normalized.Z * sin;
			result.W = cos;
		}

		/// <summary>
		/// Creates a quaternion given a rotation and an axis.
		/// </summary>
		/// <param name="axis">The axis of rotation.</param>
		/// <param name="angle">The angle of rotation.</param>
		/// <returns>The newly created quaternion.</returns>
		public static Quat RotationAxis(Vec3 axis, float angle)
		{
			Quat result;
			RotationAxis(ref axis, angle, out result);
			return result;
		}

		/// <summary>
		/// Creates a quaternion given a yaw, pitch, and roll value.
		/// </summary>
		/// <param name="yaw">The yaw of rotation.</param>
		/// <param name="pitch">The pitch of rotation.</param>
		/// <param name="roll">The roll of rotation.</param>
		/// <param name="result">When the method completes, contains the newly created quaternion.</param>
		public static void RotationYawPitchRoll(float yaw, float pitch, float roll, out Quat result)
		{
			float halfRoll = roll * 0.5f;
			float halfPitch = pitch * 0.5f;
			float halfYaw = yaw * 0.5f;

			float sinRoll = (float)Math.Sin(halfRoll);
			float cosRoll = (float)Math.Cos(halfRoll);
			float sinPitch = (float)Math.Sin(halfPitch);
			float cosPitch = (float)Math.Cos(halfPitch);
			float sinYaw = (float)Math.Sin(halfYaw);
			float cosYaw = (float)Math.Cos(halfYaw);

			result.V.X = (cosYaw * sinPitch * cosRoll) + (sinYaw * cosPitch * sinRoll);
			result.V.Y = (sinYaw * cosPitch * cosRoll) - (cosYaw * sinPitch * sinRoll);
			result.V.Z = (cosYaw * cosPitch * sinRoll) - (sinYaw * sinPitch * cosRoll);
			result.W = (cosYaw * cosPitch * cosRoll) + (sinYaw * sinPitch * sinRoll);
		}

		/// <summary>
		/// Creates a quaternion given a yaw, pitch, and roll value.
		/// </summary>
		/// <param name="yaw">The yaw of rotation.</param>
		/// <param name="pitch">The pitch of rotation.</param>
		/// <param name="roll">The roll of rotation.</param>
		/// <returns>The newly created quaternion.</returns>
		public static Quat RotationYawPitchRoll(float yaw, float pitch, float roll)
		{
			Quat result;
			RotationYawPitchRoll(yaw, pitch, roll, out result);
			return result;
		}

		/// <summary>
		/// Interpolates between two quaternions, using spherical linear interpolation.
		/// </summary>
		/// <param name="start">Start quaternion.</param>
		/// <param name="end">End quaternion.</param>
		/// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
		/// <param name="result">When the method completes, contains the spherical linear interpolation of the two quaternions.</param>
		public static void Slerp(ref Quat start, ref Quat end, float amount, out Quat result)
		{
			float opposite;
			float inverse;
			float dot = Dot(start, end);

			if(Math.Abs(dot) > 1.0f - Math.ZeroTolerance)
			{
				inverse = 1.0f - amount;
				opposite = amount * System.Math.Sign(dot);
			}
			else
			{
				float acos = (float)Math.Acos(Math.Abs(dot));
				float invSin = (float)(1.0 / Math.Sin(acos));

				inverse = (float)Math.Sin((1.0f - amount) * acos) * invSin;
				opposite = (float)Math.Sin(amount * acos) * invSin * System.Math.Sign(dot);
			}

			result.V.X = (inverse * start.V.X) + (opposite * end.V.X);
			result.V.Y = (inverse * start.V.Y) + (opposite * end.V.Y);
			result.V.Z = (inverse * start.V.Z) + (opposite * end.V.Z);
			result.W = (inverse * start.W) + (opposite * end.W);
		}

		/// <summary>
		/// Interpolates between two quaternions, using spherical linear interpolation.
		/// </summary>
		/// <param name="start">Start quaternion.</param>
		/// <param name="end">End quaternion.</param>
		/// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
		/// <returns>The spherical linear interpolation of the two quaternions.</returns>
		public static Quat Slerp(Quat start, Quat end, float amount)
		{
			Quat result;
			Slerp(ref start, ref end, amount, out result);
			return result;
		}

		/// <summary>
		/// Interpolates between quaternions, using spherical quadrangle interpolation.
		/// </summary>
		/// <param name="value1">First source quaternion.</param>
		/// <param name="value2">Second source quaternion.</param>
		/// <param name="value3">Thrid source quaternion.</param>
		/// <param name="value4">Fourth source quaternion.</param>
		/// <param name="amount">Value between 0 and 1 indicating the weight of interpolation.</param>
		/// <param name="result">When the method completes, contains the spherical quadrangle interpolation of the quaternions.</param>
		public static void Squad(ref Quat value1, ref Quat value2, ref Quat value3, ref Quat value4, float amount, out Quat result)
		{
			Quat start, end;
			Slerp(ref value1, ref value4, amount, out start);
			Slerp(ref value2, ref value3, amount, out end);
			Slerp(ref start, ref end, 2.0f * amount * (1.0f - amount), out result);
		}

		/// <summary>
		/// Interpolates between quaternions, using spherical quadrangle interpolation.
		/// </summary>
		/// <param name="value1">First source quaternion.</param>
		/// <param name="value2">Second source quaternion.</param>
		/// <param name="value3">Thrid source quaternion.</param>
		/// <param name="value4">Fourth source quaternion.</param>
		/// <param name="amount">Value between 0 and 1 indicating the weight of interpolation.</param>
		/// <returns>The spherical quadrangle interpolation of the quaternions.</returns>
		public static Quat Squad(Quat value1, Quat value2, Quat value3, Quat value4, float amount)
		{
			Quat result;
			Squad(ref value1, ref value2, ref value3, ref value4, amount, out result);
			return result;
		}

		/// <summary>
		/// Sets up control points for spherical quadrangle interpolation.
		/// </summary>
		/// <param name="value1">First source quaternion.</param>
		/// <param name="value2">Second source quaternion.</param>
		/// <param name="value3">Third source quaternion.</param>
		/// <param name="value4">Fourth source quaternion.</param>
		/// <returns>An array of three quaternions that represent control points for spherical quadrangle interpolation.</returns>
		public static Quat[] SquadSetup(Quat value1, Quat value2, Quat value3, Quat value4)
		{
			Quat q0 = (value1 + value2).LengthSquared() < (value1 - value2).LengthSquared() ? -value1 : value1;
			Quat q2 = (value2 + value3).LengthSquared() < (value2 - value3).LengthSquared() ? -value3 : value3;
			Quat q3 = (value3 + value4).LengthSquared() < (value3 - value4).LengthSquared() ? -value4 : value4;
			Quat q1 = value2;

			Quat q1Exp, q2Exp;
			Exponential(ref q1, out q1Exp);
			Exponential(ref q2, out q2Exp);

			Quat[] results = new Quat[3];
			results[0] = q1 * Exponential(-0.25f * (Logarithm(q1Exp * q2) + Logarithm(q1Exp * q0)));
			results[1] = q2 * Exponential(-0.25f * (Logarithm(q2Exp * q3) + Logarithm(q2Exp * q1)));
			results[2] = q2;

			return results;
		}

		/// <summary>
		/// Adds two quaternions.
		/// </summary>
		/// <param name="left">The first quaternion to add.</param>
		/// <param name="right">The second quaternion to add.</param>
		/// <returns>The sum of the two quaternions.</returns>
		public static Quat operator +(Quat left, Quat right)
		{
			Quat result = new Quat();
			result.V.X = left.V.X + right.V.X;
			result.V.Y = left.V.Y + right.V.Y;
			result.V.Z = left.V.Z + right.V.Z;
			result.W = left.W + right.W;
			return result;
		}

		/// <summary>
		/// Subtracts two quaternions.
		/// </summary>
		/// <param name="left">The first quaternion to subtract.</param>
		/// <param name="right">The second quaternion to subtract.</param>
		/// <returns>The difference of the two quaternions.</returns>
		public static Quat operator -(Quat left, Quat right)
		{
			Quat result = new Quat();

			result.V.X = left.V.X - right.V.X;
			result.V.Y = left.V.Y - right.V.Y;
			result.V.Z = left.V.Z - right.V.Z;
			result.W = left.W - right.W;

			return result;
		}

		/// <summary>
		/// Reverses the direction of a given quaternion.
		/// </summary>
		/// <param name="value">The quaternion to negate.</param>
		/// <returns>A quaternion facing in the opposite direction.</returns>
		public static Quat operator -(Quat value)
		{
			Quat result = new Quat();
			result.V.X = -value.V.X;
			result.V.Y = -value.V.Y;
			result.V.Z = -value.V.Z;
			result.W = -value.W;
			return result;
		}

		/// <summary>
		/// Scales a quaternion by the given value.
		/// </summary>
		/// <param name="value">The quaternion to scale.</param>
		/// <param name="scale">The amount by which to scale the quaternion.</param>
		/// <returns>The scaled quaternion.</returns>
		public static Quat operator *(float scale, Quat value)
		{
			return value * scale;
		}

		/// <summary>
		/// Scales a quaternion by the given value.
		/// </summary>
		/// <param name="value">The quaternion to scale.</param>
		/// <param name="scale">The amount by which to scale the quaternion.</param>
		/// <returns>The scaled quaternion.</returns>
		public static Quat operator *(Quat value, float scale)
		{
			Quat result = new Quat();
			result.V.X = value.V.X * scale;
			result.V.Y = value.V.Y * scale;
			result.V.Z = value.V.Z * scale;
			result.W = value.W * scale;
			return result;
		}

		/// <summary>
		/// Multiplies a quaternion by another.
		/// </summary>
		/// <param name="left">The first quaternion to multiply.</param>
		/// <param name="right">The second quaternion to multiply.</param>
		/// <returns>The multiplied quaternion.</returns>
		public static Quat operator *(Quat left, Quat right)
		{
			Quat result = new Quat();
			float lx = left.V.X;
			float ly = left.V.Y;
			float lz = left.V.Z;
			float lw = left.W;
			float rx = right.V.X;
			float ry = right.V.Y;
			float rz = right.V.Z;
			float rw = right.W;

			result.V.X = (rx * lw + lx * rw + ry * lz) - (rz * ly);
			result.V.Y = (ry * lw + ly * rw + rz * lx) - (rx * lz);
			result.V.Z = (rz * lw + lz * rw + rx * ly) - (ry * lx);
			result.W = (rw * lw) - (rx * lx + ry * ly + rz * lz);
			return result;
		}

		/// <summary>
		/// Tests for equality between two objects.
		/// </summary>
		/// <param name="left">The first value to compare.</param>
		/// <param name="right">The second value to compare.</param>
		/// <returns><c>true</c> if <paramref name="left"/> has the same value as <paramref name="right"/>; otherwise, <c>false</c>.</returns>
		public static bool operator ==(Quat left, Quat right)
		{
			return left.Equals(right);
		}

		/// <summary>
		/// Tests for inequality between two objects.
		/// </summary>
		/// <param name="left">The first value to compare.</param>
		/// <param name="right">The second value to compare.</param>
		/// <returns><c>true</c> if <paramref name="left"/> has a different value than <paramref name="right"/>; otherwise, <c>false</c>.</returns>
		public static bool operator !=(Quat left, Quat right)
		{
			return !left.Equals(right);
		}

		/// <summary>
		/// Returns a <see cref="System.String"/> that represents this instance.
		/// </summary>
		/// <returns>
		/// A <see cref="System.String"/> that represents this instance.
		/// </returns>
		public override string ToString()
		{
			return string.Format(CultureInfo.CurrentCulture, "X:{0} Y:{1} Z:{2} W:{3}", V.X, V.Y, V.Z, W);
		}

		/// <summary>
		/// Returns a <see cref="System.String"/> that represents this instance.
		/// </summary>
		/// <param name="format">The format.</param>
		/// <returns>
		/// A <see cref="System.String"/> that represents this instance.
		/// </returns>
		public string ToString(string format)
		{
			if(format == null)
				return ToString();

			return string.Format(CultureInfo.CurrentCulture, "X:{0} Y:{1} Z:{2} W:{3}", V.X.ToString(format, CultureInfo.CurrentCulture),
				V.Y.ToString(format, CultureInfo.CurrentCulture), V.Z.ToString(format, CultureInfo.CurrentCulture), W.ToString(format, CultureInfo.CurrentCulture));
		}

		/// <summary>
		/// Returns a <see cref="System.String"/> that represents this instance.
		/// </summary>
		/// <param name="formatProvider">The format provider.</param>
		/// <returns>
		/// A <see cref="System.String"/> that represents this instance.
		/// </returns>
		public string ToString(IFormatProvider formatProvider)
		{
			return string.Format(formatProvider, "X:{0} Y:{1} Z:{2} W:{3}", V.X, V.Y, V.Z, W);
		}

		/// <summary>
		/// Returns a <see cref="System.String"/> that represents this instance.
		/// </summary>
		/// <param name="format">The format.</param>
		/// <param name="formatProvider">The format provider.</param>
		/// <returns>
		/// A <see cref="System.String"/> that represents this instance.
		/// </returns>
		public string ToString(string format, IFormatProvider formatProvider)
		{
			if(format == null)
				return ToString(formatProvider);

			return string.Format(formatProvider, "X:{0} Y:{1} Z:{2} W:{3}", V.X.ToString(format, formatProvider),
				V.Y.ToString(format, formatProvider), V.Z.ToString(format, formatProvider), W.ToString(format, formatProvider));
		}

		/// <summary>
		/// Returns a hash code for this instance.
		/// </summary>
		/// <returns>
		/// A hash code for this instance, suitable for use in hashing algorithms and data structures like a hash table. 
		/// </returns>
		public override int GetHashCode()
		{
			return V.X.GetHashCode() + V.Y.GetHashCode() + V.Z.GetHashCode() + W.GetHashCode();
		}

		/// <summary>
		/// Determines whether the specified <see cref="CryEngine.Quat"/> is equal to this instance.
		/// </summary>
		/// <param name="other">The <see cref="CryEngine.Quat"/> to compare with this instance.</param>
		/// <returns>
		/// <c>true</c> if the specified <see cref="CryEngine.Quat"/> is equal to this instance; otherwise, <c>false</c>.
		/// </returns>
		public bool Equals(Quat other)
		{
			return (this.V.X == other.V.X) && (this.V.Y == other.V.Y) && (this.V.Z == other.V.Z) && (this.W == other.W);
		}

		/// <summary>
		/// Determines whether the specified <see cref="CryEngine.Quat"/> is equal to this instance.
		/// </summary>
		/// <param name="other">The <see cref="CryEngine.Quat"/> to compare with this instance.</param>
		/// <param name="epsilon">The amount of error allowed.</param>
		/// <returns>
		/// <c>true</c> if the specified <see cref="CryEngine.Quat"/> is equal to this instance; otherwise, <c>false</c>.
		/// </returns>
		public bool Equals(Quat other, float epsilon)
		{
			return ((float)Math.Abs(other.V.X - V.X) < epsilon &&
				(float)Math.Abs(other.V.Y - V.Y) < epsilon &&
				(float)Math.Abs(other.V.Z - V.Z) < epsilon &&
				(float)Math.Abs(other.W - W) < epsilon);
		}

		/// <summary>
		/// Determines whether the specified <see cref="System.Object"/> is equal to this instance.
		/// </summary>
		/// <param name="value">The <see cref="System.Object"/> to compare with this instance.</param>
		/// <returns>
		/// <c>true</c> if the specified <see cref="System.Object"/> is equal to this instance; otherwise, <c>false</c>.
		/// </returns>
		public override bool Equals(object value)
		{
			if(value == null)
				return false;

			if(value.GetType() != GetType())
				return false;

			return Equals((Quat)value);
		}

#if SlimDX1xInterop
        /// <summary>
        /// Performs an implicit conversion from <see cref="CryEngine.Quat"/> to <see cref="SlimDX.Quaternion"/>.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>The result of the conversion.</returns>
        public static implicit operator SlimDX.Quaternion(Quaternion value)
        {
            return new SlimDX.Quaternion(value.X, value.Y, value.Z, value.W);
        }

        /// <summary>
        /// Performs an implicit conversion from <see cref="SlimDX.Quaternion"/> to <see cref="CryEngine.Quat"/>.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>The result of the conversion.</returns>
        public static implicit operator Quaternion(SlimDX.Quaternion value)
        {
            return new Quaternion(value.X, value.Y, value.Z, value.W);
        }
#endif

#if WPFInterop
        /// <summary>
        /// Performs an implicit conversion from <see cref="CryEngine.Quat"/> to <see cref="System.Windows.Media.Media3D.Quaternion"/>.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>The result of the conversion.</returns>
        public static implicit operator System.Windows.Media.Media3D.Quaternion(Quaternion value)
        {
            return new System.Windows.Media.Media3D.Quaternion(value.X, value.Y, value.Z, value.W);
        }

        /// <summary>
        /// Performs an explicit conversion from <see cref="System.Windows.Media.Media3D.Quaternion"/> to <see cref="CryEngine.Quat"/>.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>The result of the conversion.</returns>
        public static explicit operator Quaternion(System.Windows.Media.Media3D.Quaternion value)
        {
            return new Quaternion((float)value.X, (float)value.Y, (float)value.Z, (float)value.W);
        }
#endif

#if XnaInterop
        /// <summary>
        /// Performs an implicit conversion from <see cref="CryEngine.Quat"/> to <see cref="Microsoft.Xna.Framework.Quaternion"/>.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>The result of the conversion.</returns>
        public static implicit operator Microsoft.Xna.Framework.Quaternion(Quaternion value)
        {
            return new Microsoft.Xna.Framework.Quaternion(value.X, value.Y, value.Z, value.W);
        }

        /// <summary>
        /// Performs an implicit conversion from <see cref="Microsoft.Xna.Framework.Quaternion"/> to <see cref="CryEngine.Quat"/>.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>The result of the conversion.</returns>
        public static implicit operator Quaternion(Microsoft.Xna.Framework.Quaternion value)
        {
            return new Quaternion(value.X, value.Y, value.Z, value.W);
        }
#endif
	}
}