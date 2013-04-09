using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using System.Text;

namespace CryEngine.CharacterCustomization
{
	public class CharacterAttachment
	{
		internal CharacterAttachment(CharacterAttachmentSlot slot, XElement element, bool child = false)
		{
			Element = element;
			Slot = slot;

			if (element == null)
				Name = "None";
			else
			{
				var slotAttachmentNameAttribute = element.Attribute("Name");
				if (slotAttachmentNameAttribute != null)
					Name = slotAttachmentNameAttribute.Value;

				var slotAttachmentTypeAttribute = element.Attribute("Type");
				if (slotAttachmentTypeAttribute != null)
					Type = slotAttachmentTypeAttribute.Value;

				var slotAttachmentBoneNameAttribute = element.Attribute("BoneName");
				if (slotAttachmentBoneNameAttribute != null)
					BoneName = slotAttachmentBoneNameAttribute.Value;

				var slotAttachmentObjectAttribute = element.Attribute("Binding");
				if (slotAttachmentObjectAttribute != null)
					Object = slotAttachmentObjectAttribute.Value;

				var slotAttachmentFlagsAttribute = element.Attribute("Flags");
				if (slotAttachmentFlagsAttribute != null)
					Flags = slotAttachmentFlagsAttribute.Value;

				var slotAttachmentPositionAttribute = element.Attribute("Position");
				if (slotAttachmentPositionAttribute != null)
					Position = slotAttachmentPositionAttribute.Value;

				var slotAttachmentRotationAttribute = element.Attribute("Rotation");
				if (slotAttachmentRotationAttribute != null)
					Rotation = slotAttachmentRotationAttribute.Value;

				var slotAttachmentMaterials = new List<string>();

				foreach (var materialVariation in element.Elements("Material"))
					slotAttachmentMaterials.Add(materialVariation.Attribute("path").Value);

				MaterialVariations = slotAttachmentMaterials.ToArray();
				if (MaterialVariations.Length != 0)
					Material = MaterialVariations.First();

				if (!child)
				{
					var subCharacterAttachments = new List<CharacterAttachment>();

					foreach (var subAttachmentElement in element.Elements("SubAttachment"))
					{
						var subAttachmentSlot = Slot.SubAttachmentSlots.First(x => x.Name == subAttachmentElement.Attribute("Slot").Value);

						subCharacterAttachments.Add(new CharacterAttachment(subAttachmentSlot, subAttachmentElement, true));
					}

					SubAttachmentVariations = subCharacterAttachments.ToArray();
					SubAttachment = SubAttachmentVariations.FirstOrDefault();
				}

				if (slot.MirroredSlots != null)
				{
					MirroredChildren = new CharacterAttachment[slot.MirroredSlots.Length];
					for (int i = 0; i < slot.MirroredSlots.Length; i++)
					{
						var mirroredSlot = slot.MirroredSlots.ElementAt(i);
						var mirroredAttachmentElement = element.Element(mirroredSlot.Name);
						if (mirroredAttachmentElement == null)
							throw new CustomizationConfigurationException(string.Format("Failed to get mirrored element from slot {0} and name {1}", slot.Name, mirroredSlot.Name));

						MirroredChildren[i] = new CharacterAttachment(mirroredSlot, mirroredAttachmentElement);
					}
				}
			}
		}

		public string RandomMaterial
		{
			get
			{
				if (MaterialVariations == null || MaterialVariations.Length == 0)
					return Material;

				var selector = new Random();

				return MaterialVariations.ElementAt(selector.Next(MaterialVariations.Length));
			}
		}

		public CharacterAttachment RandomSubAttachment
		{
			get
			{
				if (SubAttachmentVariations == null || SubAttachmentVariations.Length == 0)
					return SubAttachment;

				var selector = new Random();

				return SubAttachmentVariations.ElementAt(selector.Next(SubAttachmentVariations.Length));
			}
		}

		public CharacterAttachmentSlot Slot { get; set; }

		public string Name { get; set; }

		public string Type { get; set; }
		public string BoneName { get; set; }

		public string Object { get; set; }

		public string Material { get; set; }
		public string[] MaterialVariations { get; set; }

		public string Flags { get; set; }

		public string Position { get; set; }
		public string Rotation { get; set; }

		public CharacterAttachment[] SubAttachmentVariations { get; set; }
		public CharacterAttachment SubAttachment { get; set; }

		// Only used when mirroring
		public CharacterAttachment[] MirroredChildren { get; set; }

		internal XElement Element { get; private set; }
	}
}