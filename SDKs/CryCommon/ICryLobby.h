#ifndef __ICRYLOBBY_H__
#define __ICRYLOBBY_H__

#pragma once

#define CRYLOBBY_USER_NAME_LENGTH	32
#define CRYLOBBY_USER_GUID_STRING_LENGTH	40

struct SCryUserID : public CMultiThreadRefCount
{
	virtual bool operator == (const SCryUserID &other) const = 0;

	virtual CryFixedStringT<CRYLOBBY_USER_GUID_STRING_LENGTH> GetGUIDAsString() const
	{
		return CryFixedStringT<CRYLOBBY_USER_GUID_STRING_LENGTH>("");
	}
};

struct CryUserID
{
	CryUserID() : userID(NULL)
	{
	}

	CryUserID(SCryUserID *ptr) : userID(ptr)
	{
	}

	const SCryUserID* get() const
	{
		return userID.get();
	}
	
	bool operator == (const CryUserID& other) const
	{
		if (other.IsValid() && IsValid())
		{
			return ((*other.userID) == (*userID));
		}
		if ((!other.IsValid()) && (!IsValid()))
		{
			return true;
		}
		return false;
	}

	bool IsValid() const
	{
		return (userID.get() != NULL);
	}

	_smart_ptr<SCryUserID>	userID;
};


const CryUserID CryUserInvalidID = NULL;

struct ICryMatchMaking;
struct ICryVoice;
struct ICryTCPService;
struct ICryStats;
struct ICryLobbyUI;
struct ICryFriends;
struct ICryFriendsManagement;
struct ICryReward;
struct ICryDLCStore;

#include "ICryTCPService.h"

#if USE_CRYLOBBY_GAMESPY && (defined( WIN32 ) || defined( WIN64 )) && !defined(PS3) && !defined( XENON )
#define USE_CRYLOBBY_GAMESPY			1
#define USE_CRYLOBBY_GAMESPY_VOIP	1
#else
#define USE_CRYLOBBY_GAMESPY			0
#define USE_CRYLOBBY_GAMESPY_VOIP	0
#endif

#define CRYLOBBY_USER_PACKET_START	128
#define CRYLOBBY_USER_PACKET_MAX		255

typedef uint32 CryLobbyTaskID;
const CryLobbyTaskID CryLobbyInvalidTaskID = 0xffffffff;

typedef uint16 CryPing;
#define CRYLOBBY_INVALID_PING		(CryPing(~0))

struct SCrySessionID : public CMultiThreadRefCount
{
	virtual bool operator == (const SCrySessionID &other) = 0;
	virtual bool operator < (const SCrySessionID &other)  = 0;
	virtual bool IsFromInvite() const = 0;
};

typedef _smart_ptr<SCrySessionID>	CrySessionID;
const CrySessionID CrySessionInvalidID = NULL;

