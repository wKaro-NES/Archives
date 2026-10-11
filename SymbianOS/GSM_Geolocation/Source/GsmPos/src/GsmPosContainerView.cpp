/*
========================================================================
 Name        : GsmPosContainerView.cpp
 Author      : WojciechK
 Copyright   : Your copyright notice
 Description : 
========================================================================
*/
// [[[ begin generated region: do not modify [Generated System Includes]
#include <aknviewappui.h>
#include <eikmenub.h>
#include <avkon.hrh>
#include <barsread.h>
#include <eikimage.h>
#include <eikenv.h>
#include <akncontext.h>
#include <akntitle.h>
#include <stringloader.h>
#include <eikbtgpc.h>
#include <aknnumedwin.h>
#include <aknquerydialog.h>
#include <GsmPos.rsg>
// ]]] end generated region [Generated System Includes]

#include <aknnotewrappers.h>

// [[[ begin generated region: do not modify [Generated User Includes]

#include "GsmPos.hrh"
#include "GsmPosContainerView.h"
#include "GsmPosContainer.hrh"
#include "GsmPosContainer.h"
// ]]] end generated region [Generated User Includes]

// [[[ begin generated region: do not modify [Generated Constants]
// ]]] end generated region [Generated Constants]

//TODO - zlikwiduj zaleznosc od AppUi (?!)
#include "GsmPosAppUi.h"
#include "GsmPosGpsMonitor.h"
#include "GsmPosListBoxMarkableDialog.h"
#include "GsmPosGoogleMap.h"
#include "GsmPosMap.h"

/**
 * First phase of Symbian two-phase construction. Should not contain any
 * code that could leave.
 */
CGsmPosContainerView::CGsmPosContainerView()
	{
	// [[[ begin generated region: do not modify [Generated Contents]
	iGsmPosContainer = NULL;
	// ]]] end generated region [Generated Contents]
	
	}

/** 
 * The view's destructor removes the container from the control
 * stack and destroys it.
 */
CGsmPosContainerView::~CGsmPosContainerView()
	{
	// [[[ begin generated region: do not modify [Generated Contents]
	delete iGsmPosContainer;
	iGsmPosContainer = NULL;
	// ]]] end generated region [Generated Contents]
	
	delete iGlobalNote;
	}

/**
 * Symbian two-phase constructor.
 * This creates an instance then calls the second-phase constructor
 * without leaving the instance on the cleanup stack.
 * @return new instance of CGsmPosContainerView
 */
CGsmPosContainerView* CGsmPosContainerView::NewL()
	{
	CGsmPosContainerView* self = CGsmPosContainerView::NewLC();
	CleanupStack::Pop( self );
	return self;
	}

/**
 * Symbian two-phase constructor.
 * This creates an instance, pushes it on the cleanup stack,
 * then calls the second-phase constructor.
 * @return new instance of CGsmPosContainerView
 */
CGsmPosContainerView* CGsmPosContainerView::NewLC()
	{
	CGsmPosContainerView* self = new ( ELeave ) CGsmPosContainerView();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}


/**
 * Second-phase constructor for view.  
 * Initialize contents from resource.
 */ 
void CGsmPosContainerView::ConstructL()
	{
	// [[[ begin generated region: do not modify [Generated Code]
	BaseConstructL( R_GSM_POS_CONTAINER_GSM_POS_CONTAINER_VIEW );
				
	// ]]] end generated region [Generated Code]
	
	// add your own initialization code here
	
	iGlobalNote = CAknGlobalNote::NewL();
	
	// domyslnie pokaz pozycje GPS na ekranie
	iMarkCurrentPosition = ETrue;
	
	iLastMapType = EUnknown;
	}

/**
 * @return The UID for this view
 */
TUid CGsmPosContainerView::Id() const
	{
	return TUid::Uid( EGsmPosContainerViewId );
	}

/**
 * Handle a command for this view (override)
 * @param aCommand command id to be handled
 */
