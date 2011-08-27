using System;
using CryEngine.API;
using System.Diagnostics;

namespace CryEngine
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
        protected Entity()
        {
        }

        protected Entity(long id)
            : this()
        {
            _id = id;
        }
        #endregion

        public override bool Equals(object obj)
        {
            IEntity ent = obj as IEntity;

            return (ent == null) ? false : this.Id == ent.Id;
        }

        public override int GetHashCode()
        {
            return _id.GetHashCode();
        }

        protected void SetProperty<T>(string propertyName, T value)
        {
            EntitySystem.SetProperty(_id, propertyName, value);
        }

        protected object GetProperty(string propertyName)
        {
            return EntitySystem.GetProperty(_id, propertyName);
        }

        protected T GetProperty<T>(string propertyName)
        {
            try
            {
                return (T)GetProperty(propertyName);
            }
            catch (InvalidCastException ex)
            {
                Trace.TraceWarning("GetProperty<{0}>(\"{1}\") - Invalid cast, returning default");
                return default(T);
            }
        }


        protected virtual void OnInit(long entityId)
        {
            _id = entityId;
        }

        protected virtual void OnGameStarted()
        {
        }
        protected virtual void OnXForm(int flags)
        {
        }

        protected virtual void OnTimer(int timerId, int milliSeconds)
        {
        }

        protected virtual void OnDone()
        {
        }

        protected virtual void OnVisibility(bool visible)
        {
        }

        protected virtual void OnReset(bool inGame)
        {
        }

        protected virtual void OnAttach(long childEntityId)
        {
        }

        protected virtual void OnDetach(long childEntityId)
        {
        }

        protected virtual void OnDetachThis()
        {
        }

        protected virtual void OnHide()
        {
        }

        protected virtual void OnUnHide()
        {
        }

        protected virtual void OnEnablePhysics(bool enable)
        {
        }

        protected virtual void OnPhysicsChangeState(bool awake)
        {
        }

        protected virtual void OnScriptEvent(string scriptEvent, int eventValueType, IntPtr pointer)
        {
        }

        protected virtual void OnEnterArea(long triggerEntityId, int areaId, long areaEntityId)
        {
        }

        protected virtual void OnLeaveArea(long triggerEntityId, int areaId, long areaEntityId)
        {
        }

        protected virtual void OnEnterNearArea(long triggerEntityId, int areaId, long areaEntityId)
        {
        }

        protected virtual void OnLeaveNearArea(long triggerEntityId, int areaId, long areaEntityId)
        {
        }

        protected virtual void OnMoveInsideArea(long triggerEntityId, int areaId, long areaEntityId)
        {
        }


        protected virtual void OnPhysPostStep(float timeInterval)
        {
        }

        protected virtual void OnPhysBreak()
        {
        }

        protected virtual void OnAIDone()
        {
        }

        protected virtual void OnSoundDone()
        {
        }

        protected virtual void OnNotSeenTimeout()
        {
        }

        protected virtual void OnCollision()
        {
        }

        protected virtual void OnRender(IntPtr pointer)
        {
        }

        protected virtual void OnPrePhysicsUpdate(float frameTime)
        {
        }

        protected virtual void OnStartLevel()
        {
        }

        protected virtual void OnStartGame()
        {
        }

        protected virtual void OnEnterScriptState()
        {
        }

        protected virtual void OnLeaveScriptState()
        {
        }

        protected virtual void OnPreSerialize()
        {
        }

        protected virtual void OnInvisible()
        {
        }

        protected virtual void OnVisible()
        {
        }

        protected virtual void OnMaterial(IntPtr pointer)
        {
        }

        protected virtual void OnMaterialLayer()
        {
        }

        protected virtual void OnHit()
        {
        }

        protected virtual void OnPickup(bool pickup, long pickerEntityId, float force)
        {
        }

        protected virtual void OnAnimEvent(IntPtr pointer)
        {
        }

        protected virtual void OnScriptRequestColliderMode(int colliderMode)
        {
        }


    }
}
