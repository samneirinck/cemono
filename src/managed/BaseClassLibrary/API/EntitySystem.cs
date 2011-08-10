using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.CompilerServices;

namespace Cemono.API
{
    public class EntitySystem
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern protected static IEntity[] _GetEntities();


        public static ReadOnlyCollection<IEntity> GetEntities()
        {
            return new ReadOnlyCollection<IEntity>(_GetEntities().ToList());
        }
    }
}
