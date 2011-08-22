///////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   WinBase.cpp
//  Version:     v1.00 
//  Created:     10/3/2004 by Michael Glueck
//  Compilers:   GCC
//  Description: Linux port support for Win32API calls
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////





#include <CryAssert.h>
#if !defined(PS3)
	#include <signal.h>
#endif

#include <pthread.h>
#include <sys/types.h>
#include <fcntl.h>




#if !defined(PATH_MAX)
	#define PATH_MAX MAX_PATH
#endif

#include <sys/time.h>


































#if defined(LINUX)
#include <sys/types.h>
#include <unistd.h>
#include <utime.h>
#include <dirent.h>
#include "CryLibrary.h"
#endif


// File I/O compatibility macros


































#if defined(LINUX)
	#define FS_ERRNO_TYPE int
	#define FS_ENOENT ENOENT
	#define FS_EINVAL EINVAL
  #define FS_EISDIR EISDIR
	#define FS_DIRENT_TYPE struct dirent
	#define FS_DIR_TYPE DIR *
	#define FS_DIR_NULL (NULL)
	#define FS_STAT_TYPE struct stat64
	#define FS_CLOSEDIR(dir, err) \
		while (true) \
		{ \
			if (closedir((dir)) == 0) \
				(err) = 0; \
			else \
				(err) = errno; \
			break; \
		}
	#define FS_CLOSEDIR_NOERR(dir) (closedir((dir)))
	#define FS_OPENDIR(dirname, dir, err) \
		while (true) \
		{ \
			if (((dir) = opendir((dirname))) == NULL) \
				(err) = errno; \
			else \
				(err) = 0; \
			break; \
		}
	#define FS_READDIR(dir, ent, entsize, err) \
		while (true) \
		{ \
			struct dirent *pDirent = readdir((dir)); \
			if (pDirent == NULL) \
			{ \
				(entsize) = 0; \
				(err) = (errno == ENOENT) ? 0 : errno; \
			} \
			else \
			{ \
				(ent) = *pDirent; \
				(entsize) = sizeof(struct dirent); \
				(err) = 0; \
			} \
			break; \
		}
	#define FS_TYPE_DIRECTORY DT_DIR
	#define FS_STAT(filename, buffer, err) \
		while (true) \
		{ \
			if (stat64((filename), &(buffer)) == -1) \
				(err) = errno; \
			else \
				(err) = 0; \
			break; \
		}
	#define FS_FSTAT(fd, buffer, err) \
		while (true) \
		{ \
			if (fstat64((fd), &(buffer)) == -1) \
				(err) = errno; \
			else \
				(err) = 0; \
			break; \
		}
	#define FS_OPEN(filename, flags, fd, mode, err) \
		while (true) \
		{ \
			if (((fd) = open((filename), (flags), (mode))) == -1) \
				(err) = errno; \
			else \
				(err) = 0; \
			break; \
		}
	#define FS_CLOSE(fd, err) \
		while (true) \
		{ \
			if (close((fd)) == -1) \
				(err) = errno; \
			else \
				(err) = 0; \
			break; \
		}
	#define FS_CLOSE_NOERR(fd) (close((fd)))
	#define FS_O_RDWR O_RDWR
	#define FS_O_RDONLY O_RDONLY
	#define FS_O_WRONLY O_WRONLY
#endif

#if defined(PS3) && (!defined(_RELEASE) || defined(_DEBUG))
	struct SAssertData
	{
		int line;
		char fileName[256-sizeof(int)];
		const bool operator==(const SAssertData& crArg) const
		{
			return crArg.line == line && (strcmp(fileName, crArg.fileName) == 0);
		}

		const bool operator<(const SAssertData& crArg) const
		{
			if(line == crArg.line)
				return strcmp(fileName, crArg.fileName) < 0;
			else
				return line < crArg.line;
		}

		SAssertData() : line(-1){}
		SAssertData(const int cLine, const char* cpFile) : line(cLine)
		{
			strcpy(fileName, cpFile);
		}

		SAssertData(const SAssertData& crAssertData)
		{
			memcpy((void*)this, &crAssertData, sizeof(SAssertData));
		}

		void operator=(const SAssertData& crAssertData)
		{
			memcpy((void*)this, &crAssertData, sizeof(SAssertData));
		}
	};

//	#define OUTPUT_ASSERT_TO_FILE

	void HandleAssert(const char* cpMessage, const char* cpFunc, const char* cpFile, const int cLine)
	{
#if defined(OUTPUT_ASSERT_TO_FILE)
		static FILE *pAssertLogFile = cellFsOpen(SYS_APP_HOME"/Assert.log", "w+");
#endif
		bool report = true;
		static std::set<SAssertData> assertSet;
		SAssertData assertData(cLine, cpFile);
		if(!g_EnableMultipleAssert)
		{
			std::set<SAssertData>::const_iterator it = assertSet.find(assertData);
			if(it != assertSet.end())
				report = false;
			else
				assertSet.insert(assertData);
		}
		else
			assertSet.insert(assertData);
		if(report)
		{
			//added function to be able to place a breakpoint here or to print out to other consoles
			printf("ASSERT: %s in %s (%s : %d)\n",cpMessage, cpFunc, cpFile, cLine);
#if defined(OUTPUT_ASSERT_TO_FILE)
			if(pAssertLogFile)
			{
				fprintf(pAssertLogFile, "ASSERT: %s in %s (%s : %d)\n",cpMessage, cpFunc, cpFile, cLine);
				fflush(pAssertLogFile);
			}
#endif
		}
	}
#endif








#if defined LINUX
	extern size_t __attribute__((visibility("default"))) fopenwrapper_basedir_maxsize;
	extern char * fopenwrapper_basedir __attribute__((visibility("default")));
	extern bool __attribute__((visibility("default"))) fopenwrapper_trace_fopen;
#endif


void InitFOpenWrapper()
{









}












































































	const char* const ConvertFileName(char *const pBuf, const char* const cpName)
	{
		// Not supported outside of CryEngine.
		return NULL;
	}


#if defined(USE_FILE_MAP)
#define FILE_MAP_FILENAME "files.list"
struct FileNode
{
	int m_nEntries;
	int m_nIndex;							// Index relative to the parent, -1 for root.
	char *m_Buffer;			      // Buffer holding all entry strings, separated by
														// null characters.
	const char **m_Entries;		// Node entries, sorted in strcmp() order.
	FileNode **m_Children;    // Children of the node. NULL for non-directories.
	bool m_bDirty;						// Flag indicating that the node has been modified.

	FileNode();
	~FileNode();
	void Dump(int indent = 0);
	int Find(const char *);
	int FindExact(const char *);

	static FileNode *ReadFileList(FILE * = NULL, int = -1);
	static FileNode *BuildFileList(const char *prefix = NULL);
	static void Init(void)
	{
		m_FileTree = ReadFileList();
		if (m_FileTree == NULL)
			m_FileTree = BuildFileList();
	}
	static FileNode *GetTree(void) { return m_FileTree; }
	static FileNode *FindExact(
			const char *, int &, bool * = NULL, bool = false);
	static FileNode *Find(
			const char *, int &, bool * = NULL, bool = false);
	static bool CheckOpen(const char *, bool = false, bool = false);

	static FileNode *m_FileTree;

	struct Entry
	{
		string Name;
		FileNode *Node;
		Entry(const string &Name, FileNode *const Node)
			: Name(Name), Node(Node)
		{ }
	};

	struct EntryCompare
	{
		bool operator () (const Entry &op1, const Entry &op2)
		{
			return op1.Name < op2.Name;
		}
	};
};

FileNode *FileNode::m_FileTree = NULL;

FileNode::FileNode()
	: m_nEntries(0), m_nIndex(-1), m_Buffer(NULL),
	  m_Entries(NULL), m_Children(NULL), m_bDirty(false)
{ }

FileNode::~FileNode()
{
	for (int i = 0; i < m_nEntries; ++i)
	{
		if (m_Children[i]) delete m_Children[i];
	}
	delete[] m_Children;
	delete[] m_Entries;
	delete[] m_Buffer;
}

void FileNode::Dump(int indent)
{
	for (int i = 0; i < m_nEntries; ++i)
	{
		for (int j = 0; j < indent; ++j) putchar(' ');
		printf("%s%s\n", m_Entries[i], m_Children[i] ? "/" : "");
		if (m_Children[i]) m_Children[i]->Dump(indent + 2);
	}
}

// Find the index of the element with the specified name (case insensitive).
// The method returns -1 if the element is not found.
int FileNode::Find(const char *name)
{
	const int n = m_nEntries;

	for (int i = 0; i < n; ++i)
	{
		if (!strcasecmp(name, m_Entries[i]))
			return i;
	}
	return -1;
}

// Find the index of the element with the specified name (case sensitive).
// The method returns -1 if the element is not found.
int FileNode::FindExact(const char *name)
{
	int i, j, k;
	const int n = m_nEntries;
	int cmp;

	if (!n) return -1;
	for (i = 0, j = n / 2, k = n & 1;; k = j & 1, j /= 2) {
		cmp = strcmp(m_Entries[i + j], name);
		if (cmp == 0) return i + j;
		if (cmp < 0) i += j + k;
		if (!j) break;
	}
	return -1;
}

