
#include "GsmPosBluetooth.h"
#include "GsmPosBluetoothRxSocket.h"
#include "GsmPosLogger.h"
#include "GsmPos.pan"

#include <aknnotewrappers.h> 

CGsmPosBluetooth::CGsmPosBluetooth(MGsmPosBluetoothObserver& aObserver) :
	iObserver(aObserver), iNetworkInfoPckg(CTelephony::TNetworkInfoV1())
	{
	}

CGsmPosBluetooth* CGsmPosBluetooth::NewL(MGsmPosBluetoothObserver& aObserver)
	{
	CGsmPosBluetooth* self = new (ELeave) CGsmPosBluetooth(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CGsmPosBluetooth::ConstructL()
	{
	LOG_FUNCTION_LEAVE;
	
	User::LeaveIfError(iSocketServer.Connect());
	}

CGsmPosBluetooth::~CGsmPosBluetooth()
	{
	LOG_FUNCTION;
	
	if (iListenSocket)
		{
		iListenSocket->CancelAll();
		delete iListenSocket;
		}

	if (iTxSocket)
		{
		iTxSocket->CancelAll();
		delete iTxSocket;
		}
	
	iRxSocket.ResetAndDestroy();
	iRxSocket.Close();
	
	iSocketServer.Close();
	}

MGsmPosBluetoothObserver& CGsmPosBluetooth::Observer()
	{
	return iObserver;
	}

TBool CGsmPosBluetooth::BroadcastMode()
	{
	return iListenSocket == NULL;
	}

TBool CGsmPosBluetooth::GetDeviceAddressL(TBTDevAddr& aBTAddr)
{
	LOG_FUNCTION_LEAVE;
	
	RNotifier notifier;
	User::LeaveIfError(notifier.Connect());
	 
	TBTDeviceSelectionParamsPckg filter;
	TBTDeviceResponseParamsPckg response;
	 
	filter().SetUUID(KSerialPortUUID); 			//TODO sprawdz inny rodzaj
	 
	TRequestStatus status;
	notifier.StartNotifierAndGetResponse( status, KDeviceSelectionNotifierUid, filter, response);
	User::WaitForRequest(status);
	 
	notifier.Close();
	
	if (status.Int() == KErrNone)
		{
		if (response().IsValidBDAddr())
			{
			aBTAddr = response().BDAddr();
			
			#ifdef GSMPOS_LOG
				//dump address & name
				{
				TBTDeviceName dname;
				if (response().IsValidDeviceName())
					{
					dname = response().DeviceName();
					LOG(_L("BT device name %S"), &dname);
					}
		
				TBuf<64> buf;								//TODO max len (sprawdz w SDK)
				aBTAddr.GetReadable(buf, KNullDesC, _L(":"), KNullDesC);
				LOG(_L("BT address %S"), &buf);
				}
			#endif
			
			return ETrue;
			}
		}
	else 
		if (status.Int() != KErrCancel)
			{
			LOG_ERROR(_L("CGsmPosBluetooth::GetDeviceAddressL(), error %d"), status.Int());
			
			User::LeaveIfError(status.Int());
			}
	
	return EFalse;
}

void CGsmPosBluetooth::StartSendingBroadcastsL()
	{
	LOG_FUNCTION_LEAVE;
	if (iTxSocket)
		{
		LOG_ERROR(_L("already Connect()!"));	//TODO panic?

		//wyslij "ping"
		SendCurrentCellIdL();
		return;
		}

	TBTDevAddr bt_hw_addr;
	if (GetDeviceAddressL(bt_hw_addr))
		{
		TProtocolDesc	protocol_info;
		User::LeaveIfError(iSocketServer.FindProtocol(_L("L2CAP"), protocol_info));
		iTxSocket = CBluetoothSocket::NewL(*this,iSocketServer,protocol_info.iSockType,protocol_info.iProtocol);
		
		TL2CAPSockAddr bt_addr;
		bt_addr.SetPort(KGsmPosBluetoothListenPort);
		bt_addr.SetBTAddr(bt_hw_addr);
	
		User::LeaveIfError(iTxSocket->Connect(bt_addr));
		}	
	}

void CGsmPosBluetooth::StartReceivingBroadcastsL()
	{
	LOG_FUNCTION_LEAVE;
	
	if (iListenSocket)
		{
		LOG_ERROR(_L("already Listen()!"));

		return;
		}

	TProtocolDesc	protocol_info;
	User::LeaveIfError(iSocketServer.FindProtocol(_L("L2CAP"), protocol_info));
	iListenSocket = CBluetoothSocket::NewL(*this,iSocketServer,protocol_info.iSockType,protocol_info.iProtocol);
	
	TL2CAPSockAddr bt_addr;
	TBTServiceSecurity security;
	security.SetAuthentication(ETrue);
	security.SetEncryption(ETrue);
	security.SetAuthorisation(ETrue);
	security.SetDenied(EFalse);

	bt_addr.SetSecurity(security); 
	bt_addr.SetPort(KGsmPosBluetoothListenPort);
	
	User::LeaveIfError(iListenSocket->Bind(bt_addr));

	User::LeaveIfError(iListenSocket->Listen(1));
	
	CGsmPosBluetoothRxSocket* rx_socket = CGsmPosBluetoothRxSocket::NewLC(*this, iSocketServer);
	iRxSocket.AppendL(rx_socket);
	CleanupStack::Pop(rx_socket);
	
	User::LeaveIfError(iListenSocket->Accept(*(rx_socket->Socket())));
	}

void CGsmPosBluetooth::SendCurrentCellIdL()
	{
	LOG_FUNCTION_LEAVE;
	
	if (iConnected)
		{
		iObserver.GetLocalCellId(iNetworkInfoPckg);
		TInt err = iTxSocket->Write(iNetworkInfoPckg);
		if (err == KErrInUse)
			{
			//TODO - debugging!
			LOG_ERROR(_L("busy, cancel oustanding and try again..."));
			iTxSocket->CancelWrite();
			err = iTxSocket->Write(iNetworkInfoPckg);
			}
		if (err != KErrNone)
			{
			LOG_ERROR(_L("error %d"), err);
			ShowErrorNoteL(_L("SendCurrentCellIdL()"),err);
			}
		User::LeaveIfError(err);
		}
	}

void CGsmPosBluetooth::CancelListen()
	{
	LOG_FUNCTION;

	iRxSocket.ResetAndDestroy();
	
	iListenSocket->AsyncDelete();
	iListenSocket = NULL;
	iConnected = EFalse;
	}

void CGsmPosBluetooth::CancelConnect()
	{
	LOG_FUNCTION;
	
	iTxSocket->AsyncDelete();
	iTxSocket = NULL;
	iConnected = EFalse;
	}
/*
If the implementation of this function needs to delete associated 
CBluetoothSocket object, it should NOT use delete operator. The implementation 
should call CBluetoothSocket::AsyncDelete() method instead.
*/
void CGsmPosBluetooth::HandleConnectCompleteL(TInt aErr)
	{
	LOG(_L("CGsmPosBluetooth::HandleConnectCompleteL(%d)"), aErr);
	
	if (aErr != KErrNone)
		{
		ShowErrorNoteL(_L("Connect()"),aErr);
		
		CancelConnect();
		}
	else
		{
		iConnected = ETrue;
		//wyslij aktualne netinfo
		SendCurrentCellIdL();
		}
	}

void CGsmPosBluetooth::HandleAcceptCompleteL(TInt aErr)
	{
	LOG(_L("CGsmPosBluetooth::HandleAcceptCompleteL(%d)"), aErr);

	if (aErr != KErrNone)
		{
		ShowErrorNoteL(_L("Accept()"),aErr);
		CancelListen();
		}
	else
		{
		//ostatnio dodany pusty RxSocket zostal powiazany z podlaczajacym sie do servera klientem...
		//...pobierz adres klienta...
		CGsmPosBluetoothRxSocket* rx_socket = iRxSocket[iRxSocket.Count() - 1];
		TSockAddr addr;
		rx_socket->Socket()->RemoteName(addr);
		TBuf<64> buf;										//TODO max len (sprawdz w SDK)
		TBTSockAddr::Cast(addr).BTAddr().GetReadable(buf, KNullDesC, _L(":"), KNullDesC);
		rx_socket->SetRemoteName(buf);

		//...zarejestruj sie po odbieranie net info od klienta...
		aErr = rx_socket->ReceiveCurrentCellId();
		if (aErr != KErrNone)
			{
			ShowErrorNoteL(_L("req Receive() in Accept()"),aErr);
			}

		//...gotowosc do podlaczenia nastepnego klienta
		rx_socket = CGsmPosBluetoothRxSocket::NewLC(*this, iSocketServer);
		iRxSocket.AppendL(rx_socket);
		CleanupStack::Pop(rx_socket);
		
		aErr = iListenSocket->Accept(*(rx_socket->Socket()));
		if (aErr != KErrNone)
			{
			ShowErrorNoteL(_L("req Accept() in Accept()"),aErr);
			}
		}
	}

void CGsmPosBluetooth::HandleShutdownCompleteL(TInt aErr)
	{
	LOG(_L("CGsmPosBluetooth::HandleShutdownCompleteL(%d)"), aErr);
	if (aErr != KErrNone)
		{
		ShowErrorNoteL(_L("Shutdown()"),aErr);
		}
	}

void CGsmPosBluetooth::HandleSendCompleteL(TInt aErr)
	{
	LOG(_L("CGsmPosBluetooth::HandleSendCompleteL(%d)"), aErr);
	if (aErr != KErrNone)
		{
		ShowErrorNoteL(_L("Send()"),aErr);
		
		//TODO a inne KErr...?
		if(aErr == KErrDisconnected)
			{
			CancelConnect();
			}
		}
	}

void CGsmPosBluetooth::HandleReceiveCompleteL(TInt aErr)
	{
	LOG(_L("CGsmPosBluetooth::HandleReceiveCompleteL(%d)"), aErr);
	
	Panic(EGsmPosImproperApiUsage);
	}

void CGsmPosBluetooth::HandleIoctlCompleteL(TInt aErr)
	{
	LOG(_L("CGsmPosBluetooth::HandleIoctlCompleteL(%d)"), aErr);
	if (aErr != KErrNone)
		{
		ShowErrorNoteL(_L("Ioctl()"),aErr);
		}
	}

void CGsmPosBluetooth::HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification& aEventNotification)
	{
	LOG(_L("CGsmPosBluetooth::HandleActivateBasebandEventNotifierCompleteL(%d)"), aErr);
	if (aErr != KErrNone)
		{
		ShowErrorNoteL(_L("Baseband event"),aErr);
		}
	}

void CGsmPosBluetooth::ShowErrorNoteL(const TDesC& aText, TInt aError)
	{
	//TODO - teksty z RSC, UI poza ta klasa
	//TODO - usunac ?
	CAknErrorNote* note = new(ELeave) CAknErrorNote;
	TBuf<128> buf;
	buf.Format(_L("BT (%d) %S"), aError, &aText.Left(100));
	note->SetTimeout(CAknNoteDialog::ENoTimeout);
	note->ExecuteLD(buf);		
	}