//TODO - rozsynchronizowany UI designer! (usun UI designera)
void CGsmPosContainerView::HandleCommandL( TInt aCommand )
	{
	// [[[ begin generated region: do not modify [Generated Code]
	TBool commandHandled = EFalse;
	switch ( aCommand )
		{	// code to dispatch to the AknView's menu and CBA commands is generated here
		case EGsmPosContainerViewSatelitte_infoMenuItemCommand:
			commandHandled = HandleSatelitte_infoMenuItemSelectedL( aCommand );
			break;
		case EGsmPosContainerViewSelect_cell_idMenuItemCommand:
			commandHandled = HandleSelect_cell_idMenuItemSelectedL( aCommand );
			break;
		case EGsmPosContainerViewSelect_cell_id_rectangleMenuItemCommand:
			commandHandled = HandleSelect_cell_id_rectangleMenuItemSelectedL( aCommand );
			break;
		case EGsmPosContainerViewLocal_mapMenuItemCommand:
			commandHandled = HandleLocal_mapMenuItemSelectedL( aCommand );
			break;
		case EGsmPosContainerViewLocal_map_withou_fillMenuItemCommand:
			commandHandled = HandleLocal_map_withou_fillMenuItemSelectedL( aCommand );
			break;
		case EGsmPosContainerViewIncreaseMenuItemCommand:
			commandHandled = HandleIncreaseMenuItemSelectedL( aCommand );
			break;
		case EGsmPosContainerViewDecreaseMenuItemCommand:
			commandHandled = HandleDecreaseMenuItemSelectedL( aCommand );
			break;
		case EGsmPosContainerViewShow_GPS_positionMenuItemCommand:
			commandHandled = HandleShow_GPS_positionMenuItemSelectedL( aCommand );
			break;
		case EGsmPosContainerViewHide_GPS_positionMenuItemCommand:
			commandHandled = HandleHide_GPS_positionMenuItemSelectedL( aCommand );
			break;
		case EGsmPosContainerViewGoogle_mapMenuItemCommand:
			commandHandled = HandleGoogle_mapMenuItemSelectedL( aCommand );
			break;
		case EGsmPosContainerViewLocal_mapMenuItem1Command:
			commandHandled = HandleLocal_mapMenuItem1SelectedL( aCommand );
			break;
		case EGsmPosContainerViewShow_connected_devicesMenuItemCommand:
			commandHandled = HandleViewConnectMenuItemSelectedL( aCommand );
			break;
		default:
			break;
		}
	
		
	if ( !commandHandled ) 
		{
	
		if ( aCommand == EAknSoftkeyExit )
			{
			AppUi()->HandleCommandL( EEikCmdExit );
			}
	
		}
	// ]]] end generated region [Generated Code]
	
	}

void CGsmPosContainerView::DynInitMenuPaneL( TInt aResourceId,
                                   CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_GSM_POS_CONTAINER_MENU_PANE1_MENU_PANE )
        {
        if (iMarkCurrentPosition)
        	{
            aMenuPane->SetItemDimmed( EGsmPosContainerViewShow_GPS_positionMenuItemCommand, ETrue );
            aMenuPane->SetItemDimmed( EGsmPosContainerViewHide_GPS_positionMenuItemCommand, EFalse );
        	}
        else
        	{
            aMenuPane->SetItemDimmed( EGsmPosContainerViewShow_GPS_positionMenuItemCommand, EFalse );
            aMenuPane->SetItemDimmed( EGsmPosContainerViewHide_GPS_positionMenuItemCommand, ETrue );
        	}
        }
    }

/**
 *	Handles user actions during activation of the view, 
 *	such as initializing the content.
 */
void CGsmPosContainerView::DoActivateL( 
		const TVwsViewId& /*aPrevViewId*/,
		TUid /*aCustomMessageId*/,
		const TDesC8& /*aCustomMessage*/ )
	{
	// [[[ begin generated region: do not modify [Generated Contents]
	SetupStatusPaneL();
	
				
				
	
	if ( iGsmPosContainer == NULL )
		{
		iGsmPosContainer = CreateContainerL();
		iGsmPosContainer->SetMopParent( this );
		AppUi()->AddToStackL( *this, iGsmPosContainer );
		} 
	// ]]] end generated region [Generated Contents]
	
	}

/**
 */
void CGsmPosContainerView::DoDeactivate()
	{
	// [[[ begin generated region: do not modify [Generated Contents]
	CleanupStatusPane();
	
	if ( iGsmPosContainer != NULL )
		{
		AppUi()->RemoveFromViewStack( *this, iGsmPosContainer );
		delete iGsmPosContainer;
		iGsmPosContainer = NULL;
		}
	// ]]] end generated region [Generated Contents]
	
	}

