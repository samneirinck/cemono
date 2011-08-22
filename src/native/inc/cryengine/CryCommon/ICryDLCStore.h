////////////////////////////////////////////////////////////////////////////
//
//  CryEngine Source File.
//  Copyright (C), Crytek, 2010.
// -------------------------------------------------------------------------
//  File name:   ICryDLCStore.h
//  Version:     v1.00
//  Created:     01/09/2010 by Paul Mikell.
//  Description: ICryDLCStore interface definition
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////


#ifndef __ICRYDLCSTORE_H__

#define __ICRYDLCSTORE_H__


#if USE_CRYLOBBY_GAMESPY


typedef uint32								CryDLCOrderHandle;
const CryDLCOrderHandle				CryDLCOrderInvalidHandle = 0xFFFFFFFF;


enum ECryDLCStoreStatus
{
	eCDLCSS_Unknown,
	eCDLCSS_Querying,
	eCDLCSS_QueryFailed,
	eCDLCSS_Online,
	eCDLCSS_OfflineForMaintanance,
	eCDLCSS_NotYetLaunched,
	eCDLCSS_Discontinued
};


struct SCryDLCCategoryID: public CMultiThreadRefCount
{
	virtual bool operator==( const SCryDLCCategoryID& other ) = 0;
};

typedef _smart_ptr< SCryDLCCategoryID >	CryDLCCategoryID;


struct SCryDLCItemID: public CMultiThreadRefCount
{
	virtual bool operator==( const SCryDLCItemID& other ) = 0;
};

typedef _smart_ptr< SCryDLCItemID >	CryDLCItemID;


struct SCryDLCImageID: public CMultiThreadRefCount
{
	virtual bool operator==( const SCryDLCImageID& other ) = 0;
};

typedef _smart_ptr< SCryDLCImageID >	CryDLCImageID;


struct SCryDLCDownloadableID:	public CMultiThreadRefCount
{
	virtual bool operator==( const SCryDLCDownloadableID& other ) = 0;
};

typedef _smart_ptr< SCryDLCDownloadableID >	CryDLCDownloadableID;


struct SCryDLCLicense: public CMultiThreadRefCount
{
	virtual bool operator==( const SCryDLCLicense& other ) = 0;
};

typedef _smart_ptr< SCryDLCLicense >	CryDLCLicense;


struct SCryDLCPaymentMethodID: public CMultiThreadRefCount
{
	virtual bool operator==( const SCryDLCPaymentMethodID& other ) = 0;
};

class CryDLCPaymentMethodID
{
public:

	CryDLCPaymentMethodID(): m_pPaymentMethodID( NULL )
	{
	}

	CryDLCPaymentMethodID( SCryDLCPaymentMethodID* pPaymentMethodID ) : m_pPaymentMethodID( pPaymentMethodID )
	{
	}

	SCryDLCPaymentMethodID* get() const
	{
		return m_pPaymentMethodID.get();
	}

	bool operator==( const CryDLCPaymentMethodID& other ) const
	{
		SCryDLCPaymentMethodID*	pPaymentMethodID0 = m_pPaymentMethodID.get();
		SCryDLCPaymentMethodID*	pPaymentMethodID1 = other.m_pPaymentMethodID.get();
		bool										valid0 = ( pPaymentMethodID0 != NULL );
		bool										valid1 = ( pPaymentMethodID1 != NULL );

		return ( valid0 == valid1 ) && ( ( !valid0 ) || ( *pPaymentMethodID0 == *pPaymentMethodID1 ) );
	};

	typedef void ( CryDLCPaymentMethodID::*BoolType )() const;

	operator BoolType() const
	{
		BoolType		result;

		if ( m_pPaymentMethodID.get() )
		{
			result = &CryDLCPaymentMethodID::BoolFunc;
		}
		else
		{
			result = NULL;
		}

		return result;
	};

private:

	void BoolFunc() const {};

	_smart_ptr< SCryDLCPaymentMethodID >	m_pPaymentMethodID;
};


struct SCryDLCCategoryInfo
{
	SCryDLCCategoryInfo(): id( NULL ), name( L"" ) {};
	SCryDLCCategoryInfo( CryDLCCategoryID srcId, wstring srcName ): id( srcId ), name( srcName ) {};

	CryDLCCategoryID									id;
	wstring														name;
};


