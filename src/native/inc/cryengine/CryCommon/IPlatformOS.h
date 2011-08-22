////////////////////////////////////////////////////////////////////////////
//
//  CryEngine Source File.
//  Copyright (C), Crytek.
// -------------------------------------------------------------------------
//  File name:   IPlatformOS.h
//  Created:     18/12/2009 by Alex Weighell.
//  Description: Interface to the Platform OS
// -------------------------------------------------------------------------
//  History:	19/05/2010 Rob Jessop: Added localisation query functions to get the SKU ID and platform supported language information
//
////////////////////////////////////////////////////////////////////////////

#include DEVIRTUALIZE_HEADER_FIX(IPlatformOS.h)

#ifndef __IPLATFORMOS_H_
#define __IPLATFORMOS_H_

#include <BoostHelpers.h>
#include <CryFixedString.h>
#include <ILocalizationManager.h>
#include <ITimer.h>





#if defined(WIN32) || defined(WIN64)
//#include <Lmcons.h> // this causes issues when including other windows headers later by defining PASCAL
#endif

class CSysCallThread;
struct IConsoleCmdArgs;


struct IVirtualKeyboardEvents
{
	virtual ~IVirtualKeyboardEvents(){}
	virtual void		KeyboardCancelled()=0;
	virtual void		KeyboardFinished(const wchar_t *pInString)=0;
};

enum EStringVerifyResponse
{
	eSVR_Invalid = -1,
	eSVR_OK = 0,
	eSVR_TooLong,
	eSVR_OffensiveText,
	eSVR_NoDefaultString,
	eSVR_InvalidLanguage
};

struct IStringVerifyEvents
{
	virtual ~IStringVerifyEvents(){}
	virtual void StringVerifyFinished(const wchar_t *inString, EStringVerifyResponse response) = 0;
	virtual void StringVerifyFailed(const wchar_t *inString) = 0;
};

enum EUserPIIStatus
{
	k_pii_notRequested,									// pii not available and not requested
	k_pii_pending,											// pii requested but not yet available
	k_pii_available,										// pii available
	k_pii_notPermittedByOS,							// pii access denied by os
	k_pii_notPermittedByUser,						// pii access denied by user
	k_pii_error													// misc error
};

struct SUserPII
{
	string						firstName;
	string						lastName;
	string						email;
	uint16						languageId;
	uint8							countryId;				// XONLINE_COUNTRY_ enum
	uint8							age;
	struct
	{
		string					street1;
		string					street2;
		string					city;
		string					district;
		string					state;
		string					postCode;
	} address;
};

