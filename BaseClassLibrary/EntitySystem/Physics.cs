
namespace CryEngine
{
	public static class GlobalPhysics
	{
		const string gravityCVar = "p_gravity_z";
		public static float GravityZ
		{
			get { return Debug.GetCVar(gravityCVar).FVal; }
			set { Debug.GetCVar(gravityCVar).FVal = value; }
		}
	}
}