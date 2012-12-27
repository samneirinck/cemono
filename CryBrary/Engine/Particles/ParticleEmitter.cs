using System;

using CryEngine.Engine.Particles.Native;

namespace CryEngine
{
    public class ParticleEmitter
    {
        internal ParticleEmitter(IntPtr ptr)
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (ptr == IntPtr.Zero)
                throw new NullPointerException();
#endif

            Handle = ptr;
        }

        public ParticleSpawnParameters SpawnParameters
        {
            get { return NativeParticleEffectMethods.GetParticleEmitterSpawnParams(Handle); }
            set { NativeParticleEffectMethods.SetParticleEmitterSpawnParams(Handle, ref value); }
        }

        public GeometryType AttachType { get { return SpawnParameters.AttachType; } set { var spawnParams = SpawnParameters; spawnParams.AttachType = value; SpawnParameters = spawnParams; } }
        public GeometryForm AttachForm { get { return SpawnParameters.AttachForm; } set { var spawnParams = SpawnParameters; spawnParams.AttachForm = value; SpawnParameters = spawnParams; } }

        public bool CountPerUnit { get { return SpawnParameters.CountPerUnit; } set { var spawnParams = SpawnParameters; spawnParams.CountPerUnit = value; SpawnParameters = spawnParams; } }
        public bool EnableSound { get { return SpawnParameters.EnableSound; } set { var spawnParams = SpawnParameters; spawnParams.EnableSound = value; SpawnParameters = spawnParams; } }

        public float CountScale { get { return SpawnParameters.CountScale; } set { var spawnParams = SpawnParameters; spawnParams.CountScale = value; SpawnParameters = spawnParams; } }
        public float SizeScale { get { return SpawnParameters.SizeScale; } set { var spawnParams = SpawnParameters; spawnParams.SizeScale = value; SpawnParameters = spawnParams; } }
        public float SpeedScale { get { return SpawnParameters.SpeedScale; } set { var spawnParams = SpawnParameters; spawnParams.SpeedScale = value; SpawnParameters = spawnParams; } }

        public float PulsePeriod { get { return SpawnParameters.PulsePeriod; } set { var spawnParams = SpawnParameters; spawnParams.PulsePeriod = value; SpawnParameters = spawnParams; } }
        public float Strength { get { return SpawnParameters.Strength; } set { var spawnParams = SpawnParameters; spawnParams.Strength = value; SpawnParameters = spawnParams; } }

        public ParticleEffect ParticleEffect 
        { 
            get
            {
                var ptr = NativeParticleEffectMethods.GetParticleEmitterEffect(Handle);
                if (ptr != IntPtr.Zero)
                    return new ParticleEffect(ptr);

                return null;
            } 
        }

        internal IntPtr Handle { get; set; }
    }
}
