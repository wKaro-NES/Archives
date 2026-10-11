/*
========================================================================
 Name        : GsmPosContainerView.h
 Author      : WojciechK
 Copyright   : Your copyright notice
 Description : 
========================================================================
*/
#ifndef GSMPOSCONTAINERVIEW_H
#define GSMPOSCONTAINERVIEW_H

// [[[ begin generated region: do not modify [Generated Includes]
#include <aknview.h>
// ]]] end generated region [Generated Includes]

#include <AknGlobalNote.h>

// [[[ begin [Event Handler Includes]
// ]]] end [Event Handler Includes]

#include "GsmPosEngine.h"
#include "GsmPosMapObserver.h"

// [[[ begin generated region: do not modify [Generated Constants]
// ]]] end generated region [Generated Constants]

// [[[ begin generated region: do not modify [Generated Forward Declarations]
class CGsmPosContainer;
// ]]] end generated region [Generated Forward Declarations]

/**
 * Avkon view class for GsmPosContainerView. It is register with the view server
 * by the AppUi. It owns the container control.
 * @class	CGsmPosContainerView GsmPosContainerView.h
 */						
			
class CGsmPosContainerView : public CAknView,
								public MGsmPosMapObserver
	{
	
	
	// [[[ begin [Public Section]
public:
	// constructors and destructor
	CGsmPosContainerView();
	static CGsmPosContainerView* NewL();
	static CGsmPosContainerView* NewLC();        
	void ConstructL();
	virtual ~CGsmPosContainerView();
						
	// from base class CAknView
	TUid Id() const;
	void HandleCommandL( TInt aCommand );
	
	// [[[ begin generated region: do not modify [Generated Methods]
	CGsmPosContainer* CreateContainerL();
	static TInt RunDataQuery1L( 
			TInt& aData, 
			TBool aUseDefaults = ETrue, 
			const TDesC* aOverridePrompt = NULL );
	static TInt RunConfQuery1L( const TDesC* aOverrideText = NULL );
	// ]]] end generated region [Generated Methods]
	
	// ]]] end [Public Section]
	
	typedef enum TMapType{
		EUnknown,
		EGoogleMapCellIdWithAllPointsNoFill,
		EGoogleMapCellIdWithoutAllPointsFill,
		ELocalMapCellIdWithAllPointsFill,
		ELocalMapCellIdWithAllPointsNoFill,
		EGoogleMapAllCellIdInRange,
		ELocalMapAllCellIdInRange
	};
	
	void GetCellsFromListL(TMapType aMapType, TBool aUseLastValues = EFalse);
	
	// [[[ begin [Protected Section]
protected:
	// from base class CAknView
	void DoActivateL(
		const TVwsViewId& aPrevViewId,
		TUid aCustomMessageId,
		const TDesC8& aCustomMessage );
	void DoDeactivate();
	void HandleStatusPaneSizeChange();
	
	// [[[ begin generated region: do not modify [Overridden Methods]
	// ]]] end generated region [Overridden Methods]
	
	
	// [[[ begin [User Handlers]
	TBool HandleSatelitte_infoMenuItemSelectedL( TInt aCommand );
	TBool HandleDecreaseMenuItemSelectedL( TInt aCommand );
	TBool HandleIncreaseMenuItemSelectedL( TInt aCommand );
	TBool HandleSelect_cell_idMenuItemSelectedL( TInt aCommand );
	TBool HandleSelect_cell_id_rectangleMenuItemSelectedL( TInt aCommand );
	TBool HandleLocal_mapMenuItemSelectedL( TInt aCommand );
	TBool HandleLocal_map_withou_fillMenuItemSelectedL( TInt aCommand );
	TBool HandleShow_GPS_positionMenuItemSelectedL( TInt aCommand );
	TBool HandleHide_GPS_positionMenuItemSelectedL( TInt aCommand );
	TBool HandleGoogle_mapMenuItemSelectedL( TInt aCommand );
	TBool HandleLocal_mapMenuItem1SelectedL( TInt aCommand );
	TBool HandleViewConnectMenuItemSelectedL( TInt aCommand );
	// ]]] end [User Handlers]
	
	// ]]] end [Protected Section]
	
	CGsmPosEngine* Engine();
	
	//MGsmPosMapObserver
	void MapCreated(CFbsBitmap* aBitmap);
	void MapPositionMarked(const TPosition& aGps, const TPosition& aCentroid);
	void MapDownloadStarted();
	
	//MEikMenuObserver
	void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
	
	// [[[ begin [Private Section]
private:
	void SetupStatusPaneL();
	void CleanupStatusPane();
	
	// [[[ begin generated region: do not modify [Generated Instance Variables]
	CGsmPosContainer* iGsmPosContainer;
	// ]]] end generated region [Generated Instance Variables]
	
	// [[[ begin generated region: do not modify [Generated Methods]
	// ]]] end generated region [Generated Methods]
	
	// ]]] end [Private Section]
	
private:
	CAknGlobalNote* iGlobalNote;
	TInt iGlobalNoteId;
	TBuf<128> iGlobalNoteText;
	TBool iGlobalNoteModal;
	
	TBool iMarkCurrentPosition;
	
	TMapType iLastMapType;
	TInt iLastRadius;
	};

#endif // GSMPOSCONTAINERVIEW_H
