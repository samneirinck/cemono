using CryEngine;

namespace CryGameCode.FlowNodes.Samples
{
    [FlowNode(UICategory = "Samples", Description = "Reimplementation of multiplication in C#", Category = FlowNodeCategory.Approved)]
    public class Multiplier : FlowNode
    {
        [InputPort(Name = "Activate", Description = "Do the maths")]
        public void Activate()
        {
            ActivateOutput(answerOutput, GetFloatValue(LeftSide) * GetFloatValue(RightSide));
        }

        [InputPort(Name = "Left Side", Description = "The left side of the calculation")]
        public void LeftSide(float value) { }

        [InputPort(Name = "Right Side", Description = "The right side of the calculation")]
        public void RightSide(float value) { }

        [OutputPort(Name = "Answer", Description = "Get the answer", Type = NodePortType.Float)]
        public static int answerOutput;
    }
}