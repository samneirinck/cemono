using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
	[Flags]
	public enum AnimationFlags
	{
		CleanBending = 1 << 1,
		NoBlend = 1 << 2,
		RestartAnimation = 1 << 4,
		RepeatLastFrame = 1 << 8,
		Loop = 1 << 16,
	}
}
