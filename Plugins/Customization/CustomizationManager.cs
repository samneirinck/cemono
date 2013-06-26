using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Xml;
using System.Xml.Linq;

using CryEngine;
using CryEngine.Utilities;

namespace CryEngine.CharacterCustomization
{
    public class CustomizationManager
    {
		public CustomizationManager(string characterDefinitionLocation, string baseCharacterDefinition = "Scripts/Config/base.cdf", string availableAttachmentsDirectory = "Scripts/Config/Attachments")
        {
			var writeableCdfPath = CryPak.AdjustFileName(characterDefinitionLocation, PathResolutionRules.RealPath | PathResolutionRules.ForWriting);
			CharacterDefinitionLocation = characterDefinitionLocation;

			if (File.Exists(writeableCdfPath))
				CharacterDefinition = XDocument.Load(writeableCdfPath);
            else
            {
				var directory = new DirectoryInfo(Path.GetDirectoryName(writeableCdfPath));
				while (!directory.Exists)
				{
					Directory.CreateDirectory(directory.FullName);

					directory = Directory.GetParent(directory.FullName);
				}

				File.Copy(Path.Combine(CryPak.GameFolder, baseCharacterDefinition), writeableCdfPath);

				CharacterDefinition = XDocument.Load(writeableCdfPath);
            }

			CharacterAttachmentsLocation = Path.Combine(CryPak.GameFolder, availableAttachmentsDirectory);

			Initialize();
        }

		void Initialize()
        {
            var slots = new List<CharacterAttachmentSlot>();
            Slots = slots;

			foreach (var file in Directory.EnumerateFiles(CharacterAttachmentsLocation, "*.xml"))
			{
				var xDocument = XDocument.Load(file);
				if (xDocument == null)
					continue;

				var attachmentSlotElement = xDocument.Element("AttachmentSlot");
				if (attachmentSlotElement == null)
					continue;

				slots.Add(new CharacterAttachmentSlot(this, attachmentSlotElement));
			}
        }

        public CharacterAttachmentSlot GetSlot(string slotName)
        {
            foreach (var slot in Slots)
            {
                if (slot.Name.Equals(slotName, StringComparison.CurrentCultureIgnoreCase))
                    return slot;
            }

            return null;
        }

        public void Save()
        {
			CharacterDefinition.Save(CryPak.AdjustFileName(CharacterDefinitionLocation, PathResolutionRules.RealPath | PathResolutionRules.ForWriting));
        }

        public IEnumerable<CharacterAttachmentSlot> Slots { get; set; }

        internal XDocument CharacterDefinition { get; set; }

        public string CharacterDefinitionLocation { get; set; }
        public string CharacterAttachmentsLocation { get; set; }

        internal static Random Selector = new Random();
    }
}
