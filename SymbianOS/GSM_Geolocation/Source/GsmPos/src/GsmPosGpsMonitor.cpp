/*
 ============================================================================
 Name		: GsmPosGpsMonitor.cpp
 Author	  : w.karolik
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CGsmPosGpsMonitor implementation
 ============================================================================
 */

//TODO - monitorowanie zaniku widoku satelitow (sygnal dzwiekowy, TextToSpeech itp.)
//TODO - ignorowanie pozycji blizszej niz XXmetrow od ostatniej pozycji
//TODO - wybor modulow GPS

#include "GsmPosGpsMonitor.h"
#include "GsmPosLogger.h"

CGsmPosGpsMonitor::CGsmPosGpsMonitor(MGsmPosGpsMonitorObserver& aObserver) :
	CActive(EPriorityStandard), iObserver(aObserver)
	/* TODO remove
	 iLandmarkCategory(KPosLmNullItemId) 
	 */
	{
	}

CGsmPosGpsMonitor* CGsmPosGpsMonitor::NewL(MGsmPosGpsMonitorObserver& aObserver)
	{
	CGsmPosGpsMonitor* self = new (ELeave) CGsmPosGpsMonitor(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CGsmPosGpsMonitor::ConstructL()
	{
	LOG_FUNCTION_LEAVE;
	
	CActiveScheduler::Add(this);
	
	User::LeaveIfError(iPosServer.Connect());

	TPositionModuleId module_id = KPositionNullModuleId;
	TPositionModuleInfo module_info;

	TUint num_of_modules = 0;
	iPosServer.GetNumModules(num_of_modules);
	for( TUint i = 0; i < num_of_modules; i++ )
		{
		iPosServer.GetModuleInfoByIndex(i, module_info);

		//TODO dump ?
		
		if(module_info.IsAvailable())
			{
			TBuf<KPositionMaxModuleName> module_name;
			module_info.GetModuleName( module_name );
			module_id = module_info.ModuleId();
			
			LOG(_L("wybierz modul %S / id: 0x%x"), &module_name, module_id);
			
			break;
			}
		}
	
	if (module_id == KPositionNullModuleId)
		{
		LOG_ERROR(_L("brak modulu GPS!"));
		User::LeaveIfError(KErrNotFound);
		}

	LOG(_L("Open()"));
	User::LeaveIfError(iPositioner.Open(iPosServer, module_id));
	
	LOG(_L("SetUpdateOptions()"));
	TPositionUpdateOptions updateops;
	updateops.SetUpdateInterval(TTimeIntervalMicroSeconds(KGpsUpdateIntervalMicroseconds));	//TODO - zalezne od predkosci poruszania sie? albo w settingsach?
	updateops.SetUpdateTimeOut(TTimeIntervalMicroSeconds(KGpsUpdateTimeOutMicroseconds)); 	//TODO - tuning albo w settingsach
	updateops.SetMaxUpdateAge(TTimeIntervalMicroSeconds(0));		//disable cache
	updateops.SetAcceptPartialUpdates(EFalse);						//disable partial position data,
	User::LeaveIfError(iPositioner.SetUpdateOptions(updateops));
	
	//Identifying the requestor:
	//To safeguard the privacy of a user, the Location Framework requires each requestor of location
	//information to identify themselves and handle the display of appropriate notifications to the user
	//if necessary.
	LOG(_L("SetRequestor()"));
	User::LeaveIfError(iPositioner.SetRequestor(CRequestor::ERequestorService, CRequestor::EFormatApplication, KRequestor));
	
	LOG(_L("CPosLandmarkDatabase::OpenL()"));
	
	iLandmarkDb = CPosLandmarkDatabase::OpenL();
	if (iLandmarkDb->IsInitializingNeeded())
		{
		LOG(_L("CPosLandmarkDatabase::InitializeL()"));
		
		CPosLmOperation* operation = iLandmarkDb->InitializeL();
		CleanupStack::PushL(operation);
		operation->ExecuteL();							//TODO asynchronous?
		CleanupStack::PopAndDestroy(operation);
		}

	// 1sza pozycja - interesuja nas nawet dane z cache
	// TODO - jak to sie ma do w/w ustawien cache na blokade?
	RequestPositionUpdate(ETrue);
	
	}

CGsmPosGpsMonitor::~CGsmPosGpsMonitor()
	{
	LOG_FUNCTION;
	
	Cancel();

	delete iLandmarkDb;
	
	iPositioner.Close();
	iPosServer.Close();
	}

void CGsmPosGpsMonitor::DoCancel()
	{
	LOG_FUNCTION;

	iPositioner.CancelRequest(iCancellation);
	}

void CGsmPosGpsMonitor::RunL()
	{
	//LOG_FUNCTION_LEAVE;
	
	/*
	 » Symbian OS v9.4  » Symbian OS guide  » Location Based Services  » LBS guide documents  
		» The Location Acquisition API » Location Acquisition API runtime behaviour !
		
		Timeout in Location Server. Module has previously returned an inaccurate position. 
		Else if LBS is configured to return only accurate positions: Inaccurate position is returned. TRequestStatus is KPositionQualityLoss.
	*/ 
	
    TInt status = iStatus.Int();

    //sprawdz czy dane pochodza z cache (1sze uruchomienie)
    TBool cached = (iCancellation == EPositionerGetLastKnownPosition);
    
    //najpierw rejestruj sie na zmiany pozycji (zanim moze wystapic Leave)
	RequestPositionUpdate(EFalse);
    
    //a nastepnie opracuj aktualne wyniki
	if (status == KErrNone || status == KPositionQualityLoss)
		{
		if (iStatus == KPositionQualityLoss)
			{
			LOG_ERROR(_L("uwaga: KPositionQualityLoss!"));	//TODO
			}
		iObserver.GpsChange(iPosInfo, cached);
		}
	else
		{
		iObserver.GpsChangeError(status);
		}
	}

TInt CGsmPosGpsMonitor::RunError(TInt aError)
	{
	LOG_ERROR(_L("CGsmPosGpsMonitor::RunError(%d)"), aError);
	
	return KErrNone;
	}

void CGsmPosGpsMonitor::RequestPositionUpdate(TBool aCached)
	{
	LOG_FUNCTION;

	if (!IsActive())
		{
		if (aCached)
			{
			iCancellation = EPositionerGetLastKnownPosition;
			iPositioner.GetLastKnownPosition(iPosInfo, iStatus);
			LOG(_L("GetLastKnownPosition()"));
			}
		else
			{
			iCancellation = EPositionerNotifyPositionUpdate;
			iPositioner.NotifyPositionUpdate(iPosInfo, iStatus);
			}
		SetActive();
		}
	else
		{
		LOG_ERROR(_L("already active"));
		}
	}


void CGsmPosGpsMonitor::AddLandmarkL(const TDesC& aName, const TDesC& aDescription, const TLocality& aLocality)
	{
	LOG_FUNCTION_LEAVE;
	
	if (iLandmarkDb)
		{
		//grupuj dodane landmarki w oddzielnej kategorii...
		if (iLandmarkCategory == KPosLmNullItemId)
			{
			CPosLmCategoryManager* mgr = CPosLmCategoryManager::NewL(*iLandmarkDb);
			CleanupStack::PushL(mgr);

			//wlasnej
		    CPosLandmarkCategory* category = CPosLandmarkCategory::NewLC();
		    _LIT(KLandmarkCategoryName, "Stacje bazowe GSM");		//TODO - do *.H, nazwa inna?
		    category->SetCategoryNameL(KLandmarkCategoryName);
		    
		    TRAPD(err, iLandmarkCategory = mgr->AddCategoryL(*category));
		    if (err == KErrAlreadyExists)
		    	{
		    	LOG_ERROR(_L("kategoria %S juz istnieje"), &KLandmarkCategoryName());
		    	
		    	iLandmarkCategory = mgr->GetCategoryL(KLandmarkCategoryName);
		    	}
		    else if (err != KErrNone)
		    	{
		    	User::Leave(err);
		    	}

		    //albo wbudowanej Communication
			//9000 Communication Internet Access Point, Public Telephone, Wireless LAN Hot Spot
			//iLandmarkCategory = mgr->GetGlobalCategoryL(9000);
			
			CleanupStack::PopAndDestroy(category);
			CleanupStack::PopAndDestroy(mgr);
			}

		CPosLandmark* lm = CPosLandmark::NewLC();
		lm->SetLandmarkNameL(aName);
		lm->SetLandmarkDescriptionL(aDescription);
		lm->SetPositionL(aLocality);
		lm->AddCategoryL(iLandmarkCategory);
		iLandmarkDb->AddLandmarkL(*lm);
		
		//TODO - duplikaty
		//iLandmarkDb->UpdateLandmarkL(*lm); //TODO - leave KErrNotFound
		
		CleanupStack::PopAndDestroy(lm);
		}
	}

//TODO teraz zwraca wszystkie landmarki, bez filtracji... czy warto bawic sie w filtrowanie? chyba ze do omowienia w dokumencie mgr API landmark
//TODO CPosLmSearchCriteria , a konkretnie CPosLmNearestCriteria
vector<TPosition>* CGsmPosGpsMonitor::ReadLandmarkL(const TPosition& aPosition, TUint aRadius)
	{
	LOG_FUNCTION_LEAVE;
	
	vector<TPosition>* landmarks = new (ELeave) vector<TPosition>();
	
	CPosLmItemIterator* iter = iLandmarkDb->LandmarkIteratorL();
	CleanupStack::PushL( iter );

	TPosLmItemId lmID = KPosLmNullItemId;
	while ( ( lmID = iter->NextL() ) != KPosLmNullItemId )
	    {
	    CPosLandmark* lm = iLandmarkDb->ReadLandmarkLC( lmID );
	    
	    //wybierz tylko te w odleglosci < aRadius
	    TPosition position;
	    TInt status = lm->GetPosition(position);
	    if (status == KErrNone)
	    	{
	    	TReal32 dist = 0;
	    	status = aPosition.Distance(position, dist);
	    	if (status == KErrNone && dist < aRadius)
	    		landmarks->push_back(position);
	    	}
	    if (status != KErrNone)
	    	{
	    	LOG_ERROR(_L("can't read landmarks error %d"), status);
	    	}
	    CleanupStack::PopAndDestroy( lm );
	    }
	CleanupStack::PopAndDestroy( iter );
	
	return landmarks;
	}

void CGsmPosGpsMonitor::ShowSatelliteInfoL()
	{
	LOG_FUNCTION_LEAVE;
	
	// Creating instance of CSatelliteInfoUI
	iSatelliteInfo = CSatelliteInfoUI::NewL();
				
	// ExecuteLD displays the satellite dialog.
	// The dialog display satellite ID's along with 
	// their signal strengths depicted using bar's.
	// TSatelliteView enumeration (ESatelliteFirmamentView or ESatelliteSignalStrengthView)
	iSatelliteInfo->SetLaunchView(CSatelliteInfoUI::ESatelliteSignalStrengthView);
	iSatelliteInfo->ExecuteLD(KRequestor);
	iSatelliteInfo = NULL;
	}

void CGsmPosGpsMonitor::HandleForegroundEventL(TBool aForeground)
	{
	LOG_FUNCTION_LEAVE;
	
	if(iSatelliteInfo)
		{
		// SatelliteInfo To Handle Foreground Event
		iSatelliteInfo->HandleForegroundEventL(aForeground);		}
	}

