// This is the prototypes of interfaces that will be used for asynchronous
// I/O (streaming).
// THIS IS NOT FINAL AND IS SUBJECT TO CHANGE WITHOUT NOTICE

// Some excerpts explaining basic ideas behind streaming design here:

/*
 * The idea is that the data loaded is ready for usage and ideally doesn't need further transformation,
 * therefore the client allocates the buffer (to avoid extra copy). All the data transformations should take place in the Resource Compiler. If you have to allocate a lot of small memory objects, you should revise this strategy in favor of one big allocation (again, that will be read directly from the compiled file).
 * Anyway, we can negotiate that the streaming engine allocates this memory.
 * In the end, it could make use of a memory pool, and copying data is not the bottleneck in our engine
 *
 * The client should take care of all fast operations. Looking up file size should be fast on the virtual
 * file system in a pak file, because the directory should be preloaded in memory
 */
#include DEVIRTUALIZE_HEADER_FIX(IStreamEngine.h)

#ifndef _CRY_COMMON_STREAM_ENGINE_HDR_
#define _CRY_COMMON_STREAM_ENGINE_HDR_

#include <list>
#include "smartptr.h"
#include "IThreadTask.h"
#include "CryThread.h"

class IStreamCallback;
class ICrySizer;

enum
{
	ERROR_UNKNOWN_ERROR          = 0xF0000000,
	ERROR_UNEXPECTED_DESTRUCTION = 0xF0000001,
	ERROR_INVALID_CALL           = 0xF0000002,
	ERROR_CANT_OPEN_FILE         = 0xF0000003,
	ERROR_REFSTREAM_ERROR        = 0xF0000004,
	ERROR_OFFSET_OUT_OF_RANGE    = 0xF0000005,
	ERROR_REGION_OUT_OF_RANGE    = 0xF0000006,
	ERROR_SIZE_OUT_OF_RANGE      = 0xF0000007,
	ERROR_CANT_START_READING     = 0xF0000008,
	ERROR_OUT_OF_MEMORY          = 0xF0000009,
	ERROR_ABORTED_ON_SHUTDOWN    = 0xF000000A,
	ERROR_OUT_OF_MEMORY_QUOTA    = 0xF000000B,
	ERROR_ZIP_CACHE_FAILURE      = 0xF000000C,
	ERROR_USER_ABORT             = 0xF000000D
};

// Summary:
//	 Types of streaming tasks
//	 Affects priority directly
enum EStreamTaskType
{
	eStreamTaskTypeCount      = 13,
	eStreamTaskTypePak				= 12,
	eStreamTaskTypeFlash      = 11,
	eStreamTaskTypeVideo      = 10,
	eStreamTaskTypeReadAhead  = 9,
	eStreamTaskTypeShader     = 8,
	eStreamTaskTypeSound      = 7,
	eStreamTaskTypeMusic      = 6,
	eStreamTaskTypeFSBCache   = 5,
	eStreamTaskTypeAnimation  = 4,
	eStreamTaskTypeTerrain    = 3,
	eStreamTaskTypeGeometry   = 2,
	eStreamTaskTypeTexture    = 1,
};

// Summary:
//	 Priority types of streaming tasks
//	 Affects priority directly
//   Limiting number of priority values allows streaming system to minimize seek time
enum EStreamTaskPriority
{
	estpUrgent		=  0,
  estpNormal		=  1,
	estpIdle		  =  2,
};

class IReadStream;

// Description:
//	 This is used as parameter to the asynchronous read function
//	 all the unnecessary parameters go here, because there are many of them.
struct StreamReadParams
{
public:
	StreamReadParams (
		DWORD_PTR _dwUserData = 0,
		EStreamTaskPriority _ePriority = estpNormal,
		unsigned _nLoadTime = 0,
		unsigned _nMaxLoadTime = 0,
		unsigned _nOffset = 0,
		unsigned _nSize = 0,
		void* _pBuffer = NULL,
		unsigned _nFlags = 0
	):
		dwUserData (_dwUserData),
		ePriority(_ePriority),
		nLoadTime(_nLoadTime),
		nMaxLoadTime(_nMaxLoadTime),
		pBuffer (_pBuffer),
		nOffset (_nOffset),
		nSize (_nSize),
		nFlags (_nFlags)
	{
	}

	// Summary:
	//	 File name.
	//const char* szFile;

	// Summary:
	//	 The callback.
	//IStreamCallback* pAsyncCallback;

    // Summary:
	//	 The user data that'll be used to call the callback.
	DWORD_PTR dwUserData;

	// The priority of this read
	EStreamTaskPriority ePriority;

