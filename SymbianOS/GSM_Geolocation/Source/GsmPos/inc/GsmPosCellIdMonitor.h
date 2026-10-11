/*
 ============================================================================
 Name		: GsmPosCellIdMonitor.h
 Author	  : WojciechK
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CGsmPosCellIdMonitor declaration
 ============================================================================
 */

#ifndef GSMPOSCELLIDMONITOR_H
#define GSMPOSCELLIDMONITOR_H

#include <e32std.h>
#include <e32base.h>
#include <etel3rdparty.h>

_LIT(KOperatorGsmEra,   "Era");
_LIT(KOperatorGsmPlus,  "Plus");
_LIT(KOperatorGsmOrange,"Orange");

class MGsmPosCellIdMonitorObserver
    {
    public:
        virtual void CellIdChangeL(const CTelephony::TNetworkInfoV1& aNetworkInfo) = 0;
        virtual void CellIdChangeErrorL(TInt aError) = 0;
    };
 
class CGsmPosCellIdMonitor : public CActive
    {
    public:
        static CGsmPosCellIdMonitor* NewL(MGsmPosCellIdMonitorObserver& aObserver);
        ~CGsmPosCellIdMonitor();
 
        void DumpNetworkInfo(const CTelephony::TNetworkInfoV1& aNetworkInfo);
        
    protected:
    	CGsmPosCellIdMonitor(MGsmPosCellIdMonitorObserver& aObserver);
        void ConstructL();
        
        //CActive
        void RunL();
        TInt RunError(TInt aError);
        void DoCancel();
        
    private:
        CTelephony* iTelephony;
        CTelephony::TCancellationRequest iCancellation;
        CTelephony::TNetworkInfoV1 iNwInfo;
        CTelephony::TNetworkInfoV1Pckg iNwInfoPckg;
        MGsmPosCellIdMonitorObserver& iObserver;
    };
 
#endif // GSMPOSCELLIDMONITOR_H
