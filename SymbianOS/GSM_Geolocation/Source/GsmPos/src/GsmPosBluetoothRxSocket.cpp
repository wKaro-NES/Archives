
#include "GsmPosBluetoothRxSocket.h"
#include "GsmPosLogger.h"
#include "GsmPos.pan"

#include <aknnotewrappers.h> 

CGsmPosBluetoothRxSocket::CGsmPosBluetoothRxSocket(CGsmPosBluetooth& aBluetooth) :
	iBluetooth(aBluetooth), iNetworkInfoPckg(CTelephony::TNetworkInfoV1())
	{
	}

CGsmPosBluetoothRxSocket* CGsmPosBluetoothRxSocket::NewLC(CGsmPosBluetooth& aBluetooth, RSocketServ& aSocketServer)
	{
	CGsmPosBluetoothRxSocket* self = new (ELeave) CGsmPosBluetoothRxSocket(aBluetooth);
	CleanupStack::PushL(self);
	self->ConstructL(aSocketServer);
	return self;
	}

void CGsmPosBluetoothRxSocket::ConstructL(RSocketServ& aSocketServer)
	{
	LOG_FUNCTION_LEAVE;
	
	iConnectedSocket = CBluetoothSocket::NewL(*this, aSocketServer);
	}
	
CBluetoothSocket* CGsmPosBluetoothRxSocket::Socket()
	{
	return iConnectedSocket;
	}

	
CGsmPosBluetoothRxSocket::~CGsmPosBluetoothRxSocket()
	{
	LOG_FUNCTION;
	
	if (iConnectedSocket)
		{
		iConnectedSocket->CancelAll();
		delete iConnectedSocket;
		}
	}

void CGsmPosBluetoothRxSocket::CancelConnect()
	{
	LOG_FUNCTION;
	
	iConnectedSocket->AsyncDelete();
	iConnectedSocket = NULL;
	}

TInt CGsmPosBluetoothRxSocket::ReceiveCurrentCellId()
	{
	LOG_FUNCTION;
	
	if (iConnectedSocket)
		{
		return iConnectedSocket->Read(iNetworkInfoPckg);
		}
	return KErrGeneral;
	}

void CGsmPosBluetoothRxSocket::SetRemoteName(const TDesC& aRemoteName)
	{
	LOG(_L("CGsmPosBluetoothRxSocket::SetRemoteName(%S)"), &aRemoteName);
	
	iRemoteName = aRemoteName;
	}

const TDesC& CGsmPosBluetoothRxSocket::RemoteName()
	{
	return iRemoteName;
	}


	/*
If the implementation of this function needs to delete associated 
CBluetoothSocket object, it should NOT use delete operator. The implementation 
should call CBluetoothSocket::AsyncDelete() method instead.
*/
void CGsmPosBluetoothRxSocket::HandleConnectCompleteL(TInt aErr)
	{
	LOG(_L("CGsmPosBluetoothRxSocket::HandleConnectCompleteL(%d)"), aErr);
	
	Panic(EGsmPosImproperApiUsage);
	}

void CGsmPosBluetoothRxSocket::HandleAcceptCompleteL(TInt aErr)
	{
	LOG(_L("CGsmPosBluetoothRxSocket::HandleAcceptCompleteL(%d)"), aErr);

	Panic(EGsmPosImproperApiUsage);
	}

void CGsmPosBluetoothRxSocket::HandleShutdownCompleteL(TInt aErr)
	{
	LOG(_L("CGsmPosBluetoothRxSocket::HandleShutdownCompleteL(%d)"), aErr);
	
	//TODO empty ?
	Panic(EGsmPosImproperApiUsage);	
	}

void CGsmPosBluetoothRxSocket::HandleSendCompleteL(TInt aErr)
	{
	LOG(_L("CGsmPosBluetoothRxSocket::HandleSendCompleteL(%d)"), aErr);
	
	Panic(EGsmPosImproperApiUsage);
	}

void CGsmPosBluetoothRxSocket::HandleReceiveCompleteL(TInt aErr)
	{
	LOG(_L("CGsmPosBluetoothRxSocket::HandleReceiveCompleteL(%d)"), aErr);
	if (aErr != KErrNone)
		{
		iBluetooth.ShowErrorNoteL(_L("Receive()"),aErr);

		//TODO a inne KErr...?
		if(aErr == KErrDisconnected)
			{
			CancelConnect();
			}
		
		//uniewaznij net info od rozlaczonego klienta
		CTelephony::TNetworkInfoV1 info = iNetworkInfoPckg();
		info.iStatus = CTelephony::ENetworkStatusUnknown;
		info.iAreaKnown = EFalse;
		TPckgBuf<CTelephony::TNetworkInfoV1> infopckg(info);
		iBluetooth.Observer().RemoteCellIdChange(infopckg, KNullDesC);
		}
	else
		{
		//odebrano net info od klienta - powiadom
		iBluetooth.Observer().RemoteCellIdChange(iNetworkInfoPckg, RemoteName());
		
		//zarejstruj sie po nastepne
		aErr = ReceiveCurrentCellId();
		if (aErr != KErrNone)
			{
			iBluetooth.ShowErrorNoteL(_L("req Receive() again"),aErr);
			}
		}
	}

void CGsmPosBluetoothRxSocket::HandleIoctlCompleteL(TInt aErr)
	{
	LOG(_L("CGsmPosBluetoothRxSocket::HandleIoctlCompleteL(%d)"), aErr);
	
	Panic(EGsmPosImproperApiUsage);
	}

void CGsmPosBluetoothRxSocket::HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification& /* aEventNotification */)
	{
	LOG(_L("CGsmPosBluetoothRxSocket::HandleActivateBasebandEventNotifierCompleteL(%d)"), aErr);
	
	Panic(EGsmPosImproperApiUsage);
	}