	// Description:
	//	 The desirable loading time, in milliseconds, from the time of call
	//	 0 means as fast as possible (desirably in this frame).
	unsigned nLoadTime;

	// Description:
	//	 The maximum load time, in milliseconds. 0 means forever. If the read lasts longer, it can be discarded.
	//	 WARNING: avoid too small max times, like 1-10 ms, because many loads will be discarded in this case.
	unsigned nMaxLoadTime;

	// Description:
	//	 The buffer into which to read the file or the file piece
	//	 if this is NULL, the streaming engine will supply the buffer.
	// Notes:
	//	 DO NOT USE THIS BUFFER during read operation! DO NOT READ from it, it can lead to memory corruption!
	void* pBuffer;

	// Description:
	//	 Offset in the file to read; if this is not 0, then the file read
	//	 occurs beginning with the specified offset in bytes.
	//	 The callback interface receives the size of already read data as nSize
	//	 and generally behaves as if the piece of file would be a file of its own.
	unsigned nOffset;

	// Description:
	//	 Number of bytes to read; if this is 0, then the whole file is read,
	//	 if nSize == 0 && nOffset != 0, then the file from the offset to the end is read.
	//	 If nSize != 0, then the file piece from nOffset is read, at most nSize bytes
	//	 (if less, an error is reported). So, from nOffset byte to nOffset + nSize - 1 byte in the file.
	unsigned nSize;

	// Description:
	//	 The combination of one or several flags from the stream engine general purpose flags.
	// See also:
	//	 IStreamEngine::EFlags
	unsigned nFlags;
};

// typedef IReadStream_AutoPtr auto ptr wrapper
TYPEDEF_AUTOPTR(IReadStream);
typedef IReadStream_AutoPtr IReadStreamPtr;

