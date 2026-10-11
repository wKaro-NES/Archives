
#ifndef CGSMPOSBLUETOOTH_H
#define CGSMPOSBLUETOOTH_H

#include <e32base.h>
#include <e32std.h>

#include <btsdp.h>
#include <btextnotifiers.h>
#include <bttypes.h>
#include <etel3rdparty.h>
#include <bt_sock.h>

class CGsmPosBluetoothRxSocket;

class MGsmPosBluetoothObserver
    {
    public:
        virtual void RemoteCellIdChange(const TPckgBuf<CTelephony::TNetworkInfoV1>& aNetworkInfo, const TDesC& aRemoteName) = 0;
        virtual void GetLocalCellId(TPckgBuf<CTelephony::TNetworkInfoV1>& aNetworkInfo) = 0;
    };

const TInt KGsmPosBluetoothListenPort = 1111;

class CGsmPosBluetooth : public CBase,
							public MBluetoothSocketNotifier
	{
	public:
		~CGsmPosBluetooth();
	
		static CGsmPosBluetooth* NewL(MGsmPosBluetoothObserver& aObserver);
	
	public:
		// New functions
		void StartReceivingBroadcastsL();
		void StartSendingBroadcastsL();
		
		TBool BroadcastMode();
		void SendCurrentCellIdL();
		
		MGsmPosBluetoothObserver& Observer();

		void ShowErrorNoteL(const TDesC& aText, TInt aError);
		
	private:
		CGsmPosBluetooth(MGsmPosBluetoothObserver& aObserver);
		void ConstructL();
	
		TBool GetDeviceAddressL(TBTDevAddr& aBTAddr);
		
		void CancelListen();
		void CancelConnect();
		
	private:
		//MBluetoothSocketNotifier
		/** Notification of a connection complete event.		
		 If no error is reported, then the connection is ready for use.
		*/
		virtual void HandleConnectCompleteL(TInt aErr);

		/** Notification of an accept complete event.		
		 If no error is reported, then we have accepted a connection request 
		 and that connection is ready for use.
		*/
		virtual void HandleAcceptCompleteL(TInt aErr);

		/** Notification of a shutdown complete event.
		 If no error is reported, then the connection has been closed.
		*/
		virtual void HandleShutdownCompleteL(TInt aErr);

		/** Notification of a send complete event.
		 If no error is reported, then an attempt to send data over Bluetooth has succeeded.
		*/
		virtual void HandleSendCompleteL(TInt aErr);

		/** Notification of a receive complete event.
		 If no error is reported, then then we have successfully received
		 a specified quantity of data.
		*/
		virtual void HandleReceiveCompleteL(TInt aErr);

		/** Notification of a ioctl complete event.
		 An HCI request that has an associated command complete has completed.
		*/
		virtual void HandleIoctlCompleteL(TInt aErr);

		/** Notification of a baseband event.
		 If no error is reported, then a baseband event has been retrieved successfully.
		*/
		virtual void HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification& aEventNotification);

	
	private:
		MGsmPosBluetoothObserver& iObserver;
		
		RSocketServ iSocketServer;
		CBluetoothSocket *iListenSocket, *iTxSocket;
		RPointerArray<CGsmPosBluetoothRxSocket> iRxSocket;
		
		TPckgBuf<CTelephony::TNetworkInfoV1> iNetworkInfoPckg;
		
		TBool iConnected;
	};

#endif // CGSMPOSBLUETOOTH_H