/** 
 * Handle status pane size change for this view (override)
 */
void CGsmPosContainerView::HandleStatusPaneSizeChange()
	{
	CAknView::HandleStatusPaneSizeChange();
	
	// this may fail, but we're not able to propagate exceptions here
	TVwsViewId view;
	AppUi()->GetActiveViewId( view );
	if ( view.iViewUid == Id() )
		{
		TInt result;
		TRAP( result, SetupStatusPaneL() );
		}
	
	// [[[ begin generated region: do not modify [Generated Code]
	// ]]] end generated region [Generated Code]
	
	}

// [[[ begin generated function: do not modify
void CGsmPosContainerView::SetupStatusPaneL()
	{
	// reset the context pane
	TUid contextPaneUid = TUid::Uid( EEikStatusPaneUidContext );
	CEikStatusPaneBase::TPaneCapabilities subPaneContext = 
		StatusPane()->PaneCapabilities( contextPaneUid );
	if ( subPaneContext.IsPresent() && subPaneContext.IsAppOwned() )
		{
		CAknContextPane* context = static_cast< CAknContextPane* > ( 
			StatusPane()->ControlL( contextPaneUid ) );
		context->SetPictureToDefaultL();
		}
	
	// setup the title pane
	TUid titlePaneUid = TUid::Uid( EEikStatusPaneUidTitle );
	CEikStatusPaneBase::TPaneCapabilities subPaneTitle = 
		StatusPane()->PaneCapabilities( titlePaneUid );
	if ( subPaneTitle.IsPresent() && subPaneTitle.IsAppOwned() )
		{
		CAknTitlePane* title = static_cast< CAknTitlePane* >( 
			StatusPane()->ControlL( titlePaneUid ) );
		TResourceReader reader;
		iEikonEnv->CreateResourceReaderLC( reader, R_GSM_POS_CONTAINER_TITLE_RESOURCE );
		title->SetFromResourceL( reader );
		CleanupStack::PopAndDestroy(); // reader internal state
		}
				
	}

// ]]] end generated function

// [[[ begin generated function: do not modify
void CGsmPosContainerView::CleanupStatusPane()
	{
	}

// ]]] end generated function

/**
 *	Creates the top-level container for the view.  You may modify this method's
 *	contents and the CGsmPosContainer::NewL() signature as needed to initialize the
 *	container, but the signature for this method is fixed.
 *	@return new initialized instance of CGsmPosContainer
 */
CGsmPosContainer* CGsmPosContainerView::CreateContainerL()
	{
	return CGsmPosContainer::NewL( ClientRect(), NULL, this );
	}

CGsmPosEngine* CGsmPosContainerView::Engine()
	{
	return static_cast<CGsmPosAppUi*>(AppUi())->Engine();
	}

/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CGsmPosContainerView::HandleSatelitte_infoMenuItemSelectedL( TInt aCommand )
	{
	//TODO - Handle background event
	
	CGsmPosEngine* engine = Engine();
	if (engine)
		{
		CGsmPosGpsMonitor* gps = engine->GpsMonitor();
		if (gps)
			{
			gps->ShowSatelliteInfoL();
			}
		}
	
	return ETrue;	
	}

void CGsmPosContainerView::MapCreated(CFbsBitmap* aBitmap)
	{
	iGlobalNote->CancelNoteL(iGlobalNoteId);
	
	if (aBitmap)
		{
		CEikImage* image = static_cast<CEikImage*>(iGsmPosContainer->ComponentControl(CGsmPosContainer::EImage1));
		image->SetPictureOwnedExternally(EFalse);
		image->SetPicture(aBitmap);
		iGsmPosContainer->DrawDeferred();
	
		if (iGlobalNoteModal)
			{
			iGlobalNoteModal = EFalse;
			//zmien notke wait na zwykla z Close pod RSK
			iGlobalNote->SetSoftkeys(R_AVKON_SOFTKEYS_CLOSE);
			iGlobalNoteId = iGlobalNote->ShowNoteL(EAknGlobalInformationNote, iGlobalNoteText);
			}
		}
	}