// Find the node for the specified file name. The method performs a case
// sensitive search.
// The name parameter must be absolute (i.e. start with a '/').
// The method returns the node associated with the directory containing the
// specified file or directory. The index of the requested file is stored into
// &index.
// The skipInitial flag indicates if the initial path component should be
// skipped (e.g. "/work" or "/app_home").
// If the containing directory node is found, but the requested file is not,
// then the method returns the node of the containing directory and sets
// index to -1.
// If containing directory is not found, then the method returns NULL and sets
// index to -1.
// If the file and/or containing directory is not found and dirty parameter is
// not NULL, then a flag value will be written to *dirty indicating if the
// search failed on a dirty node. If the failing node is dirty, then the
// requested file might have been created at runtime.
FileNode *FileNode::FindExact(
		const char *name,
		int &index,
		bool *dirty,
		bool skipInitial
		)
{
	const char *sep;
	FileNode *node = m_FileTree;

	assert(name[0] == '/');
	index = -1;
	if (node == NULL)
	{
		// No node tree present.
		if (dirty != NULL)
			*dirty = true;
		node = NULL;
	}
	if (skipInitial)
	{
		do ++name; while (*name && *name != '/');
		assert(!name[0] || name[0] == '/');
	}
	while (*name == '/') ++name;
	if (!name[0])
	{
		// Root directory requested.
		node = NULL;
	}
	while (node != NULL)
	{
		if (dirty != NULL)
			*dirty = node->m_bDirty;
		for (sep = name; *sep && *sep != '/'; ++sep);
		char component[sep - name + 1];
		memcpy(component, name, sep - name);
		component[sep - name] = 0;
		index = node->FindExact(component);
		name = sep;
		while (*name == '/') ++name;
		if (!name[0]) break;
		if (index != -1)
			node = node->m_Children[index];
		else
			node = NULL;
	}
	if (node == NULL)
		index = -1;
	return node;
}

FileNode *FileNode::Find(
		const char *name,
		int &index,
		bool *dirty,
		bool skipInitial
		)
{
	abort();
	// FIXME: Implement
	index = -1;
	return NULL;
}

// Check if a file open operation can be successful.
// The create flag specified if file creation is requested.
// The skipInitial flag indicates if the initial path component should be
// skipped (e.g. "/work" or "/app_home").
bool FileNode::CheckOpen(
		const char *filename,
		bool create,
		bool skipInitial
		)
{
	int index = -1;
	FileNode *node = NULL;
	bool dirty = false;
	bool fail = false;

	node = FileNode::FindExact(filename, index, &dirty, skipInitial);
	if (node == NULL && !dirty)
	{
		// The containing directory does not exist.
		fail = true;
	} else if (!create && index == -1 && !dirty)
	{
		// The file does not exist and file creation is not requested.
		fail = true;
	} else if (index == -1 && create && node != NULL && !dirty)
	{
		// The file does not exist and file creation *is* requested. We'll mark
		// the containing directory dirty.
		node->m_bDirty = true;
	}
	return !fail;
}











FileNode *FileNode::ReadFileList(FILE *in, int index)
{
	char line[256];



	static const char listFilename[] = FILE_MAP_FILENAME;

	int i, count = 0, size = 0;
	const int maxCount = 4096, maxSize = 256 * 4096;
	FileNode *node = NULL;
	bool closeFile = false;
	
	if (in == NULL)
	{
		in = fopen(listFilename, "r");
		if (in == NULL)
		{
#if !defined(LINUX)
			fprintf(stderr, "can't open file map '%s' for reading: %s\n",
					listFilename, strerror(errno));
#endif
			return NULL;
		}
		closeFile = true;
	}
	if (fgets(line, sizeof line, in) == NULL)
	{
		fprintf(stderr, "unexpected EOF in file map '%s'\n", listFilename);
		fclose(in);
		return NULL;
	}
	line[sizeof line - 1] = 0;
	if (sscanf(line, "%i,%i", &count, &size) != 2
			|| count < 0 || count > maxCount
			|| size < 0 || size > maxSize)
	{
		fprintf(stderr, "syntax error in file map '%s'\n", listFilename);
		fclose(in);
		return NULL;
	}
	node = new FileNode;
	node->m_nEntries = count;
	node->m_nIndex = index;
	node->m_Buffer = new char[size];
	node->m_Entries = new const char *[count];
	node->m_Children = new FileNode *[count];
	char *buffer = node->m_Buffer;
	for (i = 0; i < count; ++i)
	{
		if (fgets(line, sizeof line, in) == NULL)
		{
			fprintf(stderr, "unexpected EOF in file map '%s'\n", listFilename);
			fclose(in);
			delete node;
			return NULL;
		}
		int lineLen = strlen(line);
		while (lineLen > 0 && isspace(line[lineLen - 1])) --lineLen;
		line[lineLen] = 0;
		char *p = strchr(line, '/');
		if (p && p[1])
		{
			fprintf(stderr, "syntax error in file map '%s'\n", listFilename);
			fclose(in);
			delete node;
			return NULL;
		}
		bool isDir = false;
		if (p) 
		{
			*p = 0;
			isDir = true;
			--lineLen;
		}
		if (lineLen + (buffer - node->m_Buffer) >= size)
		{
			fprintf(stderr, "broken file map '%s'\n", listFilename);
			fclose(in);
			delete node;
			return NULL;
		}
		memcpy(buffer, line, lineLen + 1);
		node->m_Entries[i] = buffer;
		buffer += lineLen + 1;
		if (isDir)
		{
			node->m_Children[i] = ReadFileList(in, i);
			if (node->m_Children[i] == NULL)
			{
				fclose(in);
				delete node;
				return NULL;
			}
		}
	}
	if (buffer - node->m_Buffer != size)
	{
		fprintf(stderr, "broken file map '%s'\n", listFilename);
		fclose(in);
		delete node;
		return NULL;
	}
	if (closeFile)
		fclose(in);
	return node;
}

FileNode *FileNode::BuildFileList(const char *prefix)
{
	FileNode *node = NULL;
	FS_DIR_TYPE dir = FS_DIR_NULL;
	FS_ERRNO_TYPE err = 0;
	char curdir[PATH_MAX + 1];
	bool addPrefix = false;

	if (prefix == NULL)
	{
		// Start with the current working directory.
		GetCurrentDirectory(sizeof curdir - 1, curdir);
		curdir[sizeof curdir - 1] = 0;
		prefix = curdir;
		addPrefix = true;
	}
	FS_OPENDIR(prefix, dir, err);
	if (dir == FS_DIR_NULL || err != 0)
	{
		fprintf(stderr, "warning: can't access directory '%s': %s\n",
				prefix, strerror(err));
		return NULL;
	}
	std::vector<Entry> entryVec;
	size_t nameLengthTotal = 0;
	while (true)
	{
		FS_DIRENT_TYPE entry;
		size_t entrySize = 0;
		FS_READDIR(dir, entry, entrySize, err);
		if(err)
		{
			FS_CLOSEDIR_NOERR(dir);
			return NULL;
		}
		if (entrySize == 0)
			break;
		if (!strcmp(entry.d_name, ".") || !strcmp(entry.d_name, ".."))
			continue;
		FileNode *subNode = NULL;
		if (entry.d_type == FS_TYPE_DIRECTORY)
		{
			char path[PATH_MAX + 1];
			snprintf(path, sizeof path - 1, "%s/%s", prefix, entry.d_name);
			path[sizeof path - 1] = 0;
			subNode = BuildFileList(path);
			if (subNode == NULL)
				continue;
		}
		nameLengthTotal += strlen(entry.d_name);
		entryVec.push_back(Entry(entry.d_name, subNode));
	}
	FS_CLOSEDIR_NOERR(dir);
	const int count = entryVec.size();
	node = new FileNode;
	node->m_nEntries = count;
	if (count > 0)
	{
		std::sort(entryVec.begin(), entryVec.end(), EntryCompare());
		node->m_Buffer = new char[nameLengthTotal + count];
		node->m_Entries = new const char *[count];
		node->m_Children = new FileNode *[count];
		char *buffer = node->m_Buffer;
		for (int i = 0; i < count; ++i)
		{
			strcpy(buffer, entryVec[i].Name.c_str());
			node->m_Entries[i] = buffer;
			buffer += strlen(buffer) + 1;
			FileNode *subNode = entryVec[i].Node;
			node->m_Children[i] = subNode; // NULL for non-directories.
			if (subNode != NULL)
				subNode->m_nIndex = i;
		}
		assert(buffer - node->m_Buffer == nameLengthTotal + count);
	}
	if (addPrefix && strcmp(prefix, "/"))
	{
		// Add the file nodes for the default prefix (current working directory).
		// The nodes are marked as dirty, so the on disk directories are scanned
		// whenever one of these directories is accessed.
		const char *p, *q;
		assert(prefix[0] == '/');
		FileNode *rootNode = NULL, *x = NULL;
		for (p = q = prefix + 1;; ++p)
		{
			if (*p == '/' || *p == 0)
			{
				if (rootNode == NULL)
				{
					rootNode = new FileNode;
					x = rootNode;
					rootNode->m_nIndex = -1;
				}
				else
				{
					assert(x != NULL);
					x->m_Children[0] = new FileNode;
					x = x->m_Children[0];
					x->m_nIndex = 0;
				}
				x->m_nEntries = 1;
				x->m_Children = new FileNode *[1];
				x->m_bDirty = true;
				x->m_Buffer = new char[p - q + 1];
				memcpy(x->m_Buffer, q, p - q);
				x->m_Buffer[p - q] = 0;
				x->m_Entries = new const char *[1];
				x->m_Entries[0] = x->m_Buffer;
				q = p + 1;
			}
			if (*p == 0)
				break;
		}
		x->m_Children[0] = node;
		node->m_nIndex = 0;
		node = rootNode;
	}
	return node;
}

void InitFileList(void)
{
	FileNode::Init();






}
#else
void InitFileList(void) { }
#endif