enum ECryLobbyError
{
	eCLE_Success=0,								// Task is successfully started if returned from function and successfully finished if callback is called with this error.
	eCLE_SuccessContinue=1,				// For tasks that return multiple results the callback will be called with this error if there is a valid result and the callback will be called again.
	eCLE_ServiceNotSupported=2,		// The service is not supported on this platform.
	eCLE_AlreadyInitialised=3,		// Service has already been initialised.
	eCLE_NotInitialised=4,				// Service has not been initialised.
	eCLE_TooManyTasks=5,					// The task could not be started because too many tasks are already running.
	eCLE_OutOfMemory=6,						// Not enough memory to complete task.
	eCLE_OutOfSessionUserData=7,	// Trying to register too much session user data
	eCLE_UserDataNotRegistered=8,	// Using a session user data id that has not been registered.
	eCLE_UserDataTypeMissMatch=9,	// Live - The data type of the session user data is not compatible with the data type defined in the xlast program.
	eCLE_TooManySessions=10,			// The session could not be created because there are too many session already created.
	eCLE_InvalidSession=11,				// The specified session handle does not exist.
	eCLE_InvalidRequest=12,				// The task being performed is invalid.
	eCLE_SPAFileOutOfDate=13,			// Live - The SPA file used doesn't match the one on the live servers.
	eCLE_ConnectionFailed=14,			// Connection to session host failed.
	eCLE_SessionFull=15,					// Can't join session because it is full.
	eCLE_SessionWrongState=16,		// The session is in the wrong state for the requested operation to be performed.
	eCLE_UserNotSignedIn=17,			// The user specified is not signed in.
	eCLE_InvalidParam=18,					// An invalid parameter was passed to function.
	eCLE_TimeOut=19,							// The current task has timed out waiting for a response
	eCLE_InsufficientPrivileges=20,	// User had insufficient privileges to perform the requested task. In Live a silver account is being used when a gold account is required.
	eCLE_AlreadyInSession=21,			// Trying to join a session that has already been joined.
	eCLE_LeaderBoardNotRegistered=22,	// Using a leaderboard id that has not been registered.
	eCLE_UserNotInSession = 23,		// Trying to write to a leaderboard for a user who is not in the session.
	eCLE_OutOfUserData=24,				// Trying to register too much user data
	eCLE_NoUserDataRegistered=25,	// Trying to read/write user data when no data has been registered.
	eCLE_ReadDataNotWritten=26,		// Trying to read user data that has never been written.
	eCLE_UserDataMissMatch=27,		// Trying to write user data that is different to registered data.
	eCLE_InvalidUser=28,					// Trying to use an invalid user id.
	eCLE_PSNContextError=29,			// Somethings wrong with one of the various PSN contexts
	eCLE_PSNWrongSupportState=30,	// Invalid state in PSN support state machine
	eCLE_SuccessUnreachable=31,		// For session search the callback will be called with this error if a session was found but is unreachable and the callback will be called again with other results.
	eCLE_ServerNotDefined=32,			// Server not set
	eCLE_WorldNotDefined=33,			// World not set
	eCLE_SystemIsBusy=34,					// System is busy. XMB is doing something else and cannot work on the new task. trying to bring up more than 1 UI dialogs at a time, or look at friends list while it is being downloaded in the background, etc.
	eCLE_TooManyParameters=35,		// Too many parameters were passed to the task. e.g an array of SCryLobbyUserData has more items than the task expected.
	eCLE_NotEnoughParameters=36,	// Not enough parameters were passed to the task. e.g an array of SCryLobbyUserData has less items than the task expected.
	eCLE_DuplicateParameters=37,	// Duplicate parameters were passed to the task. e.g an array of SCryLobbyUserData has the same item more than once.
	eCLE_ExceededReadWriteLimits=38,	// This error can be returned if the underlying SDK has limits on how often the task can be run.
	eCLE_InvalidTitleID=39,				// Title is using an invalid title id
	eCLE_IllegalSessionJoin=40,		// An illegal session join has been performed. e.g in Live a session has been created or joined that has invites enabled when the user is already in a session with invites enabled.
	eCLE_NotJoined=41,						// GameSpy: peer not a member of a room of this type
	eCLE_JoinInProgress=42,				// GameSpy: join already in progress for a room of this type
	eCLE_AlreadyJoined=43,				// GameSpy: peer already joined room of this type
	eCLE_InternetDisabled=44,			// PSN: No access to Internet
	eCLE_NoOnlineAccount=45,			// PSN: No online account.
	eCLE_NotConnected=46,					// PSN: Not connected
	eCLE_ObjectNotConnected=47,		// GameSpy: operation required a GameSpy object to be in connected state but it wasn't
	eCLE_CyclingForInvite=48,			// PSN: Currently handling a PS3 invite.
	eCLE_UnhandledNickError=49,		// GameSpy: a nick error occurred during a login attempt but it was not handled
	eCLE_InvalidLoginCredentials=50, // GameSpy: login credentials invalid
	eCLE_ServerUnreachable=51,		// GameSpy: server unreachable
	eCLE_GP_ParameterError=52,		// GameSpy: GP_PARAMETER_ERROR
	eCLE_GP_NetworkError=53,			// GameSpy: GP_NETWORK_ERROR
	eCLE_CableNotConnected = 54,	// Ethernet cable is not connected
	eCLE_SessionNotMigratable = 55,	// An attempt to migrate a non-migratable session was detected
	eCLE_SuccessInvalidSession = 56,	// If SessionEnd or SessionDelete is called for an invalid session, return this as a successful failure.
	eCLE_RoomDoesNotExist = 57,		// A PSN request was triggered for a room that doesn't exist on the PSN service
	eCLE_PSNUnavailable = 58,			// PSN: service is currently unavailable (might be under maintenance, etc. Not the same as unreachable!)
	eCLE_TooManyOrders = 59,			// Too many DLC store orders
	eCLE_InvalidOrder = 60,				// Order does not exist
	eCLE_OrderInUse = 61,					// Order already has an active task
	eCLE_OnlineAccountBlocked = 62,	// PSN: Account is suspended or banned
	eCLE_AgeRestricted = 63,			// PSN: Online access restricted because of age (child accounts only).
	eCLE_ReadDataCorrupt = 64,		// UserData is corrupted/wrong size/invalid. (Not the same as eCLE_ReadDataNotWritten, which means not present).
	eCLE_PasswordIncorrect = 65,	// GameSpy: client passed the wrong password to the server in the session join request
	eCLE_InvalidInviteFriendData = 66,		// PSN: Invite friend data is invalid/unavailable
	eCLE_InvalidJoinFriendData = 67,			// PSN: Join friend data is invalid/unavailable
	eCLE_OnlineNameRejected = 68,	// GameSpy: online name rejected while creating account
	eCLE_OtherUsersAccount = 69,	// GameSpy: Attempt to create account with email address already used by another user
	eCLE_ThisUsersAccount = 70,		// GameSpy: Attempt to create account with email address already used by this user, if wrong uniquenick a reminder will be dispatched in an eCLSE_OnlineNameRejected event.
	eCLE_Cancelled = 71,					// GameSpy: login attempt was cancelled
	eCLE_InvalidPing = 72,				// No valid ping value found for user in session

	eCLE_InternalError,

	eCLE_NumErrors
};

enum ECryLobbyService
{
	eCLS_LAN,
	eCLS_Online,
	eCLS_NumServices
};

typedef uint32 CryLobbyUserDataID;

enum ECryLobbyUserDataType
{
	eCLUDT_Int64,
	eCLUDT_Int32,
	eCLUDT_Int16,
	eCLUDT_Int8,
	eCLUDT_Float64,
	eCLUDT_Float32,
	eCLUDT_Int64NoEndianSwap
};

struct SCryLobbyUserData
{
	CryLobbyUserDataID			m_id;
	ECryLobbyUserDataType		m_type;

	union
	{
		int64									m_int64;
		f64										m_f64;
		int32									m_int32;
		f32										m_f32;
		int16									m_int16;
		int8									m_int8;
	};

	const SCryLobbyUserData& operator=( const SCryLobbyUserData& src )
	{
		m_id = src.m_id;
		m_type = src.m_type;

		switch ( m_type )
		{
		case eCLUDT_Int64:
			m_int64 = src.m_int64;
			break;
		case eCLUDT_Int32:
			m_int32 = src.m_int32;
			break;
		case eCLUDT_Int16:
			m_int16 = src.m_int16;
			break;
		case eCLUDT_Int8:
			m_int8 = src.m_int8;
			break;
		case eCLUDT_Float64:
			m_f64 = src.m_f64;
			break;
		case eCLUDT_Float32:
			m_f32 = src.m_f32;
			break;
		case eCLUDT_Int64NoEndianSwap:
			m_int64 = src.m_int64;
			break;
		default:
			CryLog( "Unhandled ECryLobbyUserDataType %d", m_type );
			break;
		}

		return *this;
	};

