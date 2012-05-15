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
	/// Represents a three dimensional mathematical vector.
	/// </summary>
	[Serializable]
	[StructLayout(LayoutKind.Sequential, Pack = 4)]
	public struct Vec3 : IEquatable<Vec3>, IFormattable
	{
		public static readonly Vec3 Zero = new Vec3(0, 0, 0);
		public static readonly Vec3 Up = new Vec3(0, 0, 1);
		public static readonly Vec3 Down = new Vec3(0, 0, -1);

		/// <summary>
		/// The X component of the vector.
		/// </summary>
		public float X;

		/// <summary>
		/// The Y component of the vector.
		/// </summary>
		public float Y;

		/// <summary>
		/// The Z component of the vector.
		/// </summary>
		public float Z;

		/// <summary>
		/// Initializes a new instance of the <see cref="CryEngine.Vec3"/> struct.
		/// </summary>
		/// <param name="value">The value that will be assigned to all components.</param>
		public Vec3(float value)
		{
			X = value;
			Y = value;
			Z = value;
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="CryEngine.Vec3"/> struct.
		/// </summary>
		/// <param name="x">Initial value for the X component of the vector.</param>
		/// <param name="y">Initial value for the Y component of the vector.</param>
		/// <param name="z">Initial value for the Z component of the vector.</param>
		public Vec3(float x, float y, float z)
		{
			X = x;
			Y = y;
			Z = z;
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="CryEngine.Vec3"/> struct.
		/// </summary>
		/// <param name="value">A vector containing the values with which to initialize the X and Y components.</param>
		/// <param name="z">Initial value for the Z component of the vector.</param>
		public Vec3(Vec2 value, float z)
		{
			X = value.X;
			Y = value.Y;
			Z = z;
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="CryEngine.Vec3"/> struct.
		/// </summary>
		/// <param name="values">The values to assign to the X, Y, and Z components of the vector. This must be an array with three elements.</param>
		/// <exception cref="ArgumentNullException">Thrown when <paramref name="values"/> is <c>null</c>.</exception>
		/// <exception cref="ArgumentOutOfRangeException">Thrown when <paramref name="values"/> contains more or less than three elements.</exception>
		public Vec3(float[] values)
		{
			if(values == null)
				throw new ArgumentNullException("values");
			if(values.Length != 3)
				throw new ArgumentOutOfRangeException("values", "There must be three and only three input values for Vector3.");

			X = values[0];
			Y = values[1];
			Z = values[2];
		}

		public Vec3(Quat q)
		{
			Y = (float)Math.Asin(Math.Max(-1.0f, Math.Min(1.0f, -(q.Axis.X * q.Axis.Z - q.Angle * q.Axis.Y) * 2.0f)));
			if(Math.Abs(Math.Abs(Y) - (Math.PI * 0.5)) < 0.01)
			{
				X = 0;
				Z = (float)Math.Atan2(-2 * (q.Axis.X * q.Axis.Y - q.Angle * q.Axis.Z), 1 - (q.Axis.X * q.Axis.X + q.Axis.Z * q.Axis.Z) * 2);
			}
			else
			{
				X = (float)Math.Atan2((q.Axis.Y * q.Axis.Z + q.Angle * q.Axis.X) * 2, 1 - (q.Axis.X * q.Axis.X + q.Axis.Y * q.Axis.Y) * 2);
				Z = (float)Math.Atan2((q.Axis.X * q.Axis.Y + q.Angle * q.Axis.Z) * 2, 1 - (q.Axis.Z * q.Axis.Z + q.Axis.Y * q.Axis.Y) * 2);
			}
		}

		/// <summary>
		/// Gets a value indicting whether this instance is normalized.
		/// </summary>
		public bool IsNormalized
		{
			get { return Math.Abs((X * X) + (Y * Y) + (Z * Z) - 1f) < Math.ZeroTolerance; }
		}

		/// <summary>
		/// Gets or sets the component at the specified index.
		/// </summary>
		/// <value>The value of the X, Y, or Z component, depending on the index.</value>
		/// <param name="index">The index of the component to access. Use 0 for the X component, 1 for the Y component, and 2 for the Z component.</param>
		/// <returns>The value of the component at the specified index.</returns>
		/// <exception cref="System.ArgumentOutOfRangeException">Thrown when the <paramref name="index"/> is out of the range [0, 2].</exception>
		public float this[int index]
		{
			get
			{
				switch(index)
				{
					case 0: return X;
					case 1: return Y;
					case 2: return Z;
				}

				throw new ArgumentOutOfRangeException("index", "Indices for Vector3 run from 0 to 2, inclusive.");
			}

			set
			{
				switch(index)
				{
					case 0: X = value; break;
					case 1: Y = value; break;
					case 2: Z = value; break;
					default: throw new ArgumentOutOfRangeException("index", "Indices for Vector3 run from 0 to 2, inclusive.");
				}
			}
		}

		/// <summary>
		/// Calculates the length of the vector.
		/// </summary>
		/// <returns>The length of the vector.</returns>
		/// <remarks>
		/// <see cref="CryEngine.Vec3.LengthSquared"/> may be preferred when only the relative length is needed
		/// and speed is of the essence.
		/// </remarks>
		public float Length
		{
			get { return (float)Math.Sqrt((X * X) + (Y * Y) + (Z * Z)); }
		}

		/// <summary>
		/// Calculates the squared length of the vector.
		/// </summary>
		/// <returns>The squared length of the vector.</returns>
		/// <remarks>
		/// This method may be preferred to <see cref="CryEngine.Vec3.Length"/> when only a relative length is needed
		/// and speed is of the essence.
		/// </remarks>
		public float LengthSquared()
		{
			return (X * X) + (Y * Y) + (Z * Z);
		}

		/// <summary>
		/// Converts the vector into a unit vector.
		/// </summary>
		public void Normalize()
		{
			float length = Length;
			if(length > Math.ZeroTolerance)
			{
				float inv = 1.0f / length;
				X *= inv;
				Y *= inv;
				Z *= inv;
			}
		}

		public Vec3 Normalized
		{
			get
			{
				var Vec3 = this;
				Vec3.Normalize();

				return Vec3;
			}
		}

		/// <summary>
		/// Creates an array containing the elements of the vector.
		/// </summary>
		/// <returns>A three-element array containing the components of the vector.</returns>
		public float[] ToArray()
		{
			return new float[] { X, Y, Z };
		}

		/// <summary>
		/// Adds two vectors.
		/// </summary>
		/// <param name="left">The first vector to add.</param>
		/// <param name="right">The second vector to add.</param>
		/// <param name="result">When the method completes, contains the sum of the two vectors.</param>
		public static void Add(ref Vec3 left, ref Vec3 right, out Vec3 result)
		{
			result = new Vec3(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
		}

		/// <summary>
		/// Adds two vectors.
		/// </summary>
		/// <param name="left">The first vector to add.</param>
		/// <param name="right">The second vector to add.</param>
		/// <returns>The sum of the two vectors.</returns>
		public static Vec3 Add(Vec3 left, Vec3 right)
		{
			return new Vec3(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
		}

		/// <summary>
		/// Subtracts two vectors.
		/// </summary>
		/// <param name="left">The first vector to subtract.</param>
		/// <param name="right">The second vector to subtract.</param>
		/// <param name="result">When the method completes, contains the difference of the two vectors.</param>
		public static void Subtract(ref Vec3 left, ref Vec3 right, out Vec3 result)
		{
			result = new Vec3(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
		}

		/// <summary>
		/// Subtracts two vectors.
		/// </summary>
		/// <param name="left">The first vector to subtract.</param>
		/// <param name="right">The second vector to subtract.</param>
		/// <returns>The difference of the two vectors.</returns>
		public static Vec3 Subtract(Vec3 left, Vec3 right)
		{
			return new Vec3(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
		}

		/// <summary>
		/// Scales a vector by the given value.
		/// </summary>
		/// <param name="value">The vector to scale.</param>
		/// <param name="scale">The amount by which to scale the vector.</param>
		/// <param name="result">When the method completes, contains the scaled vector.</param>
		public static void Multiply(ref Vec3 value, float scale, out Vec3 result)
		{
			result = new Vec3(value.X * scale, value.Y * scale, value.Z * scale);
		}

		/// <summary>
		/// Scales a vector by the given value.
		/// </summary>
		/// <param name="value">The vector to scale.</param>
		/// <param name="scale">The amount by which to scale the vector.</param>
		/// <returns>The scaled vector.</returns>
		public static Vec3 Multiply(Vec3 value, float scale)
		{
			return new Vec3(value.X * scale, value.Y * scale, value.Z * scale);
		}

		/// <summary>
		/// Modulates a vector with another by performing component-wise multiplication.
		/// </summary>
		/// <param name="left">The first vector to modulate.</param>
		/// <param name="right">The second vector to modulate.</param>
		/// <param name="result">When the method completes, contains the modulated vector.</param>
		public static void Modulate(ref Vec3 left, ref Vec3 right, out Vec3 result)
		{
			result = new Vec3(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
		}

		/// <summary>
		/// Modulates a vector with another by performing component-wise multiplication.
		/// </summary>
		/// <param name="left">The first vector to modulate.</param>
		/// <param name="right">The second vector to modulate.</param>
		/// <returns>The modulated vector.</returns>
		public static Vec3 Modulate(Vec3 left, Vec3 right)
		{
			return new Vec3(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
		}

		/// <summary>
		/// Scales a vector by the given value.
		/// </summary>
		/// <param name="value">The vector to scale.</param>
		/// <param name="scale">The amount by which to scale the vector.</param>
		/// <param name="result">When the method completes, contains the scaled vector.</param>
		public static void Divide(ref Vec3 value, float scale, out Vec3 result)
		{
			result = new Vec3(value.X / scale, value.Y / scale, value.Z / scale);
		}

		/// <summary>
		/// Scales a vector by the given value.
		/// </summary>
		/// <param name="value">The vector to scale.</param>
		/// <param name="scale">The amount by which to scale the vector.</param>
		/// <returns>The scaled vector.</returns>
		public static Vec3 Divide(Vec3 value, float scale)
		{
			return new Vec3(value.X / scale, value.Y / scale, value.Z / scale);
		}

		/// <summary>
		/// Reverses the direction of a given vector.
		/// </summary>
		/// <param name="value">The vector to negate.</param>
		/// <param name="result">When the method completes, contains a vector facing in the opposite direction.</param>
		public static void Negate(ref Vec3 value, out Vec3 result)
		{
			result = new Vec3(-value.X, -value.Y, -value.Z);
		}

		/// <summary>
		/// Reverses the direction of a given vector.
		/// </summary>
		/// <param name="value">The vector to negate.</param>
		/// <returns>A vector facing in the opposite direction.</returns>
		public static Vec3 Negate(Vec3 value)
		{
			return new Vec3(-value.X, -value.Y, -value.Z);
		}

		/// <summary>
		/// Returns a <see cref="CryEngine.Vec3"/> containing the 3D Cartesian coordinates of a point specified in Barycentric coordinates relative to a 3D triangle.
		/// </summary>
		/// <param name="value1">A <see cref="CryEngine.Vec3"/> containing the 3D Cartesian coordinates of vertex 1 of the triangle.</param>
		/// <param name="value2">A <see cref="CryEngine.Vec3"/> containing the 3D Cartesian coordinates of vertex 2 of the triangle.</param>
		/// <param name="value3">A <see cref="CryEngine.Vec3"/> containing the 3D Cartesian coordinates of vertex 3 of the triangle.</param>
		/// <param name="amount1">Barycentric coordinate b2, which expresses the weighting factor toward vertex 2 (specified in <paramref name="value2"/>).</param>
		/// <param name="amount2">Barycentric coordinate b3, which expresses the weighting factor toward vertex 3 (specified in <paramref name="value3"/>).</param>
		/// <param name="result">When the method completes, contains the 3D Cartesian coordinates of the specified point.</param>
		public static void Barycentric(ref Vec3 value1, ref Vec3 value2, ref Vec3 value3, float amount1, float amount2, out Vec3 result)
		{
			result = new Vec3((value1.X + (amount1 * (value2.X - value1.X))) + (amount2 * (value3.X - value1.X)),
				(value1.Y + (amount1 * (value2.Y - value1.Y))) + (amount2 * (value3.Y - value1.Y)),
				(value1.Z + (amount1 * (value2.Z - value1.Z))) + (amount2 * (value3.Z - value1.Z)));
		}

		/// <summary>
		/// Returns a <see cref="CryEngine.Vec3"/> containing the 3D Cartesian coordinates of a point specified in Barycentric coordinates relative to a 3D triangle.
		/// </summary>
		/// <param name="value1">A <see cref="CryEngine.Vec3"/> containing the 3D Cartesian coordinates of vertex 1 of the triangle.</param>
		/// <param name="value2">A <see cref="CryEngine.Vec3"/> containing the 3D Cartesian coordinates of vertex 2 of the triangle.</param>
		/// <param name="value3">A <see cref="CryEngine.Vec3"/> containing the 3D Cartesian coordinates of vertex 3 of the triangle.</param>
		/// <param name="amount1">Barycentric coordinate b2, which expresses the weighting factor toward vertex 2 (specified in <paramref name="value2"/>).</param>
		/// <param name="amount2">Barycentric coordinate b3, which expresses the weighting factor toward vertex 3 (specified in <paramref name="value3"/>).</param>
		/// <returns>A new <see cref="CryEngine.Vec3"/> containing the 3D Cartesian coordinates of the specified point.</returns>
		public static Vec3 Barycentric(Vec3 value1, Vec3 value2, Vec3 value3, float amount1, float amount2)
		{
			Vec3 result;
			Barycentric(ref value1, ref value2, ref value3, amount1, amount2, out result);
			return result;
		}

		/// <summary>
		/// Restricts a value to be within a specified range.
		/// </summary>
		/// <param name="value">The value to clamp.</param>
		/// <param name="min">The minimum value.</param>
		/// <param name="max">The maximum value.</param>
		/// <param name="result">When the method completes, contains the clamped value.</param>
		public static void Clamp(ref Vec3 value, ref Vec3 min, ref Vec3 max, out Vec3 result)
		{
			float x = value.X;
			x = (x > max.X) ? max.X : x;
			x = (x < min.X) ? min.X : x;

			float y = value.Y;
			y = (y > max.Y) ? max.Y : y;
			y = (y < min.Y) ? min.Y : y;

			float z = value.Z;
			z = (z > max.Z) ? max.Z : z;
			z = (z < min.Z) ? min.Z : z;

			result = new Vec3(x, y, z);
		}

		/// <summary>
		/// Restricts a value to be within a specified range.
		/// </summary>
		/// <param name="value">The value to clamp.</param>
		/// <param name="min">The minimum value.</param>
		/// <param name="max">The maximum value.</param>
		/// <returns>The clamped value.</returns>
		public static Vec3 Clamp(Vec3 value, Vec3 min, Vec3 max)
		{
			Vec3 result;
			Clamp(ref value, ref min, ref max, out result);
			return result;
		}

		/// <summary>
		/// Restricts entire vector to be within a specified range.
		/// </summary>
		/// <param name="value">The value to clamp.</param>
		/// <param name="min">The minimum value.</param>
		/// <param name="max">The maximum value.</param>
		/// <returns>The clamped value.</returns>
		public static Vec3 ClampXYZ(Vec3 value, float min, float max)
		{
			return new Vec3(Math.Clamp(value.X, min, max), Math.Clamp(value.Y, min, max), Math.Clamp(value.Z, min, max));
		}

		/// <summary>
		/// Sets all values within vector to a specific minimum value
		/// </summary>
		/// <param name="value">The vector to restrict</param>
		/// <param name="min">The minimum value</param>
		/// <returns>Restricted vector</returns>
		public static Vec3 MinXYZ(Vec3 value, float min)
		{
			return new Vec3(Math.Min(value.X, min), Math.Min(value.Y, min), Math.Min(value.Z, min));
		}

		/// <summary>
		/// Sets all values within vector to a specific maximum value
		/// </summary>
		/// <param name="value">The vector to restrict</param>
		/// <param name="max">The maximum value</param>
		/// <returns>Restricted vector</returns>
		public static Vec3 MaxXYZ(Vec3 value, float max)
		{
			return new Vec3(Math.Max(value.X, max), Math.Max(value.Y, max), Math.Max(value.Z, max));
		}

		/// <summary>
		/// Calculates the cross product of two vectors.
		/// </summary>
		/// <param name="left">First source vector.</param>
		/// <param name="right">Second source vector.</param>
		/// <param name="result">When the method completes, contains he cross product of the two vectors.</param>
		public static void Cross(ref Vec3 left, ref Vec3 right, out Vec3 result)
		{
			result = new Vec3(
				(left.Y * right.Z) - (left.Z * right.Y),
				(left.Z * right.X) - (left.X * right.Z),
				(left.X * right.Y) - (left.Y * right.X));
		}

		/// <summary>
		/// Calculates the cross product of two vectors.
		/// </summary>
		/// <param name="left">First source vector.</param>
		/// <param name="right">Second source vector.</param>
		/// <returns>The cross product of the two vectors.</returns>
		public static Vec3 Cross(Vec3 left, Vec3 right)
		{
			Vec3 result;
			Cross(ref left, ref right, out result);
			return result;
		}

		/// <summary>
		/// Calculates the distance between two vectors.
		/// </summary>
		/// <param name="value1">The first vector.</param>
		/// <param name="value2">The second vector.</param>
		/// <param name="result">When the method completes, contains the distance between the two vectors.</param>
		/// <remarks>
		/// DistanceSquared(ref Vector3, ref Vector3, out float) may be preferred when only the relative distance is needed
		/// and speed is of the essence.
		/// </remarks>
		public static void Distance(ref Vec3 value1, ref Vec3 value2, out float result)
		{
			float x = value1.X - value2.X;
			float y = value1.Y - value2.Y;
			float z = value1.Z - value2.Z;

			result = (float)Math.Sqrt((x * x) + (y * y) + (z * z));
		}

		/// <summary>
		/// Calculates the distance between two vectors.
		/// </summary>
		/// <param name="value1">The first vector.</param>
		/// <param name="value2">The second vector.</param>
		/// <returns>The distance between the two vectors.</returns>
		/// <remarks>
		/// CryEngine.Vec3.DistanceSquared(Vector3, Vector3) may be preferred when only the relative distance is needed
		/// and speed is of the essence.
		/// </remarks>
		public static float Distance(Vec3 value1, Vec3 value2)
		{
			float x = value1.X - value2.X;
			float y = value1.Y - value2.Y;
			float z = value1.Z - value2.Z;

			return (float)Math.Sqrt((x * x) + (y * y) + (z * z));
		}

		/// <summary>
		/// Calculates the squared distance between two vectors.
		/// </summary>
		/// <param name="value1">The first vector.</param>
		/// <param name="value2">The second vector.</param>
		/// <param name="result">When the method completes, contains the squared distance between the two vectors.</param>
		/// <remarks>Distance squared is the value before taking the square root. 
		/// Distance squared can often be used in place of distance if relative comparisons are being made. 
		/// For example, consider three points A, B, and C. To determine whether B or C is further from A, 
		/// compare the distance between A and B to the distance between A and C. Calculating the two distances 
		/// involves two square roots, which are computationally expensive. However, using distance squared 
		/// provides the same information and avoids calculating two square roots.
		/// </remarks>
		public static void DistanceSquared(ref Vec3 value1, ref Vec3 value2, out float result)
		{
			float x = value1.X - value2.X;
			float y = value1.Y - value2.Y;
			float z = value1.Z - value2.Z;

			result = (x * x) + (y * y) + (z * z);
		}

		/// <summary>
		/// Calculates the squared distance between two vectors.
		/// </summary>
		/// <param name="value1">The first vector.</param>
		/// <param name="value2">The second vector.</param>
		/// <returns>The squared distance between the two vectors.</returns>
		/// <remarks>Distance squared is the value before taking the square root. 
		/// Distance squared can often be used in place of distance if relative comparisons are being made. 
		/// For example, consider three points A, B, and C. To determine whether B or C is further from A, 
		/// compare the distance between A and B to the distance between A and C. Calculating the two distances 
		/// involves two square roots, which are computationally expensive. However, using distance squared 
		/// provides the same information and avoids calculating two square roots.
		/// </remarks>
		public static float DistanceSquared(Vec3 value1, Vec3 value2)
		{
			float x = value1.X - value2.X;
			float y = value1.Y - value2.Y;
			float z = value1.Z - value2.Z;

			return (x * x) + (y * y) + (z * z);
		}

		/// <summary>
		/// Calculates the dot product of two vectors.
		/// </summary>
		/// <param name="left">First source vector.</param>
		/// <param name="right">Second source vector.</param>
		/// <param name="result">When the method completes, contains the dot product of the two vectors.</param>
		public static void Dot(ref Vec3 left, ref Vec3 right, out float result)
		{
			result = (left.X * right.X) + (left.Y * right.Y) + (left.Z * right.Z);
		}

		/// <summary>
		/// Calculates the dot product of two vectors.
		/// </summary>
		/// <param name="left">First source vector.</param>
		/// <param name="right">Second source vector.</param>
		/// <returns>The dot product of the two vectors.</returns>
		public static float Dot(Vec3 left, Vec3 right)
		{
			return (left.X * right.X) + (left.Y * right.Y) + (left.Z * right.Z);
		}

		public bool IsEquivalent(Vec3 v1, float epsilon = 0.05f)
		{
			return ((Math.Abs(X - v1.X) <= epsilon) && (Math.Abs(Y - v1.Y) <= epsilon) && (Math.Abs(Z - v1.Z) <= epsilon));
		}

		/// <summary>
		/// Converts the vector into a unit vector.
		/// </summary>
		/// <param name="value">The vector to normalize.</param>
		/// <param name="result">When the method completes, contains the normalized vector.</param>
		public static void Normalize(ref Vec3 value, out Vec3 result)
		{
			result = value;
			result.Normalize();
		}

		/// <summary>
		/// Converts the vector into a unit vector.
		/// </summary>
		/// <param name="value">The vector to normalize.</param>
		/// <returns>The normalized vector.</returns>
		public static Vec3 Normalize(Vec3 value)
		{
			value.Normalize();
			return value;
		}

		/// <summary>
		/// Performs a linear interpolation between two vectors.
		/// </summary>
		/// <param name="start">Start vector.</param>
		/// <param name="end">End vector.</param>
		/// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
		/// <param name="result">When the method completes, contains the linear interpolation of the two vectors.</param>
		/// <remarks>
		/// This method performs the linear interpolation based on the following formula.
		/// <code>start + (end - start) * amount</code>
		/// Passing <paramref name="amount"/> a value of 0 will cause <paramref name="start"/> to be returned; a value of 1 will cause <paramref name="end"/> to be returned. 
		/// </remarks>
		public static void Lerp(ref Vec3 start, ref Vec3 end, float amount, out Vec3 result)
		{
			result.X = start.X + ((end.X - start.X) * amount);
			result.Y = start.Y + ((end.Y - start.Y) * amount);
			result.Z = start.Z + ((end.Z - start.Z) * amount);
		}

		/// <summary>
		/// Performs a linear interpolation between two vectors.
		/// </summary>
		/// <param name="start">Start vector.</param>
		/// <param name="end">End vector.</param>
		/// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
		/// <returns>The linear interpolation of the two vectors.</returns>
		/// <remarks>
		/// This method performs the linear interpolation based on the following formula.
		/// <code>start + (end - start) * amount</code>
		/// Passing <paramref name="amount"/> a value of 0 will cause <paramref name="start"/> to be returned; a value of 1 will cause <paramref name="end"/> to be returned. 
		/// </remarks>
		public static Vec3 Lerp(Vec3 start, Vec3 end, float amount)
		{
			Vec3 result;
			Lerp(ref start, ref end, amount, out result);
			return result;
		}

		/// <summary>
		/// Performs a cubic interpolation between two vectors.
		/// </summary>
		/// <param name="start">Start vector.</param>
		/// <param name="end">End vector.</param>
		/// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
		/// <param name="result">When the method completes, contains the cubic interpolation of the two vectors.</param>
		public static void SmoothStep(ref Vec3 start, ref Vec3 end, float amount, out Vec3 result)
		{
			amount = (amount > 1.0f) ? 1.0f : ((amount < 0.0f) ? 0.0f : amount);
			amount = (amount * amount) * (3.0f - (2.0f * amount));

			result.X = start.X + ((end.X - start.X) * amount);
			result.Y = start.Y + ((end.Y - start.Y) * amount);
			result.Z = start.Z + ((end.Z - start.Z) * amount);
		}

		/// <summary>
		/// Performs a cubic interpolation between two vectors.
		/// </summary>
		/// <param name="start">Start vector.</param>
		/// <param name="end">End vector.</param>
		/// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
		/// <returns>The cubic interpolation of the two vectors.</returns>
		public static Vec3 SmoothStep(Vec3 start, Vec3 end, float amount)
		{
			Vec3 result;
			SmoothStep(ref start, ref end, amount, out result);
			return result;
		}

		/// <summary>
		/// Performs a Hermite spline interpolation.
		/// </summary>
		/// <param name="value1">First source position vector.</param>
		/// <param name="tangent1">First source tangent vector.</param>
		/// <param name="value2">Second source position vector.</param>
		/// <param name="tangent2">Second source tangent vector.</param>
		/// <param name="amount">Weighting factor.</param>
		/// <param name="result">When the method completes, contains the result of the Hermite spline interpolation.</param>
		public static void Hermite(ref Vec3 value1, ref Vec3 tangent1, ref Vec3 value2, ref Vec3 tangent2, float amount, out Vec3 result)
		{
			float squared = amount * amount;
			float cubed = amount * squared;
			float part1 = ((2.0f * cubed) - (3.0f * squared)) + 1.0f;
			float part2 = (-2.0f * cubed) + (3.0f * squared);
			float part3 = (cubed - (2.0f * squared)) + amount;
			float part4 = cubed - squared;

			result.X = (((value1.X * part1) + (value2.X * part2)) + (tangent1.X * part3)) + (tangent2.X * part4);
			result.Y = (((value1.Y * part1) + (value2.Y * part2)) + (tangent1.Y * part3)) + (tangent2.Y * part4);
			result.Z = (((value1.Z * part1) + (value2.Z * part2)) + (tangent1.Z * part3)) + (tangent2.Z * part4);
		}

		/// <summary>
		/// Performs a Hermite spline interpolation.
		/// </summary>
		/// <param name="value1">First source position vector.</param>
		/// <param name="tangent1">First source tangent vector.</param>
		/// <param name="value2">Second source position vector.</param>
		/// <param name="tangent2">Second source tangent vector.</param>
		/// <param name="amount">Weighting factor.</param>
		/// <returns>The result of the Hermite spline interpolation.</returns>
		public static Vec3 Hermite(Vec3 value1, Vec3 tangent1, Vec3 value2, Vec3 tangent2, float amount)
		{
			Vec3 result;
			Hermite(ref value1, ref tangent1, ref value2, ref tangent2, amount, out result);
			return result;
		}

		/// <summary>
		/// Performs a Catmull-Rom interpolation using the specified positions.
		/// </summary>
		/// <param name="value1">The first position in the interpolation.</param>
		/// <param name="value2">The second position in the interpolation.</param>
		/// <param name="value3">The third position in the interpolation.</param>
		/// <param name="value4">The fourth position in the interpolation.</param>
		/// <param name="amount">Weighting factor.</param>
		/// <param name="result">When the method completes, contains the result of the Catmull-Rom interpolation.</param>
		public static void CatmullRom(ref Vec3 value1, ref Vec3 value2, ref Vec3 value3, ref Vec3 value4, float amount, out Vec3 result)
		{
			float squared = amount * amount;
			float cubed = amount * squared;

			result.X = 0.5f * ((((2.0f * value2.X) + ((-value1.X + value3.X) * amount)) +
			(((((2.0f * value1.X) - (5.0f * value2.X)) + (4.0f * value3.X)) - value4.X) * squared)) +
			((((-value1.X + (3.0f * value2.X)) - (3.0f * value3.X)) + value4.X) * cubed));

			result.Y = 0.5f * ((((2.0f * value2.Y) + ((-value1.Y + value3.Y) * amount)) +
				(((((2.0f * value1.Y) - (5.0f * value2.Y)) + (4.0f * value3.Y)) - value4.Y) * squared)) +
				((((-value1.Y + (3.0f * value2.Y)) - (3.0f * value3.Y)) + value4.Y) * cubed));

			result.Z = 0.5f * ((((2.0f * value2.Z) + ((-value1.Z + value3.Z) * amount)) +
				(((((2.0f * value1.Z) - (5.0f * value2.Z)) + (4.0f * value3.Z)) - value4.Z) * squared)) +
				((((-value1.Z + (3.0f * value2.Z)) - (3.0f * value3.Z)) + value4.Z) * cubed));
		}

		/// <summary>
		/// Performs a Catmull-Rom interpolation using the specified positions.
		/// </summary>
		/// <param name="value1">The first position in the interpolation.</param>
		/// <param name="value2">The second position in the interpolation.</param>
		/// <param name="value3">The third position in the interpolation.</param>
		/// <param name="value4">The fourth position in the interpolation.</param>
		/// <param name="amount">Weighting factor.</param>
		/// <returns>A vector that is the result of the Catmull-Rom interpolation.</returns>
		public static Vec3 CatmullRom(Vec3 value1, Vec3 value2, Vec3 value3, Vec3 value4, float amount)
		{
			Vec3 result;
			CatmullRom(ref value1, ref value2, ref value3, ref value4, amount, out result);
			return result;
		}

		/// <summary>
		/// Returns a vector containing the smallest components of the specified vectors.
		/// </summary>
		/// <param name="left">The first source vector.</param>
		/// <param name="right">The second source vector.</param>
		/// <param name="result">When the method completes, contains an new vector composed of the largest components of the source vectors.</param>
		public static void Max(ref Vec3 left, ref Vec3 right, out Vec3 result)
		{
			result.X = (left.X > right.X) ? left.X : right.X;
			result.Y = (left.Y > right.Y) ? left.Y : right.Y;
			result.Z = (left.Z > right.Z) ? left.Z : right.Z;
		}

		/// <summary>
		/// Returns a vector containing the largest components of the specified vectors.
		/// </summary>
		/// <param name="left">The first source vector.</param>
		/// <param name="right">The second source vector.</param>
		/// <returns>A vector containing the largest components of the source vectors.</returns>
		public static Vec3 Max(Vec3 left, Vec3 right)
		{
			Vec3 result;
			Max(ref left, ref right, out result);
			return result;
		}

		/// <summary>
		/// Returns a vector containing the smallest components of the specified vectors.
		/// </summary>
		/// <param name="left">The first source vector.</param>
		/// <param name="right">The second source vector.</param>
		/// <param name="result">When the method completes, contains an new vector composed of the smallest components of the source vectors.</param>
		public static void Min(ref Vec3 left, ref Vec3 right, out Vec3 result)
		{
			result.X = (left.X < right.X) ? left.X : right.X;
			result.Y = (left.Y < right.Y) ? left.Y : right.Y;
			result.Z = (left.Z < right.Z) ? left.Z : right.Z;
		}

		/// <summary>
		/// Returns a vector containing the smallest components of the specified vectors.
		/// </summary>
		/// <param name="left">The first source vector.</param>
		/// <param name="right">The second source vector.</param>
		/// <returns>A vector containing the smallest components of the source vectors.</returns>
		public static Vec3 Min(Vec3 left, Vec3 right)
		{
			Vec3 result;
			Min(ref left, ref right, out result);
			return result;
		}

		/// <summary>
		/// Returns the reflection of a vector off a surface that has the specified normal. 
		/// </summary>
		/// <param name="vector">The source vector.</param>
		/// <param name="normal">Normal of the surface.</param>
		/// <param name="result">When the method completes, contains the reflected vector.</param>
		/// <remarks>Reflect only gives the direction of a reflection off a surface, it does not determine 
		/// whether the original vector was close enough to the surface to hit it.</remarks>
		public static void Reflect(ref Vec3 vector, ref Vec3 normal, out Vec3 result)
		{
			float dot = (vector.X * normal.X) + (vector.Y * normal.Y) + (vector.Z * normal.Z);

			result.X = vector.X - ((2.0f * dot) * normal.X);
			result.Y = vector.Y - ((2.0f * dot) * normal.Y);
			result.Z = vector.Z - ((2.0f * dot) * normal.Z);
		}

		/// <summary>
		/// Returns the reflection of a vector off a surface that has the specified normal. 
		/// </summary>
		/// <param name="vector">The source vector.</param>
		/// <param name="normal">Normal of the surface.</param>
		/// <returns>The reflected vector.</returns>
		/// <remarks>Reflect only gives the direction of a reflection off a surface, it does not determine 
		/// whether the original vector was close enough to the surface to hit it.</remarks>
		public static Vec3 Reflect(Vec3 vector, Vec3 normal)
		{
			Vec3 result;
			Reflect(ref vector, ref normal, out result);
			return result;
		}

		/// <summary>
		/// Returns the fraction of a vector off a surface that has the specified normal and index.
		/// </summary>
		/// <param name="vector">The source vector.</param>
		/// <param name="normal">Normal of the surface.</param>
		/// <param name="index">Index of refraction.</param>
		/// <param name="result">When the method completes, contains the refracted vector.</param>
		public static void Refract(ref Vec3 vector, ref Vec3 normal, float index, out Vec3 result)
		{
			float cos1;
			Dot(ref vector, ref normal, out cos1);

			float radicand = 1.0f - (index * index) * (1.0f - (cos1 * cos1));

			if(radicand < 0.0f)
				result = Vec3.Zero;
			else
			{
				float cos2 = (float)Math.Sqrt(radicand);
				result = (index * vector) + ((cos2 - index * cos1) * normal);
			}
		}

		/// <summary>
		/// Returns the fraction of a vector off a surface that has the specified normal and index.
		/// </summary>
		/// <param name="vector">The source vector.</param>
		/// <param name="normal">Normal of the surface.</param>
		/// <param name="index">Index of refraction.</param>
		/// <returns>The refracted vector.</returns>
		public static Vec3 Refract(Vec3 vector, Vec3 normal, float index)
		{
			Vec3 result;
			Refract(ref vector, ref normal, index, out result);
			return result;
		}

		/// <summary>
		/// Orthogonalizes a list of vectors.
		/// </summary>
		/// <param name="destination">The list of orthogonalized vectors.</param>
		/// <param name="source">The list of vectors to orthogonalize.</param>
		/// <remarks>
		/// <para>Orthogonalization is the process of making all vectors orthogonal to each other. This
		/// means that any given vector in the list will be orthogonal to any other given vector in the
		/// list.</para>
		/// <para>Because this method uses the modified Gram-Schmidt process, the resulting vectors
		/// tend to be numerically unstable. The numeric stability decreases according to the vectors
		/// position in the list so that the first vector is the most stable and the last vector is the
		/// least stable.</para>
		/// </remarks>
		/// <exception cref="ArgumentNullException">Thrown when <paramref name="source"/> or <paramref name="destination"/> is <c>null</c>.</exception>
		/// <exception cref="ArgumentOutOfRangeException">Thrown when <paramref name="destination"/> is shorter in length than <paramref name="source"/>.</exception>
		public static void Orthogonalize(Vec3[] destination, params Vec3[] source)
		{
			//Uses the modified Gram-Schmidt process.
			//q1 = m1
			//q2 = m2 - ((q1 ⋅ m2) / (q1 ⋅ q1)) * q1
			//q3 = m3 - ((q1 ⋅ m3) / (q1 ⋅ q1)) * q1 - ((q2 ⋅ m3) / (q2 ⋅ q2)) * q2
			//q4 = m4 - ((q1 ⋅ m4) / (q1 ⋅ q1)) * q1 - ((q2 ⋅ m4) / (q2 ⋅ q2)) * q2 - ((q3 ⋅ m4) / (q3 ⋅ q3)) * q3
			//q5 = ...

			if(source == null)
				throw new ArgumentNullException("source");
			if(destination == null)
				throw new ArgumentNullException("destination");
			if(destination.Length < source.Length)
				throw new ArgumentOutOfRangeException("destination", "The destination array must be of same length or larger length than the source array.");

			for(int i = 0; i < source.Length; ++i)
			{
				Vec3 newvector = source[i];

				for(int r = 0; r < i; ++r)
				{
					newvector -= (Vec3.Dot(destination[r], newvector) / Vec3.Dot(destination[r], destination[r])) * destination[r];
				}

				destination[i] = newvector;
			}
		}

		/// <summary>
		/// Orthonormalizes a list of vectors.
		/// </summary>
		/// <param name="destination">The list of orthonormalized vectors.</param>
		/// <param name="source">The list of vectors to orthonormalize.</param>
		/// <remarks>
		/// <para>Orthonormalization is the process of making all vectors orthogonal to each
		/// other and making all vectors of unit length. This means that any given vector will
		/// be orthogonal to any other given vector in the list.</para>
		/// <para>Because this method uses the modified Gram-Schmidt process, the resulting vectors
		/// tend to be numerically unstable. The numeric stability decreases according to the vectors
		/// position in the list so that the first vector is the most stable and the last vector is the
		/// least stable.</para>
		/// </remarks>
		/// <exception cref="ArgumentNullException">Thrown when <paramref name="source"/> or <paramref name="destination"/> is <c>null</c>.</exception>
		/// <exception cref="ArgumentOutOfRangeException">Thrown when <paramref name="destination"/> is shorter in length than <paramref name="source"/>.</exception>
		public static void Orthonormalize(Vec3[] destination, params Vec3[] source)
		{
			//Uses the modified Gram-Schmidt process.
			//Because we are making unit vectors, we can optimize the math for orthogonalization
			//and simplify the projection operation to remove the division.
			//q1 = m1 / |m1|
			//q2 = (m2 - (q1 ⋅ m2) * q1) / |m2 - (q1 ⋅ m2) * q1|
			//q3 = (m3 - (q1 ⋅ m3) * q1 - (q2 ⋅ m3) * q2) / |m3 - (q1 ⋅ m3) * q1 - (q2 ⋅ m3) * q2|
			//q4 = (m4 - (q1 ⋅ m4) * q1 - (q2 ⋅ m4) * q2 - (q3 ⋅ m4) * q3) / |m4 - (q1 ⋅ m4) * q1 - (q2 ⋅ m4) * q2 - (q3 ⋅ m4) * q3|
			//q5 = ...

			if(source == null)
				throw new ArgumentNullException("source");
			if(destination == null)
				throw new ArgumentNullException("destination");
			if(destination.Length < source.Length)
				throw new ArgumentOutOfRangeException("destination", "The destination array must be of same length or larger length than the source array.");

			for(int i = 0; i < source.Length; ++i)
			{
				Vec3 newvector = source[i];

				for(int r = 0; r < i; ++r)
				{
					newvector -= Vec3.Dot(destination[r], newvector) * destination[r];
				}

				newvector.Normalize();
				destination[i] = newvector;
			}
		}

		/// <summary>
		/// Transforms a 3D vector by the given <see cref="CryEngine.Quat"/> rotation.
		/// </summary>
		/// <param name="vector">The vector to rotate.</param>
		/// <param name="rotation">The <see cref="CryEngine.Quat"/> rotation to apply.</param>
		/// <param name="result">When the method completes, contains the transformed <see cref="CryEngine.Vec4"/>.</param>
		public static void Transform(ref Vec3 vector, ref Quat rotation, out Vec3 result)
		{
			float x = rotation.V.X + rotation.V.X;
			float y = rotation.V.Y + rotation.V.Y;
			float z = rotation.V.Z + rotation.V.Z;
			float wx = rotation.W * x;
			float wy = rotation.W * y;
			float wz = rotation.W * z;
			float xx = rotation.V.X * x;
			float xy = rotation.V.X * y;
			float xz = rotation.V.X * z;
			float yy = rotation.V.Y * y;
			float yz = rotation.V.Y * z;
			float zz = rotation.V.Z * z;

			result = new Vec3(
				((vector.X * ((1.0f - yy) - zz)) + (vector.Y * (xy - wz))) + (vector.Z * (xz + wy)),
				((vector.X * (xy + wz)) + (vector.Y * ((1.0f - xx) - zz))) + (vector.Z * (yz - wx)),
				((vector.X * (xz - wy)) + (vector.Y * (yz + wx))) + (vector.Z * ((1.0f - xx) - yy)));
		}

		/// <summary>
		/// Transforms a 3D vector by the given <see cref="CryEngine.Quat"/> rotation.
		/// </summary>
		/// <param name="vector">The vector to rotate.</param>
		/// <param name="rotation">The <see cref="CryEngine.Quat"/> rotation to apply.</param>
		/// <returns>The transformed <see cref="CryEngine.Vec4"/>.</returns>
		public static Vec3 Transform(Vec3 vector, Quat rotation)
		{
			Vec3 result;
			Transform(ref vector, ref rotation, out result);
			return result;
		}

		/// <summary>
		/// Transforms an array of vectors by the given <see cref="CryEngine.Quat"/> rotation.
		/// </summary>
		/// <param name="source">The array of vectors to transform.</param>
		/// <param name="rotation">The <see cref="CryEngine.Quat"/> rotation to apply.</param>
		/// <param name="destination">The array for which the transformed vectors are stored.
		/// This array may be the same array as <paramref name="source"/>.</param>
		/// <exception cref="ArgumentNullException">Thrown when <paramref name="source"/> or <paramref name="destination"/> is <c>null</c>.</exception>
		/// <exception cref="ArgumentOutOfRangeException">Thrown when <paramref name="destination"/> is shorter in length than <paramref name="source"/>.</exception>
		public static void Transform(Vec3[] source, ref Quat rotation, Vec3[] destination)
		{
			if(source == null)
				throw new ArgumentNullException("source");
			if(destination == null)
				throw new ArgumentNullException("destination");
			if(destination.Length < source.Length)
				throw new ArgumentOutOfRangeException("destination", "The destination array must be of same length or larger length than the source array.");

			float x = rotation.V.X + rotation.V.X;
			float y = rotation.V.Y + rotation.V.Y;
			float z = rotation.V.Z + rotation.V.Z;
			float wx = rotation.W * x;
			float wy = rotation.W * y;
			float wz = rotation.W * z;
			float xx = rotation.V.X * x;
			float xy = rotation.V.X * y;
			float xz = rotation.V.X * z;
			float yy = rotation.V.Y * y;
			float yz = rotation.V.Y * z;
			float zz = rotation.V.Z * z;

			float num1 = ((1.0f - yy) - zz);
			float num2 = (xy - wz);
			float num3 = (xz + wy);
			float num4 = (xy + wz);
			float num5 = ((1.0f - xx) - zz);
			float num6 = (yz - wx);
			float num7 = (xz - wy);
			float num8 = (yz + wx);
			float num9 = ((1.0f - xx) - yy);

			for(int i = 0; i < source.Length; ++i)
			{
				destination[i] = new Vec3(
					((source[i].X * num1) + (source[i].Y * num2)) + (source[i].Z * num3),
					((source[i].X * num4) + (source[i].Y * num5)) + (source[i].Z * num6),
					((source[i].X * num7) + (source[i].Y * num8)) + (source[i].Z * num9));
			}
		}

		/// <summary>
		/// Adds two vectors.
		/// </summary>
		/// <param name="left">The first vector to add.</param>
		/// <param name="right">The second vector to add.</param>
		/// <returns>The sum of the two vectors.</returns>
		public static Vec3 operator +(Vec3 left, Vec3 right)
		{
			return new Vec3(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
		}

		/// <summary>
		/// Assert a vector (return it unchanged).
		/// </summary>
		/// <param name="value">The vector to assert (unchange).</param>
		/// <returns>The asserted (unchanged) vector.</returns>
		public static Vec3 operator +(Vec3 value)
		{
			return value;
		}

		/// <summary>
		/// Subtracts two vectors.
		/// </summary>
		/// <param name="left">The first vector to subtract.</param>
		/// <param name="right">The second vector to subtract.</param>
		/// <returns>The difference of the two vectors.</returns>
		public static Vec3 operator -(Vec3 left, Vec3 right)
		{
			return new Vec3(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
		}

		/// <summary>
		/// Reverses the direction of a given vector.
		/// </summary>
		/// <param name="value">The vector to negate.</param>
		/// <returns>A vector facing in the opposite direction.</returns>
		public static Vec3 operator -(Vec3 value)
		{
			return new Vec3(-value.X, -value.Y, -value.Z);
		}

		/// <summary>
		/// Scales a vector by the given value.
		/// </summary>
		/// <param name="value">The vector to scale.</param>
		/// <param name="scale">The amount by which to scale the vector.</param>
		/// <returns>The scaled vector.</returns>
		public static Vec3 operator *(float scale, Vec3 value)
		{
			return new Vec3(value.X * scale, value.Y * scale, value.Z * scale);
		}

		/// <summary>
		/// Scales a vector by the given value.
		/// </summary>
		/// <param name="value">The vector to scale.</param>
		/// <param name="scale">The amount by which to scale the vector.</param>
		/// <returns>The scaled vector.</returns>
		public static Vec3 operator *(Vec3 value, float scale)
		{
			return new Vec3(value.X * scale, value.Y * scale, value.Z * scale);
		}

		/// <summary>
		/// Scales a vector by the given value.
		/// </summary>
		/// <param name="value">The vector to scale.</param>
		/// <param name="scale">The amount by which to scale the vector.</param>
		/// <returns>The scaled vector.</returns>
		public static Vec3 operator /(Vec3 value, float scale)
		{
			return new Vec3(value.X / scale, value.Y / scale, value.Z / scale);
		}

		/// <summary>
		/// Tests for equality between two objects.
		/// </summary>
		/// <param name="left">The first value to compare.</param>
		/// <param name="right">The second value to compare.</param>
		/// <returns><c>true</c> if <paramref name="left"/> has the same value as <paramref name="right"/>; otherwise, <c>false</c>.</returns>
		public static bool operator ==(Vec3 left, Vec3 right)
		{
			return left.Equals(right);
		}

		/// <summary>
		/// Tests for inequality between two objects.
		/// </summary>
		/// <param name="left">The first value to compare.</param>
		/// <param name="right">The second value to compare.</param>
		/// <returns><c>true</c> if <paramref name="left"/> has a different value than <paramref name="right"/>; otherwise, <c>false</c>.</returns>
		public static bool operator !=(Vec3 left, Vec3 right)
		{
			return !left.Equals(right);
		}

		public static Vec3 operator %(Vec3 v0, Vec3 v1)
		{
			return Cross(v0, v1);
		}

		public static float operator |(Vec3 v0, Vec3 v1)
		{
			return Dot(v0, v1);
		}

		/// <summary>
		/// Performs an explicit conversion from <see cref="CryEngine.Vec3"/> to <see cref="CryEngine.Vec2"/>.
		/// </summary>
		/// <param name="value">The value.</param>
		/// <returns>The result of the conversion.</returns>
		public static explicit operator Vec2(Vec3 value)
		{
			return new Vec2(value.X, value.Y);
		}

		/// <summary>
		/// Performs an explicit conversion from <see cref="CryEngine.Vec3"/> to <see cref="CryEngine.Vec4"/>.
		/// </summary>
		/// <param name="value">The value.</param>
		/// <returns>The result of the conversion.</returns>
		public static explicit operator Vec4(Vec3 value)
		{
			return new Vec4(value, 0.0f);
		}

		public static explicit operator Vec3(Quat q)
		{
			return new Vec3(q);
		}

		/// <summary>
		/// Returns a <see cref="System.String"/> that represents this instance.
		/// </summary>
		/// <returns>
		/// A <see cref="System.String"/> that represents this instance.
		/// </returns>
		public override string ToString()
		{
			return string.Format(CultureInfo.CurrentCulture, "X:{0} Y:{1} Z:{2}", X, Y, Z);
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

			return string.Format(CultureInfo.CurrentCulture, "X:{0} Y:{1} Z:{2}", X.ToString(format, CultureInfo.CurrentCulture),
				Y.ToString(format, CultureInfo.CurrentCulture), Z.ToString(format, CultureInfo.CurrentCulture));
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
			return string.Format(formatProvider, "X:{0} Y:{1} Z:{2}", X, Y, Z);
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

			return string.Format(formatProvider, "X:{0} Y:{1} Z:{2}", X.ToString(format, formatProvider),
				Y.ToString(format, formatProvider), Z.ToString(format, formatProvider));
		}

		/// <summary>
		/// Returns a hash code for this instance.
		/// </summary>
		/// <returns>
		/// A hash code for this instance, suitable for use in hashing algorithms and data structures like a hash table. 
		/// </returns>
		public override int GetHashCode()
		{
			return X.GetHashCode() + Y.GetHashCode() + Z.GetHashCode();
		}

		/// <summary>
		/// Determines whether the specified <see cref="CryEngine.Vec3"/> is equal to this instance.
		/// </summary>
		/// <param name="other">The <see cref="CryEngine.Vec3"/> to compare with this instance.</param>
		/// <returns>
		/// <c>true</c> if the specified <see cref="CryEngine.Vec3"/> is equal to this instance; otherwise, <c>false</c>.
		/// </returns>
		public bool Equals(Vec3 other)
		{
			return (this.X == other.X) && (this.Y == other.Y) && (this.Z == other.Z);
		}

		/// <summary>
		/// Determines whether the specified <see cref="CryEngine.Vec3"/> is equal to this instance.
		/// </summary>
		/// <param name="other">The <see cref="CryEngine.Vec3"/> to compare with this instance.</param>
		/// <param name="epsilon">The amount of error allowed.</param>
		/// <returns>
		/// <c>true</c> if the specified <see cref="CryEngine.Vec3"/> is equal to this instance; otherwise, <c>false</c>.
		/// </returns>
		public bool Equals(Vec3 other, float epsilon)
		{
			return ((float)Math.Abs(other.X - X) < epsilon &&
				(float)Math.Abs(other.Y - Y) < epsilon &&
				(float)Math.Abs(other.Z - Z) < epsilon);
		}

		/// <summary>
		/// Determines whether the specified <see cref="System.Object"/> is equal to this instance.
		/// </summary>
		/// <param name="value">The <see cref="System.Object"/> to compare with this instance.</param>
		/// <returns>
		/// 	<c>true</c> if the specified <see cref="System.Object"/> is equal to this instance; otherwise, <c>false</c>.
		/// </returns>
		public override bool Equals(object value)
		{
			if(value == null)
				return false;

			if(value.GetType() != GetType())
				return false;

			return Equals((Vec3)value);
		}

#if SlimDX1xInterop
        /// <summary>
        /// Performs an implicit conversion from <see cref="CryEngine.Vec3"/> to <see cref="SlimDX.Vector3"/>.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>The result of the conversion.</returns>
        public static implicit operator SlimDX.Vector3(Vector3 value)
        {
            return new SlimDX.Vector3(value.X, value.Y, value.Z);
        }

        /// <summary>
        /// Performs an implicit conversion from <see cref="SlimDX.Vector3"/> to <see cref="CryEngine.Vec3"/>.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>The result of the conversion.</returns>
        public static implicit operator Vector3(SlimDX.Vector3 value)
        {
            return new Vector3(value.X, value.Y, value.Z);
        }
#endif

#if WPFInterop
        /// <summary>
        /// Performs an implicit conversion from <see cref="CryEngine.Vec3"/> to <see cref="System.Windows.Media.Media3D.Vector3D"/>.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>The result of the conversion.</returns>
        public static implicit operator System.Windows.Media.Media3D.Vector3D(Vector3 value)
        {
            return new System.Windows.Media.Media3D.Vector3D(value.X, value.Y, value.Z);
        }

        /// <summary>
        /// Performs an explicit conversion from <see cref="System.Windows.Media.Media3D.Vector3D"/> to <see cref="CryEngine.Vec3"/>.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>The result of the conversion.</returns>
        public static explicit operator Vector3(System.Windows.Media.Media3D.Vector3D value)
        {
            return new Vector3((float)value.X, (float)value.Y, (float)value.Z);
        }
#endif

#if XnaInterop
        /// <summary>
        /// Performs an implicit conversion from <see cref="CryEngine.Vec3"/> to <see cref="Microsoft.Xna.Framework.Vector3"/>.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>The result of the conversion.</returns>
        public static implicit operator Microsoft.Xna.Framework.Vector3(Vector3 value)
        {
            return new Microsoft.Xna.Framework.Vector3(value.X, value.Y, value.Z);
        }

        /// <summary>
        /// Performs an implicit conversion from <see cref="Microsoft.Xna.Framework.Vector3"/> to <see cref="CryEngine.Vec3"/>.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>The result of the conversion.</returns>
        public static implicit operator Vector3(Microsoft.Xna.Framework.Vector3 value)
        {
            return new Vector3(value.X, value.Y, value.Z);
        }
#endif
	}
}