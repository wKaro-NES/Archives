/*
 ============================================================================
 Name		: GsmPosCellIdMonitor.cpp
 Author	  : WojciechK
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CGsmPosCellIdMonitor implementation
 ============================================================================
 */

//TODO - symulowanie zmian CellId pod emulatorem

#include "GsmPosCellIdMonitor.h"
#include "GsmPosLogger.h"

CGsmPosCellIdMonitor::CGsmPosCellIdMonitor(MGsmPosCellIdMonitorObserver& aObserver)
: CActive(EPriorityStandard), iNwInfoPckg(iNwInfo), iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }
 
CGsmPosCellIdMonitor* CGsmPosCellIdMonitor::NewL(MGsmPosCellIdMonitorObserver& aObserver)
    {
    CGsmPosCellIdMonitor* self = new (ELeave) CGsmPosCellIdMonitor(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
 
void CGsmPosCellIdMonitor::ConstructL()
    {
    LOG_FUNCTION_LEAVE;
    
    iTelephony = CTelephony::NewL();
    
    //pobierz aktualne network info
    iTelephony->GetCurrentNetworkInfo(iStatus, iNwInfoPckg);
    iCancellation = CTelephony::EGetCurrentNetworkInfoCancel;
    SetActive();
    }
 
CGsmPosCellIdMonitor::~CGsmPosCellIdMonitor()
    {
    LOG_FUNCTION;
    
    Cancel();
    delete iTelephony;
    }
 
void CGsmPosCellIdMonitor::DoCancel()
    {
    LOG_FUNCTION;
    
    iTelephony->CancelAsync(iCancellation);
    }
 
void CGsmPosCellIdMonitor::RunL()
    {
    LOG_FUNCTION_LEAVE;
    
    TInt status = iStatus.Int();

    //najpierw rejestruj sie na zmiany network info (zanim moze wystapic Leave)
    iTelephony->NotifyChange(iStatus, CTelephony::ECurrentNetworkInfoChange, iNwInfoPckg);
    iCancellation = CTelephony::ECurrentNetworkInfoChangeCancel;
    SetActive();
    
    //a nastepnie opracuj aktualne wyniki
    if (status == KErrNone)
		{
		//TODO - usunac dump
		CTelephony::TNetworkInfoV1 info = iNwInfoPckg();
		
#ifdef __WINS__
		//UWM Zolnierska 14 (53.772089,20.488021)
		info.iCountryCode = _L("260");
		info.iNetworkId = _L("02");
		info.iShortName = _L("Era");
		info.iLocationAreaCode = 41100;
		info.iCellId = 222836218;
		info.iAreaKnown = 1;
		info.iStatus = CTelephony::ENetworkStatusCurrent;
		DumpNetworkInfo(info);
		iObserver.CellIdChangeL(info);
#else		
		DumpNetworkInfo(info);
		
		//ze wzgledu na brak wartosci iShortname w firmware niektorych terminali - ustal iShortName na podstawie obowiazkowych MCC/MNC
		if (info.iCountryCode.CompareF(_L("260")) == 0)
			{
			if (info.iNetworkId.CompareF(_L("02")) == 0)
				info.iShortName = KOperatorGsmEra;
			else if (info.iNetworkId.CompareF(_L("01")) == 0) 
				info.iShortName = KOperatorGsmPlus;
			else if (info.iNetworkId.CompareF(_L("03")) == 0) 
				info.iShortName = KOperatorGsmOrange;
			else
				{
				LOG_ERROR(_L("Uwaga! Brak wsparcia aplikacji dla operatora GSM"));
				}
			}
			
        iObserver.CellIdChangeL(info);
#endif
        }
    else
    	{
    	iObserver.CellIdChangeErrorL(status);
    	}
    }
 
TInt CGsmPosCellIdMonitor::RunError(TInt aError)
    {
    LOG(_L("CGsmPosCellIdMonitor::RunError(%d)"), aError);

    //ignoruj Leave
    
    return KErrNone;
    }

void CGsmPosCellIdMonitor::DumpNetworkInfo(const CTelephony::TNetworkInfoV1& aNetworkInfo)
	{
	LOG_FUNCTION;
	
	//Mode of the network.
	TPtrC network_mode;
	switch (aNetworkInfo.iMode)
		{
		case CTelephony::ENetworkModeUnknown:
			network_mode.Set(_L("Network mode is unknown."));
			break;
		case CTelephony::ENetworkModeUnregistered:
			network_mode.Set(_L("Mobile device is not registered."));
			break;
		case CTelephony::ENetworkModeGsm:
			network_mode.Set(_L("GSM/GPRS or DCS1800 network."));
			break;
		case CTelephony::ENetworkModeAmps:
			network_mode.Set(_L("AMPS network."));
			break;
		case CTelephony::ENetworkModeCdma95:
			network_mode.Set(_L("CDMA (IS-95) network."));
			break;
		case CTelephony::ENetworkModeCdma2000:
			network_mode.Set(_L("CDMA (cdma2000) network."));
			break;
		case CTelephony::ENetworkModeWcdma:
			network_mode.Set(_L("WCDMA (UTRA Frequency Division Duplex (FDD)) network."));
			break;
		case CTelephony::ENetworkModeTdcdma:
			network_mode.Set(_L("TD-CDMA (UTRA Time Division Duplex (TDD)) network."));
			break;
		default:
			network_mode.Set(_L("???"));
			break;
		}
	LOG(_L("\tiMode\t==%d\t%S"), aNetworkInfo.iMode, &network_mode);
	
	//Status of the network
	TPtrC network_stat;
	switch (aNetworkInfo.iStatus)
		{
		case CTelephony::ENetworkStatusUnknown:
			network_stat.Set(_L("Status is unknown"));
			break;
		case CTelephony::ENetworkStatusAvailable:
			network_stat.Set(_L("A network that the mobile device is allowed to register to."));
			break;
		case CTelephony::ENetworkStatusCurrent:
			network_stat.Set(_L("Currently registered network."));
			break;
		case CTelephony::ENetworkStatusForbidden:
			network_stat.Set(_L("A network that the ME is not allowed to register to."));
			break;
		default:
			network_stat.Set(_L("???"));
			break;
		}
	LOG(_L("\tiStatus\t==%d\t%S"), aNetworkInfo.iStatus, &network_stat);
	
	//The MCC of the network.
	//LOG(_L("\tiCountryCode==%S"), &aNetworkInfo.iCountryCode);

	//The network identity (NID in CDMA and MNC in GSM).
	LOG(_L("\tiNetworkId==%S"), &aNetworkInfo.iNetworkId);

	//The alpha-tag displayed when this is the serving network.
	//LOG(_L("\tiDisplayTag==%S"), &aNetworkInfo.iDisplayTag);

	//On GSM/WCDMA networks, the short name (up to 8 characters) of the network operator.
	LOG(_L("\tiShortName==%S"), &aNetworkInfo.iShortName);

	//On CDMA networks, the band and band class of the CDMA network operator.
	//LOG(_L("\tiBandInfo==%S"), &aNetworkInfo.iBandInfo);

	//On CDMA networks, the system identity (SID) of the CDMA or AMPS network
	//LOG(_L("\tiCdmaSID==%S"), &aNetworkInfo.iCdmaSID);

	//On GSM/WCDMA networks, the long name (up to 16 characters) of the network operator.
	//LOG(_L("\tiLongName==%S"), &aNetworkInfo.iLongName);

	//On GSM/WCDMA networks, the type of network access.
	TPtrC network_access;
	switch (aNetworkInfo.iAccess)
		{
		case CTelephony::ENetworkAccessUnknown:
			network_access.Set(_L("There is no network activity."));
			break;
		case CTelephony::ENetworkAccessGsm:
			network_access.Set(_L("The access technology is GSM."));
			break;
		case CTelephony::ENetworkAccessGsmCompact:
			network_access.Set(_L("The access technology is GSM COMPACT."));
			break;
		case CTelephony::ENetworkAccessUtran:
			network_access.Set(_L("The access technology is UTRAN (UMTS Network)."));
			break;
		default:
			network_access.Set(_L("???"));
			break;
		}
	LOG(_L("\tiAccess\t==%d\t%S"), aNetworkInfo.iAccess, &network_access);

	//On GSM/WCDMA networks, boolean indicating whether the location area is known.
	LOG(_L("\tiAreaKnown==%d"), aNetworkInfo.iAreaKnown);

	//On GSM/WCDMA networks, the location area code.  Only valid when iAccess is true.
	LOG(_L("\tiLocationAreaCode==%u"), aNetworkInfo.iLocationAreaCode);

	//On GSM/WCDMA networks, the cell identity code.  Only valid when iAccess is true.
	LOG(_L("\tiCellId==%u"), aNetworkInfo.iCellId);
	}