// Interface platform OS (Operating System) functionality
UNIQUE_IFACE struct IPlatformOS
{
	enum ECreateFlags
	{
		eCF_EarlyCorruptionDetected = BIT(1)
	};

	enum
	{
		Unknown_User = ~0,
		Invalid_User = ~0
	};

	// Minimum amount of time the animated save icon needs to be displayed on screen when saving data
	enum
	{
		MINIMUM_SAVE_ICON_DISPLAY_TIME = 3
	};

	enum
	{




#if defined(WIN32) || defined(WIN64)
		USER_MAX_NAME = 256+1,//UNLEN+1,
#else			
		USER_MAX_NAME = 256,
#endif
	};

	typedef CryFixedStringT<USER_MAX_NAME> TUserName;
	typedef CryFixedWStringT<USER_MAX_NAME> TUserNameW;

	enum EFileOperationCode
	{
		eFOC_Success						= 0x00,		// no error
		eFOC_Failure						= 0x01,		// generic error

		eFOC_ErrorOpenRead			= 0x12,		// unable to open a file to read
		eFOC_ErrorRead					= 0x14,		// error reading from a file
		eFOC_ErrorOpenWrite			= 0x22,		// unable to open a file to write
		eFOC_ErrorWrite					= 0x24,		// error writing to a file

		// Masks
		eFOC_ReadMask						= 0x10,		// error reading
		eFOC_WriteMask					= 0x20,		// error writing
		eFOC_OpenMask						= 0x02,		// error opening files/content
	};

	enum EMsgBoxResult
	{
		eMsgBox_OK,
		eMsgBox_Cancel,
		eMsgBoxNumButtons
	};

	enum 
	{
		KbdFlag_Default			= 0x1, // Keyboard in current selected language
		KbdFlag_GamerTag 		= 0x2, // Keyboard only has gamertag characters
		KbdFlag_Email				= 0x4,	 // Keyboard for URLs and e-mails
		KbdFlag_Password		= 0x8,	// subset of latin used for passwords, also does *ing out of displayed text
	};

	// each entry fills in the relevant member of SStatAnyValue
	enum EUserProfilePreference
	{
		EUPP_CONTROLLER_INVERT_Y,			// eVT_Int (0,1)
		EUPP_CONTROLLER_SENSITIVITY,	// eVT_Float (0.0f..2.0f)
		EUPP_GAME_DIFFICULTY,					// eVT_Int (0,1,2)=(easy,medium,hard)
		EUPP_AIM_ASSIST,							// eVT_Int (0,1)
		EUPP_REGION,									// eVT_Int 
	};

	enum EDLCMountFail
	{
		eDMF_FileCorrupt,
		eDMF_DiskCorrupt,
		eDMF_XmlError,
		eDMF_Unknown,
	};

	enum EZipExtractFail
	{
		eZEF_Success,
		eZEF_Unsupported,
		eZEF_WriteFail,
		eZEF_UnknownError,
	};

	// Derive listeners from this interface
	struct IDLCListener
	{
		virtual ~IDLCListener(){}
		virtual void OnDLCMounted(const XmlNodeRef &rootNode, const char* sDLCRootFolder) = 0;
		virtual void OnDLCMountFailed(EDLCMountFail reason) = 0;
		virtual void OnDLCMountFinished( int nPacksFound ) = 0;
	};

	struct SUserProfileVariant
	{
		enum EVariantType
		{
			eVT_Invalid,
			eVT_Bool,
			eVT_Int,
			eVT_Float,
		};

		SUserProfileVariant() : m_type(eVT_Invalid), m_iValue(0) {}
		SUserProfileVariant(bool bBool) : m_type(eVT_Bool), m_bValue(bBool) {}
		SUserProfileVariant(int iValue) : m_type(eVT_Int), m_iValue(iValue) {}
		SUserProfileVariant(float fValue) : m_type(eVT_Float), m_fValue(fValue) {}

		EVariantType GetType() const { return m_type; }

		bool GetBool() const { assert(GetType() == eVT_Bool); return m_bValue; }
		int GetInt() const { assert(GetType() == eVT_Int); return m_iValue; }
		float GetFloat() const { assert(GetType() == eVT_Float); return m_fValue; }

	private:
		EVariantType m_type;

		union
		{
			bool	m_bValue;
			int		m_iValue;
			float	m_fValue;
		};
	};

	// Internal event handle passed to observers from listeners. Derive platform specific events from this
	struct SPlatformEvent 
	{
		enum EEventType
		{
			eET_None,
			eET_PlatformSpecific,

			// Platform agnostic events
			eET_SignIn,								// uses m_signIn - allows a listener to know a user has been signed in or out
			eET_StorageMounted,				// uses m_storageMounted - allows a listener to know a new save location is mounted for profiles etc.
			eET_FileError,						// uses m_fileError fired when a file error occurs (see CSaveWriter_Xenon::AppendBytes for example)
			eET_FileWrite,						// uses m_fileWrite - fired when platform code opens a profile/savegame for writing
			eET_ContentInstalled,			// allows a listener to know when downloadable content has been installed
			eET_InstallComplete,			// allow a listener to know when game installation is complete
			eET_PostLocalisationBootChecksDone,	// allow a listener to know when setup tasks have completed
			eET_PIIRetrieved,					// pii has arrived and can be queried from the IPlatformOS interface
			eET_SystemMenu,						// Entered / exited system menu (uses m_systemMenu)
			eET_ContentRemoved,				// A storage device containing DLC has been removed
		};

		enum ESignInState
		{
			eSIS_NotSignedIn,					// signed out
			eSIS_SignedInLocally,			// signed in to local machine only
			eSIS_SignedInLive,				// signed in online
		};

		// NOTE: the reason for having the issued and save start checkpoint is that saves can occur at points where saving to device is illegal by TCR/TRCs. 
		//       This allows PlatformOS to buffer up the dirty saves and issue a eFWT_SaveStart or eFWT_SaveStartCheckPoint when it is safe to write to device.
		enum EFileWriteType
		{
			eFWT_Checkpoint,					// Inform the system a checkpoint has been issued (allows us to render a save icon on the screen whilst the game generates the save)
			eFWT_CheckpointLevelStart,// Inform the system a checkpoint has been issued from level start
			eFWT_SaveStart,						// Inform the system that saving to HDD/MU has started
			eFWT_SaveEnd,							// Inform the system that saving to HDD/MU has finished
			eFWT_CreatingSave,				// When creating a save, force the icon to be displayed
		};

		SPlatformEvent(unsigned int user, EEventType eventType=eET_None) // force user index on construction
			: m_user(user), m_eEventType(eventType) {}

 		unsigned int	m_user;		
		EEventType		m_eEventType;

		union UEventParams
		{
			struct SSignIn
			{
				ESignInState m_previousSignedInState;
				ESignInState m_signedInState;
			} m_signIn;

			struct SStorageMounted
			{
				bool m_bPhysicalMedia;			// true if (360) saving to a physical device - HDD or MU or USB
				bool m_bOnlyUpdateMediaState;	// true to not attempt user change (360)
			} m_storageMounted;

			struct SFileError
			{
				EFileOperationCode m_errorType;
				bool m_bRetry;					// retry the operation - load/save
			} m_fileError;

			struct SFileWrite
			{
				EFileWriteType m_type;
			} m_fileWrite;

			struct SSystemMenu
			{
				bool m_bOpened;
				bool m_bClosed;
			} m_systemMenu;

			struct SContentRemoved
			{
				char m_root[4];
			} m_contentRemoved;
		} m_uParams;
	};

	// Platform specific events now, these have to be here for access by the rest of the engine




































	struct ISaveReader
	{
		enum ESeekMode
		{
			ESM_BEGIN,
			ESM_CURRENT,
			ESM_END,
		};

		virtual ~ISaveReader() {}

		template<class T>
		EFileOperationCode ReadItem(T& data)
		{
			return ReadBytes(reinterpret_cast<void*>(&data), sizeof(T));
		}

		virtual EFileOperationCode Seek(long seek, ESeekMode mode) = 0;
		virtual EFileOperationCode GetFileCursor(long& fileCursor) = 0;
		virtual EFileOperationCode ReadBytes(void* data, size_t numBytes) = 0;
		virtual EFileOperationCode GetNumBytes(size_t& numBytes) = 0;
		virtual EFileOperationCode Close() = 0;
		virtual IPlatformOS::EFileOperationCode LastError() const = 0;
		virtual void GetMemoryUsage(ICrySizer *pSizer) const = 0;
		virtual void TouchFile() {}
	};
	DECLARE_BOOST_POINTERS(ISaveReader);
	typedef std::vector<IPlatformOS::ISaveReaderPtr> ISaveReaderPtrVector;

	struct ISaveWriter
	{
		virtual ~ISaveWriter() {}

		template<class T>
		IPlatformOS::EFileOperationCode AppendItem(const T& data)
		{
			return AppendItems(&data, 1);
		}

		template<class T>
		IPlatformOS::EFileOperationCode AppendItems(T *data, size_t elems)
		{
			return AppendBytes(static_cast<const void*>(data), sizeof(T) * elems);
		}

		virtual IPlatformOS::EFileOperationCode AppendBytes(const void* data, size_t length) = 0;
		virtual EFileOperationCode Close() = 0;
		virtual IPlatformOS::EFileOperationCode LastError() const = 0;
		virtual void GetMemoryUsage(ICrySizer *pSizer) const = 0;
	};
	DECLARE_BOOST_POINTERS(ISaveWriter);
	typedef std::vector<IPlatformOS::ISaveWriterPtr> ISaveWriterPtrVector;

	UNIQUE_IFACE struct IFileFinder
	{
		virtual ~IFileFinder(){}
		enum EFileState
		{
			eFS_NotExist,
			eFS_File,
			eFS_Directory,
			eFS_FileOrDirectory = -1, // for Win32 based bool return value we use this - see CFileFinderCryPak::FileExists
		};

		// FileExists:
		//   Determine if a file or path exists
		virtual EFileState FileExists(const char* path) = 0;

		// FindFirst:
		//   Find the first file matching a specified pattern
		//   Returns: find handle to be used with FindNext() and FindClose()
		virtual intptr_t FindFirst(const char* filePattern, _finddata_t* fd) = 0;

		// FindClose:
		//   Close and release resources associated with FindFirst/FindNext operations
		virtual int FindClose(intptr_t handle) = 0;

		// FindNext:
		//   Find the next file matching a specified pattern
		virtual int FindNext(intptr_t handle, _finddata_t* fd) = 0;

		// MapFilePath
		// Description:
		//     Provide the ability to remap a file path into a platform specific file path.
		//     This is a convenience utility function, the default implementation simply returns the input path and returns true.
		// Arguments:
		//     const char * filePath - the raw input file path
		//     string & filePathMapped - the OS specific mapped file path
		//     bool bCreateMapping - true to record the remapped path or false to only return the mapped file path
		// Return Value:
		//     bool - true if the remapped path already existed in the file mapping table
		virtual bool MapFilePath(const char* filePath, string& filePathMapped, bool bCreateMapping) { filePathMapped = filePath; return true; }

		virtual void GetMemoryUsage(ICrySizer *pSizer) const = 0;
	};
	typedef boost::shared_ptr<IFileFinder> IFileFinderPtr;

	// CScopedSaveLoad
	// Description:
	//       Before a load or save operation embed an object of this type and check Allowed().
	//       If Allowed() fails do not load or save. Function can immediately return.
	class CScopedSaveLoad
	{
	public:
		CScopedSaveLoad(unsigned int user, bool bSave) : m_user(user) { m_allow = GetISystem()->GetPlatformOS()->BeginSaveLoad(user, bSave); }
		~CScopedSaveLoad() { if(m_allow) GetISystem()->GetPlatformOS()->EndSaveLoad(m_user); }
		bool Allowed() const { return m_allow; }
	private:
		unsigned int m_user;
		bool m_allow;
	};


	// Call once to create and initialize. Use delete to destroy.
	static IPlatformOS* Create( const uint8 createParams /*see ECreateFlags*/ );
	virtual ~IPlatformOS() {}

	// Tick with each frame to determine if there are any system messages requiring handling
	virtual void Tick(float realFrameTime) = 0;

	// Local user profile functions to check/initiate user sign in:

	// UserGetMaximumSignedInUsers:
	//   Returns the maximum number of signed in users. EG: On Xbox is 4.
	virtual unsigned int 	UserGetMaximumSignedInUsers() const = 0;

	// UserIsSignedIn:
	//   Returns true if the user is signed in to the OS.
	virtual bool 			UserIsSignedIn(unsigned int userIndex) const = 0;

	// UserIsSignedIn:
	//   Returns true if a named user is signed in to the OS.
	virtual bool 			UserIsSignedIn(const IPlatformOS::TUserName& userName, unsigned int& outUserIndex) const = 0;

	// UserDoSignIn:
	//   Initiate signin dialog box in the OS. Returns true on success.
	//   Param numUsersRequested number of users requested or 0 to sign in the default user
	//   Param controllerIndex the controller used to request sign in
	virtual bool			UserDoSignIn(unsigned int numUsersRequested = 1, unsigned int controllerIndex = 0) = 0;

	// UserSignOut
	//   Sign out a user. This is required for 360 when returning to the splash screen where you can
	//   press Start to initiate sign in with any controller.
	//   Param controllerIndex the controller used to sign out
	virtual void			UserSignOut(unsigned int user) = 0;

	// UserGetPlayerIndex:
	//   Returns the player index of a signed in user or Unknown_User if the user is not signed in.
	virtual unsigned int UserGetPlayerIndex(const char* userName) const = 0;

	// UserGetName:
	//   Get the offline name of a user. Returns true on success.
	virtual bool 			UserGetName(unsigned int userIndex, IPlatformOS::TUserName& outName) const = 0;

	// UserGetOnlineName:
	//   Get the online name of a user. Returns true on success.
	virtual bool 			UserGetOnlineName(unsigned int userIndex, IPlatformOS::TUserName& outName) const = 0;

	// UserSelectStorageDevice:
	//   Get the user to select a storage device location for save data.
	//   Returns true if storage is now available for the user
	virtual bool			UserSelectStorageDevice(unsigned int userIndex, bool bForceUI = false) = 0;

	// GetUserProfilePreference:
	//   Get a specific preference for the signed in user
	virtual bool			GetUserProfilePreference(unsigned int user, EUserProfilePreference ePreference, SUserProfileVariant& outResult) const = 0;

	// Use the TCR/TRC compliant platform save/load API for save game data
	virtual bool			UsePlatformSavingAPI() const = 0;

	// Tell the platform we are about to begin a save or load and allow it to deny the service.
	//   Param bSave true if this is a save operation, false if a load operation.
	//   Returns true if the operation is allowed to proceed, false to deny save or load.
	virtual bool BeginSaveLoad(unsigned int user, bool bSave) = 0;

	// Tell the platform we have finished a save or load.
	virtual void EndSaveLoad(unsigned int user) = 0;

	//[AlexMcC|12.02.10]: PS3 devirtualization needs ISaveReaderPtr (and writer) to be fully qualified)
	// SaveGetReader:
	//   Get a reader object to read from a save file. The file is automatically opened and closed.
	virtual IPlatformOS::ISaveReaderPtr SaveGetReader(const char* fileName, unsigned int user = IPlatformOS::Unknown_User) = 0;

	// SaveGetReader:
	//   Get a writer object to write to a save file. The file is automatically opened and closed.
	virtual IPlatformOS::ISaveWriterPtr SaveGetWriter(const char* fileName, unsigned int user = IPlatformOS::Unknown_User) = 0;

	// GetFindFile:
	//   Get a IFindFile interface for searching for files.
	virtual IPlatformOS::IFileFinderPtr GetFileFinder(unsigned int user) = 0;

	// MountDLCContent:
	//   Looks for DLC and mounts the drive so that the files can be read
	// Arguments:
	//   callback		-  The class which will receive the DLC mounted events
	//   user				-  The user index used for 360
	//   keyData		-  The license key used to load the data
	virtual void MountDLCContent(IDLCListener* pCallback, unsigned int user, const uint8 keyData[16]) = 0;

	// ExtractZips:
	//	Finds any .zip files in the directory given, opens them, and writes the unpacked contents into the same directory
	//	Useful for unpacking DLC content after download
	//	Only supported on PC
	// Arguments:
	//	path	-	The path to search for zips in
	virtual IPlatformOS::EZipExtractFail ExtractZips( const char* path ) = 0;

	// CanRestartTitle:
	//   If the platform supports restarting the current title
	virtual bool CanRestartTitle() const = 0;

	// RestartTitle:
	//   Restart the current title (if CanRestartTitle is true)
	virtual void RestartTitle(const char* pTitle) = 0;

	// Notes:
	//	 Collect memory statistics in CrySizer
	virtual void GetMemoryUsage(ICrySizer *pSizer) const = 0;

	// GetFirstSignedInUser:
	//   Returns the user ID of the first signed in user, or Unknown_User if no one is signed in.
	virtual int GetFirstSignedInUser() const = 0;

	// GetPII:
	//   Returns the user's PII (personally identifiable information). on first request it will start the act of retrieval, after which
	//   the caller can poll or wait for eET_PIIRetrieved
	virtual EUserPIIStatus GetUserPII(unsigned int inUser, SUserPII *pOutPII) = 0;

	// SetOpticalDriveIdle
	//	 Inform the platform os that the optical drive is currently idle, and extra data can be read
	//	 from it if needed
	virtual void SetOpticalDriveIdle(bool bIdle) = 0;

	// SetOpticalDriveIdle
	//	 Inform the platform os that optical drive can/can't be used for extra reads
	virtual void AllowOpticalDriveUsage(bool bAllow) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Keyboard
	//
	
	// KeyboardStart
	//   Starts the virtual keyboard. Flags are from KbdFlag_* enum. title string is not copied. you must pass a set of callbacks via IVirtualKeyboardEvents
	//   returns false if the keyboard failed to start
	virtual bool KeyboardStart(unsigned int inUserIndex, unsigned int flags, const wchar_t* title, const wchar_t* initialInput, int maxInputLength, IVirtualKeyboardEvents *pInCallback) = 0;
	
	// KeyboardIsRunning:
	//   Returns whether the virtual keyboard is currently displayed
	virtual bool KeyboardIsRunning() = 0;

	// KeyboardCancel
	//   Cancels the on screen keyboard if it is running. Calls the IVirtualKeyboardEvents::KeyboardCancelled() method
	//   Not supported on 360
	//   Returns true if successful, false if the keyboard can't be cancelled, in which case check if it is still running with KeyboardIsRunning()
	virtual bool KeyboardCancel() = 0;

	//////////////////////////////////////////////////////////////////////////
	// String Verification
	//
	
	virtual bool StringVerifyStart(const wchar_t* inString, IStringVerifyEvents *pInCallback) = 0;
	virtual bool IsVerifyingString() = 0;

	//////////////////////////////////////////////////////////////////////////
	//   Platform listener
	// 

	// Derive listeners from this interface and cast event to the appropriate platform event interface
	UNIQUE_IFACE struct IPlatformListener
	{
		virtual ~IPlatformListener(){}
		virtual void OnPlatformEvent(const IPlatformOS::SPlatformEvent& event) = 0;
	};

	virtual void AddListener(IPlatformListener* pListener, const char* szName) = 0;
	virtual void RemoveListener(IPlatformListener* pListener) = 0;
	virtual void NotifyListeners(SPlatformEvent& event) = 0;

	//////////////////////////////////////////////////////////////////////////
	//   Platform specific localization and SKU information
	//

	// Get the system language and map it to a platform-independent language code
	virtual ILocalizationManager::EPlatformIndependentLanguageID GetSystemLanguage() = 0;

	// Get the SKU identifier from the platform
	virtual const char* GetSKUId( ) = 0;

	// Compose a bitfield that enumerates the languages available on this system OS. This is used to determine the overlap
	// between languages supplied in a SKU and languages available on a system. Some platforms like the PS3 have TRCs relating
	// to whether or not you should show a language select screen based on this information.
	virtual ILocalizationManager::TLocalizatonBitfield GetSystemSupportedLanguages() = 0;

	// DebugMessageBox:
	//   Displays an OS dialog box for debugging messages.
	//   A modal (blocking) dialog box with OK and Cancel options.
	// Arguments:
	//   body   -  text body of the message
	//   title	-  title text of the message
	//   flags  -  reserved for future use
	virtual IPlatformOS::EMsgBoxResult DebugMessageBox(const char* body, const char* title, unsigned int flags=0) const = 0;
	
	//Begin platform specific boot checks to meet TRCs/TCRs. Call after localization has been initialized and chosen to meet TCG's guidance.
	//This should tend to throw fatal errors using platform specific APIs in order to meet TRCs robustly.
	virtual bool PostLocalizationBootChecks() = 0;
	//This function finishes off any platform specific initialization routines and should be called when the user leaves the initial splash screen
	//Examples include game data installation on the PS3
	virtual bool PostBootCheckProcessing() = 0;
	//Show/hide a platform specific message box. This will cause a CryWarning on platforms for which it isn't implemented and is currently used on PS3.
	virtual bool ShowBootCheckMessage(const bool bShow) = 0;

	CSysCallThread* GetSysCallThread() const;

	// Save raw save/load game to/from local hard drive for use in. Implement for each system.
	virtual bool ConsoleLoadGame(IConsoleCmdArgs* pArgs) = 0;

	struct SDebugDump
	{
		bool (*OpenFile)(const char * filename, bool append);
		bool (*WriteToFile)(const void * ptr, unsigned int size, unsigned int count);
		bool (*CloseFile)();
	};
	virtual bool DebugSave(SDebugDump& dump) = 0;

	// GetHostname
	//		Get the host name of the current machine
	virtual const char* const GetHostName() = 0;

	// InitEncryptionKey
	//    Initialize the encryption key from game
	virtual void InitEncryptionKey(const char* pMagic, size_t magicLength, const uint8* pKey, size_t keyLength) = 0;

	// GetEncryptionKey
	//    All parameters are optional
	// Arguments:
	//   pMagic      -  Magic header string to recognize an encrypted file
	//   pKey        -  Encryption key to use
	virtual void GetEncryptionKey(const std::vector<char>** pMagic = NULL, const std::vector<uint8>** pKey = NULL) = 0;


  virtual bool IsHDDAvailable() const { return true; }

  	//Function to handle a verification error. Used by some platforms to handle workarounding attempts
	virtual void HandleArchiveVerificationFailure( ) {};
protected:




		class	CFileFinderCryPak : public IFileFinder
		{
		public:
			VIRTUAL IFileFinder::EFileState FileExists(const char* path) { return gEnv->pCryPak->IsFileExist(path) ? eFS_FileOrDirectory : eFS_NotExist; }
			VIRTUAL intptr_t FindFirst(const char* filePattern, _finddata_t* fd) {



				const bool bAllowFileSystem = true;

				return gEnv->pCryPak->FindFirst(filePattern, fd, 0, bAllowFileSystem);
			}
			VIRTUAL int FindClose(intptr_t handle) { return gEnv->pCryPak->FindClose(handle); }
			VIRTUAL int FindNext(intptr_t handle, _finddata_t* fd) { return gEnv->pCryPak->FindNext(handle, fd); }
			VIRTUAL void GetMemoryUsage(ICrySizer *pSizer) const { pSizer->Add(*this); }
		private:
			CDebugAllowFileAccess m_allowFileAccess;
		};

		// Class to watch FPS until it becomes stable
		class CStableFPSWatcher
		{
			float m_resetTime;			// last time that the FPS was too low
			float m_minFPS;					// minimum stable FPS to achieve
			float m_minStableTime;	// minimum time to maintain stable FPS
			float m_maxTime;				// maximum time to test before accepting

		public:
			CStableFPSWatcher(float minFPS = 15.0f, float minStableTime = 2.5f, float maxTime = 6.0f)
				: m_resetTime(0.0f)
				, m_minFPS(minFPS)
				, m_minStableTime(minStableTime)
				, m_maxTime(maxTime)
			{
			}

			void Reset()
			{
				m_resetTime = gEnv->pTimer->GetAsyncCurTime();
			}

			// Returns true when stable FPS has been attained for the required amount of time
			bool HasAchievedStableFPS()
			{
				float fps = gEnv->pTimer->GetFrameTime(ITimer::ETIMER_UI);
				if(fps > 0.000001f)
					fps = __fres(fps);

				float curTime = gEnv->pTimer->GetAsyncCurTime();
				if(fps < m_minFPS)
				{
					Reset();
					return false;
				}
				else
				{
					float timeDiff = curTime - m_resetTime;
					if(timeDiff >= m_minStableTime)
						return true;
					if(timeDiff > m_maxTime)
						return true;
					return false;
				}
			}
		};

};

ILINE CSysCallThread* IPlatformOS::GetSysCallThread() const
{



	return NULL;

}

#endif

