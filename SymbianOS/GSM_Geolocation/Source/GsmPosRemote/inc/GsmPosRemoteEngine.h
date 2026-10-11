#ifndef GSMPOSREMOTEENGINE_H
#define GSMPOSREMOTEENGINE_H

#include <e32std.h>
#include <e32base.h>

#include "GsmPos.pan"
#include "GsmPosCellIdMonitor.h"
#include "GsmPosBluetooth.h"

class CGsmPosRemoteContainer;

class CGsmPosRemoteEngine : 	public CBase,
						public MGsmPosCellIdMonitorObserver,
						public MGsmPosBluetoothObserver
	{
	public:
		static CGsmPosRemoteEngine* NewL(CGsmPosRemoteContainer& aScreen);
		~CGsmPosRemoteEngine();

        void PublishOwnCellIdL();
        
		//MGsmPosCellIdMonitorObserver
		void CellIdChangeL(const CTelephony::TNetworkInfoV1& aNetworkInfo);
		void CellIdChangeErrorL(TInt aError);
		
		//MGsmPosBluetoothObserver
	    void RemoteCellIdChange(const TPckgBuf<CTelephony::TNetworkInfoV1>& aNetworkInfo, const TDesC& aRemoteName);
        void GetLocalCellId(TPckgBuf<CTelephony::TNetworkInfoV1>& aNetworkInfo);

	private:
		CGsmPosRemoteEngine(CGsmPosRemoteContainer& aScreen);
		void ConstructL();
		
		/*
		static TInt PeriodicCallback(TAny* aAny);
		*/
		
	private:
		//monitorowanie zmian sieci GSM
		CGsmPosCellIdMonitor* iCellIdMonitor;
		TBool iCellIdDataValid;
		CTelephony::TNetworkInfoV1 iNetworkInfo;
		
		//Bluetooth
		CGsmPosBluetooth* iBluetooth;		
		
		/*
		//Periodic timer
		CPeriodic* iPeriodic;
		*/
		
		//element UI
		CGsmPosRemoteContainer& iScreen;
	};

#endif // GSMPOSREMOTEENGINE_H
