using System;

namespace CryEngine.Native
{
	[CLSCompliant(false)]
	public interface INativeViewMethods
	{
		uint GetView(uint linkedEntityId, bool forceCreate = false);
		void RemoveView(uint viewId);
		uint GetActiveView();
		void SetActiveView(uint viewId);
		ViewParams GetViewParams(uint viewId);
		void SetViewParams(uint viewId, ViewParams cam);
	}
}