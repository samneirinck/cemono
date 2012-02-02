using CryEngine;

namespace CryGameCode.Entities
{
    public class Dragon : Entity
    {
        public override void OnUpdate()
        {
            //Velocity += new Vec3(2, 2, 0);
        }

		[InputPort(Name = "Let panic ensue", Description = "Kill them all.")]
		public void FlyAroundAndSetPeopleOnFire()
		{
			// TODO
		}

        public override void OnSpawn()
        {
            ReceiveUpdates = true;

            LoadObject(Model);

			ActivateOutput(spawnedPortId);
        }

        public override void OnReset(bool enteringGame)
        {
            LoadObject(Model);

			Physics.Mass = 5000;
			Physics.Type = PhysicalizationType.Rigid;
			Physics.Stiffness = 70;
        }

		[OutputPort(Name = "Spawned", Description = "", Type = NodePortType.Void)]
		public static int spawnedPortId;

        [EditorProperty(Type = EntityPropertyType.Object, DefaultValue="Objects/Characters/Dragon/Dragon.cdf")]
        public string Model { get { return GetObjectFilePath(); } set { LoadObject(value); } }
    }
}