	bool operator == (const SCryLobbyUserData &other)
	{
		if ((m_id == other.m_id) && (m_type == other.m_type))
		{
			switch (m_type)
			{
			case eCLUDT_Int64:
				return m_int64 == other.m_int64;
			case eCLUDT_Int32:
				return m_int32 == other.m_int32;
			case eCLUDT_Int16:
				return m_int16 == other.m_int16;
			case eCLUDT_Int8:
				return m_int8 == other.m_int8;
			case eCLUDT_Float64:
				return m_f64 == other.m_f64;
			case eCLUDT_Float32:
				return m_f32 == other.m_f32;
			case eCLUDT_Int64NoEndianSwap:
				return m_int64 == other.m_int64;
			default:
				CryLog("Unhandled ECryLobbyUserDataType %d", m_type);
				return false;
			}
		}

		return false;
	}

	bool operator != (const SCryLobbyUserData &other)
	{
		if ((m_id == other.m_id) && (m_type == other.m_type))
		{
			switch (m_type)
			{
			case eCLUDT_Int64:
				return m_int64 != other.m_int64;
			case eCLUDT_Int32:
				return m_int32 != other.m_int32;
			case eCLUDT_Int16:
				return m_int16 != other.m_int16;
			case eCLUDT_Int8:
				return m_int8 != other.m_int8;
			case eCLUDT_Float64:
				return m_f64 != other.m_f64;
			case eCLUDT_Float32:
				return m_f32 != other.m_f32;
			case eCLUDT_Int64NoEndianSwap:
				return m_int64 != other.m_int64;
			default:
				CryLog("Unhandled ECryLobbyUserDataType %d", m_type);
				return true;
			}
		}

		return true;
	}
};

struct SCryLobbyPartyMember
{
	CryUserID										m_userID;
};

struct SCryLobbyPartyMembers
{
	uint32											m_numMembers;
	SCryLobbyPartyMember*				m_pMembers;
};

typedef void (*CryGameSpyVoiceCodecTerminateCallback)(void);
struct SCryGameSpyVoiceCodecInfo
{
	void*																	m_pCustomCodecInfo;
	CryGameSpyVoiceCodecTerminateCallback	m_pTerminateCallback;
};

// The callback below is fired whenever the lobby system needs access to data that is unavailable through the standard API.
//Its usually used to work around problems with specific platform lobbies requiring data not exposed through standard functionality.

//Basically whenever this callback is fired, you are required to fill in the requestedParams that are asked for.
//At present specifications are that the callback can fire multiple times.
//
// 'PCom'		void*		ptr to static SceNpCommunitcationId					(not copied - DO NOT PLACE ON STACK!)
// 'PPas'		void*		ptr to static SceNpCommunicationPassphrase	(not copied - DO NOT PLACE ON STACK!)
// 'PSig'		void*		ptr to static SceNpCommunicationSignature		(not copied - DO NOT PLACE ON STACK!)
// 'XSvc'		uint32	Service Id for XLSP servers
// 'XPor'		uint16	Port for XLSP server to communicate with Telemetry
// 'XSNm'		void*		ptr to a static const string containging the name of the required XLSP service. is used to filter the returned server list from Live. return NULL for no filtering
// 'LUnm'		void*		ptr to user name for local user - used by LAN (due to lack of guid) (is copied internally - DO NOT PLACE ON STACK)
//
// 'PInM'		char*		ptr to string used for the custom XMB button for sending invites to friends.
// 'PInS'		char*		ptr to string used for the XMB game invite message subject text.
// 'PInB'		char*		ptr to string used for the XMB game invite message body text.
// 'PFrS'		char*		ptr to string used for the XMB friend request message subject text.
// 'PFrB'		char*		ptr to string used for the XMB friend request message body text.
// 'PAge'		int32		Age limit of game (set to 0 for now, probably wants to come from some kind of configuration file)
// 'PSto'		char*		Store ID. AKA NP Commerce ServiceID and also top level category Id.
// 'PDlc'		int8		Input is DLC pack index to test is installed, output is boolean result.
//
// 'GEml'		char*		ptr to string used for GameSpy email address
// 'GKey'		char*		ptr to string used for GameSpy secret key (not copied - DO NOT PLACE ON STACK!)
// 'GKnm'		IN:			uint8 is ELOBBYIDS of key whose name is being requested.
//					OUT:		char* is ptr to string used for name of key (not copied - DO NOT PLACE ON STACK!)
// 'GKsv'		IN:			ptr to SCryLobbyUserDataStringParam
//					OUT:		ptr to string from which hash was generated
// 'GNms'		uint32	GameSpy namespace ID
// 'GGid'		uint32	GameSpy Game ID
// 'GPrd'		uint32	GameSpy product ID
// 'GPrt'		uint32	GameSpy partner ID
// 'GPwd'		char*		ptr to string used for GameSpy password
// 'GRqn'		char*		ptr to string used for nick of required profile
// 'GTtl'		char*		ptr to string used for GameSpy title (not copied - DO NOT PLACE ON STACK!)
// 'GUnk'		char*		ptr to string used for GameSpy unique nick
// 'GVer'		uint32	GameSpy game version
// 'GClf'		char*		GameSpy current leaderboard format
// 'GPlf'		char*		GameSpy P2P leaderboard format
// 'GDlf'		char*		GameSpy dedicated server leaderboard format
// 'GVCo'		GVCustomCodecInfo* GameSpy voice custom codec information structure
// 'CSgs'		uint32	ECryLobbyLoginGUIState constant to indicate whether other data requested is available
// 'CPre'		SCryLobbyPresenceConverter* in/out structure for converting a list of SCryLobbyUserData into a single string for presense
// 'Mspl'		Matchmaking session password length (not including null terminator)

