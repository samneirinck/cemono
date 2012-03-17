using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
	public struct Matrix33
	{
		public float M00 { get; set; }
		public float M01 { get; set; }
		public float M02 { get; set; }

		public float M10 { get; set; }
		public float M11 { get; set; }
		public float M12 { get; set; }

		public float M20 { get; set; }
		public float M21 { get; set; }
		public float M22 { get; set; }

		public void SetIdentity()
		{
			M00 = 1; M01 = 0; M02 = 0;
			M10 = 0; M11 = 1; M12 = 0;
			M20 = 0; M21 = 0; M22 = 1; 
		}

		public static Matrix33 CreateIdentity()
		{
			Matrix33 matrix = new Matrix33();
			matrix.SetIdentity();

			return matrix;
		}

		public void SetRotationAA(float angle, Vec3 axis)
		{
			double s, c; Math.SinCos(angle, out s, out c); float mc = 1.0f - (float)c;
			float mcx = mc * axis.X; float mcy = mc * axis.Y; float mcz = mc * axis.Z;
			float tcx = axis.X * (float)s; float tcy = axis.Y * (float)s; float tcz = axis.Z * (float)s;
			M00 = mcx * axis.X + (float)c; M01 = mcx * axis.Y - tcz; M02 = mcx * axis.Z + tcy;
			M10 = mcy * axis.X + tcz; M11 = mcy * axis.Y + (float)c; M12 = mcy * axis.Z - tcx;
			M20 = mcz * axis.X - tcy; M21 = mcz * axis.Y + tcx; M22 = mcz * axis.Z + (float)c;
		}

		public static Matrix33 CreateRotationAA(float rad, Vec3 axis)
		{
			Matrix33 matrix = new Matrix33();
			matrix.SetRotationAA(rad, axis);

			return matrix;
		}

		public void SetRotationAA(float c, float s, Vec3 axis)
		{
			float mc = 1 - c;
			M00 = mc * axis.X * axis.X + c; M01 = mc * axis.X * axis.Y - axis.Z * s; M02 = mc * axis.X * axis.Z + axis.Y * s;
			M10 = mc * axis.Y * axis.X + axis.Z * s; M11 = mc * axis.Y * axis.Y + c; M12 = mc * axis.Y * axis.Z - axis.X * s;
			M20 = mc * axis.Z * axis.X - axis.Y * s; M21 = mc * axis.Z * axis.Y + axis.X * s; M22 = mc * axis.Z * axis.Z + c;
		}

		public static Matrix33 CreateRotationAA(float c, float s, Vec3 axis)
		{
			Matrix33 matrix = new Matrix33();
			matrix.SetRotationAA(c, s, axis);

			return matrix;
		}

		public void SetRotationAA(Vec3 rot)
		{
			float angle = rot.Length;
			if(angle == 0)
				SetIdentity();
			else
				SetRotationAA(angle, rot / angle);
		}

		public static Matrix33 CreateRotationAA(Vec3 rot)
		{
			Matrix33 matrix = new Matrix33();
			matrix.SetRotationAA(rot);

			return matrix;
		}

		public void SetRotationX(float rad)
		{
			double s, c; Math.SinCos(rad, out s, out c);
			M00 = 1.0f; M01 = 0.0f; M02 = 0.0f;
			M10 = 0.0f; M11 = (float)c; M12 = (float)-s;
			M20 = 0.0f; M21 = (float)s; M22 = (float)c;
		}

		public static Matrix33 CreateRotationX(float rad)
		{
			Matrix33 matrix = new Matrix33();
			matrix.SetRotationX(rad);

			return matrix;
		}

		public void SetRotationY(float rad)
		{
			double s, c; Math.SinCos(rad, out s, out c);
			M00 = (float)c; M01 = 0; M02 = (float)s;
			M10 = 0; M11 = 1; M12 = 0;
			M20 = (float)-s; M21 = 0; M22 = (float)c;	
		}

		public static Matrix33 CreateRotationY(float rad)
		{
			Matrix33 matrix = new Matrix33();
			matrix.SetRotationY(rad);

			return matrix;
		}

		public void SetRotationZ(float rad)
		{
			double s, c; Math.SinCos(rad, out s, out c);
			M00 = (float)c; M01 = (float)-s; M02 = 0.0f;
			M10 = (float)s; M11 = (float)c; M12 = 0.0f;
			M20 = 0.0f; M21 = 0.0f; M22 = 1.0f;
		}

		public static Matrix33 CreateRotationZ(float rad)
		{
			Matrix33 matrix = new Matrix33();
			matrix.SetRotationZ(rad);

			return matrix;
		}

		public void SetRotationXYZ(Vec3 rad)
		{
			double sx, cx; Math.SinCos(rad.X, out sx, out cx);
			double sy, cy; Math.SinCos(rad.Y, out sy, out cy);
			double sz, cz; Math.SinCos(rad.Z, out sz, out cz);
			double sycz = (sy * cz), sysz = (sy * sz);
			M00 = (float)(cy * cz); M01 = (float)(sycz * sx - cx * sz); M02 = (float)(sycz * cx + sx * sz);
			M10 = (float)(cy * sz); M11 = (float)(sysz * sx + cx * cz); M12 = (float)(sysz * cx - sx * cz);
			M20 = (float)(-sy); M21 = (float)(cy * sx); M22 = (float)(cy * cx);			
		}

		public static Matrix33 CreateRotationXYZ(Vec3 rad)
		{
			Matrix33 matrix = new Matrix33();
			matrix.SetRotationXYZ(rad);

			return matrix;
		}

		public void SetScale(Vec3 s)
		{
			M00 = s.X; M01 = 0; M02 = 0;
			M10 = 0; M11 = s.Y; M12 = 0;
			M20 = 0; M21 = 0; M22 = s.Z;
		}

		public static Matrix33 CreateScale(Vec3 s)
		{
			Matrix33 matrix = new Matrix33();
			matrix.SetScale(s);

			return matrix;
		}
	}

	public struct Matrix34
	{
		public float M00 { get; set; }
		public float M01 { get; set; }
		public float M02 { get; set; }
		public float M03 { get; set; }

		public float M10 { get; set; }
		public float M11 { get; set; }
		public float M12 { get; set; }
		public float M13 { get; set; }

		public float M20 { get; set; }
		public float M21 { get; set; }
		public float M22 { get; set; }
		public float M23 { get; set; }

		#region Constructors
		public Matrix34(float v00, float v01, float v02, float v03, float v10, float v11, float v12, float v13, float v20, float v21, float v22, float v23)
			: this()
		{
			M00 = v00; M01 = v01; M02 = v02; M03 = v03; 
			M10 = v10; M11 = v11; M12 = v12; M13 = v13;
			M20 = v20; M21 = v21; M22 = v22; M23 = v23;
		}

		public Matrix34(Vec3 s, Quat q, Vec3 t = default(Vec3))
			: this()
		{
			Set(s, q, t);
		}

		public Matrix34(Matrix33 m33)
			: this()
		{

		}
		#endregion

		#region Methods
		/// <summary>
		/// apply scaling to the columns of the matrix.
		/// </summary>
		/// <param name="s"></param>
		void ScaleColumn(Vec3 s)
		{
			M00 *= s.X; M01 *= s.Y; M02 *= s.Z;
			M10 *= s.X; M11 *= s.Y; M12 *= s.Z;
			M20 *= s.X; M21 *= s.Y; M22 *= s.Z;
		}

		/// <summary>
		/// apply scaling to matrix.
		/// </summary>
		/// <returns></returns>
		void Scale(Vec3 s)
		{
			M00 *= s.X; M01 *= s.Y; M02 *= s.Z;
			M10 *= s.X; M11 *= s.Y; M12 *= s.Z;
			M20 *= s.X; M21 *= s.Y; M22 *= s.Z;
		}

		public void SetIdentity()
		{
			M00 = 1.0f; M01 = 0.0f; M02 = 0.0f; M03 = 0.0f;
			M10 = 0.0f; M11 = 1.0f; M12 = 0.0f; M13 = 0.0f;
			M20 = 0.0f; M21 = 0.0f; M22 = 1.0f; M23 = 0.0f;
		}

		public static Matrix34 CreateIdentity()
		{
			Matrix34 matrix = new Matrix34();

			matrix.SetIdentity();

			return matrix;
		}

		/*!
		*  Create a rotation matrix around an arbitrary axis (Eulers Theorem).  
		*  The axis is specified as an normalized Vector3. The angle is assumed to be in radians.  
		*  This function also assumes a translation-vector and stores it in the right column.  
		*
		*  Example:
		*		Matrix34 m34;
		*		Vector3 axis=GetNormalized( Vector3(-1.0f,-0.3f,0.0f) );
		*		m34.SetRotationAA( 3.14314f, axis, Vector3(5,5,5) );
		*/
		public void SetRotationAA(float rad, Vec3 axis, Vec3 t = default(Vec3))
		{
			this = new Matrix34(Matrix33.CreateRotationAA(rad, axis));

			SetTranslation(t);
		}

		public static Matrix34 CreateRotationAA(float rad, Vec3 axis, Vec3 t = default(Vec3))
		{
			Matrix34 matrix = new Matrix34();
			matrix.SetRotationAA(rad, axis, t);

			return matrix;
		}

		public void SetRotationAA(Vec3 rot, Vec3 t = default(Vec3))
		{
			this = new Matrix34(Matrix33.CreateRotationAA(rot));

			SetTranslation(t);
		}

		public static Matrix34 CreateRotationAA(Vec3 rot, Vec3 t = default(Vec3))
		{
			Matrix34 matrix = new Matrix34();
			matrix.SetRotationAA(rot, t);

			return matrix;
		}

		/*!
		* Create rotation-matrix about X axis using an angle.
		* The angle is assumed to be in radians. 
		* The translation-vector is set to zero.  
		*
		*  Example:
		*		Matrix34 m34;
		*		m34.SetRotationX(0.5f);
		*/
		public void SetRotationX(float rad, Vec3 t = default(Vec3))
		{
			this = new Matrix34(Matrix33.CreateRotationX(rad));

			SetTranslation(t);
		}

		public static Matrix34 CreateRotationX(float rad, Vec3 t = default(Vec3))
		{
			Matrix34 matrix = new Matrix34();
			matrix.SetRotationX(rad, t);

			return matrix;
		}

		/*!
		* Create rotation-matrix about Y axis using an angle.
		* The angle is assumed to be in radians. 
		* The translation-vector is set to zero.  
		*
		*  Example:
		*		Matrix34 m34;
		*		m34.SetRotationY(0.5f);
		*/
		public void SetRotationY(float rad, Vec3 t = default(Vec3))
		{
			this = new Matrix34(Matrix33.CreateRotationY(rad));

			SetTranslation(t);
		}

		public static Matrix34 CreateRotationY(float rad, Vec3 t = default(Vec3))
		{
			Matrix34 matrix = new Matrix34();
			matrix.SetRotationY(rad, t);

			return matrix;
		}

		/*!
		* Create rotation-matrix about Z axis using an angle.
		* The angle is assumed to be in radians. 
		* The translation-vector is set to zero.  
		*
		*  Example:
		*		Matrix34 m34;
		*		m34.SetRotationZ(0.5f);
		*/
		public void SetRotationZ(float rad, Vec3 t = default(Vec3))
		{
			this = new Matrix34(Matrix33.CreateRotationZ(rad));

			SetTranslation(t);
		}

		public static Matrix34 CreateRotationZ(float rad, Vec3 t = default(Vec3))
		{
			Matrix34 matrix = new Matrix34();
			matrix.SetRotationZ(rad, t);

			return matrix;
		}

		/*!
		*
		* Convert three Euler angle to mat33 (rotation order:XYZ)
		* The Euler angles are assumed to be in radians. 
		* The translation-vector is set to zero.  
		*
		*  Example 1:
		*		Matrix34 m34;
		*		m34.SetRotationXYZ( Ang3(0.5f,0.2f,0.9f), translation );
		*
		*  Example 2:
		*		Matrix34 m34=Matrix34::CreateRotationXYZ( Ang3(0.5f,0.2f,0.9f), translation );
		*/
		public void SetRotationXYZ(Vec3 rad, Vec3 t = default(Vec3))
		{
			this = new Matrix34(Matrix33.CreateRotationXYZ(rad));

			SetTranslation(t);
		}

		public static Matrix34 CreateRotationXYZ(Vec3 rad, Vec3 t = default(Vec3))
		{
			Matrix34 matrix = new Matrix34();
			matrix.SetRotationXYZ(rad, t);

			return matrix;
		}

		public void SetRotationAA(float c, float s, Vec3 axis, Vec3 t = default(Vec3))
		{
			this = new Matrix34(Matrix33.CreateRotationAA(c, s, axis));
			M03 = t.X; M13 = t.Y; M23 = t.Z;
		}

		public static Matrix34 CreateRotationAA(float c, float s, Vec3 axis, Vec3 t = default(Vec3))
		{
			Matrix34 matrix = new Matrix34();
			matrix.SetRotationAA(c, s, axis, t);

			return matrix;
		}

		public void Set(Vec3 s, Quat q, Vec3 t = default(Vec3))
		{
			float vxvx=q.Axis.X*q.Axis.X;		float vzvz=q.Axis.Z*q.Axis.Z;		float vyvy=q.Axis.Y*q.Axis.Y; 
			float vxvy=q.Axis.X*q.Axis.Y;		float vxvz=q.Axis.X*q.Axis.Z;		float vyvz=q.Axis.Y*q.Axis.Z; 
			float svx=q.Angle*q.Axis.X;			float svy=q.Angle*q.Axis.Y;			float svz=q.Angle*q.Axis.Z;
			M00=(1-(vyvy+vzvz)*2)*s.X;	M01=(vxvy-svz)*2*s.Y;				M02=(vxvz+svy)*2*s.Z;				M03=t.X;
			M10=(vxvy+svz)*2*s.X;				M11=(1-(vxvx+vzvz)*2)*s.Y;	M12=(vyvz-svx)*2*s.Z;				M13=t.Y;
			M20=(vxvz-svy)*2*s.X;				M21=(vyvz+svx)*2*s.Y;				M22=(1-(vxvx+vyvy)*2)*s.Z;	M23=t.Z;
		}

		public static Matrix34 Create(Vec3 s, Quat q, Vec3 t = default(Vec3))
		{
			Matrix34 matrix = new Matrix34();
			matrix.Set(s, q, t);

			return matrix;
		}

		public void SetScale(Vec3 s, Vec3 t = default(Vec3))
		{
			this = new Matrix34(Matrix33.CreateScale(s));

			SetTranslation(t);
		}

		public static Matrix34 CreateScale(Vec3 s, Vec3 t = default(Vec3))
		{
			Matrix34 matrix = new Matrix34();
			matrix.SetScale(s, t);

			return matrix;
		}

		public void SetTranslationMat(Vec3 v)
		{
			M00 = 1.0f; M01 = 0.0f; M02 = 0.0f; M03 = v.X;
			M10 = 0.0f; M11 = 1.0f; M12 = 0.0f; M13 = v.Y;
			M20 = 0.0f; M21 = 0.0f; M22 = 1.0f; M23 = v.Z;
		}

		public static Matrix34 CreateTranslationMat(Vec3 v)
		{
			Matrix34 matrix = new Matrix34();
			matrix.SetTranslationMat(v);

			return matrix;
		}

		public void SetFromVectors(Vec3 vx, Vec3 vy, Vec3 vz, Vec3 pos)
		{
			M00 = vx.X; M01 = vy.X; M02 = vz.X; M03 = pos.X;
			M10 = vx.Y; M11 = vy.Y; M12 = vz.Y; M13 = pos.Y;
			M20 = vx.Z; M21 = vy.Z; M22 = vz.Z; M23 = pos.Z;
		}

		public static Matrix34 CreateFromVectors(Vec3 vx, Vec3 vy, Vec3 vz, Vec3 pos)
		{
			Matrix34 matrix = new Matrix34();
			matrix.SetFromVectors(vx, vy, vz, pos);

			return matrix;
		}

		public void InvertFast()
		{
			float t; Vec3 v = new Vec3(M03, M13, M23);
			t = M01; M01 = M10; M10 = t; M03 = -v.X * M00 - v.Y * M01 - v.Z * M20;
			t = M02; M02 = M20; M20 = t; M13 = -v.X * M10 - v.Y * M11 - v.Z * M21;
			t = M12; M12 = M21; M21 = t; M23 = -v.X * M20 - v.Y * M21 - v.Z * M22;
		}

		public Matrix34 GetInvertedFast()
		{
			Matrix34 dst = new Matrix34();
			dst.M00 = M00; dst.M01 = M10; dst.M02 = M20; dst.M03 = -M03 * M00 - M13 * M10 - M23 * M20;
			dst.M10 = M01; dst.M11 = M11; dst.M12 = M21; dst.M13 = -M03 * M01 - M13 * M11 - M23 * M21;
			dst.M20 = M02; dst.M21 = M12; dst.M22 = M22; dst.M23 = -M03 * M02 - M13 * M12 - M23 * M22;
			return dst;
		}

		public void Invert()
		{
			//rescue members	
			Matrix34 m = this;
			// calculate 12 cofactors
			M00 = m.M22 * m.M11 - m.M12 * m.M21;
			M10 = m.M12 * m.M20 - m.M22 * m.M10;
			M20 = m.M10 * m.M21 - m.M20 * m.M11;
			M01 = m.M02 * m.M21 - m.M22 * m.M01;
			M11 = m.M22 * m.M00 - m.M02 * m.M20;
			M21 = m.M20 * m.M01 - m.M00 * m.M21;
			M02 = m.M12 * m.M01 - m.M02 * m.M11;
			M12 = m.M02 * m.M10 - m.M12 * m.M00;
			M22 = m.M00 * m.M11 - m.M10 * m.M01;
			M03 = (m.M22 * m.M13 * m.M01 + m.M02 * m.M23 * m.M11 + m.M12 * m.M03 * m.M21) - (m.M12 * m.M23 * m.M01 + m.M22 * m.M03 * m.M11 + m.M02 * m.M13 * m.M21);
			M13 = (m.M12 * m.M23 * m.M00 + m.M22 * m.M03 * m.M10 + m.M02 * m.M13 * m.M20) - (m.M22 * m.M13 * m.M00 + m.M02 * m.M23 * m.M10 + m.M12 * m.M03 * m.M20);
			M23 = (m.M20 * m.M11 * m.M03 + m.M00 * m.M21 * m.M13 + m.M10 * m.M01 * m.M23) - (m.M10 * m.M21 * m.M03 + m.M20 * m.M01 * m.M13 + m.M00 * m.M11 * m.M23);
			// calculate determinant
			float det = 1.0f / (m.M00 * M00 + m.M10 * M01 + m.M20 * M02);
			// calculate matrix inverse/
			M00 *= det; M01 *= det; M02 *= det; M03 *= det;
			M10 *= det; M11 *= det; M12 *= det; M13 *= det;
			M20 *= det; M21 *= det; M22 *= det; M23 *= det;
		}

		Matrix34 GetInverted()
		{
			Matrix34 dst = this;

			dst.Invert();

			return dst;
		}

		/// <summary>
		/// transforms a vector. the translation is not beeing considered
		/// </summary>
		/// <param name="p"></param>
		/// <returns></returns>
		public Vec3 TransformVector(Vec3 p)
		{
			return new Vec3(M00 * p.X + M01 * p.Y + M02 * p.Z + M03, M10 * p.X + M11 * p.Y + M12 * p.Z + M13, M20 * p.X + M21 * p.Y + M22 * p.Z + M23);
		}

		/// <summary>
		/// transforms a point and add translation vector
		/// </summary>
		/// <param name="p"></param>
		/// <returns></returns>
		public Vec3 TransformPoint(Vec3 p)
		{
			return new Vec3(M00 * p.X + M01 * p.Y + M02 * p.Z + M03, M10 * p.X + M11 * p.Y + M12 * p.Z + M13, M20 * p.X + M21 * p.Y + M22 * p.Z + M23);
		}

		/// <summary>
		/// Remove scale from matrix.
		/// </summary>
		public void OrthonormalizeFast()
		{
			Vec3 x = new Vec3( M00,M10,M20 );
			Vec3 y = new Vec3(M01, M11, M21);
			Vec3 z;
			x = x.Normalized;
			z = (x % y).Normalized;
			y = (z % x).Normalized;
			M00=x.X;		M10=x.Y;		M20=x.Z;
			M01=y.X;		M11=y.Y;		M21=y.Z;
			M02=z.X;		M12=z.Y;		M22=z.Z;
		}

		/// <summary>
		/// determinant is ambiguous: only the upper-left-submatrix's determinant is calculated
		/// </summary>
		/// <returns></returns>
		public float Determinant()
		{
			return (M00 * M11 * M22) + (M01 * M12 * M20) + (M02 * M10 * M21) - (M02 * M11 * M20) - (M00 * M12 * M21) - (M01 * M10 * M22);
		}

		public static Matrix34 CreateSlerp(Matrix34 m, Matrix34 n, float t)
		{
			Matrix34 matrix = new Matrix34();
			matrix.SetSlerp(m, n, t);

			return matrix;
		}

		/// <summary>
		///  Direct-Matrix-Slerp: for the sake of completeness, I have included the following expression 
		///  for Spherical-Linear-Interpolation without using quaternions. This is much faster then converting 
		///  both matrices into quaternions in order to do a quaternion slerp and then converting the slerped 
		///  quaternion back into a matrix.
		///  This is a high-precision calculation. Given two orthonormal 3x3 matrices this function calculates 
		///  the shortest possible interpolation-path between the two rotations. The interpolation curve forms 
		///  a great arc on the rotation sphere (geodesic). Not only does Slerp follow a great arc it follows 
		///  the shortest great arc.	Furthermore Slerp has constant angular velocity. All in all Slerp is the 
		///  optimal interpolation curve between two rotations. 
	
		///  STABILITY PROBLEM: There are two singularities at angle=0 and angle=PI. At 0 the interpolation-axis 
		///  is arbitrary, which means any axis will produce the same result because we have no rotation. Thats 
		///  why I'm using (1,0,0). At PI the rotations point away from each other and the interpolation-axis 
		///  is unpredictable. In this case I'm also using the axis (1,0,0). If the angle is ~0 or ~PI, then we 
		///  have to normalize a very small vector and this can cause numerical instability. The quaternion-slerp 
		///  has exactly the same problems.																	Ivo
		/// </summary>
		/// <param name="m"></param>
		/// <param name="n"></param>
		/// <param name="t"></param>
		/// <example>Matrix33 slerp=Matrix33::CreateSlerp( m,n,0.333f );</example>
		public void SetSlerp(Matrix34 m, Matrix34 n, float t)
		{
			//calculate delta-rotation between m and n (=39 flops)
			Matrix33 d = new Matrix33(), i = new Matrix33();
			d.M00=m.M00*n.M00+m.M10*n.M10+m.M20*n.M20;	d.M01=m.M00*n.M01+m.M10*n.M11+m.M20*n.M21;	d.M02=m.M00*n.M02+m.M10*n.M12+m.M20*n.M22;
			d.M10=m.M01*n.M00+m.M11*n.M10+m.M21*n.M20;	d.M11=m.M01*n.M01+m.M11*n.M11+m.M21*n.M21;	d.M12=m.M01*n.M02+m.M11*n.M12+m.M21*n.M22;
			d.M20=d.M01*d.M12-d.M02*d.M11;							d.M21=d.M02*d.M10-d.M00*d.M12;							d.M22=d.M00*d.M11-d.M01*d.M10;

			//extract angle and axis
			double cosine = Math.Clamp((d.M00+d.M11+d.M22-1.0)*0.5,-1.0,+1.0);
			double angle = Math.Atan2(Math.Sqrt(1.0-cosine*cosine),cosine);
			Vec3 axis = new Vec3(d.M21-d.M12,d.M02-d.M20,d.M10-d.M01);
			double l = Math.Sqrt(axis|axis);	if (l>0.00001) axis/=(float)l; else axis = new Vec3(1,0,0); 
			i.SetRotationAA((float)angle*t,axis); //angle interpolation and calculation of new delta-matrix (=26 flops) 

			//final concatenation (=39 flops)
			M00=m.M00*i.M00+m.M01*i.M10+m.M02*i.M20;	M01=m.M00*i.M01+m.M01*i.M11+m.M02*i.M21;	M02=m.M00*i.M02+m.M01*i.M12+m.M02*i.M22;
			M10=m.M10*i.M00+m.M11*i.M10+m.M12*i.M20;	M11=m.M10*i.M01+m.M11*i.M11+m.M12*i.M21;	M12=m.M10*i.M02+m.M11*i.M12+m.M12*i.M22;
			M20=M01*M12-M02*M11;												M21=M02*M10-M00*M12;												M22=M00*M11-M01*M10;

			M03=m.M03*(1-t) + n.M03*t;
			M13=m.M13*(1-t) + n.M13*t;
			M23=m.M23*(1-t) + n.M23*t;
		}

		//--------------------------------------------------------------------------------
		//----                  helper functions to access matrix-members     ------------
		//--------------------------------------------------------------------------------

		public Vec3 GetColumn0() { return new Vec3(M00, M10, M20); }
		public Vec3 GetColumn1() { return new Vec3(M01, M11, M21); }
		public Vec3 GetColumn2() { return new Vec3(M02, M12, M22); }
		public Vec3 GetColumn3() { return new Vec3(M03, M13, M23); }

		public void SetTranslation(Vec3 t)
		{
			M03 = t.X;
			M13 = t.Y;
			M23 = t.Z;
		}

		public Vec3 GetTranslation()
		{
			return new Vec3(M03, M13, M23);
		}

		public void ScaleTranslation(float s)
		{
			M03 *= s;
			M13 *= s;
			M23 *= s;
		}

		public Matrix34 AddTranslation(Vec3 t)
		{
			M03 += t.X;
			M13 += t.Y;
			M23 += t.Z;

			return this;
		}

		public void SetRotation33(Matrix33 m33)
		{
			M00 = m33.M00; M01 = m33.M01; M02 = m33.M02;
			M10 = m33.M10; M11 = m33.M11; M12 = m33.M12;
			M20 = m33.M20; M21 = m33.M21; M22 = m33.M22;
		}

		/// <summary>
		/// check if we have an orthonormal-base (general case, works even with reflection matrices)
		/// </summary>
		/// <param name="threshold"></param>
		/// <returns></returns>
		int IsOrthonormal(float threshold=0.001f) 
		{ 
			float d0=(float)Math.Abs(GetColumn0()|GetColumn1());	if 	(d0>threshold) return 0;
			float d1 = (float)Math.Abs(GetColumn0() | GetColumn2()); if(d1 > threshold) return 0;
			float d2 = (float)Math.Abs(GetColumn1() | GetColumn2()); if(d2 > threshold) return 0;
			int a = (int)Convert.ChangeType((Math.Abs(1 - (GetColumn0() | GetColumn0()))) < threshold, typeof(int));
			int b = (int)Convert.ChangeType((Math.Abs(1 - (GetColumn1() | GetColumn1()))) < threshold, typeof(int));
			int c = (int)Convert.ChangeType((Math.Abs(1 - (GetColumn2() | GetColumn2()))) < threshold, typeof(int));
			return a&b&c;
		}

		public int IsOrthonormalRH(float threshold = 0.001f)
		{
			int a = (int)Convert.ChangeType(GetColumn0().IsEquivalent(GetColumn1() % GetColumn2(), threshold), typeof(int));
			int b = (int)Convert.ChangeType(GetColumn1().IsEquivalent(GetColumn2() % GetColumn0(), threshold), typeof(int));
			int c = (int)Convert.ChangeType(GetColumn2().IsEquivalent(GetColumn0() % GetColumn1(), threshold), typeof(int));
			return a & b & c;
		}

		public bool IsEquivalent(Matrix34 m, float e = 0.05f)
		{
			return (
			(Math.Abs(M00 - m.M00) <= e) && (Math.Abs(M01 - m.M01) <= e) && (Math.Abs(M02 - m.M02) <= e) && (Math.Abs(M03 - m.M03) <= e) &&
			(Math.Abs(M10 - m.M10) <= e) && (Math.Abs(M11 - m.M11) <= e) && (Math.Abs(M12 - m.M12) <= e) && (Math.Abs(M13 - m.M13) <= e) &&
			(Math.Abs(M20 - m.M20) <= e) && (Math.Abs(M21 - m.M21) <= e) && (Math.Abs(M22 - m.M22) <= e) && (Math.Abs(M23 - m.M23) <= e)
			);	
		}
		#endregion
	}
}
