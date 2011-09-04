using System;
using System.Runtime.CompilerServices;
namespace CryEngine.API
{
    public class EntitySystem
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern private static void _RegisterEntityClass(EntityClassFlags flags, string name, string editorHelper, string editorIcon, string category, string fullyQualifiedName, string pathToAssembly, EntityProperty[] properties);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern private static object _EntityGetProperty(long entityId, string propertyName);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern private static void _SetPropertyFloat(long entityId, string property, float value);

        public static object GetProperty(long entityId, string propertyName)
        {
            return _EntityGetProperty(entityId, propertyName);
        }

        public static void SetProperty(long entityId, string propertyName, object value)
        {
            if (value is Single)
                _SetPropertyFloat(entityId, propertyName, (float)value);
        }

        public static void RegisterEntityClass(EntityClassFlags flags, string name, string editorHelper, string editorIcon, string category, string fullyQualifiedName, string pathToAssembly, EntityProperty[] properties)
        {
            _RegisterEntityClass(flags, name, editorHelper, editorIcon, category, fullyQualifiedName, pathToAssembly, properties);
        }

    }


}