#define CLCC_LAN_USER_NAME								'LUnm'
#define CLCC_XLSP_SERVICE_ID							'XSvc'
#define CLCC_XLSP_SERVICE_PORT						'XPor'
#define CLCC_XLSP_SERVICE_NAME						'XSNm'
#define CLCC_PSN_COMMUNICATION_ID					'PCom'
#define CLCC_PSN_COMMUNICATION_PASSPHRASE	'PPas'
#define CLCC_PSN_COMMUNICATION_SIGNATURE	'PSig'
#define CLCC_PSN_CUSTOM_MENU_GAME_INVITE_STRING		'PInM'
#define CLCC_PSN_CUSTOM_MENU_GAME_JOIN_STRING			'PJoM'
#define CLCC_PSN_INVITE_SUBJECT_STRING						'PInS'
#define CLCC_PSN_INVITE_BODY_STRING								'PInB'
#define CLCC_PSN_FRIEND_REQUEST_SUBJECT_STRING		'PFrS'
#define CLCC_PSN_FRIEND_REQUEST_BODY_STRING				'PFrB'
#define CLCC_PSN_AGE_LIMIT								'PAge'
#define CLCC_PSN_STORE_ID									'PSto'
#define CLCC_PSN_IS_DLC_INSTALLED					'PDlc'

#define CLCC_GAMESPY_D2GCATALOGREGION		'GDcr'
#define CLCC_GAMESPY_D2GCATALOGTOKEN		'GDct'
#define CLCC_GAMESPY_D2GCATALOGVERSION	'GDcv'
#define CLCC_GAMESPY_EMAIL					'GEml'
#define CLCC_GAMESPY_SECRETKEY			'GKey'
#define CLCC_GAMESPY_KEYNAME				'GKnm'
#define CLCC_GAMESPY_KEYSTRINGVALUE	'GKsv'
#define CLCC_GAMESPY_NAMESPACEID		'GNms'
#define CLCC_GAMESPY_PRODUCTID			'GPrd'
#define CLCC_GAMESPY_GAMEID					'GGid'
#define CLCC_GAMESPY_PARTNERID			'GPrt'
#define CLCC_GAMESPY_PASSWORD				'GPwd'
#define CLCC_GAMESPY_REQUIREDNICK		'GRqn'
#define CLCC_GAMESPY_TITLE					'GTtl'
#define CLCC_GAMESPY_UNIQUENICK			'GUnk'
#define CLCC_GAMESPY_GAMEVERSION		'GVer'
#define CLCC_GAMESPY_P2PLEADERBOARDFMT				'GPlf'
#define CLCC_GAMESPY_DEDICATEDLEADERBOARDFMT	'GDlf'
#define CLCC_GAMESPY_VOICE_CODEC		'GVCo'

#define CLCC_CRYLOBBY_LOGINGUISTATE	'CSgs'	// Common to all online services for which login is not handled by OS

#define CLCC_CRYLOBBY_PRESENCE_CONVERTER	'CPre'	// Used by PSN and Gamespy for converting presence info into a string form

#define CLCC_CRYSTATS_ENCRYPTION_KEY	'CEnc' // Used for all platforms to encrypt UserData buffers

#define CLCC_MATCHMAKING_SESSION_PASSWORD_MAX_LENGTH 'Mspl' // Used to determine the maximum length of the session password (not including NULL character)


enum ECryLobbyLeaderboardType
{
	eCLLT_P2P,
	eCLLT_Dedicated,
	eCLLT_Num
};

enum ECryLobbyLoginGUIState
{
	eCLLGS_NotImplemented,	// Login GUI not implemented
	eCLLGS_NotFinished,			// Values in GUI fields not confirmed, engine must not read values returned along with this
	eCLLGS_Cancelled,				// Abort the login attempt
	eCLLGS_NewAccount,			// Create a new account with values returned along with this
	eCLLGS_ExistingAccount	// Login to existing account with values returned along with this
};

// User credential identifiers used by GetUserCredentials task
enum ECryLobbyUserCredential
{
	eCLUC_Email,				// Email address
	eCLUC_Password,			// Password
	eCLUC_ProfileNick,	// GameSpy profile nick
	eCLUC_UniqueNick		// GameSpy unique nick
};

struct SCryLobbyUserDataStringParam
{
	CryLobbyUserDataID		id;			// Do not modify. ID of a lobby user datum for which the configuration callback returned two names.
	uint32								value;	// Do not modify. A numeric hash of a string that the game wrote to the datum.
};

struct SConfigurationParams
{
	uint32		m_fourCCID;
	union
	{
		uint64	m_64;
		void*		m_pData;
		uint32	m_32;
		uint16	m_16;
		uint8		m_8;
	};
};

struct SAgeData
{
	int age;
	char countryCode[2];
};

typedef void (*CryLobbyConfigurationCallback)(ECryLobbyService service, SConfigurationParams *requestedParams, uint32 paramCount);

typedef void (*CryLobbyCallback)(ECryLobbyService service, ECryLobbyError error, void* arg);

// CryLobbyPrivilegeCallback
// taskID			-	Task ID allocated when the function was executed
// error			- Error code - eCLE_Success if the function succeeded or an error that occurred while processing the function
// privilege	- A bitfield of CLPF_* specifying the users privileges.
//							Bits get set to indicate the user should be blocked from an action if their privilege level isn't high enough.
// pArg				- Pointer to application-specified data
typedef void (*CryLobbyPrivilegeCallback)(CryLobbyTaskID taskID, ECryLobbyError error, uint32 privilege, void* pArg);

