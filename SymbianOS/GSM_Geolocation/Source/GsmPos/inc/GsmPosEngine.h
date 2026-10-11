/*
 ============================================================================
 Name		: GsmPosEngine.h
 Author	  : WojciechK
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CGsmPosEngine declaration
 ============================================================================
 */

#ifndef GSMPOSENGINE_H
#define GSMPOSENGINE_H

#include <e32std.h>
#include <e32base.h>
#include <gdi.h>

#include <algorithm>
#include <functional>
#include <vector>
#include <map>
using namespace std;

#include "GsmPos.pan"
#include "GsmPosCellIdMonitor.h"
#include "GsmPosGpsMonitor.h"
#include "GsmPosDatabase.h"
#include "GsmPosBluetooth.h"

// Forward declarations
namespace geos {
	namespace geom { // geos::geom
		class Geometry;
		class GeometryFactory;
        class MultiPoint;
        class Polygon;
	}
}

class CGsmPosGoogleMap;
class CGsmPosMap;

//TODO  - kolejkuj CellId pomiedzy kolejnymi notyfikacjami od GPS, a potem zapisz cala serie

//TODO STL w TRAP (i kazde inne operacje na STL ktore moga rzucach wyjatki!)

class TNetworkInfoV1_equal : public binary_function<CTelephony::TNetworkInfoV1, CTelephony::TNetworkInfoV1,bool> 
	{
	public:
		bool operator() (const CTelephony::TNetworkInfoV1& aNetworkInfo1, const CTelephony::TNetworkInfoV1& aNetworkInfo2) const
			{
			//wybierz tylko istotne pola z pktu widzenia lokalizacji 	
			return (aNetworkInfo1.iCellId           == aNetworkInfo2.iCellId &&
					aNetworkInfo1.iLocationAreaCode == aNetworkInfo2.iLocationAreaCode &&
					aNetworkInfo1.iNetworkId        == aNetworkInfo2.iNetworkId &&
					aNetworkInfo1.iCountryCode      == aNetworkInfo2.iCountryCode);
			}
	};

class TNetworkInfoV1_not_equal : public binary_function<CTelephony::TNetworkInfoV1, CTelephony::TNetworkInfoV1,bool> 
	{
	public:
		bool operator() (const CTelephony::TNetworkInfoV1& aNetworkInfo1, const CTelephony::TNetworkInfoV1& aNetworkInfo2) const
			{
			//wybierz tylko istotne pola z pktu widzenia lokalizacji 	
			return (aNetworkInfo1.iCellId           != aNetworkInfo2.iCellId ||
					aNetworkInfo1.iLocationAreaCode != aNetworkInfo2.iLocationAreaCode ||
					aNetworkInfo1.iNetworkId        != aNetworkInfo2.iNetworkId ||
					aNetworkInfo1.iCountryCode      != aNetworkInfo2.iCountryCode);
			}
	};

