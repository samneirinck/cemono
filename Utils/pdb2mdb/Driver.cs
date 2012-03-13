//
// Driver.cs
//
// Author:
//   Jb Evain (jbevain@novell.com)
//
// (C) 2009 Novell, Inc. (http://www.novell.com)
//

using System.Collections.Generic;
using System.Reflection;
using System.IO;
using System.Linq;

using Microsoft.Cci;
using Microsoft.Cci.Pdb;

using Mono.Cecil;

using Mono.CompilerServices.SymbolWriter;
using System.Runtime.InteropServices;

using CryEngine;

namespace CryEngine
{
	partial class Console
	{
		[DllImport("CryMono.dll")]
		extern static void _LogAlways(string msg);
		[DllImport("CryMono.dll")]
		extern static void _Log(string msg);
		[DllImport("CryMono.dll")]
		extern static void _Warning(string msg);

		/// <summary>
		/// Logs a message to the console
		/// </summary>
		/// <param name="format"></param>
		/// <param name="args"></param>
		public static void Log(string format, params object[] args)
		{
			_Log(string.Format(format, args));
		}

		/// <summary>
		/// Logs a message to the console
		/// </summary>
		public static void Log(string msg)
		{
			_Log(msg);
		}

		/// <summary>
		/// Logs a message to the console, regardless of log_verbosity settings
		/// </summary>
		/// <param name="format"></param>
		/// <param name="args"></param>
		public static void LogAlways(string format, params object[] args)
		{
			_LogAlways(string.Format(format, args));
		}

		/// <summary>
		/// Logs a message to the console, regardless of log_verbosity settings
		/// </summary>
		public static void LogAlways(string msg)
		{
			_LogAlways(msg);
		}

		/// <summary>
		/// Logs an exception message to the console
		/// </summary>
		/// <remarks>Useful when exceptions are caught and data is still needed from them</remarks>
		/// <param name="ex"></param>
		public static void LogException(System.Exception ex)
		{
			Warning(ex.ToString());
		}

		/// <summary>
		/// Outputs a warning message
		/// </summary>
		/// <param name="format"></param>
		/// <param name="args"></param>
		public static void Warning(string format, params object[] args)
		{
			_Warning(string.Format(format, args));
		}

		/// <summary>
		/// Outputs a warning message
		/// </summary>
		public static void Warning(string msg)
		{
			_Warning(msg);
		}
	}
}

namespace Pdb2Mdb {

	class Converter {

		MonoSymbolWriter mdb;
		Dictionary<string, SourceFile> files = new Dictionary<string, SourceFile> ();

		public Converter (MonoSymbolWriter mdb)
		{
			this.mdb = mdb;
		}

		public static void Convert (AssemblyDefinition assembly, IEnumerable<PdbFunction> functions, MonoSymbolWriter mdb)
		{
			var converter = new Converter (mdb);

			foreach (var function in functions)
				converter.ConvertFunction (function);

			mdb.WriteSymbolFile(assembly.MainModule.Mvid);

			converter = null;
		}

		void ConvertFunction (PdbFunction function)
		{
			if (function.lines == null)
				return;

			var method = new SourceMethod { Name = function.name, Token = (int) function.token };

			var file = GetSourceFile (mdb, function);

			var builder = mdb.OpenMethod (file.CompilationUnit, 0, method);

			ConvertSequencePoints (function, file, builder);

			ConvertVariables (function);

			mdb.CloseMethod ();
		}

		void ConvertSequencePoints (PdbFunction function, SourceFile file, SourceMethodBuilder builder)
		{
			foreach (var line in function.lines.SelectMany (lines => lines.lines))
				builder.MarkSequencePoint (
					(int) line.offset,
					file.CompilationUnit.SourceFile,
					(int) line.lineBegin,
					(int) line.colBegin, line.lineBegin == 0xfeefee);
		}

		void ConvertVariables (PdbFunction function)
		{
			foreach (var scope in function.scopes)
				ConvertScope (scope);
		}

		void ConvertScope (PdbScope scope)
		{
			ConvertSlots (scope.slots);

			foreach (var s in scope.scopes)
				ConvertScope (s);
		}

		void ConvertSlots (IEnumerable<PdbSlot> slots)
		{
			foreach (var slot in slots)
				mdb.DefineLocalVariable ((int) slot.slot, slot.name);
		}

		SourceFile GetSourceFile (MonoSymbolWriter mdb, PdbFunction function)
		{
			var name = (from l in function.lines where l.file != null select l.file.name).First ();

			SourceFile file;
			if (files.TryGetValue (name, out file))
				return file;

			var entry = mdb.DefineDocument (name);
			var unit = mdb.DefineCompilationUnit (entry);

			file = new SourceFile (unit, entry);
			files.Add (name, file);
			return file;
		}

		class SourceFile : ISourceFile {
			CompileUnitEntry comp_unit;
			SourceFileEntry entry;

			public SourceFileEntry Entry
			{
				get { return entry; }
			}

			public CompileUnitEntry CompilationUnit
			{
				get { return comp_unit; }
			}

			public SourceFile (CompileUnitEntry comp_unit, SourceFileEntry entry)
			{
				this.comp_unit = comp_unit;
				this.entry = entry;
			}
		}

		class SourceMethod : IMethodDef {

			public string Name { get; set; }

			public int Token { get; set; }
		}
	}
}


public static class Driver
{
	public static void Convert(string assembly)
	{
		var pdb = Path.ChangeExtension(assembly, "pdb");

		// No need to warn about a missing pdb, just skip conversion.
		if (File.Exists(pdb))
		{
			var assemblyDefinition = AssemblyDefinition.ReadAssembly(assembly);

			try
			{
				using (var stream = File.OpenRead(pdb))
					Pdb2Mdb.Converter.Convert(assemblyDefinition, PdbFile.LoadFunctions(stream, true), new MonoSymbolWriter(assembly));
			}
			catch (System.Exception ex)
			{
				Console.LogException(ex);
			}

			assemblyDefinition = null;
		}

		pdb = null;
	}
}