struct SCryDLCItemInfo
{
	CryDLCItemID											id;
	wstring														name;
	wstring														description;
	wstring														currency;
	wstring														culture;
	wstring														price;
	bool															allowQuantity;
	std::vector< CryDLCCategoryID >		categories;
	std::vector< CryDLCImageID >			images;
};


struct SCryDLCOrderItemInfo
{
	wstring														tax;
	wstring														subtotal;
	wstring														total;
	uint32														quantity;
};


struct SCryDLCDownloadableInfo
{
	CryDLCDownloadableID							id;
	uint32														sequence;
	wstring														name;
	wstring														assetType;
	float															version;
};


struct SCryDLCPaymentMethodInfo
{
	CryDLCPaymentMethodID							id;
	wstring														type;
	time_t														expirationDate;
	bool															isDefault;
	int																lastFourDigits;
};


struct SCryDLCOrderInfo
{
	wstring														currency;
	wstring														culture;
	wstring														subtotal;
	wstring														tax;
	wstring														total;
};


enum EDownloadableAccess
{
	eDA_None,			// User can't use this downloadable
	eDA_Limited,	// User has limited access to this downloadable e.g. in multiplayer, can render another player using it
	eDA_Full			// User has full access to this downloadable
};


// Description:
//	 Generic DLC store callback.
// Arguments:
//	 taskID - lobby task ID
//	 error - eCLE_Success if successful, otherwise an error
//	 pArg - callback argument
typedef void		( *CryDLCCallback )( CryLobbyTaskID taskID, ECryLobbyError error, void* pArg );

// Description:
//	 Receive CheckAvailability result.
// Arguments:
//	 taskID - lobby task ID
//	 error - eCLE_Success if successful, otherwise an error
//	 status - store status
//	 pArg - callback argument
typedef void		( *CryDLCCheckAvailabilityCallback )( CryLobbyTaskID taskID, ECryLobbyError error, ECryDLCStoreStatus status, void* pArg );

// Description:
//	 Receive CatalogListCategories result.
// Arguments:
//	 taskID - lobby task ID
//	 error - eCLE_SuccessContinue if successful and more to come, eCLE_Success if successful and no more to come, otherwise an error
//	 pCategoryInfo - category info
//	 pArg - callback argument
typedef void		( *CryDLCListCategoriesCallback )( CryLobbyTaskID taskID, ECryLobbyError error, SCryDLCCategoryInfo* pCategoryInfo, void* pArg );

// Description:
//	 Receive CatalogListItems result.
// Arguments:
//	 taskID - lobby task ID
//	 error - eCLE_SuccessContinue if successful and more to come, eCLE_Success if successful and no more to come, otherwise an error
//	 pItemInfo - item info
//	 pArg - callback argument
typedef void		( *CryDLCListItemsCallback )( CryLobbyTaskID taskID, ECryLobbyError error, SCryDLCItemInfo* pItemInfo, void* pArg );

// Description:
//	 Receive ImageDownload result.
// Arguments:
//	 taskID - lobby task ID
//	 error - eCLE_SuccessContinue if successful and more to come, eCLE_Success if successful and no more to come, otherwise an error
//	 pItemInfo - item info
//	 pArg - callback argument
typedef void		( *CryDLCImageDownloadCallback )( CryLobbyTaskID taskID, ECryLobbyError error, CryDLCImageID imageID, void* pArg );

// Description:
//	 Receive UserListPaymentMethods result.
// Arguments:
//	 taskID - lobby task ID
//	 error - eCLE_SuccessContinue if successful and more to come, eCLE_Success if successful and no more to come, otherwise an error
//	 pPaymentMethodInfo - payment method info
//	 pArg - callback argument
typedef void		( *CryDLCListPaymentMethodsCallback )( CryLobbyTaskID taskID, ECryLobbyError error, SCryDLCPaymentMethodInfo* pPaymentMethodInfo, void* pArg );

// Description:
//	 Receive OrderFinalize result.
// Arguments:
//	 taskID - lobby task ID
//	 error - eCLE_SuccessContinue if successful and more to come, eCLE_Success if successful and no more to come, otherwise an error
//	 itemID - ID of purchased item; if NULL this result is not a purchased item, but one of the downloadables or licenses of which purchased items are composed
//	 downloadable - ID of file to download; if NULL this result is a license or a purchased item, not a downloadable, otherwise pass to UserDownloadItem
//	 license - license data; if NULL this result is a downloadable or a purchased item, not a license
//	 pArg - callback argument
typedef void		( *CryDLCOrderFinalizeCallback )( CryLobbyTaskID taskID, ECryLobbyError error, CryDLCItemID itemID, CryDLCDownloadableID downloadable, CryDLCLicense license, void* pArg );

