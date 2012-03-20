
namespace CryEngine
{
	public static class GlobalPhysics
	{
		const string gravityCVar = "p_gravity_z";
		public static float GravityZ
		{
			get { return CVar.Get(gravityCVar).FVal; }
			set { CVar.Get(gravityCVar).FVal = value; }
		}
	}
}