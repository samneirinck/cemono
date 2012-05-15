namespace CryEngine
{
	enum EntityMoveType
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

	struct EntityMovementRequest
	{
		public EntityMoveType type;

		public Vec3 velocity;
	}
}