void CGsmPosContainerView::MapPositionMarked(const TPosition& aGps, const TPosition& aCentroid)
	{
	//TODO resource?
	TReal32 dist = 0.0f;
	(void) aGps.Distance(aCentroid, dist);
	iGlobalNoteText.Format(_L("Dokladnosc pozycjonowania:\n%.1f m - GPS (G)\n%.1f m - aplikacja (A)\n%.1f m - odleglosc G/A"),
							aGps.HorizontalAccuracy(), aCentroid.HorizontalAccuracy(), dist);
	
	//pokaz notke wait najpierw bez Close potem z
	iGlobalNoteModal = ETrue;
	iGlobalNote->CancelNoteL(iGlobalNoteId);
	iGlobalNote->SetSoftkeys(R_AVKON_SOFTKEYS_EMPTY);
	iGlobalNoteId = iGlobalNote->ShowNoteL(EAknGlobalWaitNote, iGlobalNoteText);
	}

void CGsmPosContainerView::MapDownloadStarted()
	{
	//TODO resource?
	iGlobalNoteText = (_L("Generowanie mapy"));
	
	//pokaz notke wait bez Close
	iGlobalNoteModal = EFalse;
	iGlobalNote->CancelNoteL(iGlobalNoteId);
	iGlobalNote->SetSoftkeys(R_AVKON_SOFTKEYS_EMPTY);
	iGlobalNoteId = iGlobalNote->ShowNoteL(EAknGlobalWaitNote, iGlobalNoteText);
	}

void CGsmPosContainerView::GetCellsFromListL(TMapType aMapType, TBool aUseLastValues)
	{
	if (aUseLastValues)
		{
		//wygeneruj mape bez dialogu wg ostatnich zachowanych kryterii  
		if (iLastMapType == EUnknown)
			return;
		aMapType = iLastMapType;
		}

	iLastMapType = aMapType;
	
	CGsmPosEngine* engine = Engine();
	if (engine)
		{	
		CGsmPosDatabase* db = engine->Database();
		
		TLocality position;
		if (engine->Position(position) || RunConfQuery1L())
			{
			TBool success = EFalse;
			TInt radius = 0; 			//wszystkie

			if (aUseLastValues)
				{
				radius = iLastRadius;
				}
			else
				{
				//zapytaj o maksymalna odleglosc od aktualnej pozycji GPS
				RunDataQuery1L(radius);
				iLastRadius = radius;
				}

			vector<CTelephony::TNetworkInfoV1> infos;
			TInt err = db->GetAllCellNames(infos, position, radius);
			if (err || infos.size() == 0)
				{
				//TODO - teksty z RSC, UI poza ta klasa
				TBuf<64> buf;
				buf.Format(_L("Blad %d lub brak danych w DB"), err);
				CAknErrorNote* note = new(ELeave) CAknErrorNote;
				note->ExecuteLD(buf);
				
				return;
				}
			
			//zbuduj liste z komorkami
		    CDesCArrayFlat* list = new (ELeave) CDesCArrayFlat(infos.size());
		    CleanupStack::PushL(list);
		    
			//pobierz aktualne CellId 
			vector<CTelephony::TNetworkInfoV1> current_info;
			engine->NetworkInfo(ETrue, current_info);

		    CArrayFixFlat<TInt>* marks = new (ELeave) CArrayFixFlat<TInt>(infos.size());
		    CleanupStack::PushL(marks);
		    
			for (TInt i = 0; i < infos.size(); i++)
				{
				TBuf<256> name;
				name.Format(_L("\t%S %u %u\t\t"), &infos[i].iShortName, infos[i].iLocationAreaCode, infos[i].iCellId);
				list->AppendL(name);
				
				//zaznacz aktualne CellId 
				for (TInt j = 0; j < current_info.size(); j++)
					{
					if (aMapType == EGoogleMapAllCellIdInRange || aMapType == ELocalMapAllCellIdInRange)
						{
						//zaznaczaj wszystkie komorki operatora(ow) aktualnego i podlaczonego przez Bluetootha
						if (current_info[j].iCountryCode == infos[i].iCountryCode &&
								current_info[j].iNetworkId == infos[i].iNetworkId)
							{
							marks->AppendL(i);
							break;
							}
						}
					else
						{
						//zaznaczaj tylko pasujace CellId
						if (current_info[j].iLocationAreaCode == infos[i].iLocationAreaCode &&
								current_info[j].iCellId == infos[i].iCellId)
							{
							marks->AppendL(i);
							break;
							}
						}
					}
				}
			
			//wybierz z listy
		    if ( aUseLastValues || (CGsmPosListBoxMarkableDialog::RunDialogL(list, marks) == EAknSoftkeyOk) )
		    	{
				if (marks->Count() > 0)
					{
					TSize size = iGsmPosContainer->ComponentControl(CGsmPosContainer::EImage1)->Size();
				
					switch (aMapType)
						{
						case EGoogleMapCellIdWithAllPointsNoFill:
							{
							//wybierz tylko 1sza zaznaczona (aktualna lub zmieniona przez usera)
							TInt index = marks->At(0);
							success = engine->GoogleMap()->GetMapForCellIdL(iMarkCurrentPosition, infos[index], size, EColor16M, this);
							break;
							}
						case EGoogleMapCellIdWithoutAllPointsFill:
						case EGoogleMapAllCellIdInRange:
							{
							//wybierz wszystkie zaznaczone
							vector<CTelephony::TNetworkInfoV1> selected;
							for (TInt i = 0; i < marks->Count(); i++)
								{
								selected.push_back(infos[marks->At(i)]);
								}
							TBool coverage = ( aMapType == EGoogleMapAllCellIdInRange);
							success = engine->GoogleMap()->GetMapForCellIdAsPolygonL(iMarkCurrentPosition, coverage, selected, size, EColor16M,this);
							break;
							}
						case ELocalMapCellIdWithAllPointsFill:
						case ELocalMapCellIdWithAllPointsNoFill:
						case ELocalMapAllCellIdInRange:
							{
							//wybierz wszystkie zaznaczone
							vector<CTelephony::TNetworkInfoV1> selected;
							for (TInt i = 0; i < marks->Count(); i++)
								{
								selected.push_back(infos[marks->At(i)]);
								}
							TBool fill =( aMapType == ELocalMapCellIdWithAllPointsFill);
							TBool coverage = ( aMapType == ELocalMapAllCellIdInRange);
							success = engine->Map()->GetMapForCellIdAsPolygonL(iMarkCurrentPosition, fill, coverage, selected, size, EColor16M,this);
							break;
							}

						default:
							break;
						}
					}		    	
		    	}
		    
		    CleanupStack::PopAndDestroy(marks);
		    CleanupStack::PopAndDestroy(list);
		    
			if (!success)
				{
				//TODO - teksty z RSC, UI poza ta klasa
				CAknWarningNote* note = new(ELeave) CAknWarningNote;
				note->ExecuteLD(_L("ignoruj"));		
				}
			}
		else
			{
			//TODO - teksty z RSC, UI poza ta klasa
			CAknWarningNote* note = new(ELeave) CAknWarningNote;
			note->ExecuteLD(_L("nieznana pozycja GPS"));		
			}
		}
	}