// CryLobbyOnlineStateCallback
// taskID			-	Task ID allocated when the function was executed
// error			- Error code - eCLE_Success if the function succeeded or an error that occurred while processing the function
// pArg				- Pointer to application-specified data
typedef void ( *CryLobbyOnlineStateCallback )( CryLobbyTaskID taskID, ECryLobbyError error, void* pArg );

// CryLobbyGetUserCredentialsCallback
// taskID			-	Task ID allocated when the function was executed
// error			- Error code - eCLE_Success if the function succeeded or an error that occurred while processing the function
// pArg				- Pointer to application-specified data
typedef void ( *CryLobbyGetUserCredentialsCallback )( CryLobbyTaskID taskID, ECryLobbyError error, std::map< ECryLobbyUserCredential, string >* pCredentials, void* pArg );

// CryLobbyCheckProfanityCallback
// taskID			-	Task ID allocated when the function was executed
// error			- Error code - eCLE_Success if the function succeeded or an error that occurred while processing the function
// pString		- string
// isProfanity - true if the string contained a profanity, otherwise false
// pArg				- Pointer to application-specified data
typedef void ( *CryLobbyCheckProfanityCallback )( CryLobbyTaskID taskID, ECryLobbyError error, const char* pString, bool isProfanity, void* pArg );

// When using the GameSpy online service, session user data IDs must be in the
// range 0-253.
//
// User data IDs in the range 0-49 are defined by GameSpy:
//
//		IDs in that range that don't have a constant in the ECryGameSpyKey enum
//		must not be used.
//
//		Some IDs in that range that do have a constant in the ECryGameSpyKey enum
//		are handled internally by CryNetwork, and must not be used as user data
//		IDs by the game. These IDs have comments in the enum.
//
//		One ID in that range that has a constant in the ECryGameSpyKey enum
//		(eCGSK_GameModeKey) may be used as a user data ID, but will have a default
//		value provided by CryNetwork if it is not. This ID has a comment in the
//		enum.
//
//		Some IDs in that range that do have a constant in the ECryGameSpyKey enum
//		are handled internally by GameSpy, and must not be used as user data IDs
//		by the game. These IDs have comments in the enum.
//
//		All other IDs in that range that do have constants in the ECryGameSpyKey
//		enum may be used as user data IDs by the game. Ideally, they should be used
//		for the type of user data that their names describe. Games are not required
//		to provide a value for every ID in this range.
//
// User data IDs in the the range 50-53 are defined by CryNetwork, and must not
// be used by the game. These IDs have comments in the enum.
//
// User data IDs in the range 54-253 are not defined and may be used by the game
// for any purpose.

enum ECryGameSpyKey
{
	// GameSpy.

	 eCGSK_NumGameSpyReservedKeys = 50,
	 eCGSK_FirstGameSpyReservedKey = 0,
	 eCGSK_LastGameSpyReservedKey = eCGSK_FirstGameSpyReservedKey + eCGSK_NumGameSpyReservedKeys - 1,
	 eCGSK_FirstGameSpyUsedKey = 1,
	 eCGSK_LastGameSpyUsedKey = 32,

	 eCGSK_HostNameKey = 1,					// Name of session host. Handled internally by CryNetwork; no user data ID should equal this.
	 eCGSK_GameNameKey = 2,					// Required to match assigned GameSpy title. Handled internally by CryNetwork; no user data ID should equal this.
	 eCGSK_GameVerKey = 3,					// Game version. Handled internally by CryNetwork; no user data ID should equal this.
	 eCGSK_HostPortKey = 4,
	 eCGSK_MapNameKey = 5,
	 eCGSK_GameTypeKey = 6,
	 eCGSK_GameVariantKey = 7,
	 eCGSK_NumPlayersKey = 8,				// Number of players in game session. Handled internally by CryNetwork; no user data ID should equal this.
	 eCGSK_NumTeamsKey = 9,
	 eCGSK_MaxPlayersKey = 10,			// Maximum number of players in game session. Handled internally by CryNetwork; no user data ID should equal this.
	 eCGSK_GameModeKey = 11,				// Game mode. The game may provide a user data ID equal to this. If not, it will be handled internally by CryNetwork.
	 eCGSK_TeamPlayKey = 12,
	 eCGSK_FragLimitKey = 13,
	 eCGSK_TeamFragLimitKey = 14,
	 eCGSK_TimeElapsedKey = 15,
	 eCGSK_TimeLimitKey = 16,
	 eCGSK_RoundTimeKey = 17,
	 eCGSK_RoundElapsedKey = 18,
	 eCGSK_PasswordKey = 19,
	 eCGSK_GroupIDKey = 20,					
	 eCGSK_PlayerKey = 21,
	 eCGSK_ScoreKey = 22,
	 eCGSK_SkillKey = 23,
	 eCGSK_PingKey = 24,
	 eCGSK_TeamKey = 25,
	 eCGSK_DeathsKey = 26,
	 eCGSK_PidKey = 27,
	 eCGSK_TeamTKey = 28,
	 eCGSK_ScoreTKey = 29,
	 eCGSK_NNGroupIDKey = 30,
	 eCGSK_CountryKey = 31,					// Country. Handled internally by GameSpy; no user data ID should equal this.
	 eCGSK_RegionKey = 32,					// Region. Handled internally by GameSpy; no user data ID should equal this.

	// CryNetwork.

	 eCGSK_NumCryNetworkReservedKeys = 8,
	 eCGSK_FirstCryNetworkReservedKey = eCGSK_LastGameSpyReservedKey + 1,
	 eCGSK_LastCryNetworkReservedKey = eCGSK_FirstCryNetworkReservedKey + eCGSK_NumCryNetworkReservedKeys - 1,

