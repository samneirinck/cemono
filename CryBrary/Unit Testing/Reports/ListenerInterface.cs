
namespace CryEngine.Testing
{
	// TODO: Add autodetection for CryBrary-loaded types
	
	/// <summary>
	/// Represents an object that is capable of receiving unit test reports.
	/// </summary>
	public interface IReportListener
	{
		/// <summary>
		/// Called when the unit tester has finished a run.
		/// </summary>
		/// <param name="report">The test data.</param>
		void OnTestsRun(TestReport report);
	}
}
