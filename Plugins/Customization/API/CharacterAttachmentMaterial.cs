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
	public class CharacterAttachmentMaterial
	{
		internal CharacterAttachmentMaterial(XElement element)
		{
			Element = element;

			var pathAttribute = element.Attribute("path");
			if (pathAttribute != null)
				BaseFilePath = pathAttribute.Value;

			Debug.LogAlways("Found material {0}", BaseFilePath);

			var colorModifierElement = element.Element("ColorModifier");
			if (colorModifierElement != null)
			{
				var redAttribute = colorModifierElement.Attribute("red");
				if (redAttribute != null)
					ColorRed = Vec3.Parse(redAttribute.Value) / 255;

				var greenAttribute = colorModifierElement.Attribute("green");
				if (greenAttribute != null)
					ColorGreen = Vec3.Parse(greenAttribute.Value) / 255;

				var blueAttribute = colorModifierElement.Attribute("blue");
				if (blueAttribute != null)
					ColorBlue = Vec3.Parse(blueAttribute.Value) / 255;

				var alphaAttribute = colorModifierElement.Attribute("alpha");
				if (alphaAttribute != null)
					ColorAlpha = Vec3.Parse(alphaAttribute.Value) / 255;
			}

			var diffuseElement = element.Element("Diffuse");
			if (diffuseElement != null)
			{
				var texPathAttribute = diffuseElement.Attribute("path");
				if (texPathAttribute != null)
					DiffuseTexture = texPathAttribute.Value;
			}

			var specularElement = element.Element("Specular");
			if (specularElement != null)
			{
				var texPathAttribute = specularElement.Attribute("path");
				if (texPathAttribute != null)
					SpecularTexture = texPathAttribute.Value;
			}

			var bumpmapElement = element.Element("Bumpmap");
			if (bumpmapElement != null)
			{
				var texPathAttribute = bumpmapElement.Attribute("path");
				if (texPathAttribute != null)
					BumpmapTexture = texPathAttribute.Value;
			}

			var customTexElement = element.Element("Custom");
			if (customTexElement != null)
			{
				var texPathAttribute = customTexElement.Attribute("path");
				if (texPathAttribute != null)
					CustomTexture = texPathAttribute.Value;
			}

			Save();
		}

		public void Save()
		{
			// Used to determine whether we need to save to an alternate location.
			bool modifiedMaterial = false;

			var basePath = Path.Combine(CryPak.GameFolder, BaseFilePath);
			if (!File.Exists(basePath + ".mtl"))
				throw new CustomizationConfigurationException(string.Format("Could not save modified material, base {0} did not exist.", basePath));

			var materialDocument = XDocument.Load(basePath + ".mtl");
			var materialElement = materialDocument.Element("Material");

			var genMaskAttribute = materialElement.Attribute("StringGenMask");
			if (genMaskAttribute != null && genMaskAttribute.Value.Contains("%COLORMASKING"))
			{
				var publicParamsElement = materialElement.Element("PublicParams");

				publicParamsElement.SetAttributeValue("ColorMaskR", ColorRed.ToString());
				publicParamsElement.SetAttributeValue("ColorMaskG", ColorGreen.ToString());
				publicParamsElement.SetAttributeValue("ColorMaskB", ColorBlue.ToString());
				publicParamsElement.SetAttributeValue("ColorMaskA", ColorAlpha.ToString());

				modifiedMaterial = true;
			}

			var texturesElement = materialElement.Element("Textures");
			if (texturesElement != null)
			{
				if (WriteTexture(texturesElement, "Diffuse", DiffuseTexture)
					|| WriteTexture(texturesElement, "Specular", SpecularTexture)
					|| WriteTexture(texturesElement, "Bumpmap", BumpmapTexture)
					|| WriteTexture(texturesElement, "Custom", CustomTexture))
				{
					modifiedMaterial = true;
				}
			}

			if (modifiedMaterial)
			{
				if (string.IsNullOrEmpty(FilePath))
				{
					var chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
					var stringChars = new char[16];

					for (int i = 0; i < stringChars.Length; i++)
						stringChars[i] = chars[CustomizationManager.Selector.Next(chars.Length)];

					var fileName = new string(stringChars);
					Debug.LogAlways("{0} generated {1}", BaseFilePath, fileName);

					FilePath = Path.Combine("%USER%", "Cosmetics", "Materials", fileName);
				}

				var fullFilePath = CryPak.AdjustFileName(FilePath, PathResolutionRules.RealPath | PathResolutionRules.ForWriting) + ".mtl";

				if (!File.Exists(fullFilePath))
				{
					var directory = new DirectoryInfo(Path.GetDirectoryName(fullFilePath));
					while (!directory.Exists)
					{
						Directory.CreateDirectory(directory.FullName);

						directory = Directory.GetParent(directory.FullName);
					}

					var file = File.Create(fullFilePath);
					file.Close();
				}

				Debug.LogAlways("Writing {0} to {1}", BaseFilePath, fullFilePath);
				materialDocument.Save(fullFilePath);
			}
			else
			{
				Debug.LogAlways("Material {0} was not modified, using original path", BaseFilePath);
				FilePath = BaseFilePath;
			}
		}

		bool WriteTexture(XElement texturesElement, string textureType, string texturePath)
		{
			if (texturePath == null)
				return false;

			var element = texturesElement.Elements("Texture").FirstOrDefault(x => x.Attribute("Map").Value == textureType);
			if (element == null)
			{
				element = new XElement("Texture");

				element.SetAttributeValue("Map", textureType);
				element.SetAttributeValue("File", texturePath);
			}
			else
			{
				element.SetAttributeValue("File", texturePath);

				texturesElement.SetElementValue("Texture", element);
			}

			return true;
		}

		public Vec3 ColorRed { get; set; }
		public Vec3 ColorGreen { get; set; }
		public Vec3 ColorBlue { get; set; }
		public Vec3 ColorAlpha { get; set; }

		public string DiffuseTexture { get; set; }
		public string SpecularTexture { get; set; }
		public string BumpmapTexture { get; set; }
		public string CustomTexture { get; set; }

		/// <summary>
		/// Path to the mtl file.
		/// </summary>
		public string FilePath { get; set; }
		/// <summary>
		/// Path to the mtl file this material is based on.
		/// </summary>
		public string BaseFilePath { get; set; }

		public XElement Element { get; set; }
	}
}