// Description:
//	 The highest level. There is only one StreamingEngine in the application
//	 and it controls all I/O streams.
UNIQUE_IFACE struct IStreamEngine
{
public:

	struct SStatistics
	{
		struct SMediaTypeInfo
		{
			SMediaTypeInfo() {
				ResetStats();
			}
			void ResetStats() {
				memset(this, 0, sizeof(SMediaTypeInfo));
			}

			float	 fActiveDuringLastSecond;	// Amount of time media device was active during last second
			float	 fAverageActiveTime;			// Average time since last reset that the media device was active

			uint32 nBytesRead;							// Bytes read during last second.
			uint32 nRequestCount;						// Amount of requests during last second.
			uint64 nTotalBytesRead;					// Read bytes total from reset.
			uint32 nTotalRequestCount;			// Number of request from reset.

			uint64 nSeekOffsetLastSecond;		// Average seek offset during the last second
			uint64 nAverageSeekOffset;			// Average seek offset since last reset

			uint32 nCurrentReadBandwidth;		// Bytes/second for last second 
			uint32 nSessionReadBandwidth;		// Bytes/second for last second

			uint32 nActualReadBandwidth;		// Bytes/second for last second - only taking actual reading into account
			uint32 nAverageActualReadBandwidth;		// Average read bandwidth in total from reset - only taking actual read time into account
		};

		SMediaTypeInfo hddInfo;
		SMediaTypeInfo memoryInfo;
		SMediaTypeInfo discInfo;

		uint32 nTotalSessionReadBandwidth;// Average read bandwidth in total from reset - taking full time into account from reset 
		uint32 nTotalCurrentReadBandwidth;// Total bytes/sec over all types and systems.

		int nPendingReadBytes;						// How many bytes still need to be read
		float fAverageCompletionTime; 		// Time in seconds on average takes to complete file request.
		float fAverageRequestCount;				// Average requests per second being done to streaming engine
		int	nOpenRequestCount;						// Amount of open requests

		uint64 nMainStreamingThreadWait;

		uint64 nTotalBytesRead;								// Read bytes total from reset.
		uint32 nTotalRequestCount;						// Number of request from reset to the streaming engine.
		uint32 nTotalStreamingRequestCount;		// Number of request from reset which actually resulted in streaming data.

		int nCurrentDecompressCount;		// Number of requests currently waiting to be decompresses
		int nCurrentAsyncCount;					// Number of requests currently waiting to be async callback
		int nCurrentFinishedCount;			// Number of requests currently waiting to be finished by mainthread

		uint32  nDecompressBandwidth;			// Bytes/second for last second		
		uint32  nDecompressBandwidthAverage; // Bytes/second in total.

		bool bTempMemOutOfBudget;			// Was the temporary streaming memory out of budget during the last second
		int nMaxTempMemory;				// Maximum temporary memory used by the streaming system

		struct SRequestTypeInfo 
		{
			SRequestTypeInfo() : nOpenRequestCount(0), nPendingReadBytes(0) {
				ResetStats();
			}
			void ResetStats()	{
				nTmpReadBytes = 0;
				nTotalStreamingRequestCount = 0;
				nTotalReadBytes = 0;
				nTotalRequestDataSize = 0;
				nTotalRequestCount = 0;
				nCurrentReadBandwidth = 0;
				nSessionReadBandwidth = 0; 
				fTotalCompletionTime = .0f;
				fAverageCompletionTime = .0f;
			}

			void Merge(const SRequestTypeInfo& _other)
			{
				nOpenRequestCount += _other.nOpenRequestCount;
				nPendingReadBytes += _other.nPendingReadBytes;
				nTmpReadBytes += _other.nTmpReadBytes;
				nTotalStreamingRequestCount += _other.nTotalStreamingRequestCount;
				nTotalReadBytes += _other.nTotalReadBytes;
				nTotalRequestDataSize += _other.nTotalRequestDataSize;
				nTotalRequestCount += _other.nTotalRequestCount;
				fTotalCompletionTime += _other.fTotalCompletionTime;
			}

			int nOpenRequestCount;					// Amount of open request for this type
			int nPendingReadBytes;					// How many bytes still need to be read from media

			uint64 nTmpReadBytes;						// Read bytes since last update to compute current bandwidth
			
			uint32 nTotalStreamingRequestCount;	// Total actual streaming requests of this type
			uint64 nTotalReadBytes;					// Total actual read bytes (compressed data)
			uint64 nTotalRequestDataSize;		// Total requested bytes from client (uncompressed data)
			uint32 nTotalRequestCount;			// Total number of finished requests

			uint32 nCurrentReadBandwidth;		// Bytes/second for this type during last second
			uint32 nSessionReadBandwidth;		// Average read bandwidth in total from reset - taking full time into account from reset 

			float fTotalCompletionTime;			// Time it took to finish all current requests
			float fAverageCompletionTime;		// Average time it takes to fully complete a request of this type
			float fAverageRequestCount;			// Average amount of requests made per second
		};

		SRequestTypeInfo typeInfo[eStreamTaskTypeCount];
		
		struct SAsset
		{
			CryStringLocal	m_sName;
			int			m_nSize;
			const bool operator<(const SAsset& a) const { return m_nSize > a.m_nSize; }
			SAsset() {}
			SAsset(const CryStringLocal& sName, const int nSize):m_sName(sName), m_nSize(nSize) { }

			friend void swap(SAsset& a, SAsset& b)
			{
				using std::swap;

				a.m_sName.swap(b.m_sName);
				swap(a.m_nSize, b.m_nSize);
			}
		};
		DynArray<SAsset> vecHeavyAssets;
	};

	enum EJobType
	{
		ejtStarted = 1<<0,
		ejtPending = 1<<1,
		ejtFinished = 1<<2,
	};

	// Summary:
	//	 General purpose flags.
	enum EFlags
	{
		// Description:
		//	 If this is set only asynchronous callback will be called.
		FLAGS_NO_SYNC_CALLBACK = BIT(0),
		// Description:
		//	 If this is set the file will be read from disc directly, instead of from the pak system.
		FLAGS_FILE_ON_DISK = BIT(1),
		// Description:
		//	 Ignore the tmp out of streaming memory for this request
		FLAGS_IGNORE_TMP_OUT_OF_MEM = BIT(2)
	};

	// Description:
	//	 Starts asynchronous read from the specified file (the file may be on a
	//	 virtual file system, in pak or zip file or wherever).
	//	 Reads the file contents into the given buffer, up to the given size.
	//	 Upon success, calls success callback. If the file is truncated or for other 
	//	 reason can not be read, calls error callback. The callback can be NULL (in this case, the client should poll
	//	 the returned IReadStream object; the returned object must be locked for that)
	// NOTE: the error/success/ progress callbacks can also be called from INSIDE this function.
	// Arguments:
	//	 szSource	-
	//	 szFile		-
	//	 pCallback	-
	//	 pParams	- PLACEHOLDER for the future additional parameters (like priority), or really
	//				  a pointer to a structure that will hold the parameters if there are too many of them.
	// Return Value:
	//	 IReadStream is reference-counted and will be automatically deleted if you don't refer to it;
	//	 if you don't store it immediately in an auto-pointer, it may be deleted as soon as on the next line of code,
	//	 because the read operation may complete immediately inside StartRead() and the object is self-disposed
	//	 as soon as the callback is called.
	// Remarks:
	//	 In some implementations disposal of the old pointers happen synchronously
	//	 (in the main thread) outside StartRead() (it happens in the entity update),
	//	 so you're guaranteed that it won't trash inside the calling function. However, this may change in the future
	//	 and you'll be required to assign it to IReadStream immediately (StartRead will return IReadStream_AutoPtr then).
	// See also:
	//	 IReadStream,IReadStream_AutoPtr
	virtual IReadStreamPtr StartRead (const EStreamTaskType tSource, const char* szFile, IStreamCallback* pCallback = NULL, StreamReadParams* pParams = NULL) = 0;

	// Call this methods before/after submitting large number of new requests.
	virtual void BeginReadGroup() = 0;
	virtual void EndReadGroup() = 0;

	// Pause/resumes streaming of specific data types.
	// nPauseTypesBitmask is a bit mask of data types (ex, 1<<eStreamTaskTypeGeometry)
	virtual void PauseStreaming( bool bPause,uint32 nPauseTypesBitmask ) = 0;

	// Pause/resumes any IO active from the streaming engine
	virtual void PauseIO( bool bPause ) = 0;

	// Description:
	//	 Is the streaming data available on harddisc for fast streaming 
	virtual bool IsStreamDataOnHDD() const = 0;	

	// Description:
	//	 Inform streaming engine that the streaming data is available on HDD 
	virtual void SetStreamDataOnHDD(bool bFlag) = 0;

	// Description:
	//	 Per frame update ofthe streaming engine, synchronous events are dispatched from this function.
	virtual void Update() = 0;

	// Description:
	//	 Per frame update of the streaming engine, synchronous events are dispatched from this function, by particular TypesBitmask.
	virtual void Update(uint32 nUpdateTypesBitmask) = 0;

	// Description:
	//	 Waits until all submitted requests are complete. (can abort all reads which are currently in flight)
	virtual void UpdateAndWait(bool bAbortAll = false) = 0;

	// Description:
	//	 Puts the memory statistics into the given sizer object.
	//	 According to the specifications in interface ICrySizer.
	// See also:
	//	 ICrySizer
	virtual void GetMemoryStatistics(ICrySizer *pSizer) = 0;

	// Description:
	//	 Returns the streaming statistics collected from the previous call.
	virtual IStreamEngine::SStatistics& GetStreamingStatistics() = 0;
	virtual void ClearStatistics() = 0;

	virtual const char* GetStreamTaskTypeName( EStreamTaskType type ) = 0;

	// Description:
	//	 returns the bandwidth used for the given type of streaming task
	virtual void GetBandwidthStats(EStreamTaskType type, float * bandwidth) = 0;


	virtual ~IStreamEngine() {}
};

