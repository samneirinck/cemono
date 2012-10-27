using CryEngine.Native;

namespace CryEngine
{
    /// <summary>
    /// CVar created outside CryMono
    /// </summary>
    internal class ExternalCVar : CVar
    {
        internal ExternalCVar(string name)
        {
            Name = name;
        }

        public override string String
        {
            get { return NativeMethods.CVar.GetCVarString(Name); }
            set { NativeMethods.CVar.SetCVarString(Name, value); }
        }

        public override float FVal
        {
            get { return NativeMethods.CVar.GetCVarFloat(Name); }
            set { NativeMethods.CVar.SetCVarFloat(Name, value); }
        }

        public override int IVal
        {
            get { return NativeMethods.CVar.GetCVarInt(Name); }
            set { NativeMethods.CVar.SetCVarInt(Name, value); }
        }
    }
}
