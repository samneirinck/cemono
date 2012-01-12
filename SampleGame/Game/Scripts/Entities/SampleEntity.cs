using System;

using CryEngine;

namespace CryGameCode.Entities
{
	[Entity(Category="TestCategory", EditorHelper="Editor/Objects/anchor.cgf", Icon="", Flags=EntityClassFlags.Default)]
    public class SampleEntity : BaseEntity
    {
        public override void OnUpdate()
        {
        }

        public override void OnSpawn()
        {
            CryConsole.LogAlways("OnSpawn");
        }

        public override bool OnRemove()
        {
            return true;
        }

		//Floats/ints have optional constraints
		[EditorProperty(Description="How awesome is this entity?", Min=0, Max=9001)]
		public float awesomenessLevel;

		[EditorProperty(Type=EntityPropertyType.File)]
		public string fileSelector;

		[EditorProperty(Type=EntityPropertyType.Object)]
		public string objectSelector;

		[EditorProperty(Type=EntityPropertyType.Texture)]
		public string textureSelector;

		[EditorProperty(Type=EntityPropertyType.Sound)]
		public string soundSelector;

		[EditorProperty(Type=EntityPropertyType.Dialogue)]
		public string dialogueSelector;

		[EditorProperty(Type=EntityPropertyType.Color)]
		public Vec3 colorSelector;

		[EditorProperty]
		public Vec3 vectorTest;

		[EditorProperty(Type=EntityPropertyType.Sequence)]
		public string sequenceTest;

		[EditorProperty(Description="Is this entity epic?")]
		public bool isEpic;
    }
}