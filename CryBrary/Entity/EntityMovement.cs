namespace CryEngine
{
	public enum EntityMoveType
	{
		None = 0,
		Normal,
		Fly,
		Swim,
		ZeroG,

		Impulse,
		JumpInstant,
		JumpAccumulate
	}

	public struct PredictedCharacterStates
	{
		object motionParameter;
		object motionParameterId;
		ushort numParams;
	}

	public struct EntityMovementRequest
	{
		public EntityMoveType type;

		public Vec3 velocity;
		public Quat rotation;

		public PredictedCharacterStates predictedCharacterStates;

		public bool allowStrafe;
		public float proceduralLeaning;
		public bool jumping;
	}
}
