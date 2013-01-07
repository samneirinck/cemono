using System;
using System.Runtime.CompilerServices;

namespace CryEngine.Serialization
{
    public struct CrySerialize
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void BeginGroup(IntPtr handle, string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void EndGroup(IntPtr handle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void ValueString(IntPtr handle, string name, ref string obj, int policy);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void ValueInt(IntPtr handle, string name, ref int obj, int policy);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void ValueBool(IntPtr handle, string name, ref bool obj, int policy);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void ValueEntityId(IntPtr handle, string name, ref uint obj, int policy);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void ValueFloat(IntPtr handle, string name, ref float obj, int policy);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void ValueVec3(IntPtr handle, string name, ref Vec3 obj, int policy);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void ValueQuat(IntPtr handle, string name, ref Quat obj, int policy);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void EnumValue(IntPtr handle, string name, ref Int32 obj, int first, int last);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern bool IsReading(IntPtr handle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern SerializationTarget GetSerializationTarget(IntPtr handle);

        public void BeginGroup(string name)
        {
            BeginGroup(Handle, name);
        }

        public void EndGroup()
        {
            EndGroup(Handle);
        }

        public void Value(string name, ref string obj, int policy = 0)
        {
            ValueString(Handle, name, ref obj, policy);
        }

        public void Value(string name, ref int obj, int policy = 0)
        {
            ValueInt(Handle, name, ref obj, policy);
        }

        public void Value(string name, ref bool obj, int policy = 0)
        {
            ValueBool(Handle, name, ref obj, policy);
        }

        public void Value(string name, ref EntityId obj, int policy = 0)
        {
            ValueEntityId(Handle, name, ref obj._value, policy);
        }

        public void Value(string name, ref float obj, int policy = 0)
        {
            ValueFloat(Handle, name, ref obj, policy);
        }

        public void Value(string name, ref Vec3 obj, int policy = 0)
        {
            ValueVec3(Handle, name, ref obj, policy);
        }

        public void Value(string name, ref Quat obj, int policy = 0)
        {
            ValueQuat(Handle, name, ref obj, policy);
        }

        public void EnumValue(string name, ref Int32 obj, int first, int last)
        {
            EnumValue(Handle, name, ref obj, first, last);
        }

        public bool IsReading()
        {
            return IsReading(Handle);
        }

        public SerializationTarget Target { get { return GetSerializationTarget(Handle); } }

        internal IntPtr Handle { get; set; }
    }
}
