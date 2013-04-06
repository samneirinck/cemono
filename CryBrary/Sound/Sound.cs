using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using CryEngine.Native;

namespace CryEngine
{
	public class Sound
	{
		#region Statics
		public static Sound CreateSound(string name)
		{
			return TryGet(NativeSoundMethods.CreateSound(name, 0));
		}

		public static Sound CreateLineSound(string name, Vec3 start, Vec3 end)
		{
			return TryGet(NativeSoundMethods.CreateLineSound(name, 0, 0, start, end));
		}

		public static Sound CreateSphereSound(string name, float radius)
		{
			return TryGet(NativeSoundMethods.CreateSphereSound(name, 0, 0, radius));
		}

		static Sound TryGet(IntPtr soundPtr)
		{
			if (soundPtr == IntPtr.Zero)
				return null;

			var sound = Sounds.FirstOrDefault(x => x.Handle == soundPtr);
			if(sound == null)
			{
				sound = new Sound(soundPtr);
				Sounds.Add(sound);
			}

			return sound;
		}

		static List<Sound> Sounds = new List<Sound>();
		#endregion

		internal Sound(IntPtr ptr)
		{
			Handle = ptr;
		}

		public void Play()
		{
			NativeSoundMethods.Play(Handle);
		}

		public Vec3 Position { set { NativeSoundMethods.SetPosition(Handle, value); } }

		internal IntPtr Handle { get; set; }
	}
}