// Description:
//	 Receive OrderListItems result.
// Arguments:
//	 taskID - lobby task ID
//	 error - eCLE_SuccessContinue if successful and more to come, eCLE_Success if successful and no more to come, otherwise an error
//	 itemID - ID of ordered item
//	 quantity - quantity of ordered item
//	 pArg - callback argument
typedef void		( *CryDLCOrderListItemsCallback )( CryLobbyTaskID taskID, ECryLobbyError error, CryDLCItemID itemID, uint32 quantity, void* pArg );

// Description:
//	 Receive OrderGetInfo result.
// Arguments:
//	 taskID - lobby task ID
//	 error - eCLE_Success if successful, otherwise an error
//	 pOrderInfo - order info
//	 pArg - callback argument
typedef void		( *CryDLCOrderGetInfoCallback )( CryLobbyTaskID taskID, ECryLobbyError error, SCryDLCOrderInfo* pOrderInfo, void* pArg );

// Description:
//	 Receive OrderGetItemInfo result.
// Arguments:
//	 taskID - lobby task ID
//	 error - eCLE_Success if successful, otherwise an error
//	 pItemInfo - item info
//	 pOrderItemInfo - order item info
//	 pArg - callback argument
typedef void		( *CryDLCOrderGetItemInfoCallback )( CryLobbyTaskID taskID, ECryLobbyError error, SCryDLCItemInfo* pItemInfo, SCryDLCOrderItemInfo* pOrderItemInfo, void* pArg );

// Description:
//	 Receive UserGetDownloadableAccess result.
// Arguments:
//	 taskID - lobby task ID
//	 error - eCLE_SuccessContinue if successful and more to come, eCLE_Success if successful and no more to come, otherwise an error
//	 access - access
//	 pArg - callback argument
typedef void		( *CryDLCUserGetDownloadableAccessCallback )( CryLobbyTaskID taskID, ECryLobbyError error, EDownloadableAccess access, void* pArg );

// Description:
//	 Receive UserDownloadItem progress and result.
//			eCLE_Success, NULL, false, 0, 0	- UserRedownloadAllMissingItems found nothing missing
//			eCLE_Success, non-NULL, false, 0, 0 - UserDownloadItem found item to be already downloaded
//			eCLE_Success, non-NULL, true, 0, 0 - UserDownloadItem successfully downloaded an item, or UserRedownloadAllMissingItems successfully downloaded the final item
//			eCLE_SuccessContinue, non-NULL, true, 0, 0 - UserRedownloadAllMissingItems successfully downloaded an item
//			eCLE_SuccessContinue, non-NULL, false, non-zero, non-zero - UserDownloadItem or UserRedownloadAllMissingItems partially downloaded an item
//			error, non-NULL, false, 0, 0 - an error occurred
// Arguments:
//	 taskID - lobby task ID
//	 error - eCLE_Success on completion of final file, eCLE_SuccessContinue on completion of any other file or partial download of any file, otherwise an error
//	 downloadableID - downloadable ID
//	 complete - true on completion of download of a file, otherwise false
//	 bytesReceived - number of bytes received so far on partial download of a file, otherwise 0
//	 bytesTotal - size of file on partial download of a file, otherwise 0,
//	 pPath - path of downloaded file on completion of a download, otherwise NULL
//	 pArg - callback argument
typedef void		( *CryDLCUserDownloadItemCallback )( CryLobbyTaskID taskID, ECryLobbyError error, CryDLCDownloadableID downloadableID, bool complete, uint32 bytesReceived, uint32 bytesTotal, const char* pPath, void* pArg );

// Description:
//	 Receive DownloadableIsDownloaded result.
// Arguments:
//	 taskID - lobby task ID
//	 error - eCLE_SuccessContinue if successful and more to come, eCLE_Success if successful and no more to come, otherwise an error
//	 downloaded - true if downloaded
//	 pArg - callback argument
typedef void		( *CryDLCDownloadableIsDownloadedCallback )( CryLobbyTaskID taskID, ECryLobbyError error, bool downloaded, void* pArg );

