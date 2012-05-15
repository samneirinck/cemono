using System;
using System.Collections.Generic;

namespace CryEngine.Extensions
{
	public static class IDictionaryExtensions
	{
		public static bool TryGetKey<TKey, TValue>(this IDictionary<TKey, TValue> dictionary, TValue value, out TKey key)
		{
			if(dictionary == null)
				throw new ArgumentNullException("dictionary");

			foreach(KeyValuePair<TKey, TValue> pair in dictionary)
			{
				if(value.Equals(pair.Value))
				{
					key = pair.Key;
					return true;
				}
			}

			key = default(TKey);
			return false;
		}
	}
}
