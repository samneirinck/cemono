using System.Runtime.CompilerServices;

namespace CryEngine
{
	/// <summary>
	/// Renderer functionality; TODO
	/// </summary>
	public class Renderer
	{
		// Change to protected once we've implemented properties etc.
		#region ViewSystem
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static uint _CreateView();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _RemoveView(uint viewId);

		/*public static ViewId CreateView()
		{
			return _CreateView();
		}

		public static void RemoveView(ViewId viewId)
		{
			_RemoveView(viewId);
		}*/

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static uint _GetActiveView();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetActiveView(uint viewId);

		/*public static ViewId GetActiveView()
		{
			return _GetActiveView();
		}

		public static void SetActiveView(ViewId viewId)
		{
			_SetActiveView(viewId);
		}*/

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static ViewParams _GetViewParams(uint viewId);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetViewParams(uint viewId, ViewParams cam);

		/*public static ViewSettings GetViewParams(ViewId viewId)
		{
			return new ViewSettings(_GetViewParams(viewId));
		}

		public static void SetViewParams(ViewId viewId, ViewSettings viewParams)
		{
			_SetViewParams(viewId, viewParams._params);
		}*/
		#endregion

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _GetWidth();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _GetHeight();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static Vec3 _ScreenToWorld(int x, int y);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _DrawTextToScreen(float x, float y, float fontSize, Color color, bool center, string text);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _LoadTexture(string path);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _DrawTextureToScreen(float xpos, float ypos, float width, float height, int textureId, float s0 = 0, float t0 = 0, float s1 = 1, float t1 = 1, float angle = 0, float r = 1, float g = 1, float b = 1, float a = 1, float z = 1);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _CreateRenderTarget(int width, int height, int flags);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _DestroyRenderTarget(int id);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetRenderTarget(int id);

		public static Vec3 ScreenToWorld(int x, int y)
		{
			return _ScreenToWorld(x, y);
		}

		public static void DrawTextToScreen(float x, float y, float fontSize, Color color, string text, params object[] args)
		{
			DrawTextToScreen(x, y, fontSize, color, false, text, args);
		}

		public static void DrawTextToScreen(float x, float y, float fontSize, Color color, bool centered, string text, params object[] args)
		{
			_DrawTextToScreen(x, y, fontSize, color, centered, string.Format(text, args));
		}

		public static float FieldOfView
		{
			get
			{
				return _GetViewParams(_GetActiveView()).fov;
			}
			set
			{
				var view = _GetActiveView();
				var viewParams = _GetViewParams(view);
				viewParams.fov = (float)Math.DegToRad(value);
				_SetViewParams(view, viewParams);
			}
		}

		/// <summary>
		/// The width of the screen in pixels.
		/// </summary>
		public static int ScreenHeight { get { return _GetHeight(); } }

		/// <summary>
		/// The height of the screen in pixels.
		/// </summary>
		public static int ScreenWidth { get { return _GetWidth(); } }

		/// <summary>
		/// Loads a texture into memory and generates its ID.
		/// </summary>
		/// <param name="texturePath">The path to the texture.</param>
		/// <returns></returns>
		public static int LoadTexture(string texturePath)
		{
			return _LoadTexture(texturePath);
		}

		/// <summary>
		/// Draws a texture to the screen for one frame.
		/// </summary>
		/// <param name="xPos">The x position on the screen, relative to the top-left corner.</param>
		/// <param name="yPos">The y position on the screen, relative to the top-left corner.</param>
		/// <param name="width">The width to render the texture at.</param>
		/// <param name="height">The height to render the texture at.</param>
		/// <param name="textureId">The ID of the texture (obtained using Renderer.LoadTexture(path)).</param>
		public static void DrawTexture(int xPos, int yPos, int width, int height, int textureId)
		{
			_DrawTextureToScreen(xPos, yPos, width, height, textureId);
		}

		public static int CreateRenderTarget(int width, int height, int flags = 0)
		{
			return _CreateRenderTarget(width, height, flags);
		}

		public static void SetRenderTarget(int id)
		{
			_SetRenderTarget(id);
		}

		public static void DestroyRenderTarget(int id)
		{
			_DestroyRenderTarget(id);
		}
	}

	/*public struct ViewId
	{
		internal uint _value;

		public ViewId(int id)
		{
			if(id >= 0)
				_value = (uint)id;
			else
				throw new System.ArgumentException("Tried to set a negative view ID");
		}

		public static implicit operator int(ViewId id)
		{
			return (int)id._value;
		}

		public static implicit operator ViewId(int value)
		{
			return new ViewId(value);
		}

		[System.CLSCompliant(false)]
		public static implicit operator uint(ViewId id)
		{
			return id._value;
		}

		[System.CLSCompliant(false)]
		public static implicit operator ViewId(uint value)
		{
			return new ViewId { _value = value };
		}
	}

	public class ViewSettings
	{
		internal ViewParams _params;

		internal ViewSettings(ViewParams viewParams)
		{
			_params = viewParams;
		}
	}*/

	internal struct ViewParams
	{
		/// <summary>
		/// view position
		/// </summary>
		public Vec3 position;
		/// <summary>
		/// view orientation
		/// </summary>
		public Quat rotation;
		public Quat localRotationLAST;

		/// <summary>
		/// custom near clipping plane, 0 means use engine defaults
		/// </summary>
		public float nearplane;
		public float fov;

		public byte viewID;

		/// <summary>
		/// view shake status
		/// </summary>
		public bool groundOnly;
		/// <summary>
		/// whats the amount of shake, from 0.0 to 1.0
		/// </summary>
		public float shakingRatio;
		/// <summary>
		/// what the current angular shake
		/// </summary>
		public Quat currentShakeQuat;
		/// <summary>
		/// what is the current translational shake
		/// </summary>
		public Vec3 currentShakeShift;

		// For damping camera movement.
		/// <summary>
		/// Who we're watching. 0 == nobody.
		/// </summary>
		public uint idTarget;
		/// <summary>
		/// Where the target was.
		/// </summary>
		public Vec3 targetPos;
		/// <summary>
		/// current dt.
		/// </summary>
		public float frameTime;
		/// <summary>
		/// previous rate of change of angle.
		/// </summary>
		public float angleVel;
		/// <summary>
		/// previous rate of change of dist between target and camera.
		/// </summary>
		public float vel;
		/// <summary>
		/// previous dist of cam from target
		/// </summary>
		public float dist;

		// blending
		public bool blend;
		public float blendPosSpeed;
		public float blendRotSpeed;
		public float blendFOVSpeed;
		public Vec3 blendPosOffset;
		public Quat blendRotOffset;
		public float blendFOVOffset;
		public bool justActivated;

		private byte viewIDLast;
		private Vec3 positionLast;
		private Quat rotationLast;
		private float FOVLast;
	}
}