	 eCGSK_SessionIDKey = eCGSK_FirstCryNetworkReservedKey,						// Session ID. Handled internally by CryNetwork; no user data ID should equal this.
	 eCGSK_NumPrivateSlotsKey = eCGSK_FirstCryNetworkReservedKey + 1,	// Number of private slots. Handled internally by CryNetwork; no user data ID should equal this.
	 eCGSK_NumPublicSlotsKey = eCGSK_FirstCryNetworkReservedKey + 2,	// Number of public slots. Handled internally by CryNetwork; no user data ID should equal this.
	 eCGSK_RankedKey = eCGSK_FirstCryNetworkReservedKey + 3,					// Is the game ranked? Handled internally by CryNetwork; no user data ID should equal this.
	 eCGSK_CreateFlagsKey = eCGSK_FirstCryNetworkReservedKey + 4,			// Flags used to create the session. Handled internally by CryNetwork; no user data ID should equal this.
	 eCGSK_NumMigrationsKey = eCGSK_FirstCryNetworkReservedKey + 5,		// Number of times the session has been migrated. Handled internally by CryNetwork; no user data ID should equal this.
	 eCGSK_PasswordRequiredKey = eCGSK_FirstCryNetworkReservedKey + 6,// Is a password required. Handled internally by CryNetwork; no user data ID should equal this.
	 eCGSK_DedicatedKey = eCGSK_FirstCryNetworkReservedKey + 7,

	 eCGSK_NumCryNetworkOptionalKeys = 1, // Number of keys that CryNetwork doesn't need to use itself, but would require special handling if the game needs to use them. Currently only eCGSK_HostProfileKey.
	 eCGSK_FirstCryNetworkOptionalKey = eCGSK_LastCryNetworkReservedKey + 1,
	 eCGSK_LastCryNetworkOptionalKey = eCGSK_FirstCryNetworkOptionalKey + eCGSK_NumCryNetworkOptionalKeys - 1,
	 eCGSK_HostProfileKey = eCGSK_FirstCryNetworkOptionalKey,					// Hosting profile ID. The game may provide a user data ID equal to this, but attempts to set it on session creation will have no effect.

	// Game.

	 eCGSK_FirstGameReservedKey = 64,
	 eCGSK_LastGameReservedKey = 127,
	 eCGSK_NumGameReservedKeys = eCGSK_LastGameReservedKey - eCGSK_FirstGameReservedKey + 1
};

// Namespaces.

#define GAMESPY_DEFAULT_NAMESPACEID				( 1 )

// Partner IDs.

#define GAMESPY_DEFAULT_PARTNERID				( 0 )


// Flags for use with ICryLobby::GetLobbyServiceFlag.
enum ECryLobbyServiceFlag
{
	eCLSF_AllowMultipleSessions = BIT( 0 ),	// Does the lobby service allow multiple sessions to be joined? (Default True)
	eCLSF_SupportHostMigration = BIT( 1 )		// Does the lobby service support host migration? (Default: True)
};

// CryLobbyInviteAcceptedData 
// If the underlying SDK has a frontend invites can be accepted from then this callback will be called if an invite is accepted this way.
// When this call back is called the game must switch to the given service and join the session.
// If the user is in a session already then the old session must be deleted before the join is started.
// Structure returned when a registered callback is triggered for invite acceptance
// service	- Which of the CryLobby services this is for
// user			- id of local user this pertains to
// id				- session identifier of which session to join
struct SCryLobbyInviteAcceptedData
{
	ECryLobbyService	m_service;
	uint32						m_user;
	CrySessionID			m_id;
	ECryLobbyError		m_error;
};

// SCryLobbyPresenceConverter
// When a call to set Rich Presence is made, the SCryLobbyUserData is just passed back to the game code, so 
// it's the game code's responsibility to correctly turn all the bits and bobs of SCryLobbyUserData into a single UTF-8 string.
// m_pData - ptr of the SCryLobbyUserData passed into CryLobbyUI::SetRichPresence.
// m_numData - number of SCryLobbyUserData in array pointed to by m_pData.
// m_pStringBuffer - buffer to write the result string in to.
//   This buffer is pre-allocated internally, no need to create your own.
// m_sizeOfStringBuffer - size of the string buffer in bytes, not characters.
//   [in] = maximum size of the buffer pre-allocated [SCE_NP_BASIC_PRESENCE_EXTENDED_STATUS_SIZE_MAX].
//   [out] = modify the value to contain the actual length of the string in bytes.
// m_sessionId - joinable session ID, to allow friends to join your game if desired.
struct SCryLobbyPresenceConverter
{
	SCryLobbyUserData*		m_pData;								// [in]
	uint32								m_numData;							// [in]
	uint8*								m_pStringBuffer;				// [in/out]
	uint32								m_sizeOfStringBuffer;		// [in/out]
	CrySessionID					m_sessionId;						// [out]
};

// SCryLobbyXMBString
// A request to fill in an XMB string requires a buffer allocated by the calling function, and a value defining the size of the buffer
// It's the game's responsibility to fill the buffer with a UTF-8 string, and not to overflow the buffer.
// m_pStringBuffer - pointer to a UTF-8 compatible string buffer.
// m_sizeOfStringBuffer - maximum sizeof the buffer. 
struct SCryLobbyXMBString
{
	uint8*				m_pStringBuffer;
	uint32				m_sizeOfStringBuffer;
};

// Privilege bits returned from ICryLobbyService::GetUserPrivileges in the CryLobbyPrivilegeCallback.
const uint32 CLPF_BlockMultiplayerSessons = BIT(0);		// If set the user is not allowed to participate in multiplayer game sessions.

