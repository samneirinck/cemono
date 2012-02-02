using System.Runtime.CompilerServices;


namespace CryEngine.Utils
{
    public class Tester
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static void _TestScriptBind(string testString, int testInt, object[] array);

        // constructor tests
        public Tester()
        {
        }

        public Tester(string testString, int testInt, object[] testArray)
        {
            StaticMethodWithParameters(testString, testInt, testArray);
        }

        // instantiated tests
        public float MethodNoParameters()
        {
            return 13.37f;
        }

        public void MethodWithParameters(string testString, int testInt, object[] testArray)
        {
            StaticMethodWithParameters(testString, testInt, testArray);
        }

        // static tests
        public static float StaticMethodNoParameters()
        {
            return 13.37f;
        }

        public static void StaticMethodWithParameters(string testString, int testInt, object[] testArray)
        {
            _TestScriptBind(testString, testInt, testArray);
        }
    }
}