// Description:
//	 This is the file "handle" that can be used to query the status
//	 of the asynchronous operation on the file. The same object may be returned
//	 for the same file to multiple clients.
// Notes:
//	 It will actually represent the asynchronous object in memory, and will be
//	 thread-safe reference-counted (both AddRef() and Release() will be virtual
//	 and thread-safe, just like the others)
// Example:
//	 USE:
//		 IReadStream_AutoPtr pReadStream = pStreamEngine->StartRead ("bla.xxx", this);
//	 OR:
//		 pStreamEngine->StartRead ("MusicSystem","bla.xxx", this);
UNIQUE_IFACE class IReadStream : public CMultiThreadRefCount
{
public:
	// Summary:
	//	 Returns true if the file read was not successful.
	virtual bool IsError() = 0;
	// Return Value:
	//	 True if the file read was completed successfully.
	// Summary:
	//	 Checks IsError to check if the whole requested file (piece) was read.
	virtual bool IsFinished() = 0;
	// Description:
	//	 Returns the number of bytes read so far (the whole buffer size if IsFinished())
	// Arguments:
	//	 bWait - if == true, then waits until the pending I/O operation completes.
	// Return Value:
	//	 The total number of bytes read (if it completes successfully, returns the size of block being read)
	virtual unsigned int GetBytesRead(bool bWait=false) = 0;
	// Description:
	//	 Returns the buffer into which the data has been or will be read
	//	 at least GetBytesRead() bytes in this buffer are guaranteed to be already read.
	// Notes:
	//	 DO NOT USE THIS BUFFER during read operation! DO NOT READ from it, it can lead to memory corruption!
	virtual const void* GetBuffer () = 0;

	// Description:
	//	 Returns the transparent DWORD that was passed in the StreamReadParams::dwUserData field
	//	 of the structure passed in the call to IStreamEngine::StartRead.
	// See also:
	//	 StreamReadParams::dwUserData,IStreamEngine::StartRead
	virtual DWORD_PTR GetUserData() = 0;

	// Summary:
	//	 Set user defined data into stream's params.
	virtual void SetUserData(DWORD_PTR dwUserData) = 0;

	// Description:
	//	 Tries to stop reading the stream; this is advisory and may have no effect
	//	 but the callback will not be called after this. If you just destructing object,
	//	 dereference this object and it will automatically abort and release all associated resources.
	virtual void Abort() = 0;

	// Summary:
	//	 Unconditionally waits until the callback is called.
	//   if nMaxWaitMillis is not negative wait for the specified ammount of milliseconds then exit.
	// Example:
	//	 If the stream hasn't yet finish, it's guaranteed that the user-supplied callback
	//	 is called before return from this function (unless no callback was specified).
	virtual void Wait( int nMaxWaitMillis=-1 ) = 0;

	// Summary:
	//	 Returns stream params.
	virtual const StreamReadParams& GetParams() const = 0;

  // Summary:
  //	 Returns caller type.
  virtual const EStreamTaskType GetCallerType() const = 0;

	// Summary:
	//	 Returns pointer to callback routine(can be NULL).
	virtual IStreamCallback* GetCallback() const = 0;

	// Summary:
	//	 Returns IO error #.
	virtual unsigned GetError() const = 0; 

	// Summary:
	//	 Returns IO error name
	virtual const char* GetErrorName() const = 0; 

	// Summary:
	//	 Returns stream name.
	virtual const char* GetName() const = 0;

	// Summary:
	//	 Free temporary memory allocated for this stream, when not needed anymore.
	//   Can be called from Async callback, to free memory earlier, not waiting for synchrounus callback.
	virtual void FreeTemporaryMemory() = 0;

protected:
	// Summary:
	//	 The clients are not allowed to destroy this object directly; only via Release().
	virtual ~IReadStream() {}
};

