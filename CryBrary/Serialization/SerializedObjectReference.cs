using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

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

		public ObjectReference(string name, object value, ObjectReference owner)
			: this(name, value)
		{
			FullName = owner.FullName + "." + Name;
		}

		string name;
		public string Name
		{
			get { return name; }
			set
			{
				name = value;

				if(FullName == null)
					FullName = value;
			}
		}
		/// <summary>
		/// The full name including owner objects seperated by '.'
		/// </summary>
		public string FullName { get; set; }
		public object Value { get; set; }

		public override int GetHashCode()
		{
			return Value.GetHashCode();
		}
	}
}