inline void WrappedF_InitCWD()
{
#if defined(LINUX)
	if (getcwd(fopenwrapper_basedir, fopenwrapper_basedir_maxsize) == NULL)
	{
		fprintf(stderr, "getcwd(): %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fopenwrapper_basedir[fopenwrapper_basedir_maxsize - 1] = 0;

	// Move up to parent of BinXX directory.
	// /home/user/depot/Main/Build/Linux-Arc/Bin -> /home/user/depot/Main
	// [K01]: comment below code till real build
	/*
	char *ptr = strrchr(fopenwrapper_basedir, '/');
	if (ptr != NULL)
	{
		bool upOneDir = false;
		if (strcasecmp(ptr, "/Bin32") == 0)
			upOneDir = true;
		else if (strcasecmp(ptr, "/Bin64") == 0)
			upOneDir = true;
			
		if (upOneDir)
		{
			*ptr = '\0';
			chdir(fopenwrapper_basedir);
		}
	}*/
	SetModulePath(fopenwrapper_basedir);
	char *ptr = strstr(fopenwrapper_basedir, "/Build");
	if (ptr != NULL)
	{
		*ptr = '\0';
		chdir(fopenwrapper_basedir);
	}
#endif
}

bool IsBadReadPtr(void* ptr, unsigned int size )
{
	//too complicated to really support it
	return ptr? false : true;
}

//////////////////////////////////////////////////////////////////////////
char* _strtime(char* date)
{
	strcpy( date,"0:0:0" );
	return date;
}

//////////////////////////////////////////////////////////////////////////
char* _strdate(char* date)
{
	strcpy( date,"0" );
	return date;
}

//////////////////////////////////////////////////////////////////////////
char* strlwr (char * str)
{
  char *cp;               /* traverses string for C locale conversion */

  for (cp=str; *cp; ++cp)
  {
    if ('A' <= *cp && *cp <= 'Z')
      *cp += 'a' - 'A';
  }
  return str;
}

char* strupr (char * str)
{
  char *cp;               /* traverses string for C locale conversion */

  for (cp=str; *cp; ++cp)
  {
    if ('a' <= *cp && *cp <= 'z')
      *cp += 'A' - 'a';
  }
  return str;
}

char *ltoa ( long i , char *a , int radix )
{
	if ( a == NULL ) return NULL ;
	strcpy ( a , "0" ) ;
	if ( i && radix > 1 && radix < 37 ) {
		char buf[35] ;
		unsigned long u = i , p = 34 ;
		buf[p] = 0 ;
		if ( i < 0 && radix == 10 ) u = -i ;
		while ( u ) {
			unsigned int d = u % radix ;
			buf[--p] = d < 10 ? '0' + d : 'a' + d - 10 ;
			u /= radix ;
		}
		if ( i < 0 && radix == 10 ) buf[--p] = '-' ;
		strcpy ( a , buf + p ) ;
	}
	return a ;
}














































































void _makepath(char * path, const char * drive, const char *dir, const char * filename, const char * ext)
{
  char ch;
  char tmp[MAX_PATH];
  if ( !path )
	  return;
  tmp[0] = '\0';
  if (drive && drive[0])
  {
    tmp[0] = drive[0];
    tmp[1] = ':';
    tmp[2] = 0;
  }
  if (dir && dir[0])
  {
    strcat(tmp, dir);
    ch = tmp[strlen(tmp)-1];
    if (ch != '/' && ch != '\\')
	    strcat(tmp,"\\");
  }
  if (filename && filename[0])
  {
    strcat(tmp, filename);
    if (ext && ext[0])
    {
      if ( ext[0] != '.' )
				strcat(tmp,".");
      strcat(tmp,ext);
    }
  }
  strcpy( path, tmp );
}

char * _ui64toa(unsigned long long value,	char *str, int radix)
{
	if(str == 0)
		return 0;

	char buffer[65];
	char *pos;
	int digit;

	pos = &buffer[64];
	*pos = '\0';

	do 
	{
		digit = value % radix;
		value = value / radix;
		if (digit < 10) 
		{
			*--pos = '0' + digit;
		} else 
		{
			*--pos = 'a' + digit - 10;
		} /* if */
	} while (value != 0L);

	memcpy(str, pos, &buffer[64] - pos + 1);
	return str;
}

long long _atoi64( char *str )
{
	if(str == 0)
		return -1;
	unsigned long long RunningTotal = 0;
	char bMinus = 0;
	while (*str == ' ' || (*str >= '\011' && *str <= '\015')) 
	{
		str++;
	} /* while */
	if (*str == '+') 
	{
		str++;
	} else if (*str == '-') 
	{
		bMinus = 1;
		str++;
	} /* if */
	while (*str >= '0' && *str <= '9') 
	{
		RunningTotal = RunningTotal * 10 + *str - '0';
		str++;
	} /* while */
	return bMinus? ((long long)-RunningTotal) : (long long)RunningTotal;
}
 












#if !defined(PS3)
bool QueryPerformanceCounter(LARGE_INTEGER *counter)
{
#if defined(LINUX)
	// replaced gettimeofday
	// http://fixunix.com/kernel/378888-gettimeofday-resolution-linux.html
	timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);
	counter->QuadPart = (uint64)tv.tv_sec * 1000000 + tv.tv_nsec / 1000;
	return true;
#else
	return false;
#endif
}

bool QueryPerformanceFrequency(LARGE_INTEGER *frequency)
{
#if defined(LINUX)
	// On Linux we'll use gettimeofday().  The API resolution is microseconds,
	// so we'll report that to the caller.
	frequency->u.LowPart  = 1000000;
	frequency->u.HighPart = 0;
	return true;
#else
	return false;
#endif
}
#endif//PS3

void _splitpath(const char* inpath, char * drv, char * dir, char* fname, char * ext )
{	
	if (drv) 
		drv[0] = 0;

  typedef CryStackStringT<char, PATH_MAX> path_stack_string;

	const path_stack_string inPath(inpath);
	string::size_type s = inPath.rfind('/', inPath.size());//position of last /
	path_stack_string fName;
	if(s == string::npos)
	{
		if(dir)
			dir[0] = 0;
		fName = inpath;	//assign complete string as rest
	}
	else
	{
		if(dir)
			strcpy(dir, (inPath.substr((string::size_type)0,(string::size_type)(s+1))).c_str());	//assign directory
		fName = inPath.substr((string::size_type)(s+1));					//assign remaining string as rest
	}
	if(fName.size() == 0)
	{
		if(ext)
			ext[0] = 0;
		if(fname)
			fname[0] = 0;
	}
	else
	{
		//dir and drive are now set
		s = fName.find(".", (string::size_type)0);//position of first .
		if(s == string::npos)
		{
			if(ext)
				ext[0] = 0;
			if(fname)
				strcpy(fname, fName.c_str());	//assign filename
		}
		else
		{
			if(ext)
				strcpy(ext, (fName.substr(s)).c_str());		//assign extension including .
			if(fname)
			{
				if(s == 0)
					fname[0] = 0;
				else
					strcpy(fname, (fName.substr((string::size_type)0,s)).c_str());	//assign filename
			}
		}  
	} 
}

DWORD GetFileAttributes(LPCSTR lpFileName)
{




	struct stat fileStats;
	const int success = stat(lpFileName, &fileStats);
	if(success == -1)
	{
		char adjustedFilename[MAX_PATH];
		GetFilenameNoCase(lpFileName, adjustedFilename);
		if(stat(adjustedFilename, &fileStats) == -1)
			return (DWORD)INVALID_FILE_ATTRIBUTES;
	}
	DWORD ret = 0; 
	const int acc = access (lpFileName, W_OK);
	if(acc != 0)
	if(S_ISDIR(fileStats.st_mode) != 0)
		ret |= FILE_ATTRIBUTE_DIRECTORY;
	return (ret == 0)?FILE_ATTRIBUTE_NORMAL:ret;//return file attribute normal as the default value, must only be set if no other attributes have been found

}
 










































int _mkdir(const char *dirname)
{
#if !defined(PS3)
	const mode_t mode = 0x0777;
	const int suc = mkdir(dirname, mode);
	return (suc == 0)? 0 : -1;

























#endif
}

//////////////////////////////////////////////////////////////////////////
int memicmp( LPCSTR s1, LPCSTR s2, DWORD len )
{
  int ret = 0;
  while (len--)
  {
      if ((ret = tolower(*s1) - tolower(*s2))) break;
      s1++;
      s2++; 
  }
  return ret; 
}

//////////////////////////////////////////////////////////////////////////
int strcmpi( const char *str1, const char *str2 )
{
	for (;;)
	{
		int ret = tolower(*str1) - tolower(*str2);
		if (ret || !*str1) return ret;
		str1++;
		str2++;
	}
}

//-----------------------------------------other stuff-------------------------------------------------------------------

void GlobalMemoryStatus(LPMEMORYSTATUS lpmem)
{
		//not complete implementation









	FILE *f;
	lpmem->dwMemoryLoad    = 0;
	lpmem->dwTotalPhys     = 16*1024*1024;
	lpmem->dwAvailPhys     = 16*1024*1024;
	lpmem->dwTotalPageFile = 16*1024*1024;
	lpmem->dwAvailPageFile = 16*1024*1024;
	f = ::fopen( "/proc/meminfo", "r" );
	if (f)
	{
			char buffer[256];
		memset(buffer, '0', 256);
		int total, used, free, shared, buffers, cached;

		lpmem->dwLength = sizeof(MEMORYSTATUS);
		lpmem->dwTotalPhys = lpmem->dwAvailPhys = 0;
		lpmem->dwTotalPageFile = lpmem->dwAvailPageFile = 0;
		while (fgets( buffer, sizeof(buffer), f ))
		{
				if (sscanf( buffer, "Mem: %d %d %d %d %d %d", &total, &used, &free, &shared, &buffers, &cached ))
				{
						lpmem->dwTotalPhys += total;
						lpmem->dwAvailPhys += free + buffers + cached;
				}
				if (sscanf( buffer, "Swap: %d %d %d", &total, &used, &free ))
				{
						lpmem->dwTotalPageFile += total;
						lpmem->dwAvailPageFile += free;
				}
				if (sscanf(buffer, "MemTotal: %d", &total))
						lpmem->dwTotalPhys = total*1024;
				if (sscanf(buffer, "MemFree: %d", &free))
						lpmem->dwAvailPhys = free*1024;
				if (sscanf(buffer, "SwapTotal: %d", &total))
						lpmem->dwTotalPageFile = total*1024;
				if (sscanf(buffer, "SwapFree: %d", &free))
						lpmem->dwAvailPageFile = free*1024;
				if (sscanf(buffer, "Buffers: %d", &buffers))
						lpmem->dwAvailPhys += buffers*1024;
				if (sscanf(buffer, "Cached: %d", &cached))
						lpmem->dwAvailPhys += cached*1024;
		}
		fclose( f );
		if (lpmem->dwTotalPhys)
		{
				DWORD TotalPhysical = lpmem->dwTotalPhys+lpmem->dwTotalPageFile;
				DWORD AvailPhysical = lpmem->dwAvailPhys+lpmem->dwAvailPageFile;
				lpmem->dwMemoryLoad = (TotalPhysical-AvailPhysical)  / (TotalPhysical / 100);
		}
  }

}

static const int YearLengths[2] = {DAYSPERNORMALYEAR, DAYSPERLEAPYEAR};
static const int MonthLengths[2][MONSPERYEAR] =
{ 
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static int IsLeapYear(int Year)
{
	return Year % 4 == 0 && (Year % 100 != 0 || Year % 400 == 0) ? 1 : 0;
}

static void NormalizeTimeFields(short *FieldToNormalize, short *CarryField, int Modulus)
{
	*FieldToNormalize = (short) (*FieldToNormalize - Modulus);
	*CarryField = (short) (*CarryField + 1);
}

bool TimeFieldsToTime(PTIME_FIELDS tfTimeFields, PLARGE_INTEGER Time)
{
	#define SECSPERMIN         60
	#define MINSPERHOUR        60
	#define HOURSPERDAY        24
	#define MONSPERYEAR        12

	#define EPOCHYEAR          1601

	#define SECSPERDAY         86400
	#define TICKSPERMSEC       10000
	#define TICKSPERSEC        10000000
	#define SECSPERHOUR        3600

	int CurYear, CurMonth;
	LONGLONG rcTime;
	TIME_FIELDS TimeFields = *tfTimeFields;

	rcTime = 0;
	while (TimeFields.Second >= SECSPERMIN)
	{
		NormalizeTimeFields(&TimeFields.Second, &TimeFields.Minute, SECSPERMIN);
	}
	while (TimeFields.Minute >= MINSPERHOUR)
	{
		NormalizeTimeFields(&TimeFields.Minute, &TimeFields.Hour, MINSPERHOUR);
	}
	while (TimeFields.Hour >= HOURSPERDAY)
	{
		NormalizeTimeFields(&TimeFields.Hour, &TimeFields.Day, HOURSPERDAY);
	}
	while (TimeFields.Day > MonthLengths[IsLeapYear(TimeFields.Year)][TimeFields.Month - 1])
	{
		NormalizeTimeFields(&TimeFields.Day, &TimeFields.Month, SECSPERMIN);
	}
	while (TimeFields.Month > MONSPERYEAR)
	{
		NormalizeTimeFields(&TimeFields.Month, &TimeFields.Year, MONSPERYEAR);
	}
	for (CurYear = EPOCHYEAR; CurYear < TimeFields.Year; CurYear++)
	{ rcTime += YearLengths[IsLeapYear(CurYear)];
	}
	for (CurMonth = 1; CurMonth < TimeFields.Month; CurMonth++)
	{ rcTime += MonthLengths[IsLeapYear(CurYear)][CurMonth - 1];
	}
	rcTime += TimeFields.Day - 1;
	rcTime *= SECSPERDAY;
	rcTime += TimeFields.Hour * SECSPERHOUR + TimeFields.Minute * SECSPERMIN + TimeFields.Second;

	rcTime *= TICKSPERSEC;
	rcTime += TimeFields.Milliseconds * TICKSPERMSEC;

	Time->QuadPart = rcTime;

	return true;
}

BOOL SystemTimeToFileTime( const SYSTEMTIME *syst, LPFILETIME ft )
{
	TIME_FIELDS tf;
	LARGE_INTEGER t;

	tf.Year = syst->wYear;
	tf.Month = syst->wMonth;
	tf.Day = syst->wDay;
	tf.Hour = syst->wHour;
	tf.Minute = syst->wMinute;
	tf.Second = syst->wSecond;
	tf.Milliseconds = syst->wMilliseconds;

	TimeFieldsToTime(&tf, &t);
	ft->dwLowDateTime = t.u.LowPart;
	ft->dwHighDateTime = t.u.HighPart;
	return TRUE;
}

//begin--------------------------------findfirst/-next implementation----------------------------------------------------

void __finddata64_t::CopyFoundData(const char *rMatchedFileName)
{
	FS_STAT_TYPE fsStat;
	FS_ERRNO_TYPE fsErr = 0;
	bool isDir = false, isReadonly = false;

	memset(&fsStat, 0, sizeof(fsStat));
	const int cCurStrLen = strlen(name);
#if !defined(PS3)
	if(cCurStrLen > 0 && name[cCurStrLen-1] != '/')


#endif	
	{
		name[cCurStrLen] = '/';
		name[cCurStrLen+1] = 0;
	}
	strncpy(name, rMatchedFileName, sizeof name - 1);
	name[sizeof name - 1] = 0;
	// Remove trailing slash for directories.
	if (name[0] && name[strlen(name) - 1] == '/')
	{
		name[strlen(name) - 1] = 0;
		isDir = true;
	}

	//more eff.impl for: 	string filename = string(m_DirectoryName) + "/" + name;
	char filename[MAX_PATH];
	const int cDirStrLen = strlen(m_DirectoryName);
	char *pDirCur = m_DirectoryName;
	char *pDst = filename;
	for(int i=0; i<cDirStrLen; ++i)
		*pDst++ = *pDirCur++;
  if(pDirCur[-1] != '/')
		*pDirCur = '/';
	const int cNameLen = strlen(name);
	pDirCur = name;
	for(int i=0; i<cNameLen; ++i)
		*pDst++ = *pDirCur++;
	*pDirCur = 0;
	*pDst = 0;

	// Check if the file is a directory and/or is read-only.
	if (!isDir)
	{
		FS_STAT(filename, fsStat, fsErr);
		if (fsErr) return;
	}
	(void)fsErr;
#if defined(LINUX)
	// This is how it should be done. However, the st_mode field of
	// CellFsStat is currently unimplemented by the current version of the
	// Cell SDK (0.5.0).
	if (S_ISDIR(fsStat.st_mode)) isDir = true;
	if (!(fsStat.st_mode & S_IWRITE)) isReadonly = true;
#else
	{ 
/*
		// workaround
		int fd;
		if (!isDir) 
		{
			FS_OPEN(filename, FS_O_RDWR, fd, 0, fsErr);
			if (fsErr)
				isReadonly = true;
			else
				FS_CLOSE_NOERR(fd);
		}
*/
		isReadonly = false;
	}
#endif
	if (isDir)
		attrib |= _A_SUBDIR;
	else
		attrib &= ~_A_SUBDIR;
	if (isReadonly)
		attrib |= _A_RDONLY;
	else
		attrib &= ~_A_RDONLY;
	if (!isDir)
	{
		size = fsStat.st_size;
		time_access = fsStat.st_atime;
		time_write = fsStat.st_mtime;
		time_create = fsStat.st_ctime;
	} else
	{
    size = 0;
		time_access = time_write = time_create = 0;
	}
}

// Map all path components to the correct case.
// Slashes must be normalized to fwdslashes (/).
// Trailing path component is ignored (i.e. dirnames must end with a /).
// Returns 0 on success and -1 on error.
// In case of an error, all matched path components will have been case
// corrected.
int FixDirnameCase
(
	char *path, int index = 0
#if defined(USE_FILE_MAP)
	, FileNode *node = NULL
#endif
)
{







	FS_ERRNO_TYPE fsErr = 0;
  char *slash;
	FS_DIR_TYPE dir = FS_DIR_NULL;
	bool pathOk = false;
	char *parentSlash;
#if defined(USE_FILE_MAP)
	FileNode *node1 = NULL;
#endif

	slash = strchr(path + index + 1, '/');
	if (!slash) return 0;
	*slash = 0;
	parentSlash = strrchr(path, '/');








#if defined(USE_FILE_MAP)
	if (index == 0)
		node = FileNode::GetTree();
	if (node != NULL && !pathOk)
	{
		int i = node->FindExact(path + index + 1);
		if (i != -1)
		{
			node1 = node->m_Children[i];
			if (node1 != NULL)
			{
				assert(node1->m_nIndex == i);
				pathOk = true;
			}
		} else
		{
			i = node->Find(path + index + 1);
			if (i != -1)
			{
				node1 = node->m_Children[i];
				assert(node1 == NULL || node1->m_nIndex == i);
			}
		}
		if (!pathOk && node1 == NULL && !node->m_bDirty)
		{
			*slash = '/';
			return -1;
		}
	}
	if (node1 == NULL && !pathOk)
	{
#endif
		// Check if path is a valid directory.
		FS_OPENDIR(path, dir, fsErr);
		if (!fsErr)
		{
			pathOk = true;
			FS_CLOSEDIR_NOERR(dir);
			dir = FS_DIR_NULL;
		} else if (fsErr != FS_ENOENT && fsErr != FS_EINVAL)
		{
			*slash = '/';
			return -1;
		}
#if defined(USE_FILE_MAP)
	}

	if (node1 != NULL && !pathOk)
	{
		const char *name = node->m_Entries[node1->m_nIndex];
		if (parentSlash)
			memcpy(parentSlash + 1, name, strlen(name));
		else
			memcpy(path, name, strlen(name));
		pathOk = true;
	}
#endif

	if (!pathOk) {
		// Get the parent dir.
		const char *parent;
		char *name;
		if (parentSlash) {
			*parentSlash = 0;
			parent = path;
			if (!*parent) parent = "/";
			name = parentSlash + 1;
		} else
		{
			parent = ".";
			name = path;
		}

		// Scan parent.
		FS_OPENDIR(parent, dir, fsErr);
		if (fsErr)
		{
			if (parentSlash) *parentSlash = '/';
			*slash = '/';
			return -1;
		}
		FS_DIRENT_TYPE dirent;
		uint64_t direntSize = 0;
		while (true)
		{
			FS_READDIR(dir, dirent, direntSize, fsErr);
			if (fsErr)
			{
				FS_CLOSEDIR_NOERR(dir);
				if (parentSlash) *parentSlash = '/';
				*slash = '/';
				return -1;
			}
			if (direntSize == 0) break;



			size_t len = strlen(dirent.d_name);

			if (len > 0 && dirent.d_name[len - 1] == '/') len -= 1;
			if (!strncasecmp(dirent.d_name, name, len))
			{
				pathOk = true;
				if (parentSlash)
					memcpy(parentSlash + 1, dirent.d_name, len);
				else
					memcpy(path, dirent.d_name, len);
				break;
			}
		}
		FS_CLOSEDIR(dir, fsErr);
		if (parentSlash) *parentSlash = '/';
		if (fsErr)
		{
			*slash = '/';
			return -1;
		}
	}
	*slash = '/';

	// Recurse.
	if (pathOk)
	{
#if defined(USE_FILE_MAP)
		return FixDirnameCase(path, slash - path, node1);
#else
		return FixDirnameCase(path, slash - path);
#endif
	}

	return -1;
}

// Match the specified name against the specified glob-pattern.
// Returns true iff name matches pattern.
static bool matchPattern(const char *name, const char *pattern)
{
  while (true)
	{
		if (!*pattern) return !*name;
		switch (*pattern)
		{
		case '?':
			if (!*name) return false;
			++name;
			++pattern;
			break;
		case '*':
			++pattern;
			while (true)
			{
				if (matchPattern(name, pattern)) return true;
				if (!*name) return false;
				++name;
			}
			break; // Not reached.
		default:
			if (strnicmp(name, pattern, 1)) return false;
			++name;
			++pattern;
			break;
		}
	}
}

intptr_t _findfirst64(const char *pFileName, __finddata64_t *pFindData)
{





	FS_ERRNO_TYPE fsErr = 0;
	char filename[MAX_PATH];
	size_t filenameLength = 0;
	const char *dirname = 0;
	const char *pattern = 0;

	pFindData->m_LastIndex = -1;
	strcpy(filename, pFileName);
	filenameLength = strlen(filename);

	// Normalize ".*" and "*.*" suffixes to "*".
	if (!strcmp(filename + filenameLength - 3, "*.*"))
		filename[filenameLength - 2] = 0;
	else if (!strcmp(filename + filenameLength - 2, ".*"))
	{
		filename[filenameLength - 2] = '*';
		filename[filenameLength - 1] = 0;
	}

	// Map backslashes to fwdslashes.
#if !defined(PS3)
	const int cLen = strlen(pFileName);
	for (int i = 0; i<cLen; ++i)
		if (filename[i] == '\\') filename[i] = '/';
#endif
	// Get the dirname.
	char *slash = strrchr(filename, '/');
	if (slash)
	{
#if !defined(PS3)
		if (FixDirnameCase(filename) == -1)
			return -1;
#endif
		pattern = slash + 1;
		dirname = filename;
		*slash = 0;
	} 
	else 
	{
		dirname = "./";
		pattern = filename;
	}
	strncpy(pFindData->m_ToMatch, pattern, sizeof pFindData->m_ToMatch - 1);

	// Close old directory descriptor.
	if (pFindData->m_Dir != FS_DIR_NULL)
	{
		FS_CLOSEDIR(pFindData->m_Dir, fsErr);
		pFindData->m_Dir = FS_DIR_NULL;
		if (fsErr)
			return -1;
	}

	bool readDirectory = true;
#if defined(USE_FILE_MAP)



	const bool skipInitial = false;

	// Check if an up to date directory listing can be extracted from the file
	// map.
	int dirIndex = -1;
	bool dirDirty = false;
	FileNode *dirNode = FileNode::FindExact(
			dirname, dirIndex, &dirDirty, skipInitial);
	if ((dirNode == NULL || dirIndex == -1) && !dirDirty)
		return -1;
	if (dirNode != NULL && !dirNode->m_bDirty)
	{
		FileNode *const dirNode1 = dirNode->m_Children[dirIndex];
		if (dirNode1 == NULL)
			return -1;
		if (!dirNode1->m_bDirty)
		{
			// Copy the directory listing from the file node.
			strncpy(pFindData->m_DirectoryName, dirname,
					sizeof pFindData->m_DirectoryName - 1);
			pFindData->m_DirectoryName[sizeof pFindData->m_DirectoryName - 1] = 0;
			const int n = dirNode1->m_nEntries;
			for (int i = 0; i < n; ++i)
			{
				if (dirNode1->m_Children[i])
				{
					// Directory. We'll add a trailing slash to the name to identify
					// directory entries.
					char name[strlen(dirNode1->m_Entries[i]) + 2];
					strcpy(name, dirNode1->m_Entries[i]);
					strcat(name, "/");
					pFindData->m_Entries.push_back(name);
				} else
					pFindData->m_Entries.push_back(dirNode1->m_Entries[i]);
			}
			readDirectory = false;
		}
	}
#endif

	if (readDirectory)
	{
		// Open and read directory.
		FS_OPENDIR(dirname, pFindData->m_Dir, fsErr);
		if (fsErr)
			return -1;
		strncpy(pFindData->m_DirectoryName, dirname,
				sizeof pFindData->m_DirectoryName - 1);
		pFindData->m_DirectoryName[sizeof pFindData->m_DirectoryName - 1] = 0;
		FS_DIRENT_TYPE dirent;
		uint64_t direntSize = 0;
		while(true)
		{
			FS_READDIR(pFindData->m_Dir, dirent, direntSize, fsErr);
			if (fsErr)
			{
				FS_CLOSEDIR_NOERR(pFindData->m_Dir);
				pFindData->m_Dir	=	FS_DIR_NULL;
				return -1;
			}
			if (direntSize == 0)
				break;
			if (!strcmp(dirent.d_name, ".")
					|| !strcmp(dirent.d_name, "./")
					|| !strcmp(dirent.d_name, "..")
					|| !strcmp(dirent.d_name, "../"))
				continue;
			// We'll add a trailing slash to the name to identify directory
			// entries.
			char d_name[MAX_PATH];
			strcpy(d_name, dirent.d_name);
			if (dirent.d_type == FS_TYPE_DIRECTORY)
			{
				const int cLen = strlen(d_name);
				if(d_name[0] || d_name[cLen - 1] != '/')
					strcat(d_name, "/");
			}
			pFindData->m_Entries.push_back(d_name);
		}
		FS_CLOSEDIR_NOERR(pFindData->m_Dir);
		pFindData->m_Dir	=	FS_DIR_NULL;
	}

	// Locate first match.
	int i = 0;
	const std::vector<string>::const_iterator cEnd = pFindData->m_Entries.end();
	for(std::vector<string>::const_iterator iter = pFindData->m_Entries.begin(); iter != cEnd; ++iter)
	{
		const char *cpEntry = iter->c_str();
		if (matchPattern(cpEntry, pattern))
		{
			pFindData->CopyFoundData(cpEntry);
			pFindData->m_LastIndex = i;
			break;
		}
		++i;
	}
	return pFindData->m_LastIndex;
};

int _findnext64(intptr_t last, __finddata64_t *pFindData)
{
	if (last == -1 || pFindData->m_LastIndex == -1)
		return -1;
	if (pFindData->m_LastIndex + 1 >= pFindData->m_Entries.size())
		return -1;

	int found = -1;
	int i = pFindData->m_LastIndex + 1;
	pFindData->m_LastIndex = -1;
	for (
		std::vector<string>::const_iterator iter = pFindData->m_Entries.begin() + i;
		iter != pFindData->m_Entries.end();
	  ++iter)
	{
		if (matchPattern(iter->c_str(), pFindData->m_ToMatch))
		{
			pFindData->CopyFoundData(iter->c_str());
			pFindData->m_LastIndex = i;
			found = 0;
			break;
		}
		++i;
	}
	return found;
}

//////////////////////////////////////////////////////////////////////////
int _findclose( intptr_t handle )
{
	return 0;//we dont need this
}

__finddata64_t::~__finddata64_t()
{
	if (m_Dir != FS_DIR_NULL)
	{
		FS_CLOSEDIR_NOERR(m_Dir);
		m_Dir = FS_DIR_NULL;
	}
}

//end--------------------------------findfirst/-next implementation----------------------------------------------------
#if !defined(PS3)
void adaptFilenameToLinux(string& rAdjustedFilename)
{
	//first replace all \\ by /
	string::size_type loc = 0;
	while((loc = rAdjustedFilename.find( "\\", loc)) != string::npos)
	{
		rAdjustedFilename.replace(loc, 1, "/");
	}	
	loc = 0;
	//remove /./
	while((loc = rAdjustedFilename.find( "/./", loc)) != string::npos)
	{
		rAdjustedFilename.replace(loc, 3, "/");
	}
}
#endif

#if !defined(PS3)
	void replaceDoublePathFilename(char *szFileName)
	{
		//replace "\.\" by "\"
		string s(szFileName);
		string::size_type loc = 0;
		//remove /./
		while((loc = s.find( "/./", loc)) != string::npos)
		{
			s.replace(loc, 3, "/");
		}
		loc = 0;
		//remove "\.\"
		while((loc = s.find( "\\.\\", loc)) != string::npos)
		{
			s.replace(loc, 3, "\\");
		}
		strcpy((char*)szFileName, s.c_str());
	}
#endif

#if !defined(PS3)
	const int comparePathNames(const char* cpFirst, const char* cpSecond, unsigned int len)
	{
		//create two strings and replace the \\ by / and /./ by /
		string first(cpFirst);
		string second(cpSecond);
		adaptFilenameToLinux(first);
		adaptFilenameToLinux(second);
		if(strlen(cpFirst) < len || strlen(cpSecond) < len)
			return -1;
		unsigned int length = std::min(std::min(first.size(), second.size()), (size_t)len);//make sure not to access invalid memory
		return memicmp(first.c_str(), second.c_str(), length);
	}



































#endif

#if defined(LINUX)
static bool FixOnePathElement(char *path)
{
	if (*path == '\0')
		return true;
		
	if ((path[0] == '/') && (path[1] == '\0'))
		return true;  // root dir always exists.
		
	if (strchr(path, '*') || strchr(path, '?'))
		return true; // wildcard...stop correcting path.
		
		struct stat statbuf;
		if (stat(path, &statbuf) != -1)  // current case exists.
			return true;
			
		char *name = path;
		char *ptr = strrchr(path, '/');
		if (ptr)
		{
			name = ptr+1;
			*ptr = '\0';
		}
		
		if (*name == '\0')  // trailing '/' ?
		{
			*ptr = '/';
			return true;
		}
		
		const char *parent;
		if (ptr == path)
			parent = "/";
		else if (ptr == NULL)
			parent = ".";
		else
			parent = path;
			
		DIR *dirp = opendir(parent);
		if (ptr) *ptr = '/';
		
		if (dirp == NULL)
			return false;
			
		struct dirent *dent;
		bool found = false;
		while ((dent = readdir(dirp)) != NULL)
		{
			if (strcasecmp(dent->d_name, name) == 0)
			{
				strcpy(name, dent->d_name);
				found = true;
				break;
			}
		}
		
		closedir(dirp);
		return found;
	}
#endif

#if !defined(PS3)
const bool GetFilenameNoCase
(
	const char *file,
	char *pAdjustedFilename,
	const bool cCreateNew
)
{
	assert(file);
	assert(pAdjustedFilename);
	strcpy(pAdjustedFilename, file);
#ifndef PS3
	// Fix the dirname case.
	const int cLen = strlen(file);
	for (int i = 0; i<cLen; ++i) 
		if(pAdjustedFilename[i] == '\\') 
			pAdjustedFilename[i] = '/';
#endif
	char *slash;
	const char *dirname;
	char *name;
	FS_ERRNO_TYPE fsErr = 0;
	FS_DIRENT_TYPE dirent;
	uint64_t direntSize = 0;
	FS_DIR_TYPE fd = FS_DIR_NULL;

	if (
		(pAdjustedFilename) == (char*)-1)
		return false;

	slash = strrchr(pAdjustedFilename, '/');
	if (slash)
	{
		dirname = pAdjustedFilename;
		name = slash + 1;
		*slash = 0;
	} else
	{
	  dirname = ".";
		name = pAdjustedFilename;
	}

#if !defined(LINUX)		// fix the parent path anyhow.
	// Check for wildcards. We'll always return true if the specified filename is
	// a wildcard pattern.
	if (strchr(name, '*') || strchr(name, '?'))
	{
		if (slash) *slash = '/';
		return true;
	}
#endif

	// Scan for the file.
	bool found = false;
	bool skipScan = false;
#if defined(USE_FILE_MAP)



	const bool skipInitial = false;

	bool dirty = false;
	int dirIndex = -1;
	FileNode *dirNode = NULL;
	if (strrchr(dirname, '/') > dirname)
	{
		FileNode *parentDirNode = FileNode::FindExact(
				dirname, dirIndex, &dirty, skipInitial);
		if (dirty)
		{
			if (parentDirNode != NULL && dirIndex != -1)
				dirNode = parentDirNode->m_Children[dirIndex];
		} else
		{
			if (parentDirNode == NULL || dirIndex == -1)
				return false;
			dirNode = parentDirNode->m_Children[dirIndex];
			dirty = dirNode->m_bDirty;
		}
	}
	else
	{
		// The requested file is in the root directory.
		dirNode = FileNode::GetTree();
		if (dirNode != NULL)
			dirty = dirNode->m_bDirty;
		else
			dirty = true;
	}
	if (dirNode != NULL)
	{
		int index = dirNode->FindExact(name);
		if (index == -1)
		{
			index = dirNode->Find(name);
			if (index != -1)
			{
				strcpy(name, dirNode->m_Entries[index]);
				found = true;
			}
		} else
			found = true;
	}
	if (!dirty || found)
		skipScan = true;

	if (!skipScan)
	{
		FS_OPENDIR(dirname, fd, fsErr);
		if (fsErr)
			return false;
		while (true)
		{
			FS_READDIR(fd, dirent, direntSize, fsErr);
			if (fsErr)
			{
				FS_CLOSEDIR_NOERR(fd);
				return false;
			}
			if (direntSize == 0) break;
			if (!stricmp(dirent.d_name, name))
			{
				strcpy(name, dirent.d_name);
				found = true;
				break;
			}
		}
		FS_CLOSEDIR(fd, fsErr);
		if (fsErr)
			return false;
	}

	if (slash)
		*slash = '/';
	//if (!found && !cCreateNew) return false;
	return true;

#else //USE_FILE_MAP

	#if defined(LINUX)
		if (slash) *slash = '/';
		char *path = pAdjustedFilename;
		char *sep;
		while ((sep = strchr(path, '/')) != NULL)
		{
			*sep = '\0';
			const bool exists = FixOnePathElement(pAdjustedFilename);
			*sep = '/';
			if (!exists)
				return false;
			path = sep + 1;
		}
	  return FixOnePathElement(pAdjustedFilename);  // catch last filename.
	#endif
	
#endif//USE_FILE_MAP
	return true;
}
#endif//PS3

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
HANDLE CreateFile(
													const char* lpFileName,
													DWORD dwDesiredAccess,
													DWORD dwShareMode,
													void* lpSecurityAttributes,
													DWORD dwCreationDisposition,
													DWORD dwFlagsAndAttributes,
													HANDLE hTemplateFile
												 )
{
	int flags = 0;
	int fd = -1;
	FS_ERRNO_TYPE fserr;
	bool create = false;
	HANDLE h;
#if !defined(PS3)
	if ((dwDesiredAccess & GENERIC_READ) == GENERIC_READ
			&& (dwDesiredAccess & GENERIC_WRITE) == GENERIC_WRITE)
		flags = O_RDWR;
	else if ((dwDesiredAccess & GENERIC_WRITE) == GENERIC_WRITE)
		flags = O_WRONLY;
	else
	{
		// On Windows files can be opened with no access. We'll map no access
		// to read only.
		flags = O_RDONLY;
	}
	if ((dwDesiredAccess & GENERIC_WRITE) == GENERIC_WRITE)
#endif
	{
		switch (dwCreationDisposition)
		{
		case CREATE_ALWAYS:
			flags |= O_CREAT | O_TRUNC;
			create = true;
			break;
		case CREATE_NEW:
		  flags |= O_CREAT | O_EXCL;
			create = true;
			break;
		case OPEN_ALWAYS:
			flags |= O_CREAT;
			create = true;
			break;
		case OPEN_EXISTING:
			flags = O_RDONLY;
			break;
		case TRUNCATE_EXISTING:
			flags |= O_TRUNC;
			break;
		default:
			assert(0);
		}
	}





	char adjustedFilename[MAX_PATH];
	GetFilenameNoCase(lpFileName, adjustedFilename, create);


	bool failOpen = false;
#if defined(USE_FILE_MAP)
#if defined(LINUX)
	const bool skipInitial = false;
#else
	const bool skipInitial = true;
#endif
	if (!FileNode::CheckOpen(adjustedFilename, create, skipInitial))
		failOpen = true;
#endif
	(void)create;

#if defined(FILE_MAP_DEBUG)
	fd = open(adjustedFilename, flags, mode);
	if (fd != -1 && failOpen)
	{
		puts("FileNode::CheckOpen error");
		assert(0);
	}
#else
	if (failOpen)
	{
		fd = -1;
		fserr = ENOENT;
	} 
	else
	{



		FS_OPEN(adjustedFilename, flags, fd, 0, errno);		

	}
#endif
	(void)fserr;
	if (fd == -1)
	{
		h = INVALID_HANDLE_VALUE;
	} else
	{
		h = (HANDLE)fd;
	}
	return h;
}

#define Int32x32To64(a, b) ((uint64)((uint64)(a)) * (uint64)((uint64)(b)))







/*
//////////////////////////////////////////////////////////////////////////
BOOL SetFileTime(
												HANDLE hFile,
												const FILETIME *lpCreationTime,
												const FILETIME *lpLastAccessTime,
												const FILETIME *lpLastWriteTime )
{
	CRY_ASSERT_MESSAGE(0, "SetFileTime not implemented yet");
	return FALSE;
}
*/
BOOL SetFileTime(const char* lpFileName, const FILETIME *lpLastAccessTime)
{




	// Craig: can someone get a better impl here?
	char adjustedFilename[MAX_PATH];
	GetFilenameNoCase(lpFileName, adjustedFilename, false);



















#if defined(LINUX)
	struct utimbuf timeBuf;
	memset(&timeBuf, 0, sizeof timeBuf);
	timeBuf.actime = *(time_t *)lpLastAccessTime;
	timeBuf.modtime = timeBuf.actime;
	return utime(adjustedFilename, &timeBuf) == 0;
#endif
}

//returns modification time of file
//for testing we do just store the time as is without conversion to win32 system time
const uint64 GetFileModifTime(FILE * hFile)
{
	FS_ERRNO_TYPE fsErr = 0;
	FS_STAT_TYPE st;
	FS_FSTAT(fileno(hFile), st, fsErr);

	(void)fsErr;
	// UnixTimeToFileTime
	// since the cache filesystem uses fat32 and has a 2 seconds resolution time, and the
	// cellfs has a 1 seconds resolution timer, it is necessary to mask out the last second 
	time_t fixed_time = st.st_mtime & ~1ull;
	
	return Int32x32To64(fixed_time, 10000000) + 116444736000000000ll;

}

//////////////////////////////////////////////////////////////////////////
/*
BOOL GetFileTime(HANDLE hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime)
{
	FS_ERRNO_TYPE err = 0;
	FS_STAT_TYPE st;
	int fd = (int)hFile;
	uint64 t;
	FILETIME creationTime, lastAccessTime, lastWriteTime;

	FS_FSTAT(fd, st, err);
	if (err != 0)
		return FALSE;
	t = st.st_ctime * 10000000UL + 116444736000000000ULL;
	creationTime.dwLowDateTime = (DWORD)t;
	creationTime.dwHighDateTime = (DWORD)(t >> 32);
	t = st.st_atime * 10000000UL + 116444736000000000ULL;
	lastAccessTime.dwLowDateTime = (DWORD)t;
	lastAccessTime.dwHighDateTime = (DWORD)(t >> 32);
	t = st.st_mtime * 10000000UL + 116444736000000000ULL;
	lastWriteTime.dwLowDateTime = (DWORD)t;
	lastWriteTime.dwHighDateTime = (DWORD)(t >> 32);
	if (lpCreationTime) *lpCreationTime = creationTime;
	if (lpLastAccessTime) *lpLastAccessTime = lastAccessTime;
	if (lpLastWriteTime) *lpLastWriteTime = lastWriteTime;
	return TRUE;
}
*/

//////////////////////////////////////////////////////////////////////////
#ifndef PS3
BOOL SetFileAttributes(
															LPCSTR lpFileName,
															DWORD dwFileAttributes )
{
//TODO: implement
	printf("SetFileAttributes not properly implemented yet, should not matter\n");
	return TRUE;
}
#endif

//////////////////////////////////////////////////////////////////////////
DWORD GetFileSize(HANDLE hFile,DWORD *lpFileSizeHigh )
{
	FS_ERRNO_TYPE err = 0;
	FS_STAT_TYPE st;
	int fd = (int)hFile;
	DWORD r;

	FS_FSTAT(fd, st, err);
	if (err != 0)
		return INVALID_FILE_SIZE;
	if (lpFileSizeHigh)
		*lpFileSizeHigh = (DWORD)(st.st_size >> 32);
	r = (DWORD)st.st_size;
	return r;
}

//////////////////////////////////////////////////////////////////////////
BOOL CloseHandle( HANDLE hObject )
{
	int fd = (int)hObject;

	if (fd != -1)
	{



		close(fd);

	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL CancelIo( HANDLE hFile )
{
//TODO: implement
	CRY_ASSERT_MESSAGE(0, "CancelIo not implemented yet");
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
HRESULT GetOverlappedResult( HANDLE hFile,void* lpOverlapped,LPDWORD lpNumberOfBytesTransferred, BOOL bWait )
{







//TODO: implement
	CRY_ASSERT_MESSAGE(0, "GetOverlappedResult not implemented yet");
	return 0;

}

//////////////////////////////////////////////////////////////////////////
BOOL ReadFile
(
	HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
)
{
//TODO: implement
	CRY_ASSERT_MESSAGE(0, "ReadFile not implemented yet");
	abort();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL ReadFileEx
(
	HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPOVERLAPPED lpOverlapped,
	LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
)
{




























	CRY_ASSERT_MESSAGE(0, "ReadFileEx not implemented yet");
	return TRUE;

}

//////////////////////////////////////////////////////////////////////////
DWORD SetFilePointer
(
	HANDLE hFile,
	LONG lDistanceToMove,
	PLONG lpDistanceToMoveHigh,
	DWORD dwMoveMethod
)
{
//TODO: implement
	CRY_ASSERT_MESSAGE(0, "SetFilePointer not implemented yet");
	return 0;
}

//////////////////////////////////////////////////////////////////////////
#if !defined(PS3)
DWORD GetCurrentThreadId()
{
	return static_cast<DWORD>(pthread_self());
}
#endif

//////////////////////////////////////////////////////////////////////////
HANDLE CreateEvent
(
	LPSECURITY_ATTRIBUTES lpEventAttributes,
	BOOL bManualReset,
	BOOL bInitialState,
	LPCSTR lpName
)
{
//TODO: implement
	CRY_ASSERT_MESSAGE(0, "CreateEvent not implemented yet");
	return 0;
}


//////////////////////////////////////////////////////////////////////////
#ifndef PS3
DWORD Sleep(DWORD dwMilliseconds)
{
#ifdef LINUX
	timespec req;
	timespec rem;

	memset(&req, 0, sizeof(req));
	memset(&rem, 0, sizeof(rem));

	time_t sec = (int)(dwMilliseconds/1000);
	req.tv_sec = sec;
	req.tv_nsec = (dwMilliseconds - (sec*1000))*1000000L;
	if (nanosleep(&req, &rem) == -1)
		nanosleep(&rem, 0);

	return 0;
#else
	timeval tv, start, now;
	uint64 tStart;

	memset(&tv, 0, sizeof tv);
	memset(&start, 0, sizeof start);
	memset(&now, 0, sizeof now);
	gettimeofday(&now, NULL);
	start = now;
	tStart = (uint64)start.tv_sec * 1000000 + start.tv_usec;
	while (true)
	{
		uint64 tNow, timePassed, timeRemaining;
		tNow = (uint64)now.tv_sec * 1000000 + now.tv_usec;
		timePassed = tNow - tStart;
		if (timePassed >= dwMilliseconds)
			break;
		timeRemaining = dwMilliseconds * 1000 - timePassed;
		tv.tv_sec = timeRemaining / 1000000;
		tv.tv_usec = timeRemaining % 1000000;
		select(1, NULL, NULL, NULL, &tv);
		gettimeofday(&now, NULL);
	}
	return 0;
#endif
}
#endif

//////////////////////////////////////////////////////////////////////////
DWORD SleepEx( DWORD dwMilliseconds,BOOL bAlertable )
{
//TODO: implement
//	CRY_ASSERT_MESSAGE(0, "SleepEx not implemented yet");
	printf("SleepEx not properly implemented yet\n");
	Sleep(dwMilliseconds);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
DWORD WaitForSingleObjectEx(HANDLE hHandle,	DWORD dwMilliseconds,	BOOL bAlertable)
{
//TODO: implement
	CRY_ASSERT_MESSAGE(0, "WaitForSingleObjectEx not implemented yet");
	return 0;
}















//////////////////////////////////////////////////////////////////////////
DWORD WaitForSingleObject( HANDLE hHandle,DWORD dwMilliseconds )
{
//TODO: implement
	CRY_ASSERT_MESSAGE(0, "WaitForSingleObject not implemented yet");
	return 0;
}

//////////////////////////////////////////////////////////////////////////
BOOL SetEvent( HANDLE hEvent )
{
//TODO: implement
	CRY_ASSERT_MESSAGE(0, "SetEvent not implemented yet");
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL ResetEvent( HANDLE hEvent )
{
//TODO: implement
	CRY_ASSERT_MESSAGE(0, "ResetEvent not implemented yet");
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
HANDLE CreateMutex
(
	LPSECURITY_ATTRIBUTES lpMutexAttributes,
	BOOL bInitialOwner,
	LPCSTR lpName
)
{
//TODO: implement
	CRY_ASSERT_MESSAGE(0, "CreateMutex not implemented yet");
	return 0;
}

//////////////////////////////////////////////////////////////////////////
BOOL ReleaseMutex( HANDLE hMutex )
{
//TODO: implement
	CRY_ASSERT_MESSAGE(0, "ReleaseMutex not implemented yet");
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////


typedef DWORD (*PTHREAD_START_ROUTINE)( LPVOID lpThreadParameter );
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

//////////////////////////////////////////////////////////////////////////
HANDLE CreateThread
(
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	SIZE_T dwStackSize,
	LPTHREAD_START_ROUTINE lpStartAddress,
	LPVOID lpParameter,
	DWORD dwCreationFlags,
	LPDWORD lpThreadId
)
{
//TODO: implement
	CRY_ASSERT_MESSAGE(0, "CreateThread not implemented yet");
	return 0;
}

//////////////////////////////////////////////////////////////////////////








































































//////////////////////////////////////////////////////////////////////////
DWORD GetCurrentDirectory( DWORD nBufferLength, char* lpBuffer )
{








































	// Not supported outside of CryEngine.
	return 0;

}

//////////////////////////////////////////////////////////////////////////
BOOL DeleteFile(LPCSTR lpFileName)
{




#if defined(LINUX)
	int err = unlink(lpFileName);
	return (0 == err);
#else
	CRY_ASSERT_MESSAGE(0, "DeleteFile not implemented yet");
	return TRUE;
#endif
}

//////////////////////////////////////////////////////////////////////////
BOOL MoveFile( LPCSTR lpExistingFileName,LPCSTR lpNewFileName )
{
#ifdef LINUX
	int err = rename(lpExistingFileName, lpNewFileName);
	return (0 == err);
#else
	CRY_ASSERT_MESSAGE(0, "MoveFile not implemented yet");
#endif
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL CopyFile(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists)
{
#if defined (LINUX)
    if (bFailIfExists)
    {
		// return false if file already exists
		FILE* fTemp = fopen(lpNewFileName, "rb");
		if (fTemp != NULL)
		{
	    	fclose(fTemp);
	    	return FALSE;
		}
    }
    
    FILE* fFrom = fopen(lpExistingFileName, "rb");
    if (NULL == fFrom)
		return FALSE;
	
    FILE* fTo = fopen(lpNewFileName, "wb");
    if (NULL == fTo)
    {
		fclose(fFrom);
		return FALSE;
    }
    
    #define COPY_FILE_BUF_SIZE 1024
    char buf[COPY_FILE_BUF_SIZE];
    size_t lenRead;
    size_t lenWrite;
        
    while (!feof(fFrom))
    {
		lenRead = fread(buf, sizeof(char), COPY_FILE_BUF_SIZE, fFrom);
		lenWrite = fwrite(buf, sizeof(char), lenRead, fTo);
		assert(lenWrite == lenRead);
    }
    
    fclose(fFrom);
    fclose(fTo);
    
    return TRUE;
	    
#else
    CRY_ASSERT_MESSAGE(0, "CopyFile not implemented yet");
#endif
}

#if defined (LINUX)
BOOL GetComputerName(LPSTR lpBuffer, LPDWORD lpnSize)
{
	if (!lpBuffer || !lpnSize)
		return FALSE;

 	int err = gethostname(lpBuffer, *lpnSize);

	if (-1 == err)
	{
		CryLog("GetComputerName falied [%d]\n", errno);
		return FALSE;
	}
	return TRUE;
}

DWORD GetCurrentProcessId(void)
{
	return (DWORD)getpid();
}
#endif //LINUX

//////////////////////////////////////////////////////////////////////////
BOOL RemoveDirectory(LPCSTR lpPathName)
{





	CRY_ASSERT_MESSAGE(0, "RemoveDirectory not implemented yet");
	return TRUE;

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void CrySleep( unsigned int dwMilliseconds )
{
	Sleep( dwMilliseconds );
}




























//////////////////////////////////////////////////////////////////////////
int CryMessageBox( const char *lpText,const char *lpCaption,unsigned int uType)
{
#ifdef WIN32
	return MessageBox( NULL,lpText,lpCaption,uType );
#else
	printf("Messagebox: cap: %s  text:%s\n",lpCaption?lpCaption:" ",lpText?lpText:" ");
	return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////
int CryCreateDirectory( const char *lpPathName,void *lpSecurityAttributes )
{
#ifdef LINUX
	int res = mkdir(lpPathName, S_IRWXU);
#else
	int res = _mkdir(lpPathName);
#endif
	if (res != 0)
	{
		SetLastError( ERROR_PATH_NOT_FOUND );
		return -1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
int CryGetCurrentDirectory( unsigned int nBufferLength,char *lpBuffer )
{
	return GetCurrentDirectory(nBufferLength,lpBuffer);
}

#if !defined(PS3) || !defined(_LIB)
//////////////////////////////////////////////////////////////////////////
short CryGetAsyncKeyState( int vKey )
{
//TODO: implement
	CRY_ASSERT_MESSAGE(0, "CryGetAsyncKeyState not implemented yet");
	return 0;
}
#endif

#if defined(LINUX)
//[K01]: http://www.memoryhole.net/kyle/2007/05/atomic_incrementing.html
//http://forums.devx.com/archive/index.php/t-160558.html
//////////////////////////////////////////////////////////////////////////
long CryInterlockedIncrement( int volatile *lpAddend )
{
	int r;
	__asm__ __volatile__ (
		"lock ; xaddl %0, (%1) \n\t"
		: "=r" (r)
		: "r" (lpAddend), "0" (1)
		: "memory"
	);
	return (long) (r + 1);  // add, since we get the original value back.
}

//////////////////////////////////////////////////////////////////////////
long CryInterlockedDecrement( int volatile *lpAddend )
{
	int r;
	__asm__ __volatile__ (
		"lock ; xaddl %0, (%1) \n\t"
		: "=r" (r)
		: "r" (lpAddend), "0" (-1)
		: "memory"
	);
	return (long) (r - 1);  // subtract, since we get the original value back.
}

//////////////////////////////////////////////////////////////////////////
long	 CryInterlockedExchangeAdd(long volatile * lpAddend, long Value)
{
	long r;
	__asm__ __volatile__ (
	#ifdef LINUX64  // long is 64 bits on amd64.
		"lock ; xaddq %0, (%1) \n\t"
	#else
		"lock ; xaddl %0, (%1) \n\t"
	#endif
		: "=r" (r)
		: "r" (lpAddend), "0" (Value)
		: "memory"
	);
	return r;
}

long	CryInterlockedCompareExchange(long volatile * dst, long exchange, long comperand)
{
	long r;
	__asm__ __volatile__ (
	#ifdef LINUX64  // long is 64 bits on amd64.
		"lock ; cmpxchgq %2, (%1) \n\t"
	#else
		"lock ; cmpxchgl %2, (%1) \n\t"
	#endif
		: "=a" (r)
		: "r" (dst), "r" (exchange), "0" (comperand)
		: "memory"
	);
	return r;
}

unsigned int CryGetCurrentThreadId()
{
    return (unsigned int)pthread_self();
}

void CryDebugBreak()
{
	__builtin_trap();
}
#endif//LINUX








//////////////////////////////////////////////////////////////////////////
uint32 CryGetFileAttributes(const char *lpFileName)
{



#if defined(LINUX)
	string fn = lpFileName;
	adaptFilenameToLinux(fn);
	const char* buffer = fn.c_str();
#else
	const char* buffer = lpFileName;
#endif
	//test if it is a file, a directory or does not exist
	FS_ERRNO_TYPE fsErr = 0;
	int fd = -1;
	FS_OPEN(buffer, FS_O_RDONLY, fd, 0, fsErr);
	if(!fsErr)
  {
    FS_CLOSE_NOERR(fd);
		return FILE_ATTRIBUTE_NORMAL;
  }
	return (fsErr == FS_EISDIR)? FILE_ATTRIBUTE_DIRECTORY : INVALID_FILE_ATTRIBUTES;
}

#if !defined(PS3)
//////////////////////////////////////////////////////////////////////////
bool CrySetFileAttributes( const char *lpFileName,uint32 dwFileAttributes )
{
//TODO: implement
	printf("CrySetFileAttributes not properly implemented yet\n");
	return false;
}
#endif




















#if !defined(PS3)
	int file_counter = 0;
	long file_op_counter = 0;
	long file_op_break = -1;
#endif

static void WrappedF_Break(long op_counter)
{
	printf("WrappedF_Break(op_counter = %li)\n", op_counter);
}
























































































































































































































































































































































































































































































































































































































































































































































































































































































namespace std
{
// ==== file io wrapper ==== //





















































































































extern "C" FILE *WrappedFopen(const char *__restrict filename,	const char *__restrict mode)
{
	bool isWrite = false;
	bool skipOpen = false;
	char buffer[MAX_PATH + 1];
	if (fopenwrapper_basedir[0] != '/')
		WrappedF_InitCWD();

	++file_op_counter;
	if (file_op_counter == file_op_break)
		WrappedF_Break(file_op_counter);

	const bool cContainsWrongSlash = strchr(filename, '\\');
	if((cContainsWrongSlash || filename[0] != '/'))
	{
		char *bp = buffer, *const buffer_end = buffer + sizeof buffer;
		buffer_end[-1] = 0;
		if (filename[0] != '/')
		{
			strncpy(bp, fopenwrapper_basedir, buffer_end - bp - 1);
			bp += strlen(bp);
			if (bp > buffer && bp[-1] != '/' && bp < buffer_end - 2)
				*bp++ = '/';
		}
		strncpy(bp, filename, buffer_end - bp - 1);
		//replace '\\' by '/' and lower it
		const int cLen = strlen(buffer);
		for (int i = 0; i<cLen; ++i)
		{
			buffer[i] = tolower(buffer[i]);
			if (buffer[i] == '\\')
				buffer[i] = '/';
		}
		filename = buffer;
	}
	else
		if(cContainsWrongSlash)
		{
			const int cLen = strlen(filename);
			for (int i = 0; i<cLen; ++i)
			{
				buffer[i] = filename[i];
				if (buffer[i] == '\\')
					buffer[i] = '/';
			}
			filename = buffer;
		}
		// Note: "r+" is not considered to be a write-open, since fopen() will
		// fail if the specified file does not exist.
		if (strchr(mode, 'w') || strchr(mode, 'a')) isWrite = true;
#if defined(LINUX)
		const bool skipInitial = false;
#else
		const bool skipInitial = true;
#endif

		bool failOpen = false;
#if defined(USE_FILE_MAP)
		if (!FileNode::CheckOpen(filename, isWrite, skipInitial))
			failOpen = true;
#endif
		FILE *fp = 0;
#if defined(FILE_MAP_DEBUG)
		if (!skipOpen)
			fp = fopen(filename, mode);
		if (fp && failOpen)
		{
			puts("FileNode::CheckOpen error");
			assert(0);
		}
#else
		if (failOpen)
		{
			fp = NULL;
			errno = ENOENT;
		} else if (!skipOpen)
			fp = fopen(filename, mode);
#endif
		if (fp)
			++file_counter;

		return fp;
}

extern "C" int WrappedFclose(FILE *fp)
{
	const bool error = false;
	int err = 0;
	if (!error)
	{
		err  = fclose(fp);
	}
	if (err == 0)
	{
		if (!error) --file_counter;
	} else
	{
		printf("WrappedFclose: fclose() failed: %s [%i]\n",	strerror(err), err);
		WrappedF_Break(file_op_counter);
	}
	return err;
}

} // namespace std

