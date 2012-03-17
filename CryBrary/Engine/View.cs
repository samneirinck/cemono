using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
    public class View
    {
        internal View(uint viewId)
        {
            Id = new EntityId(viewId);
        }

        internal View(EntityId viewId)
        {
            Id = viewId;
        }

        public void SetActiveView()
        {
            ViewSystem.ActiveView = this;
        }

        public Vec3 Position { get { return ViewParams.Position; } set { var viewParams = ViewParams; viewParams.Position = value; ViewParams = viewParams; } }
        public Quat Rotation { get { return ViewParams.Rotation; } set { var viewParams = ViewParams; viewParams.Rotation = value; ViewParams = viewParams; } }

        public float FieldOfView { get { return ViewParams.FieldOfView; } set { var viewParams = ViewParams; viewParams.FieldOfView = value; ViewParams = viewParams; } }

        internal ViewParams ViewParams { get { return ViewSystem._GetViewParams(Id); } set { ViewSystem._SetViewParams(Id, value); } }
        public EntityId Id;
    }
}
