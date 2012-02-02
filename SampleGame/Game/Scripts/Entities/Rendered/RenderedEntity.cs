using CryEngine;

[Entity(Category = "Test", Icon = "")]
public class RenderTest : Entity
{
	[EditorProperty(Type = EntityPropertyType.Object)]
	public string Model { set { LoadObject(value); } }
}