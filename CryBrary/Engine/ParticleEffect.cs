using System;
using System.Runtime.CompilerServices;
using CryEngine.Native;

namespace CryEngine
{
    public class ParticleEffect
    {
        internal ParticleEffect(IntPtr ptr)
        {
            Handle = ptr;
        }

        internal IntPtr Handle { get; set; }

        #region Statics
        /// <summary>
        /// </summary>
        /// <param name="effectName"></param>
        /// <param name="loadResources">Load all required resources?</param>
        /// <returns>The specified particle effect, or null if failed.</returns>
        public static ParticleEffect Get(string effectName, bool loadResources = true)
        {
            var ptr = NativeParticleEffectMethods.FindEffect(effectName, loadResources);
            if (ptr != IntPtr.Zero)
                return new ParticleEffect(ptr);

            return null;
        }
        #endregion

        /// <summary>
        /// Spawns this effect
        /// </summary>
        /// <param name="independent"></param>
        /// <param name="pos">World location to place emitter at.</param>
        /// <param name="dir">World rotation of emitter, set to Vec3.Up if null.</param>
        /// <param name="scale">Scale of the emitter.</param>
        public void Spawn(Vec3 pos, Vec3? dir = null, float scale = 1f, bool independent = true)
        {
            NativeParticleEffectMethods.Spawn(Handle, independent, pos, dir ?? Vec3.Up, scale);
        }

        public void Remove()
        {
            NativeParticleEffectMethods.Remove(Handle);
        }

        public void LoadResources()
        {
            NativeParticleEffectMethods.LoadResoruces(Handle);
        }

        #region Overrides
        public override bool Equals(object obj)
        {
            if (obj != null && obj is ParticleEffect)
                return this == obj;

            return false;
        }

        public override int GetHashCode()
        {
            unchecked // Overflow is fine, just wrap
            {
                int hash = 17;

                hash = hash * 29 + Handle.GetHashCode();

                return hash;
            }
        }
        #endregion
    }
}