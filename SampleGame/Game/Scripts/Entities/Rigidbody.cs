using CryEngine;

namespace CryGameCode.Entities
{
	public class Rigidbody : Entity
	{
		[EditorProperty(Type = EntityPropertyType.Object)]
		public string Model { get { return GetObjectFilePath(); } set { LoadObject(value); } }

		[EditorProperty]
		public float Mass { get { return Physics.Mass; } set { Physics.Mass = value; } }

		public override void OnReset(bool enteringGame)
		{
			LoadObject(Model);
			Physics.Type = PhysicalizationType.Rigid;
		}
	}
}
