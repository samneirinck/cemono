using System;
using System.Runtime.CompilerServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{

    public class CVar
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static int _GetIVal(string CVar);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static float _GetFVal(string CVar);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static string _GetString(string CVar);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static void _SetCVar(string CVar, string newVal);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static void _SetCVarIVal(string CVar, int newVal);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static void _SetCVarFVal(string CVar, float newVal);

        public int GetIVal()
        {
            return _GetIVal(name);
        }

        public float GetFVal()
        {
            return _GetFVal(name);
        }

        public string GetString()
        {
            return _GetString(name);
        }

        public void Set(string newVal)
        {
            _SetCVar(name, newVal);
        }

        public void Set(float newVal)
        {
            _SetCVarFVal(name, newVal);
        }

        public void Set(int newVal)
        {
            _SetCVarIVal(name, newVal);
        }

        public string GetName()
        {
            return name;
        }

        string name;
    }
}
