using System;

namespace Cemono
{
    [Serializable]
    public class Entity : IEntity
    {
        #region Properties

        private long _id;
        public long Id
        {
            get { return _id; }
        }

        public string Name
        {
            get
            {
                throw new NotImplementedException();
            }
            set
            {
                throw new NotImplementedException();
            }
        }

        public EntityFlags Flags
        {
            get
            {
                throw new NotImplementedException();
            }
            set
            {
                throw new NotImplementedException();
            }
        }

        public Vec3 Position
        {
            get
            {
                throw new NotImplementedException();
            }
            set
            {
                throw new NotImplementedException();
            }
        }
        #endregion

        #region Constructor(s)
        public Entity()
        {
        }

        public Entity(long id)
            : this()
        {
            _id = id;
        }
        #endregion
    }
}
