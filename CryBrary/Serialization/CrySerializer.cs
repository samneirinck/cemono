using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.Serialization;
using CryEngine;
using CryEngine.Extensions;

namespace CryEngine.Serialization
{

    public class CrySerializer : IFormatter
    {
        SerializationBinder binder;
        StreamingContext context;
        ISurrogateSelector surrogateSelector;

        public CrySerializer()
        {
            context = new StreamingContext(StreamingContextStates.All);
        }

        public object Deserialize(Stream serializationStream)
        {
            StreamReader sr = new StreamReader(serializationStream);

            // Get Type from serialized data.
            string line = sr.ReadLine();
            char[] delim = new char[] { '=' };
            string[] sarr = line.Split(delim);
            string className = sarr[1];

            System.Type type = null;

            var script = ScriptCompiler.CompiledScripts.FirstOrDefault(x => x.ScriptType.FullName.Equals(className));
            if(script != default(CryScript))
                type = script.ScriptType;

            type = type ?? System.Type.GetType(className);

            if (type == null)
                return null;

            ObjectReference reference = null;

            if (type.GetConstructor(System.Type.EmptyTypes) != null || type.IsValueType)
                reference = new ObjectReference(System.Activator.CreateInstance(type));
            else
                Debug.Log("[Warning] Could not serialize type {0} since it did not contain a parameterless constructor", type.Name);

            if (reference == null || reference.Object == null)
                return null;

            // This is really, really, really, really bad.
            string streamName = ((FileStream)serializationStream).Name;

            if (ObjectReferences.ContainsValue(streamName))
                return ObjectReferences.Keys.First(x => ObjectReferences[x] == streamName).Object;

            // Store serialized variable name -> value pairs.
            StringDictionary sdict = new StringDictionary();
            while (sr.Peek() >= 0)
            {
                line = sr.ReadLine();
                sarr = line.Split(delim);

                // key = variable name, value = variable value.
                sdict[sarr[0].Trim()] = sarr[1].Trim();
            }
            sr.Close();

            if (type.Implements(typeof(IEnumerable)))
            {
                var array = (reference.Object as IEnumerable).Cast<object>().ToArray();

                foreach (var dict in sdict.Keys)
                {
                    string indexVal = (string)sdict[(string)dict];

                    if (indexVal.StartsWith(ReferenceSeperator) && indexVal.EndsWith(ReferenceSeperator))
                    {
                        // This was a reference to a serialized object in another file, lets localize it and deserialize.
                        string referenceFile = indexVal.Replace(ReferenceSeperator, "");

                        var formatter = new CrySerializer();
                        var stream = File.Open(referenceFile, FileMode.Open);

                        array.SetValue(formatter.Deserialize(stream), System.Convert.ToInt32((string)dict));

                        stream.Close();
                    }
                    else
                        array.SetValue(Convert.ChangeType(indexVal, array.GetType().GetElementType()), System.Convert.ToInt32((string)dict) /* index as a string */);
                }

                reference.Object = array;
            }
            else if (!type.IsPrimitive && !type.IsEnum && type != typeof(string))
            {
                while (type != null)
                {
                    foreach (var fieldInfo in type.GetFields(BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.DeclaredOnly))
                    {
                        if (sdict.ContainsKey(fieldInfo.Name))
                        {
                            if (sdict[fieldInfo.Name].StartsWith(ReferenceSeperator) && sdict[fieldInfo.Name].EndsWith(ReferenceSeperator))
                            {
                                // This was a reference to a serialized object in another file, lets localize it and deserialize.
                                string referenceFile = sdict[fieldInfo.Name].Replace(ReferenceSeperator, "");

                                var formatter = new CrySerializer();
                                var stream = File.Open(referenceFile, FileMode.Open);

                                object deserializedObject = formatter.Deserialize(stream);

                                stream.Close();

                                fieldInfo.SetValue(reference.Object, deserializedObject);
                            }
                            else
                                fieldInfo.SetValue(reference.Object, Convert.ChangeType(sdict[fieldInfo.Name], fieldInfo.FieldType));
                        }
                    }

                    type = type.BaseType;
                }
            }

            ObjectReferences.Add(reference, ((FileStream)serializationStream).Name);

            return reference.Object;
        }

        public void Serialize(Stream serializationStream, object objectInstance)
        {
            Serialize(serializationStream, new ObjectReference(objectInstance));
        }

        static System.Type[] forbiddenTypes = new System.Type[] { typeof(MethodInfo) };