// [[[ begin generated function: do not modify
/**
 * Show the popup dialog for dataQuery1
 * @param aData in-out TInt data
 * @param aUseDefaults TBool use designer default editor data if ETrue
 * @param aOverridePrompt optional override prompt text
 * @return EAknSoftkeyOk (left soft key id) or 0
 */
TInt CGsmPosContainerView::RunDataQuery1L( 
		TInt& aData, 
		TBool aUseDefaults, 
		const TDesC* aOverridePrompt )
	{
	if ( aUseDefaults )
		{
		aData = 0;
		}
				
	CAknNumberQueryDialog* queryDialog = CAknNumberQueryDialog::NewL( aData );	
	
	if ( aOverridePrompt != NULL )
		{
		CleanupStack::PushL( queryDialog );
		queryDialog->SetPromptL( *aOverridePrompt );
		CleanupStack::Pop(); // queryDialog
		}
	
	return queryDialog->ExecuteLD( R_GSM_POS_CONTAINER_DATA_QUERY1 );
	}
// ]]] end generated function

/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CGsmPosContainerView::HandleDecreaseMenuItemSelectedL( TInt aCommand )
	{
	// TODO: implement selected event handler
	
	CGsmPosEngine* engine = Engine();
	if (engine)
		{
		if (engine->GoogleMap()->ZoomLevelDecrease())
			{
			if (!engine->GoogleMap()->GetMapAgainL())
				{
				//TODO - teksty z RSC, UI poza ta klasa
				CAknWarningNote* note = new(ELeave) CAknWarningNote;
				note->ExecuteLD(_L("ignoruj"));		
				}
			}
		}
	return ETrue;
	}
				