// Description:
//	 Verify local copy of a downloadable.
// Arguments:
//	 pInfo - IN downloadable info
//	 pPath - OUT directory where file should be downloaded
//	 pValid - OUT true if local copy of path valid, otherwise false
typedef void		( *CryDLCDownloadableVerifyCallback )( const SCryDLCDownloadableInfo& pInfo, CryFixedStringT< _MAX_PATH >* pPath, bool* pValid ); 


struct ICryDLCStore
{
	// Description:
	//	 Set the DownloadableVerifyCallback.
	// Arguments:
	//	 cb - the DownloadableVerifyCallback
	virtual void SetDownloadableVerifyCallback( CryDLCDownloadableVerifyCallback cb ) = 0;

	// Description:
	//	 Check availability of online store.
	// Arguments:
	//	 userID - user whose privileges to use the store will be checked
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	CheckAvailability( CryUserID userID, CryLobbyTaskID* pTaskID, CryDLCCheckAvailabilityCallback cb, void* pCbArg ) = 0;

	// Description:
	//	 List all items in catalog.
	// Arguments:
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	CatalogListItems( CryLobbyTaskID* pTaskID, CryDLCListItemsCallback cb, void* pCbArg ) = 0;

	// Description:
	//	 List all categories in catalog.
	// Arguments:
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	CatalogListCategories( CryLobbyTaskID* pTaskID, CryDLCListCategoriesCallback cb, void* pCbArg ) = 0;

	// Description:
	//	 Get category info.
	// Arguments:
	//	 categoryID - category ID
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	CategoryGetInfo( CryDLCCategoryID categoryID, CryLobbyTaskID* pTaskID, CryDLCListCategoriesCallback cb, void* pCbArg ) = 0;

	// Description:
	//	 List all items in category.
	// Arguments:
	//	 categoryID - category ID
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	CategoryListItems( CryDLCCategoryID categoryID, CryLobbyTaskID* pTaskID, CryDLCListItemsCallback cb, void* pCbArg ) = 0;

	// Description:
	//	 Get additional information for an item.
	// Arguments:
	//	 itemID - item ID
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	ItemGetInfo( CryDLCItemID itemID, CryLobbyTaskID* pTaskID, CryDLCListItemsCallback cb, void* pCbArg ) = 0;

	// Description:
	//	 Check whether a downloadable is downloaded.
	// Arguments:
	//	 downloadableID - downloadable ID
	//	 pDstPath - destination path
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	DownloadableIsDownloaded( CryDLCDownloadableID downloadableID, const char* pDstPath, CryLobbyTaskID* pTaskID, CryDLCDownloadableIsDownloadedCallback cb, void* pCbArg ) = 0;

	// Description:
	//	 Get a user's purchase history.
	// Arguments:
	//	 userID - user ID
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	UserGetPurchaseHistory( CryUserID userID, CryLobbyTaskID* pTaskID, CryDLCOrderFinalizeCallback cb, void* pCbArg ) = 0;

	// Description:
	//	 Start an order.
	// Arguments:
	//	 userID - user ID
	//	 pOrderHandle - order handle
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	UserStartOrder( CryUserID userID, CryDLCOrderHandle* pOrderHandle, CryLobbyTaskID* pTaskID, CryDLCCallback cb, void* pCbArg ) = 0;

	// Description:
	//	 Check what usage a user can make of a downloaded item.
	// Arguments:
	//	 userID - user whose entitlement will be checked
	//	 downloadableID - downloaded item
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	UserGetDownloadableAccess( CryUserID userID, CryDLCDownloadableID downloadableID, CryLobbyTaskID* pTaskID, CryDLCUserGetDownloadableAccessCallback cb, void* pCbArg ) = 0;

	// Description:
	//	 Download an item.
	// Arguments:
	//	 userID - user whose entitlement will be checked
	//	 downloadableID - item
	//	 pDstPath - destination path
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	UserDownloadItem( CryUserID userID, CryDLCDownloadableID downloadableID, CryLobbyTaskID* pTaskID, CryDLCUserDownloadItemCallback cb, void* pCbArg ) = 0;

	// Description:
	//	 Are any downloads in progress?
	// Return:
	//	 true if any downloads are in progress
	virtual bool IsDownloading() = 0;