struct SCrySystemTime
{
	uint16 m_Year;
	uint16 m_Milliseconds;
	uint8 m_Month;
	uint8 m_DayOfWeek;
	uint8 m_Day;
	uint8 m_Hour;
	uint8 m_Minute;
	uint8 m_Second;
};

struct ICryLobbyService
{
public:
	virtual ~ICryLobbyService(){}
	virtual ICryMatchMaking*				GetMatchMaking() = 0;
	virtual ICryVoice*							GetVoice() = 0;
	virtual ICryStats*							GetStats() = 0;
	virtual ICryLobbyUI*						GetLobbyUI() = 0;
	virtual ICryFriends*						GetFriends() = 0;
	virtual ICryFriendsManagement*	GetFriendsManagement() = 0;
	virtual ICryReward*							GetReward() = 0;
	virtual ICryDLCStore*						GetDLCStore() = 0;

	// CancelTask
	// Cancel the given task. The task will still be running in the background but the callback will not be called when it finishes.
	// taskID			- The task to cancel
	virtual void CancelTask(CryLobbyTaskID lTaskID) = 0;

	// Description:
	//	 Get an ICryTCPService.
	// Arguments:
	//	 pUrl - server url.
	//	 port - port
	// Return:
	//	 Pointer to ICryTCPService for given server & port.
	virtual ICryTCPServicePtr	GetTCPService(const char* pUrl, uint16 port) = 0;

	// GetUserID
	// Get the user id of a user signed in to this service locally.
	// user				- The pad number of the local user.
	// return			- The CryUserID of the user.
	virtual	CryUserID					GetUserID(uint32 user) = 0;

	// GetUserPrivileges
	// Get the given users privileges
	// user				- The pad number of the local user.
	// pTaskID		- Pointer to buffer to store the task ID to identify this call in the callback
	// pCB				- Callback function that is called when function completes
	// pCBArg			- Pointer to application-specified data that is passed to the callback
	// return			- eCLE_Success if function successfully started or an error code if function failed to start
	virtual	ECryLobbyError		GetUserPrivileges(uint32 user, CryLobbyTaskID* pTaskID, CryLobbyPrivilegeCallback pCB, void* pCBArg) = 0;

	// GetUserCredentials
	// Get the given user's credentials
	// user				- The pad number of the local user.
	// pTaskID		- Pointer to buffer to store the task ID to identify this call in the callback
	// pCB				- Callback function that is called when function completes
	// pCBArg			- Pointer to application-specified data that is passed to the callback
	// return			- eCLE_Success if function successfully started or an error code if function failed to start
	virtual	ECryLobbyError		GetUserCredentials( uint32 user, CryLobbyTaskID* pTaskID, CryLobbyGetUserCredentialsCallback pCB, void* pCBArg ) = 0;

	// UserSignOut
	// Sign out a local user
	// user				- The pad number of the local user.
	// pTaskID		- Pointer to buffer to store the task ID to identify this call in the callback
	// pCB				- Callback function that is called when function completes
	// pCBArg			- Pointer to application-specified data that is passed to the callback
	// return			- eCLE_Success if function successfully started or an error code if function failed to start
	virtual ECryLobbyError		UserSignOut( uint32 user, CryLobbyTaskID* pTaskID, CryLobbyOnlineStateCallback pCb, void* pCbArg ) = 0;

	// CheckProfanity
	// Check a string for profanity.
	// pString		- string to check.
	// pTaskID		- Pointer to buffer to store the task ID to identify this call in the callback
	// pCB				- Callback function that is called when function completes
	// pCBArg			- Pointer to application-specified data that is passed to the callback
	// return			- eCLE_Success if function successfully started or an error code if function failed to start
	virtual ECryLobbyError		CheckProfanity( const char* const pString, CryLobbyTaskID* pTaskID, CryLobbyCheckProfanityCallback pCb, void* pCbArg ) = 0;

	// GetSystemTime
	// Returns the current time from the online time server (must be signed in)
	// pSystemTime - pointer to the CrySystemTime structure to contain the time
	//							 N.B. only year, month, day, hours, minutes and seconds will be populated.
	// returns		 - eCLE_Success if the function was successful, or an error code if not
	virtual ECryLobbyError		GetSystemTime(uint32 user, SCrySystemTime* pSystemTime) = 0;
};

#include "CryLobbyEvent.h"		// Separated out for readability

// Allows users of the lobby to not initialise features that they don't require

enum ECryLobbyServiceFeatures
{
	eCLSO_Base				= BIT(0),
	eCLSO_Matchmaking	= BIT(1),
	eCLSO_Voice				= BIT(2),
	eCLSO_Stats				= BIT(3),
	eCLSO_LobbyUI			= BIT(4),
	eCLSO_Friends			= BIT(5),
	eCLSO_Reward			= BIT(6),
	eCLSO_TCPService	= BIT(7),

	eCLSO_All = eCLSO_Base | eCLSO_Matchmaking | eCLSO_Voice | eCLSO_Stats | eCLSO_LobbyUI | eCLSO_Friends | eCLSO_Reward | eCLSO_TCPService
};

// Intended to make future extension easier. At present I only need the port exposing
struct SCryLobbyParameters
{
	uint16		m_listenPort;							// Listen port the lobby service will use for connections
	uint16		m_connectPort;						// Connect port the lobby service will use for connections
};

struct ICryLobby
{
public:
	virtual ~ICryLobby(){}
	// Initialise
	// Initialise a lobby service
	// service		- The service to be initialised
	// features		- Which features of the service to initialise (e.g.  ECryLobbyServiceFeatures(eCLSO_All & (~eCLSO_Reward)))
	// cfgCb			- Callback function that is called whenever the lobby system needs access to data that is unavailable through the standard API.
	// cb					- Callback function that is called when function completes
	// cbArg			- Pointer to application-specified data that is passed to the callback
	// return			- eCLE_Success if function successfully started or an error code if function failed to start
	virtual ECryLobbyError		Initialise(ECryLobbyService service, ECryLobbyServiceFeatures features, CryLobbyConfigurationCallback cfgCb, CryLobbyCallback cb, void* cbArg) = 0;

