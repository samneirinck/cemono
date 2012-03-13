using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
	public struct Matrix33
	{
		public float m00 { get; set; }
		public float m01 { get; set; }
		public float m02 { get; set; }

		public float m10 { get; set; }
		public float m11 { get; set; }
		public float m12 { get; set; }

		public float m20 { get; set; }
		public float m21 { get; set; }
		public float m22 { get; set; }

		public void SetIdentity()
		{
			m00 = 1; m01 = 0; m02 = 0;
			m10 = 0; m11 = 1; m12 = 0;
			m20 = 0; m21 = 0; m22 = 1; 
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
			m00 = mcx * axis.X + (float)c; m01 = mcx * axis.Y - tcz; m02 = mcx * axis.Z + tcy;
			m10 = mcy * axis.X + tcz; m11 = mcy * axis.Y + (float)c; m12 = mcy * axis.Z - tcx;
			m20 = mcz * axis.X - tcy; m21 = mcz * axis.Y + tcx; m22 = mcz * axis.Z + (float)c;
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
			m00 = mc * axis.X * axis.X + c; m01 = mc * axis.X * axis.Y - axis.Z * s; m02 = mc * axis.X * axis.Z + axis.Y * s;
			m10 = mc * axis.Y * axis.X + axis.Z * s; m11 = mc * axis.Y * axis.Y + c; m12 = mc * axis.Y * axis.Z - axis.X * s;
			m20 = mc * axis.Z * axis.X - axis.Y * s; m21 = mc * axis.Z * axis.Y + axis.X * s; m22 = mc * axis.Z * axis.Z + c;
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
			m00 = 1.0f; m01 = 0.0f; m02 = 0.0f;
			m10 = 0.0f; m11 = (float)c; m12 = (float)-s;
			m20 = 0.0f; m21 = (float)s; m22 = (float)c;
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
			m00 = (float)c; m01 = 0; m02 = (float)s;
			m10 = 0; m11 = 1; m12 = 0;
			m20 = (float)-s; m21 = 0; m22 = (float)c;	
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
			m00 = (float)c; m01 = (float)-s; m02 = 0.0f;
			m10 = (float)s; m11 = (float)c; m12 = 0.0f;
			m20 = 0.0f; m21 = 0.0f; m22 = 1.0f;
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
			m00 = (float)(cy * cz); m01 = (float)(sycz * sx - cx * sz); m02 = (float)(sycz * cx + sx * sz);
			m10 = (float)(cy * sz); m11 = (float)(sysz * sx + cx * cz); m12 = (float)(sysz * cx - sx * cz);
			m20 = (float)(-sy); m21 = (float)(cy * sx); m22 = (float)(cy * cx);			
		}

		public static Matrix33 CreateRotationXYZ(Vec3 rad)
		{
			Matrix33 matrix = new Matrix33();
			matrix.SetRotationXYZ(rad);

			return matrix;
		}

		public void SetScale(Vec3 s)
		{
			m00 = s.X; m01 = 0; m02 = 0;
			m10 = 0; m11 = s.Y; m12 = 0;
			m20 = 0; m21 = 0; m22 = s.Z;
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
		public float m00 { get; set; }
		public float m01 { get; set; }
		public float m02 { get; set; }
		public float m03 { get; set; }

		public float m10 { get; set; }
		public float m11 { get; set; }
		public float m12 { get; set; }
		public float m13 { get; set; }

		public float m20 { get; set; }
		public float m21 { get; set; }
		public float m22 { get; set; }
		public float m23 { get; set; }

		#region Constructors
		public Matrix34(float v00, float v01, float v02, float v03, float v10, float v11, float v12, float v13, float v20, float v21, float v22, float v23)
			: this()
		{
			m00 = v00; m01 = v01; m02 = v02; m03 = v03; 
			m10 = v10; m11 = v11; m12 = v12; m13 = v13;
			m20 = v20; m21 = v21; m22 = v22; m23 = v23;
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
			m00 *= s.X; m01 *= s.Y; m02 *= s.Z;
			m10 *= s.X; m11 *= s.Y; m12 *= s.Z;
			m20 *= s.X; m21 *= s.Y; m22 *= s.Z;
		}

		/// <summary>
		/// apply scaling to matrix.
		/// </summary>
		/// <returns></returns>
		void Scale(Vec3 s)
		{
			m00 *= s.X; m01 *= s.Y; m02 *= s.Z;
			m10 *= s.X; m11 *= s.Y; m12 *= s.Z;
			m20 *= s.X; m21 *= s.Y; m22 *= s.Z;
		}

		public void SetIdentity()
		{
			m00 = 1.0f; m01 = 0.0f; m02 = 0.0f; m03 = 0.0f;
			m10 = 0.0f; m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
			m20 = 0.0f; m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
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
			m03 = t.X; m13 = t.Y; m23 = t.Z;
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
			m00=(1-(vyvy+vzvz)*2)*s.X;	m01=(vxvy-svz)*2*s.Y;				m02=(vxvz+svy)*2*s.Z;				m03=t.X;
			m10=(vxvy+svz)*2*s.X;				m11=(1-(vxvx+vzvz)*2)*s.Y;	m12=(vyvz-svx)*2*s.Z;				m13=t.Y;
			m20=(vxvz-svy)*2*s.X;				m21=(vyvz+svx)*2*s.Y;				m22=(1-(vxvx+vyvy)*2)*s.Z;	m23=t.Z;
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
			m00 = 1.0f; m01 = 0.0f; m02 = 0.0f; m03 = v.X;
			m10 = 0.0f; m11 = 1.0f; m12 = 0.0f; m13 = v.Y;
			m20 = 0.0f; m21 = 0.0f; m22 = 1.0f; m23 = v.Z;
		}

		public static Matrix34 CreateTranslationMat(Vec3 v)
		{
			Matrix34 matrix = new Matrix34();
			matrix.SetTranslationMat(v);

			return matrix;
		}

		public void SetFromVectors(Vec3 vx, Vec3 vy, Vec3 vz, Vec3 pos)
		{
			m00 = vx.X; m01 = vy.X; m02 = vz.X; m03 = pos.X;
			m10 = vx.Y; m11 = vy.Y; m12 = vz.Y; m13 = pos.Y;
			m20 = vx.Z; m21 = vy.Z; m22 = vz.Z; m23 = pos.Z;
		}

		public static Matrix34 CreateFromVectors(Vec3 vx, Vec3 vy, Vec3 vz, Vec3 pos)
		{
			Matrix34 matrix = new Matrix34();
			matrix.SetFromVectors(vx, vy, vz, pos);

			return matrix;
		}

		public void InvertFast()
		{
			float t; Vec3 v = new Vec3(m03, m13, m23);
			t = m01; m01 = m10; m10 = t; m03 = -v.X * m00 - v.Y * m01 - v.Z * m20;
			t = m02; m02 = m20; m20 = t; m13 = -v.X * m10 - v.Y * m11 - v.Z * m21;
			t = m12; m12 = m21; m21 = t; m23 = -v.X * m20 - v.Y * m21 - v.Z * m22;
		}

		public Matrix34 GetInvertedFast()
		{
			Matrix34 dst = new Matrix34();
			dst.m00 = m00; dst.m01 = m10; dst.m02 = m20; dst.m03 = -m03 * m00 - m13 * m10 - m23 * m20;
			dst.m10 = m01; dst.m11 = m11; dst.m12 = m21; dst.m13 = -m03 * m01 - m13 * m11 - m23 * m21;
			dst.m20 = m02; dst.m21 = m12; dst.m22 = m22; dst.m23 = -m03 * m02 - m13 * m12 - m23 * m22;
			return dst;
		}

		public void Invert()
		{
			//rescue members	
			Matrix34 m = this;
			// calculate 12 cofactors
			m00 = m.m22 * m.m11 - m.m12 * m.m21;
			m10 = m.m12 * m.m20 - m.m22 * m.m10;
			m20 = m.m10 * m.m21 - m.m20 * m.m11;
			m01 = m.m02 * m.m21 - m.m22 * m.m01;
			m11 = m.m22 * m.m00 - m.m02 * m.m20;
			m21 = m.m20 * m.m01 - m.m00 * m.m21;
			m02 = m.m12 * m.m01 - m.m02 * m.m11;
			m12 = m.m02 * m.m10 - m.m12 * m.m00;
			m22 = m.m00 * m.m11 - m.m10 * m.m01;
			m03 = (m.m22 * m.m13 * m.m01 + m.m02 * m.m23 * m.m11 + m.m12 * m.m03 * m.m21) - (m.m12 * m.m23 * m.m01 + m.m22 * m.m03 * m.m11 + m.m02 * m.m13 * m.m21);
			m13 = (m.m12 * m.m23 * m.m00 + m.m22 * m.m03 * m.m10 + m.m02 * m.m13 * m.m20) - (m.m22 * m.m13 * m.m00 + m.m02 * m.m23 * m.m10 + m.m12 * m.m03 * m.m20);
			m23 = (m.m20 * m.m11 * m.m03 + m.m00 * m.m21 * m.m13 + m.m10 * m.m01 * m.m23) - (m.m10 * m.m21 * m.m03 + m.m20 * m.m01 * m.m13 + m.m00 * m.m11 * m.m23);
			// calculate determinant
			float det = 1.0f / (m.m00 * m00 + m.m10 * m01 + m.m20 * m02);
			// calculate matrix inverse/
			m00 *= det; m01 *= det; m02 *= det; m03 *= det;
			m10 *= det; m11 *= det; m12 *= det; m13 *= det;
			m20 *= det; m21 *= det; m22 *= det; m23 *= det;
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
			return new Vec3(m00 * p.X + m01 * p.Y + m02 * p.Z + m03, m10 * p.X + m11 * p.Y + m12 * p.Z + m13, m20 * p.X + m21 * p.Y + m22 * p.Z + m23);
		}

		/// <summary>
		/// transforms a point and add translation vector
		/// </summary>
		/// <param name="p"></param>
		/// <returns></returns>
		public Vec3 TransformPoint(Vec3 p)
		{
			return new Vec3(m00 * p.X + m01 * p.Y + m02 * p.Z + m03, m10 * p.X + m11 * p.Y + m12 * p.Z + m13, m20 * p.X + m21 * p.Y + m22 * p.Z + m23);
		}

		/// <summary>
		/// Remove scale from matrix.
		/// </summary>
		public void OrthonormalizeFast()
		{
			Vec3 x = new Vec3( m00,m10,m20 );
			Vec3 y = new Vec3(m01, m11, m21);
			Vec3 z;
			x = x.Normalized;
			z = (x % y).Normalized;
			y = (z % x).Normalized;
			m00=x.X;		m10=x.Y;		m20=x.Z;
			m01=y.X;		m11=y.Y;		m21=y.Z;
			m02=z.X;		m12=z.Y;		m22=z.Z;
		}

		/// <summary>
		/// determinant is ambiguous: only the upper-left-submatrix's determinant is calculated
		/// </summary>
		/// <returns></returns>
		public float Determinant()
		{
			return (m00 * m11 * m22) + (m01 * m12 * m20) + (m02 * m10 * m21) - (m02 * m11 * m20) - (m00 * m12 * m21) - (m01 * m10 * m22);
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
			d.m00=m.m00*n.m00+m.m10*n.m10+m.m20*n.m20;	d.m01=m.m00*n.m01+m.m10*n.m11+m.m20*n.m21;	d.m02=m.m00*n.m02+m.m10*n.m12+m.m20*n.m22;
			d.m10=m.m01*n.m00+m.m11*n.m10+m.m21*n.m20;	d.m11=m.m01*n.m01+m.m11*n.m11+m.m21*n.m21;	d.m12=m.m01*n.m02+m.m11*n.m12+m.m21*n.m22;
			d.m20=d.m01*d.m12-d.m02*d.m11;							d.m21=d.m02*d.m10-d.m00*d.m12;							d.m22=d.m00*d.m11-d.m01*d.m10;

			//extract angle and axis
			double cosine = Math.Clamp((d.m00+d.m11+d.m22-1.0)*0.5,-1.0,+1.0);
			double angle = Math.Atan2(Math.Sqrt(1.0-cosine*cosine),cosine);
			Vec3 axis = new Vec3(d.m21-d.m12,d.m02-d.m20,d.m10-d.m01);
			double l = Math.Sqrt(axis|axis);	if (l>0.00001) axis/=(float)l; else axis = new Vec3(1,0,0); 
			i.SetRotationAA((float)angle*t,axis); //angle interpolation and calculation of new delta-matrix (=26 flops) 

			//final concatenation (=39 flops)
			m00=m.m00*i.m00+m.m01*i.m10+m.m02*i.m20;	m01=m.m00*i.m01+m.m01*i.m11+m.m02*i.m21;	m02=m.m00*i.m02+m.m01*i.m12+m.m02*i.m22;
			m10=m.m10*i.m00+m.m11*i.m10+m.m12*i.m20;	m11=m.m10*i.m01+m.m11*i.m11+m.m12*i.m21;	m12=m.m10*i.m02+m.m11*i.m12+m.m12*i.m22;
			m20=m01*m12-m02*m11;												m21=m02*m10-m00*m12;												m22=m00*m11-m01*m10;

			m03=m.m03*(1-t) + n.m03*t;
			m13=m.m13*(1-t) + n.m13*t;
			m23=m.m23*(1-t) + n.m23*t;
		}

		//--------------------------------------------------------------------------------
		//----                  helper functions to access matrix-members     ------------
		//--------------------------------------------------------------------------------

		public Vec3 GetColumn0() { return new Vec3(m00, m10, m20); }
		public Vec3 GetColumn1() { return new Vec3(m01, m11, m21); }
		public Vec3 GetColumn2() { return new Vec3(m02, m12, m22); }
		public Vec3 GetColumn3() { return new Vec3(m03, m13, m23); }

		public void SetTranslation(Vec3 t)
		{
			m03 = t.X;
			m13 = t.Y;
			m23 = t.Z;
		}

		public Vec3 GetTranslation()
		{
			return new Vec3(m03, m13, m23);
		}

		public void ScaleTranslation(float s)
		{
			m03 *= s;
			m13 *= s;
			m23 *= s;
		}

		public Matrix34 AddTranslation(Vec3 t)
		{
			m03 += t.X;
			m13 += t.Y;
			m23 += t.Z;

			return this;
		}

		public void SetRotation33(Matrix33 m33)
		{
			m00 = m33.m00; m01 = m33.m01; m02 = m33.m02;
			m10 = m33.m10; m11 = m33.m11; m12 = m33.m12;
			m20 = m33.m20; m21 = m33.m21; m22 = m33.m22;
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
			(Math.Abs(m00 - m.m00) <= e) && (Math.Abs(m01 - m.m01) <= e) && (Math.Abs(m02 - m.m02) <= e) && (Math.Abs(m03 - m.m03) <= e) &&
			(Math.Abs(m10 - m.m10) <= e) && (Math.Abs(m11 - m.m11) <= e) && (Math.Abs(m12 - m.m12) <= e) && (Math.Abs(m13 - m.m13) <= e) &&
			(Math.Abs(m20 - m.m20) <= e) && (Math.Abs(m21 - m.m21) <= e) && (Math.Abs(m22 - m.m22) <= e) && (Math.Abs(m23 - m.m23) <= e)
			);	
		}
		#endregion
	}
}