        bool IsTypeAllowed(System.Type type)
        {
            foreach (var forbiddenType in forbiddenTypes)
            {
                if (type == forbiddenType || (type.HasElementType && type.GetElementType() == forbiddenType))
                    return false;

                if (type.Implements(forbiddenType))
                    return false;
            }

            return true;
        }

        void Serialize(Stream serializationStream, ObjectReference objectReference)
        {
            // Write class name and all fields & values to file
            StreamWriter sw = new StreamWriter(serializationStream);

            if (ObjectReferences.ContainsKey(objectReference))
            {
                sw.WriteLine("@ClassName=" + ReferenceSeperator + "{0}" + ReferenceSeperator, ObjectReferences[objectReference]);
                sw.Close();

                return;
            }

            if (!IsTypeAllowed(objectReference.Type))
                return;

            sw.WriteLine("@ClassName={0}", objectReference.Type.FullName);

            // Should definitely not hardcode FileStream like this
            ObjectReferences.Add(objectReference, ((FileStream)serializationStream).Name);

            if (objectReference.Type.Implements(typeof(IEnumerable)))
            {
                var array = (objectReference.Object as IEnumerable).Cast<object>();

                for (int i = 0; i < array.Count(); i++)
                {
                    object indexedItem = array.ElementAt(i);

                    if (indexedItem != null)
                        WriteReference(indexedItem, i.ToString(), ref sw);
                }
            }
            else if (!objectReference.Type.IsPrimitive && !objectReference.Type.IsEnum && objectReference.Type != typeof(string))
            {
                BindingFlags bindingFlags = BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.DeclaredOnly;

                var type = objectReference.Type;

                while (type != null)
                {
                    foreach (var field in type.GetFields(bindingFlags))
                    {
                        object fieldValue = field.GetValue(objectReference.Object);

                        if (fieldValue != null)
                            WriteReference(fieldValue, field.Name, ref sw);
                    }

                    type = type.BaseType;
                }
            }
            else
                throw new System.Exception(string.Format("This should not happen! Hand this data over to the nearest highly trained monkey: {1}{1}{0}{1}{1}", objectReference.Type.FullName, objectReference.Type.GetHashCode()));

            sw.Close();
        }

        void WriteReference(object objectInstance, string name, ref StreamWriter sw)
        {
            ObjectReference reference = new ObjectReference(objectInstance);

            if (!reference.Type.IsPrimitive && !reference.Type.IsEnum && reference.Type != typeof(string))
            {
                // We can't simply write value.ToString() here, write to file and leave the path within brackets to reference it.
                if (!ObjectReferences.ContainsKey(reference))
                {
                    string targetDir = Path.Combine(PathUtils.GetScriptDumpFolder(), "Objects");
                    if (!Directory.Exists(targetDir))
                        Directory.CreateDirectory(targetDir);

                    targetDir = Path.Combine(targetDir, reference.Type.Namespace + "." + reference.Type.Name);
                    if (!Directory.Exists(targetDir))
                        Directory.CreateDirectory(targetDir);

                    var stream = File.Create(Path.Combine(targetDir, Directory.GetFiles(targetDir).Length.ToString()));

                    var serializer = new CrySerializer();
                    serializer.Serialize(stream, reference);

                    stream.Close();

                    if (ObjectReferences.ContainsKey(reference))
                        sw.WriteLine("{0}={1}", name, string.Format(ReferenceSeperator + "{0}" + ReferenceSeperator, ObjectReferences[reference]));
                }
                else // Reference already existed, write path to its script dump.
                    sw.WriteLine("{0}={1}", name, string.Format(ReferenceSeperator + "{0}" + ReferenceSeperator, ObjectReferences[reference]));
            }
            else
                sw.WriteLine("{0}={1}", name, objectInstance.ToString());
        }

        public ISurrogateSelector SurrogateSelector
        {
            get { return surrogateSelector; }
            set { surrogateSelector = value; }
        }
        public SerializationBinder Binder
        {
            get { return binder; }
            set { binder = value; }
        }
        public StreamingContext Context
        {
            get { return context; }
            set { context = value; }
        }

        internal class ObjectReference
        {
            public ObjectReference(object obj)
            {
                Object = obj;
                Type = obj.GetType();
            }

            public object Object { get; set; }
            public System.Type Type { get; set; }
        }

        /// <summary>
        /// Keep references to objects around as we can't print them in the same script dump as their declaring type, also avoids processing them multiple times.
        /// string = path to the object's state dump.
        /// </summary>
        internal static Dictionary<ObjectReference, string> ObjectReferences = new Dictionary<ObjectReference, string>();

        /// <summary>
        /// The character used to signal a reference path.
        /// </summary>
        internal static string ReferenceSeperator = "'";
    }

}