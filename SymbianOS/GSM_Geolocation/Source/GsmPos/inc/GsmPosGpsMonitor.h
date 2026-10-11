/*
 ============================================================================
 Name		: GsmPosGpsMonitor.h
 Author	  : w.karolik
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CGsmPosGpsMonitor declaration
 ============================================================================
 */

#ifndef GSMPOSGPSMONITOR_H
#define GSMPOSGPSMONITOR_H

#include <vector>
#include <algorithm>
using namespace std;

#include <e32base.h>
#include <e32std.h>	
#include <lbs.h>
#include <epos_cposlandmarkdatabase.h> 
#include <epos_cposlandmarkcategory.h>
#include <epos_cposlmcategorymanager.h>
#include <csatelliteinfoui.h>

_LIT(KRequestor, "CGsmPosGpsMonitor");

#ifdef __WINS__
const TInt KGpsUpdateIntervalMicroseconds = 30000000;
const TInt KGpsUpdateTimeOutMicroseconds  = 60000000;
#else
const TInt KGpsUpdateIntervalMicroseconds = 5000000;
const TInt KGpsUpdateTimeOutMicroseconds  = 30000000;
#endif

class MGsmPosGpsMonitorObserver
    {
    public:
        virtual void GpsChange(const TPositionInfo& aPositionInfo, TBool aCached) = 0;
        virtual void GpsChangeError(TInt aError) = 0;
    };

class CGsmPosGpsMonitor : public CActive
	{
	public:
		static CGsmPosGpsMonitor* NewL(MGsmPosGpsMonitorObserver& aObserver);
		~CGsmPosGpsMonitor();
		
		void AddLandmarkL(const TDesC& aName, const TDesC& aDescription, const TLocality& aLocality);
		vector<TPosition>* ReadLandmarkL(const TPosition& aPosition, TUint aRadius);
		
		//CSattelliteInfoUI
		void ShowSatelliteInfoL();
		void HandleForegroundEventL(TBool aForeground);
		
	private:
		CGsmPosGpsMonitor(MGsmPosGpsMonitorObserver& aObserver);
		void ConstructL();

		void RequestPositionUpdate(TBool aCached);

	protected:
		// From CActive
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);
	
	private:
		MGsmPosGpsMonitorObserver& iObserver;
		TPositionIpcId iCancellation;

		RPositionServer iPosServer;
		RPositioner iPositioner;
		TPositionInfo iPosInfo;
		
		CPosLandmarkDatabase* iLandmarkDb;
		TPosLmItemId iLandmarkCategory;
		
		CSatelliteInfoUI* iSatelliteInfo;
	};

#endif // GSMPOSGPSMONITOR_H
