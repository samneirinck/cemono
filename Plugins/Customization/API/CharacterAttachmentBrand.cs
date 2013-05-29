using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Linq;

using CryEngine;


namespace CryEngine.CharacterCustomization
{
	public class CharacterAttachmentBrand
	{
		internal CharacterAttachmentBrand(CharacterAttachmentSlot slot, XElement element)
		{
			Slot = slot;

			// Set up strings
			var brandNameAttribute = element.Attribute("Name");
			if (brandNameAttribute != null)
				Name = brandNameAttribute.Value;

			var brandThumbnailAttribute = element.Attribute("Thumbnail");
			if (brandThumbnailAttribute != null)
				ThumbnailPath = brandThumbnailAttribute.Value;

			// Get attachments
			var slotAttachmentElements = element.Elements("Attachment");

			int count = slotAttachmentElements.Count();
			if (Slot.CanBeEmpty)
				count++;

			var slotAttachments = new CharacterAttachment[count];

			for (int i = 0; i < slotAttachmentElements.Count(); i++)
			{
				var slotAttachmentElement = slotAttachmentElements.ElementAt(i);

				slotAttachments[i] = new CharacterAttachment(Slot, slotAttachmentElement);
			}

			if (Slot.CanBeEmpty)
				slotAttachments[slotAttachmentElements.Count()] = new CharacterAttachment(Slot, null);

			Attachments = slotAttachments;
		}

		public string Name { get; private set; }
		public string ThumbnailPath { get; private set; }

		public CharacterAttachment[] Attachments { get; private set; }

		public CharacterAttachmentSlot Slot { get; private set; }
	}
}
