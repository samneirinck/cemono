
namespace CryEngine
{
	public static class Physics
	{
		const string gravityCVar = "p_gravity_z";
		public static float GravityZ
		{
			get { return Console.GetCVar(gravityCVar).FVal; }
			set { Console.GetCVar(gravityCVar).FVal = value; }
		}
	}
}