TYPEDEF_AUTOPTR(IReadStream);

// Description:
//   CryPak supports asynchronous reading through this interface. The callback 
//   is called from the main thread in the frame update loop.
//
//   The callback receives packets through StreamOnComplete() and 
//   StreamOnProgress(). The second one can be used to update the asset based 
//   on the partial data that arrived. the callback that will be called by the 
//   streaming engine must be implemented by all clients that want to use 
//   StreamingEngine services
// Remarks:
//   the pStream interface is guaranteed to be locked (have reference count > 0)
//   while inside the function, but can vanish any time outside the function.
//   If you need it, keep it from the beginning (after call to StartRead())
//   some or all callbacks MAY be called from inside IStreamEngine::StartRead()
// 
// Example:
//   <code>
//   IStreamEngine *pStreamEngine = g_pISystem->GetStreamEngine();	 // get streaming engine
//   IStreamCallback *pAsyncCallback = &MyClass;									// user
// 
//   StreamReadParams params;
// 
//   params.dwUserData = 0;
//   params.nSize = 0;
//   params.pBuffer = NULL;
//   params.nLoadTime = 10000;
//   params.nMaxLoadTime = 10000;
// 
//   pStreamEngine->StartRead(  .. pAsyncCallback .. params .. );			 // registers callback
//   </code>
class IStreamCallback
{
public:
	virtual ~IStreamCallback(){}
	// Description:
	//	 Signals that reading the requested data has completed (with or without error).
	//	 This callback is always called, whether an error occurs or not.
	//	 pStream will signal either IsFinished() or IsError() and will hold the (perhaps partially) read data until this interface is released.
	//	 GetBytesRead() will return the size of the file (the completely read buffer) in case of successful operation end
	//	 or the size of partially read data in case of error (0 if nothing was read).
	//	 Pending status is true during this callback, because the callback itself is the part of IO operation.
	//	 nError == 0 : Success
	//	 nError != 0 : Error code
	virtual void StreamAsyncOnComplete (IReadStream* pStream, unsigned nError) {}

	// Description:
	//	 Signals that reading the requested data has completed (with or without error).
	//	 This callback is always called, whether an error occurs or not.
	//	 pStream will signal either IsFinished() or IsError() and will hold the (perhaps partially) read data until this interface is released.
	//	 GetBytesRead() will return the size of the file (the completely read buffer) in case of successful operation end
	//	 or the size of partially read data in case of error (0 if nothing was read).
	//	 Pending status is true during this callback, because the callback itself is the part of IO operation.
	//	 nError == 0 : Success
	//	 nError != 0 : Error code
	virtual void StreamOnComplete (IReadStream* pStream, unsigned nError) = 0;
};

#endif //_CRY_COMMON_STREAM_ENGINE_HDR_
