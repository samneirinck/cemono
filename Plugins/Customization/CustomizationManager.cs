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
        public CustomizationManager(string availableAttachmentsDirectory = "Scripts/Config/Attachments", string baseCharacterDefinition = "Scripts/Config/base.cdf")
        {
            CharacterDefinitionLocation = Path.Combine(PathUtils.GameFolder, "Scripts/Config/MyCharacter.cdf");

            if (File.Exists(CharacterDefinitionLocation))
                CharacterDefinition = XDocument.Load(CharacterDefinitionLocation);
            else
            {
                File.Copy(Path.Combine(PathUtils.GameFolder, baseCharacterDefinition), CharacterDefinitionLocation);

                CharacterDefinition = XDocument.Load(CharacterDefinitionLocation);
            }

			CharacterAttachmentsLocation = Path.Combine(PathUtils.GameFolder, availableAttachmentsDirectory);
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

				slots.Add(new CharacterAttachmentSlot(attachmentSlotElement));
			}
        }

        public CharacterAttachmentSlot GetSlot(string slotName)
        {
            foreach (var slot in Slots)
            {
                if (slot.Name == slotName)
                    return slot;
            }

            return null;
        }

        public void Save(string alternateLocation = null)
        {
            if (alternateLocation == null)
                alternateLocation = CharacterDefinitionLocation;

            CharacterDefinition.Save(alternateLocation);
        }

        public IEnumerable<CharacterAttachmentSlot> Slots { get; set; }

        internal XDocument CharacterDefinition { get; set; }

        public string CharacterDefinitionLocation { get; set; }
        public string CharacterAttachmentsLocation { get; set; }

        Random Selector = new Random();

        static CustomizationManager _instance;
        public static CustomizationManager Instance
        {
            get
            {
                if (_instance == null)
                {
                    _instance = new CustomizationManager();
                    _instance.Initialize();
                }

                return _instance;
            }
        }
    }
}