	// Description:
	//	 Redownload all missing items that a user is entitled to.
	// Arguments:
	//	 userID - user whose entitlements will be checked
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	UserRedownloadAllMissingItems( CryUserID userID, CryLobbyTaskID* pTaskID, CryDLCUserDownloadItemCallback cb, void* pCbArg ) = 0;

	// Description:
	//	 List user's payment methods.
	// Arguments:
	//	 userID - user
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	UserListPaymentMethods( CryUserID userID, CryLobbyTaskID* pTaskID, CryDLCListPaymentMethodsCallback cb, void* pCbArg ) = 0;

	// Description:
	//	 Finalize an order.
	// Arguments:
	//	 orderHandle - order handle
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	OrderFinalize( CryDLCOrderHandle orderHandle, bool freeOnCompletion, CryLobbyTaskID* pTaskID, CryDLCOrderFinalizeCallback cb, void* pCbArg ) = 0;

	// Description:
	//	 Add an item to an order.
	// Arguments:
	//	 orderHandle - order handle
	//	 itemID - item ID
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	OrderAddItem( CryDLCOrderHandle orderHandle, CryDLCItemID itemID, CryLobbyTaskID* pTaskID, CryDLCCallback cb, void* pCbArg  ) = 0;

	// Description:
	//	 Remove an item from an order.
	// Arguments:
	//	 orderHandle - order handle
	//	 itemID - item ID
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	OrderRemoveItem( CryDLCOrderHandle orderHandle, CryDLCItemID itemID, CryLobbyTaskID* pTaskID, CryDLCCallback cb, void* pCbArg  ) = 0;

	// Description:
	//	 Add a quantity of an item to an order.
	// Arguments:
	//	 orderHandle - order handle
	//	 itemID - item ID
	//	 quantity - quantity
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	OrderAddItemQuantity( CryDLCOrderHandle orderHandle, CryDLCItemID itemID, uint32 quantity, CryLobbyTaskID* pTaskID, CryDLCCallback cb, void* pCbArg  ) = 0;

	// Description:
	//	 Remove a quantity of an item from an order.
	// Arguments:
	//	 orderHandle - order handle
	//	 itemID - item ID
	//	 quantity - quantity
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	OrderRemoveItemQuantity( CryDLCOrderHandle orderHandle, CryDLCItemID itemID, uint32 quantity, CryLobbyTaskID* pTaskID, CryDLCCallback cb, void* pCbArg  ) = 0;

	// Description:
	//	 List items in an order.
	// Arguments:
	//	 orderHandle - order handle
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	OrderListItems( CryDLCOrderHandle orderHandle, CryLobbyTaskID* pTaskID, CryDLCOrderListItemsCallback cb, void* pCbArg  ) = 0;

	// Description:
	//	 Get order information.
	// Arguments:
	//	 orderHandle - order handle
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	OrderGetInfo( CryDLCOrderHandle orderHandle, CryLobbyTaskID* pTaskID, CryDLCOrderGetInfoCallback cb, void* pCbArg ) = 0;

	// Description:
	//	 Get order item information.
	// Arguments:
	//	 orderHandle - order handle
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	OrderGetItemInfo( CryDLCOrderHandle orderHandle, CryDLCItemID itemID, CryLobbyTaskID* pTaskID, CryDLCOrderGetItemInfoCallback cb, void* pCbArg ) = 0;

	// Description:
	//	 Set the payment method for an order.
	// Arguments:
	//	 orderHandle - order handle
	//	 paymentMethodID - payment method ID
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	OrderSetPaymentMethod( CryDLCOrderHandle orderHandle, CryDLCPaymentMethodID paymentMethodID, CryLobbyTaskID* pTaskID, CryDLCCallback cb, void* pCbArg  ) = 0;

	// Description:
	//	 Cancel an order.
	// Arguments:
	//	 orderHandle - order handle
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	OrderCancel( CryDLCOrderHandle orderHandle, CryLobbyTaskID* pTaskID, CryDLCCallback cb, void* pCbArg  ) = 0;

	// Description:
	//	 Free an order.
	// Arguments:
	//	 orderHandle - order handle
	//	 pTaskID - lobby task ID
	//	 cb - callback
	//	 pCbArg - callback argument
	// Return:
	//	 eCLE_Success if task was started successfully, otherwise an error.
	virtual ECryLobbyError	OrderFree( CryDLCOrderHandle orderHandle, CryLobbyTaskID* pTaskID, CryDLCCallback cb, void* pCbArg  ) = 0;
};


#endif


#endif
