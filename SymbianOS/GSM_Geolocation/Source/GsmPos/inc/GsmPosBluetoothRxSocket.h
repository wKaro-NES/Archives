
#ifndef CGSMPOSBLUETOOTHSOCKET_H
#define CGSMPOSBLUETOOTHSOCKET_H

#include <e32base.h>
#include <e32std.h>

#include <btsdp.h>
#include <btextnotifiers.h>
#include <bttypes.h>
#include <etel3rdparty.h>
#include <bt_sock.h>

#include "GsmPosBluetooth.h"

class CGsmPosBluetoothRxSocket : public CBase,
							public MBluetoothSocketNotifier
	{
	public:
		static CGsmPosBluetoothRxSocket* NewLC(CGsmPosBluetooth& aBluetooth, RSocketServ& aSocketServer);
		~CGsmPosBluetoothRxSocket();
		
		void SetRemoteName(const TDesC& aRemoteName);
		const TDesC& RemoteName();
		
		TInt ReceiveCurrentCellId();
		CBluetoothSocket* Socket();
		
	private:
		CGsmPosBluetoothRxSocket(CGsmPosBluetooth& aBluetooth);
		void ConstructL(RSocketServ& aSocketServer);

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
		CGsmPosBluetooth& iBluetooth;
		
		CBluetoothSocket *iConnectedSocket;
		
		TPckgBuf<CTelephony::TNetworkInfoV1> iNetworkInfoPckg;
		
		TBuf<64> iRemoteName;	//TODO max len (sprawdz w SDK)
	};

#endif // CGSMPOSBLUETOOTHSOCKET_H