/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CGsmPosContainerView::HandleIncreaseMenuItemSelectedL( TInt aCommand )
	{
	// TODO: implement selected event handler
	CGsmPosEngine* engine = Engine();
	if (engine)
		{
			if (engine->GoogleMap()->ZoomLevelIncrease())
				{
				if (!engine->GoogleMap()->GetMapAgainL())
					{
					//TODO - teksty z RSC, UI poza ta klasa
					CAknWarningNote* note = new(ELeave) CAknWarningNote;
					note->ExecuteLD(_L("ignoruj"));		
					}
				}
		}
	return ETrue;
	}
			
/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CGsmPosContainerView::HandleSelect_cell_idMenuItemSelectedL( TInt aCommand )
	{
	GetCellsFromListL(EGoogleMapCellIdWithAllPointsNoFill);
	return ETrue;
	}
				
/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
//TODO rename
TBool CGsmPosContainerView::HandleSelect_cell_id_rectangleMenuItemSelectedL( TInt aCommand )
	{
	GetCellsFromListL(EGoogleMapCellIdWithoutAllPointsFill);
	return ETrue;
	}
				
// [[[ begin generated function: do not modify
/**
 * Show the popup dialog for confQuery1
 * @param aOverrideText optional override text
 * @return EAknSoftkeyYes (left soft key id) or 0
 */
TInt CGsmPosContainerView::RunConfQuery1L( const TDesC* aOverrideText )
	{
				
	CAknQueryDialog* queryDialog = CAknQueryDialog::NewL();	
	
	if ( aOverrideText != NULL )
		{
		queryDialog->SetPromptL( *aOverrideText );
		}
	return queryDialog->ExecuteLD( R_GSM_POS_CONTAINER_CONF_QUERY1 );
	}
// ]]] end generated function

/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CGsmPosContainerView::HandleLocal_mapMenuItemSelectedL( TInt aCommand )
	{
	GetCellsFromListL(ELocalMapCellIdWithAllPointsFill);
	return ETrue;
	}
				
/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CGsmPosContainerView::HandleLocal_map_withou_fillMenuItemSelectedL( TInt aCommand )
	{
	GetCellsFromListL(ELocalMapCellIdWithAllPointsNoFill);
	return ETrue;
	}
				
/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CGsmPosContainerView::HandleShow_GPS_positionMenuItemSelectedL( TInt aCommand )
	{
	iMarkCurrentPosition = ETrue;
	return ETrue;
	}
				
/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CGsmPosContainerView::HandleHide_GPS_positionMenuItemSelectedL( TInt aCommand )
	{
	iMarkCurrentPosition = EFalse;
	return ETrue;
	}
				
/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CGsmPosContainerView::HandleGoogle_mapMenuItemSelectedL( TInt aCommand )
	{
	GetCellsFromListL(EGoogleMapAllCellIdInRange);
	return ETrue;
	}
				
/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CGsmPosContainerView::HandleLocal_mapMenuItem1SelectedL( TInt aCommand )
	{
	GetCellsFromListL(ELocalMapAllCellIdInRange);
	return ETrue;
	}
				
//TODO dodana bez UI designera
TBool CGsmPosContainerView::HandleViewConnectMenuItemSelectedL( TInt aCommand )
	{

	CGsmPosEngine* engine = Engine();
	if (engine)
		{
		if (engine->BluetoothRemoteName().Length())
			iGlobalNoteText.Format(_L("GsmPosRemote bluetooth MAC address: %S"), &engine->BluetoothRemoteName());
		else
			iGlobalNoteText.Copy(_L("Brak !"));
		
		//pokaz notke z Close
		iGlobalNoteModal = EFalse;
		iGlobalNote->CancelNoteL(iGlobalNoteId);
		iGlobalNote->SetSoftkeys(R_AVKON_SOFTKEYS_CLOSE);
		iGlobalNoteId = iGlobalNote->ShowNoteL(EAknGlobalInformationNote, iGlobalNoteText);
		iGlobalNoteText = KNullDesC;
		}
	
	return ETrue;
	}