class CGsmPosEngine : 	public CBase,
						public MGsmPosCellIdMonitorObserver,
						public MGsmPosGpsMonitorObserver,
						public MGsmPosBluetoothObserver
	{
	public:
		static CGsmPosEngine* NewL();
		~CGsmPosEngine();

        CGsmPosGpsMonitor* GpsMonitor();
        CGsmPosDatabase* Database();
        CGsmPosBluetooth* Bluetooth();
        const TPtrC BluetoothRemoteName();
        CGsmPosGoogleMap* GoogleMap();
        CGsmPosMap* Map();
        
        TBool NetworkInfo(const TBuf<CTelephony::KNetworkShortNameSize>& aShortName, CTelephony::TNetworkInfoV1& aNetworkInfo);
        void UpdateNetworkInfoL(const CTelephony::TNetworkInfoV1& aNetworkInfo);
        
        void NetworkInfo(TBool aOnlyValid, vector<CTelephony::TNetworkInfoV1>& aNetworkInfos) const;
        TBool Position(TLocality& aLocality) const;
        
        //TODO - z GEOS?
        //oblicz aRectangle (kolejnosc ul-ur-br-bl opisujacy aCoordinates, dodatkowo zwroc graniczne aPoints 
        void GeomComputeBoundingRect(const vector<TPosition>& aCoordinates, vector<TPosition>& aRectangle, vector<TPosition>& aPoints);
        
        //TODO - do private:
        geos::geom::MultiPoint* GeomConvertTPosition2MultiPointL(geos::geom::GeometryFactory& aGeomFactory, const vector<TPosition>& aCoords);
        geos::geom::Polygon*    GeomConvertTPosition2PolygonL(geos::geom::GeometryFactory& aGeomFactory, const vector<TPosition>& aCoords);
        void 					GeomConvertGeometry2TPosition(const geos::geom::Geometry* aGeometry, vector<TPosition>& aCoords);
        
        //oblicz otoczke wypukla dla koordynatow geograficznych
        void GeomComputeConvexHullL(const vector<TPosition>& aCoords, vector<TPosition>& aPolygon);
        
        //oblicz czesc wspolna dla otoczek wypuklych, zwroc centroid i wielokat (jesli aPolygon != NULL)
        TPosition* GeomComputeIntersectionL(const vector<const vector<TPosition>*>& aConvexHulls, vector<vector<TPosition>*>* aPolygon, TDes* aDescription = NULL);
        
        //oblicz srodek wielokata, zwrocona TPosition jako HorizontalAccuracy() ma ustawione odleglosc od najdalszego wierzcholka
        TPosition* GeomComputeCentroidL(geos::geom::GeometryFactory& aGeomFactory, const geos::geom::Geometry* aGeometry);
        
        //oblicz N-KAT foremny jako przyblizenie elipsy o srodku (aX,aY) i promieniach aRadiusX,aRadiusY
        vector<TPosition>* GeomComputeGeoCircleL(const TReal& aX, const TReal& aY, const TReal& aRadiusX, const TReal& aRadiusY);
        
        //unikalny kolor dla CellId danego operatora GSM w formacie 32bit Symbian
        TRgb GetColorForCellId(const CTelephony::TNetworkInfoV1& aNetworkInfo);
        
        //unikalny kolor dla CellId danego operatora GSM w formacie 24bit dla GoogleMaps
        TUint32 GetColorForCellId24bit(const CTelephony::TNetworkInfoV1& aNetworkInfo);
        
		//MGsmPosCellIdMonitorObserver
		void CellIdChangeL(const CTelephony::TNetworkInfoV1& aNetworkInfo);
		void CellIdChangeErrorL(TInt aError);
		
		//MGsmPosBluetoothObserver
        void RemoteCellIdChange(const TPckgBuf<CTelephony::TNetworkInfoV1>& aNetworkInfo, const TDesC& aRemoteName);
        void GetLocalCellId(TPckgBuf<CTelephony::TNetworkInfoV1>& aNetworkInfo);

		//MGsmPosGpsMonitorObserver
        //TODO - leave handling
        void GpsChange(const TPositionInfo& aPositionInfo, TBool aCached);
        void GpsChangeError(TInt aError);
        
	private:
		CGsmPosEngine();
		void ConstructL();
		
		//metoda dla statystyki (kolekcjonowanie wynikow dla Excel) - oblicza aktualna pozycje i zapisuje w pliku Log
		void ComputeCurrentPositionAndWriteToLogL();
			
	private:
		//monitorowanie zmian sieci GSM
		CGsmPosCellIdMonitor* iCellIdMonitor;
		TBool iCellIdDataValid;
		CTelephony::TNetworkInfoV1 iNetworkInfo;	//lokalny operator (tj. SIM w terminalu z GPS)
		
		//kazdy operator (tj. lokalny i zdalny (pozyskany przez BT)
		//TODO rozwaz, bo klucz TBuf<CTelephony::KNetworkShortNameSize> nie jest optymalny (wyklucza inne terminale tego samego operatora z handoverem, ktore moglyby zapewnic detekcje handovera)
		map<TBuf<CTelephony::KNetworkShortNameSize>, CTelephony::TNetworkInfoV1> iNetworkInfos;
		TBool iNetworkInfosFlag;
		
		//monitorowanie pozycji GPS
		//TODO - TLocality
		CGsmPosGpsMonitor* iGpsMonitor;
		TBool iGpsDataValid;
		TPosition iPosition;

		//baza danych 
		CGsmPosDatabase* iDatabase;
		
		//Google Maps
		CGsmPosGoogleMap* iGoogleMap;
		
		//wlasna mapa (kontury i punkty)
		CGsmPosMap* iMap;
		
		//Bluetooth
		CGsmPosBluetooth* iBluetooth;
		TBuf<32> iBluetoothLastRemoteName;
	};

#endif // GSMPOSENGINE_H
