using CryEngine;

namespace CryGameCode.FlowNodes.Testing
{
	[FlowNode(UICategory = "Samples",
		Description = "Testing node for various node operations",
		Category = FlowNodeCategory.Debug)]
	public class TestALLThePortTypes : BaseFlowNode
	{
		[InputPort(Name = "Activation Test", Description = "")]
		public void Activate() { ActivateOutput(activatedOutput); }

		[InputPort(Name = "Default Value Test", Description = "")]
		public void TestAll()
		{
			ActivateOutput(activatedOutput);
			ActivateOutput(intOutput, GetIntValue(IntInput));
			ActivateOutput(floatOutput, GetFloatValue(FloatInput));
			ActivateOutput(stringOutput, GetStringValue(StringInput));
			ActivateOutput(boolOutput, GetBoolValue(BoolInput));
			ActivateOutput(vec3Output, GetVec3Value(Vec3Input));
		}

		#region Data Inputs

		[InputPort(Name = "Integer Test", Description = "")]
		public void IntInput(int value) { ActivateOutput(intOutput, value); }

		[InputPort(Name = "Float Test", Description = "")]
		public void FloatInput(float value) { ActivateOutput(floatOutput, value); }

		[InputPort(Name = "Bool Test", Description = "")]
		public void BoolInput(bool value = true) { ActivateOutput(boolOutput, value); }

		[InputPort(Name = "String Test", Description = "")]
		public void StringInput(string value = "woo default value") { ActivateOutput(stringOutput, value); }

		[InputPort(Name = "Vec3 Test", Description = "")]
        public void Vec3Input(Vec3 value) { ActivateOutput(vec3Output, value); }

		#endregion

		#region Outputs

		[OutputPort(Name = "Activated Output", Description = "", Type = NodePortType.Void)]
		public static int activatedOutput;

		[OutputPort(Name = "Int Output", Description = "", Type = NodePortType.Int)]
		public static int intOutput;

		[OutputPort(Name = "Float Output", Description = "", Type = NodePortType.Float)]
		public static int floatOutput;

		[OutputPort(Name = "String Output", Description = "", Type = NodePortType.String)]
		public static int stringOutput;

		[OutputPort(Name = "Vec3 Output", Description = "", Type = NodePortType.Vec3)]
		public static int vec3Output;

		[OutputPort(Name = "Bool Output", Description = "", Type = NodePortType.Bool)]
		public static int boolOutput;

		#endregion
	}
}