	// Terminate
	// Shut down a lobby service
	// service		- The service to be shut down
	// features		- Which features of the service to terminate (e.g.  ECryLobbyServiceFeatures(eCLSO_All & (~eCLSO_Reward)))
	// cb					- Callback function that is called when function completes
	// cbArg			- Pointer to application-specified data that is passed to the callback
	// return			- eCLE_Success if function successfully started or an error code if function failed to start
	virtual ECryLobbyError		Terminate(ECryLobbyService service, ECryLobbyServiceFeatures features, CryLobbyCallback cb, void* cbArg) = 0;

	// ProcessEvents
	// Should be called at least once a frame, causes the lobby->game message queue to be acted on
	// return			- eCLE_Success if function successfully completed, or an error code if function failed for some reason
	virtual ECryLobbyError		ProcessEvents() = 0;

	// SetLobbyService
	// Set the current lobby service
	// service		- The service to use
	// return			- The service that was in use before the call
	virtual ECryLobbyService	SetLobbyService(ECryLobbyService service) = 0;

	// GetLobbyServiceType
	// Get the current lobby service type
	// return			- The ECryLobbyService of the current lobby service
	virtual ECryLobbyService	GetLobbyServiceType() = 0;

	// GetLobbyService
	// Get the current lobby service
	// return			- Pointer to an ICryLobbyService for the current lobby service
	virtual ICryLobbyService*	GetLobbyService() = 0;

	// GetMatchMaking
	// Get the current matchmaking service
	// return			- Pointer to an ICryMatchMaking associated with the current lobby service
	virtual ICryMatchMaking*	GetMatchMaking() = 0;

	// GetVoice
	// Get the current voice service
	// return			- Pointer to an ICryVoice associated with the current lobby service
	virtual ICryVoice*	GetVoice() = 0;

	// GetStats
	// Get the current stats service
	// return			- Pointer to an ICryStats associated with the current lobby service
	virtual ICryStats*	GetStats() = 0;

	// GetLobbyUI
	// Get the current lobby ui service
	// return			- Pointer to an ICryLobbyUI associated with the current lobby service
	virtual ICryLobbyUI*	GetLobbyUI() = 0;

	// GetFriends
	// Get the current friends service
	// return			- Pointer to an ICryFriends associated with the current lobby service
	virtual ICryFriends*	GetFriends() = 0;

	// GetFriendsManagement
	// Get the current friends management service
	// return			- Pointer to an ICryFriendsManagement associated with the current lobby service
	virtual ICryFriendsManagement*	GetFriendsManagement() = 0;

	// GetReward
	// Get the current reward service (achievements/trophies)
	// return			- Pointer to an ICryReward associated with the current lobby service
	virtual ICryReward*		GetReward() = 0;

	// GetDLCStore
	// Get the current DLC store service
	// return			- Pointer to an ICryDLCStore associated with the current lobby service
	virtual ICryDLCStore*		GetDLCStore() = 0;

	// GetLobbyService
	// Get the requested lobby service
	// service		- The service to wanted
	// return			- Pointer to an ICryLobbyService for the lobby service requested
	virtual ICryLobbyService*	GetLobbyService(ECryLobbyService service) = 0;

	// RegisterEventInterest
	// Register interest in particular lobby events (such as invite accepted), the passed in callback will be fired when the event happens.
	// cb					- Callback function that will be called
	// arg				- Pointer to application-specified data
	virtual void RegisterEventInterest(ECryLobbySystemEvent eventOfInterest, CryLobbyEventCallback cb,void *pUserArg) = 0;

	// UnregisterEventInterest
	// Unregister interest in particular lobby events (such as invite accepted), the passed in callback will no longer be fired when the event happens.
	// cb					- Callback function that will no longer be called
	// arg					- Pointer to application-specified data that was passed in when the callback was registered
	virtual void UnregisterEventInterest(ECryLobbySystemEvent eventOfInterest, CryLobbyEventCallback cb,void *pUserArg) = 0;

	// GetLobbyParameters
	// Retrieve various information about the current lobby service
	// return			- SCryLobbyParameters - reference to a structure that contains some essential information about the lobby
	virtual const SCryLobbyParameters& GetLobbyParameters() const = 0;

	// Description:
	//	 Is the given flag true for the lobby service of the given type?
	// Arguments:
	//	 service - lobby service to be queried
	//	 flag - flag to be queried
	// Return:
	//	 True if the flag is true.
	virtual bool GetLobbyServiceFlag( ECryLobbyService service, ECryLobbyServiceFlag flag ) = 0;

	// The following three functions are used to allow clients to start loading assets earlier in the flow when using the lobby service
	//At present we don't support more than one nub session, so these are in the lobby for easier access.
	
	// SetCryEngineLoadHint
	//
	// An optimisation for client load times, allows the engine to load the assets ahead of being informed by the server
	virtual void SetCryEngineLoadHint(const char* levelName,const char* gameRules) = 0;

	// GetCryEngineLevelNameHint
	// Returns the level name previously set by SetCryEngineLoadHint or "" if never set
	virtual const char* GetCryEngineLevelNameHint() = 0;

	// GetCryEngineRulesHint
	// Returns the game rules name previously set by SetCryEngineLoadHint or "" if never set 
	virtual const char* GetCryEngineRulesNameHint() = 0;
};


#endif // __ICRYLOBBY_H__
