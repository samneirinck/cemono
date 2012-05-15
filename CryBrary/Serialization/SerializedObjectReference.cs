
namespace CryEngine.Serialization
{
	class ObjectReference
	{
		public ObjectReference()
		{
		}

		public ObjectReference(string name, object value)
		{
			Name = name;
			Value = value;
		}

		public string Name { get; set; }

		public object Value { get; set; }
	}
}
