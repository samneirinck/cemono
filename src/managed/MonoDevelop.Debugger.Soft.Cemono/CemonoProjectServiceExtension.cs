using System;
using MonoDevelop.Projects;
using MonoDevelop.Ide;

namespace MonoDevelop.Debugger.Soft.Cemono
{
	public class CemonoProjectServiceExtension : ProjectServiceExtension
	{
		protected override bool CanExecute (SolutionEntityItem item, ExecutionContext context, ConfigurationSelector configuration)
		{
			return true;
		}
		
		public override void Execute (MonoDevelop.Core.IProgressMonitor monitor, IBuildTarget item, ExecutionContext context, ConfigurationSelector configuration)
		{
		}
	}
}

