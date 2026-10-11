#include "GsmPosRemoteEngine.h"
#include "GsmPosLogger.h"
#include "GsmPosRemoteContainer.h"

#include <aknnotewrappers.h> 
#include <aknkeylock.h>

CGsmPosRemoteEngine::CGsmPosRemoteEngine(CGsmPosRemoteContainer& aScreen) 
: iScreen(aScreen)
	{
	}

CGsmPosRemoteEngine::~CGsmPosRemoteEngine()
	{
	LOG_FUNCTION;
	
	delete iCellIdMonitor;
	
	delete iBluetooth;
	
	/*
	delete iPeriodic;
	*/
	}

CGsmPosRemoteEngine* CGsmPosRemoteEngine::NewL(CGsmPosRemoteContainer& aScreen)
	{
	CGsmPosRemoteEngine* self = new (ELeave) CGsmPosRemoteEngine(aScreen);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CGsmPosRemoteEngine::ConstructL()
	{
	LOG_FUNCTION_LEAVE;

	iCellIdMonitor = CGsmPosCellIdMonitor::NewL(*this);
		
	iBluetooth = CGsmPosBluetooth::NewL(*this);
	iBluetooth->StartSendingBroadcastsL();
	
	/*
	iPeriodic = CPeriodic::NewL(CActive::EPriorityStandard);
	iPeriodic->Start(TTimeIntervalMicroSeconds32(1000000), TTimeIntervalMicroSeconds32(2000000), PeriodicCallback);
	*/
	}

/*
TInt CGsmPosRemoteEngine::PeriodicCallback(TAny* aAny)
	{
	//User::ResetInactivityTime();
	
	RAknKeyLock key_lock;
	if (key_lock.Connect() == KErrNone)
		{
		if (key_lock.IsKeyLockEnabled())
			{
			key_lock.DisableWithoutNote();
			}
		else
			{
			key_lock.EnableWithoutNote();
			}
		}
	key_lock.Close();
	return (TInt) ETrue;
	}
*/

void CGsmPosRemoteEngine::PublishOwnCellIdL()
	{
	LOG_FUNCTION_LEAVE;
	
	//publikuj wlasne CellId przez BT (takze niewazne)
	if (iBluetooth && iBluetooth->BroadcastMode())
		{
		iBluetooth->SendCurrentCellIdL();
		}
	//TODO BT - reconnect jesli bledy
	}

//TODO - przemysl mechanizm UNIEWAZNIANIA cellid przez odbiorce!
void CGsmPosRemoteEngine::CellIdChangeL(const CTelephony::TNetworkInfoV1& aNetworkInfo)
	{
	LOG_FUNCTION;
	
	LOG(_L("iStatus: %d / iAreaKnown: %d / iAccess: %d"), aNetworkInfo.iStatus, aNetworkInfo.iAreaKnown, aNetworkInfo.iAccess);
	
	iNetworkInfo = aNetworkInfo;
	
	//publikuj wlasne CellId przez BT (takze niewazne)
	PublishOwnCellIdL();
	
	if (aNetworkInfo.iStatus == CTelephony::ENetworkStatusCurrent && aNetworkInfo.iAreaKnown)
		{
		iCellIdDataValid = ETrue;
		
		//pokaz na ekranie
		iScreen.UpdateScreen(aNetworkInfo.iShortName, aNetworkInfo.iLocationAreaCode, aNetworkInfo.iCellId);

		//TODO - teksty z RSC, UI poza ta klasa
		CAknInformationNote* note = new(ELeave) CAknInformationNote(ETrue);
		TBuf<128> buf;
		buf.Format(_L("CellId: %S/%u/%u"), &iNetworkInfo.iShortName, iNetworkInfo.iLocationAreaCode, iNetworkInfo.iCellId);
		note->SetTimeout(CAknNoteDialog::EShortestTimeout);
		note->ExecuteLD(buf);
		}
	else
		{
		LOG_ERROR(_L("CellId nieznane!"));
		
		iCellIdDataValid = EFalse;
		
		//pokaz na ekranie
		iScreen.UpdateScreen(aNetworkInfo.iShortName, -1, -1);
		
		//TODO - teksty z RSC, UI poza ta klasa
		CAknWarningNote* note = new(ELeave) CAknWarningNote;
		note->SetTimeout(CAknNoteDialog::EShortestTimeout);
		note->ExecuteLD(_L("CellId nieznane"));		
		}
	}

//TODO wyslij przez BT msg o niewaznym CellID!!!
//np. TNetworkInfo z flagami ENetworkStatusUnknown
//(to samo i w CGsmPosEngine)
void CGsmPosRemoteEngine::CellIdChangeErrorL(TInt aError)
	{
	LOG(_L("CGsmPosRemoteEngine::CellIdChangeErrorL(%d)"), aError);

	LOG_ERROR(_L("CellId nieznane!"));
	
	iCellIdDataValid = EFalse;
	iNetworkInfo.iStatus = CTelephony::ENetworkStatusUnknown;
	iNetworkInfo.iAreaKnown = EFalse;

	//publikuj wlasne CellId przez BT (takze niewazne)
	PublishOwnCellIdL();
	
	//pokaz na ekranie
	iScreen.UpdateScreen(iNetworkInfo.iShortName, -1, -1);
	
	//TODO - teksty z RSC, UI poza ta klasa
	CAknErrorNote* note = new(ELeave) CAknErrorNote;
	TBuf<128> buf;
	buf.Format(_L("CellId: (%d)"), aError);
	note->SetTimeout(CAknNoteDialog::EShortestTimeout);
	note->ExecuteLD(buf);
	}


void CGsmPosRemoteEngine::RemoteCellIdChange(const TPckgBuf<CTelephony::TNetworkInfoV1>& aNetworkInfo, const TDesC& aRemoteName)
	{
	LOG_FUNCTION;

	//implementacja niewymagana
	}

void CGsmPosRemoteEngine::GetLocalCellId(TPckgBuf<CTelephony::TNetworkInfoV1>& aNetworkInfo)
	{
	LOG_FUNCTION;
	
	aNetworkInfo = TPckgBuf<CTelephony::TNetworkInfoV1>(iNetworkInfo);
	}
