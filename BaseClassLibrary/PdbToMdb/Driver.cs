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

using CryEngine;

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

	public static class Driver 
	{
		public static void Convert(Assembly assembly)
		{
			Convert(assembly.Location);
		}
		
		public static void Convert(string assembly)
		{
			try
			{
				// Remove the existing file to prevent annoying Win32 exceptions.
				string mdbFile = assembly + ".mdb";
				if (File.Exists(mdbFile))
					File.Delete(mdbFile);
			}
			catch (System.Exception ex) { Console.LogException(ex); }

			var assemblyDefinition = AssemblyDefinition.ReadAssembly(assembly);

			var pdb = assembly.Replace(".dll", ".pdb");

			// No need to warn about a missing pdb, just skip conversion.
			if (File.Exists(pdb))
			{
				using (var stream = File.OpenRead(pdb))
				{
					try
					{
						Converter.Convert(assemblyDefinition, PdbFile.LoadFunctions(stream, true), new MonoSymbolWriter(assembly));
					}
					catch (System.Exception e)
					{
						Console.LogException(e);
					}

					stream.Close();
				}
			}

			pdb = null;
			assemblyDefinition = null;
		}
	}
}
