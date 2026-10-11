/*
 ============================================================================
 Name		: GsmPosEngine.cpp
 Author	  : WojciechK
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CGsmPosEngine implementation
 ============================================================================
 */

#include "GsmPosEngine.h"
#include "GsmPosLogger.h"
#include "GsmPosGoogleMap.h"
#include "GsmPosMap.h"

#include <aknnotewrappers.h> 

// geos
#include <geos.h>
#include <geos/algorithm/ConvexHull.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/Dimension.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/io/WKTReader.h>
#include <geos/util/GEOSException.h>

CGsmPosEngine::CGsmPosEngine()
	{
	}

CGsmPosEngine::~CGsmPosEngine()
	{
	LOG_FUNCTION;
	
	delete iCellIdMonitor;
	
	delete iGpsMonitor;
	
	delete iDatabase;
	
	delete iBluetooth;

	delete iGoogleMap;
	
	delete iMap;
	}

CGsmPosEngine* CGsmPosEngine::NewL()
	{
	CGsmPosEngine* self = new (ELeave) CGsmPosEngine();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CGsmPosEngine::ConstructL()
	{
	LOG_FUNCTION_LEAVE;

	iCellIdMonitor = CGsmPosCellIdMonitor::NewL(*this);
	
	TRAPD (err,
			iGpsMonitor = CGsmPosGpsMonitor::NewL(*this);
		);
	
	if (err != KErrNone)
		{
		LOG_ERROR(_L("inicjalizacja GPS, kod %d"), err);
		}
	
	iDatabase = CGsmPosDatabase::NewL();
	
	iBluetooth = CGsmPosBluetooth::NewL(*this);
	iBluetooth->StartReceivingBroadcastsL();
	
	iGoogleMap = CGsmPosGoogleMap::NewL(*this);
	
	iMap = CGsmPosMap::NewL(*this);
	}

void CGsmPosEngine::CellIdChangeL(const CTelephony::TNetworkInfoV1& aNetworkInfo)
	{
	LOG_FUNCTION;
	
	LOG(_L("iStatus: %d / iAreaKnown: %d / iAccess: %d"), aNetworkInfo.iStatus, aNetworkInfo.iAreaKnown, aNetworkInfo.iAccess);

	//zapamietaj aktualne
	UpdateNetworkInfoL(aNetworkInfo);

	if (aNetworkInfo.iStatus == CTelephony::ENetworkStatusCurrent && aNetworkInfo.iAreaKnown)
		{
		iCellIdDataValid = ETrue;
		iNetworkInfo = aNetworkInfo;
		
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
		iNetworkInfo.iStatus = CTelephony::ENetworkStatusUnknown;
		iNetworkInfo.iAreaKnown = EFalse;
		
		//TODO - uaktualnij UI (pozostaw ostatnia mape, zmien kolory itp?)
		//TODO - publikuj CellId&koordynaty przez BT
		
		//TODO - teksty z RSC, UI poza ta klasa
		CAknWarningNote* note = new(ELeave) CAknWarningNote;
		note->SetTimeout(CAknNoteDialog::EShortestTimeout);
		note->ExecuteLD(_L("CellId nieznane"));		
		}
	}

void CGsmPosEngine::CellIdChangeErrorL(TInt aError)
	{
	LOG(_L("CGsmPosEngine::CellIdChangeErrorL(%d)"), aError);

	LOG_ERROR(_L("CellId nieznane!"));
	
	iCellIdDataValid = EFalse;
	iNetworkInfo.iStatus = CTelephony::ENetworkStatusUnknown;
	iNetworkInfo.iAreaKnown = EFalse;
	
	UpdateNetworkInfoL(iNetworkInfo);
	
	//TODO - teksty z RSC, UI poza ta klasa
	CAknErrorNote* note = new(ELeave) CAknErrorNote;
	TBuf<128> buf;
	buf.Format(_L("CellId: (%d)"), aError);
	note->SetTimeout(CAknNoteDialog::EShortestTimeout);
	note->ExecuteLD(buf);		
	}

//TODO - dodac rozwiazywanie MNC do nazw krajowych operatorow?
//TODO - metoda Leave
void CGsmPosEngine::RemoteCellIdChange(const TPckgBuf<CTelephony::TNetworkInfoV1>& aNetworkInfo, const TDesC& aRemoteName)
	{
	LOG_FUNCTION;
	
	CTelephony::TNetworkInfoV1 info = aNetworkInfo();

	//TODO disable dump
	iCellIdMonitor->DumpNetworkInfo(info);
	
	//zapamietaj aktualne
	UpdateNetworkInfoL(info);
	
	if (info.iStatus == CTelephony::ENetworkStatusCurrent && info.iAreaKnown)
		{
		//zapamietaj MAC address klienta
		iBluetoothLastRemoteName = aRemoteName.Left(iBluetoothLastRemoteName.MaxLength());
		
		//powiadom uzytkownika
		CAknInformationNote* note = new(ELeave) CAknInformationNote(ETrue);
		TBuf<128> buf;
		buf.Format(_L("Remote CellId: %S/%u/%u"), &info.iShortName, info.iLocationAreaCode, info.iCellId);
		note->SetTimeout(CAknNoteDialog::EShortestTimeout);
		note->ExecuteLD(buf);		
		}
	else
		{
		//usun MAC address klienta
		iBluetoothLastRemoteName = KNullDesC;
		
		CAknWarningNote* note = new(ELeave) CAknWarningNote(ETrue);
		TBuf<128> buf;
		buf.Format(_L("Remote CellId unknown: %S"), &info.iShortName);
		note->SetTimeout(CAknNoteDialog::EShortestTimeout);
		note->ExecuteLD(buf);
		}
	}

void CGsmPosEngine::GetLocalCellId(TPckgBuf<CTelephony::TNetworkInfoV1>& aNetworkInfo)
	{
	LOG_FUNCTION;
	
	aNetworkInfo = TPckgBuf<CTelephony::TNetworkInfoV1>(iNetworkInfo);
	}

//TODO - po przetestowaniu dodaj do GpsChange
void CGsmPosEngine::NetworkInfo(TBool aOnlyValid, vector<CTelephony::TNetworkInfoV1>& aNetworkInfos) const
	{
	map<TBuf<CTelephony::KNetworkShortNameSize>, CTelephony::TNetworkInfoV1>::const_iterator iter_map;
	for (iter_map = iNetworkInfos.begin(); iter_map != iNetworkInfos.end(); iter_map++)
		{
		CTelephony::TNetworkInfoV1 info = iter_map->second;
		if (!aOnlyValid || (info.iStatus == CTelephony::ENetworkStatusCurrent && info.iAreaKnown))
			{
			aNetworkInfos.push_back(info);
			}
		}
	}

//TODO - potrzebna?
TBool CGsmPosEngine::NetworkInfo(const TBuf<CTelephony::KNetworkShortNameSize>& aShortName, CTelephony::TNetworkInfoV1& aNetworkInfo)
	{
	map<TBuf<CTelephony::KNetworkShortNameSize>, CTelephony::TNetworkInfoV1>::iterator iter;
	if ( (iter = iNetworkInfos.find(aShortName)) != iNetworkInfos.end() )
		{
		aNetworkInfo = iter->second;
		return ETrue;
		}
	return EFalse;
	}

//TODO - STL exceptions
void CGsmPosEngine::UpdateNetworkInfoL(const CTelephony::TNetworkInfoV1& aNetworkInfo)
	{
	LOG_FUNCTION_LEAVE;
	
	map<TBuf<CTelephony::KNetworkShortNameSize>, CTelephony::TNetworkInfoV1>::iterator iter;
	if ( (iter = iNetworkInfos.find(aNetworkInfo.iShortName)) == iNetworkInfos.end() )
		{
		LOG(_L("map insert")); 	//TODO disable
		
		pair<TBuf<CTelephony::KNetworkShortNameSize>, CTelephony::TNetworkInfoV1> element(aNetworkInfo.iShortName, aNetworkInfo);
		pair<map<TBuf<CTelephony::KNetworkShortNameSize>, CTelephony::TNetworkInfoV1>::iterator,bool> ret;
		ret = iNetworkInfos.insert(element);
		if (!ret.second)
			{
			LOG_ERROR(_L("map insert failed")); 	//TODO disable
			
			User::Leave(KErrGeneral);
			}
		iNetworkInfosFlag = ETrue;
		}
	else
		{
		LOG(_L("map update")); 	//TODO disable
		
		iter->second = aNetworkInfo;
		iNetworkInfosFlag = ETrue;
		}
	}

TBool CGsmPosEngine::Position(TLocality& aLocality) const
	{
	aLocality = TLocality(TCoordinate(iPosition.Latitude(), iPosition.Longitude()), iPosition.HorizontalAccuracy());
	return iGpsDataValid;
	}

CGsmPosGpsMonitor* CGsmPosEngine::GpsMonitor()
	{
	return iGpsMonitor;
	}

CGsmPosDatabase* CGsmPosEngine::Database()
	{
	return iDatabase;
	}

CGsmPosBluetooth* CGsmPosEngine::Bluetooth()
	{
	return iBluetooth;
	}

const TPtrC CGsmPosEngine::BluetoothRemoteName()
	{
	return iBluetoothLastRemoteName;
	}

CGsmPosGoogleMap* CGsmPosEngine::GoogleMap()
	{
	return iGoogleMap;
	}

CGsmPosMap* CGsmPosEngine::Map()
	{
	return iMap;
	}

//TODO - metoda Leave
void CGsmPosEngine::GpsChange(const TPositionInfo& aPositionInfo, TBool aCached)
	{
	LOG_FUNCTION;
	
	TPosition pos;
	aPositionInfo.GetPosition(pos);

	TReal32 hor_acc = pos.HorizontalAccuracy();
	TReal64 latitude = pos.Latitude();
	TReal64 longitude = pos.Longitude();
	TTime time = pos.Time();
	
	if (!Math::IsNaN(latitude) && !Math::IsNaN(longitude))
		{
		//dodaj do DB kazde zmiany w CellId lub zmierz dystans od poprzedniej pozycji i wtedy dodaj
		TReal32 distance;
		if ( iNetworkInfosFlag || (pos.Distance(TCoordinate(iPosition.Latitude(), iPosition.Longitude()),distance) == KErrNone) )
			{
			const TReal32 KGpsLastPositionMinimalDistance = 10.0f;		//w metrach
			if ( iNetworkInfosFlag || distance > KGpsLastPositionMinimalDistance )
				{
				//TODO formatowanie bezpieczne jesli IsNaN(hor_acc)?
				LOG(_L("\tlat: %f\t\tlong: %f\t\thor_acc: %f\t\tdist: %f"), latitude, longitude, hor_acc, distance);
				
				//zapamietaj nowa pozycje
				iPosition = pos;

				//orientacyjnej pozycji z cache  (albo o zbyt duzej niedokladnosci) uzyj tylko do kreslenia map a nie do tworzenia map pokrycia!
				if (Math::IsNaN(hor_acc) || (hor_acc > 10 * KGpsLastPositionMinimalDistance) )
					{
					LOG(_L("\tignoruj! horizontal accuracy > %d m"), (TInt)(10 * KGpsLastPositionMinimalDistance));
					
					iGpsDataValid = EFalse;
					return;
					}
				
				if (aCached)
					{
					LOG(_L("ignoruj! cached position"));
					
					iGpsDataValid = EFalse;
					return;
					}
				
				//potwierdz waznosc pozycji
				iGpsDataValid = ETrue;
				
				//TODO STL exception
				//do DB dodaj tylko znane CellId
				vector<CTelephony::TNetworkInfoV1> valid_cells;
				map<TBuf<CTelephony::KNetworkShortNameSize>, CTelephony::TNetworkInfoV1>::iterator iter_map;
				for (iter_map = iNetworkInfos.begin(); iter_map != iNetworkInfos.end(); iter_map++)
					{
					CTelephony::TNetworkInfoV1 info = iter_map->second;
					if (info.iStatus == CTelephony::ENetworkStatusCurrent && info.iAreaKnown)		//TODO refactor (oddzielna metoda do walidacji - wiele miejsc w kodzie)
						{
						valid_cells.push_back(info);
						}
					}
				if (!valid_cells.empty())
					{
					#ifdef __WINS__
					//pomin emulator
					TInt err = KErrNone;
					#else
					//do DB dodaj tylko prawdziwe dane z device
					TInt err = iDatabase->Add(valid_cells, aPositionInfo);
					#endif
					if (err != KErrNone)
						{
						//TODO - teksty z RSC, UI poza ta klasa
						CAknErrorNote* note = new(ELeave) CAknErrorNote;
						TBuf<128> buf;
						buf.Format(_L("Database: (%d)"), err);
						note->ExecuteLD(buf);		

						return;
						}
					else
						{
						iNetworkInfosFlag = EFalse;
						}

					//dialog na ekran z informacja o aktualnych CellId
					_LIT(KLandmarkDialogFormatString, "%S %u %u \n");
					const TInt KLandmarkDialogFormatStringMaxLength = KLandmarkDialogFormatString().Length() + CTelephony::KNetworkShortNameSize + 32;
					
					RBuf buf;
					CleanupClosePushL(buf);
					buf.CreateL(KLandmarkDialogFormatStringMaxLength);

					RBuf buf_note;
					CleanupClosePushL(buf_note);
					buf_note.CreateL(KLandmarkDialogFormatStringMaxLength * valid_cells.size());
					
					for(TInt i = 0; i < valid_cells.size(); i++)
						{
						CTelephony::TNetworkInfoV1 info = valid_cells[i];
						buf.Format(KLandmarkDialogFormatString(), &info.iShortName, info.iLocationAreaCode, info.iCellId);
						buf_note.Append(buf);
						}
					
					//TODO UI poza ta klasa ?
					CAknConfirmationNote* note = new(ELeave) CAknConfirmationNote;
					note->ExecuteLD(buf_note);
					CleanupStack::PopAndDestroy(&buf_note);
					CleanupStack::PopAndDestroy(&buf);
					
					//zapisz do log pozycje wyliczona przez aplikacje
					ComputeCurrentPositionAndWriteToLogL();
					}
				}
			else
				{
				LOG(_L("\tignoruj! dystans: %f < min krok (%f)"), distance, KGpsLastPositionMinimalDistance);
				}
			}
		}
	}

void CGsmPosEngine::GpsChangeError(TInt aError)
	{
	LOG(_L("CGsmPosEngine::GpsChangeError(%d)"), aError);
	
	iGpsDataValid = EFalse;
	
	//TODO - teksty z RSC, UI poza ta klasa
	CAknErrorNote* note = new(ELeave) CAknErrorNote;
	TBuf<128> buf;
	buf.Format(_L("Gps: (%d)"), aError);
	note->SetTimeout(CAknNoteDialog::EShortTimeout);
	note->ExecuteLD(buf);		
	}

TRgb CGsmPosEngine::GetColorForCellId(const CTelephony::TNetworkInfoV1& aNetworkInfo)
	{
	if (aNetworkInfo.iShortName.CompareF(KOperatorGsmEra) == 0)
		return KRgbBlue;
	if (aNetworkInfo.iShortName.CompareF(KOperatorGsmPlus) == 0)
		return KRgbGreen;
	if (aNetworkInfo.iShortName.CompareF(KOperatorGsmOrange) == 0)
		return TRgb(0xff,0xa5,0x00);

	return KRgbBlack;
	}

TUint32 CGsmPosEngine::GetColorForCellId24bit(const CTelephony::TNetworkInfoV1& aNetworkInfo)
	{
	if (aNetworkInfo.iShortName.CompareF(KOperatorGsmEra) == 0)
		return 0x0000FF;
	if (aNetworkInfo.iShortName.CompareF(KOperatorGsmPlus) == 0)
		return 0x00FF00;
	if (aNetworkInfo.iShortName.CompareF(KOperatorGsmOrange) == 0)
		return 0xFFA500;

	return 0x000000;
	}

//TODO - FIXME, ten algorytm dziala tylko dla NE polkuli!
//TODO - przypadki gdy mniej niz 4 wezly!?
//TODO - docelowo (inne algorytmy) nie licz na koordynatach a na floatach
//TODO - czy na prace algorytmow ma wplyw to ze to sa dane na sferze a nie na plaszczyznie (patrz: haversine)
//TODO - zwroc upper_left/bottm_right zamiast aRectangle
void CGsmPosEngine::GeomComputeBoundingRect(const vector<TPosition>& aCoordinates, vector<TPosition>& aRectangle, vector<TPosition>& aPoints)
	{
	//LOG_FUNCTION;

	TPosition up, down, left, right;
	TPosition upper_left, bottom_right;
	
	aRectangle.clear();
	aPoints.clear();
	
	for (TInt i = 0; i < aCoordinates.size(); i++)
		{
		const TPosition& coord = aCoordinates[i];
		
		//LOG(_L("\t%f %f"), coord.Latitude(), coord.Longitude());
		
		if (Math::IsNaN(up.Latitude()) || up.Latitude() > coord.Latitude())
			{
			up.SetCoordinate(coord.Latitude(), coord.Longitude());
			upper_left.SetCoordinate(coord.Latitude(), upper_left.Longitude());
			}

		if (Math::IsNaN(down.Latitude()) || down.Latitude() < coord.Latitude())
			{
			down.SetCoordinate(coord.Latitude(), coord.Longitude());
			bottom_right.SetCoordinate(coord.Latitude(), bottom_right.Longitude());
			}

		if (Math::IsNaN(left.Longitude()) || left.Longitude() > coord.Longitude())
			{
			left.SetCoordinate(coord.Latitude(), coord.Longitude());
			upper_left.SetCoordinate(upper_left.Latitude(), coord.Longitude());
			}

		if (Math::IsNaN(right.Longitude()) || right.Longitude() < coord.Longitude())
			{
			right.SetCoordinate(coord.Latitude(), coord.Longitude());
			bottom_right.SetCoordinate(bottom_right.Latitude(), coord.Longitude());
			}
		}
	
	//LOG(_L("upper_left(%f , %f)\tbottom_right(%f , %f)"), upper_left.Latitude(), upper_left.Longitude(), bottom_right.Latitude(), bottom_right.Longitude());
	
	//LOG(_L("up(%f , %f)"), up.Latitude(), up.Longitude());
	//LOG(_L("down(%f , %f)"), down.Latitude(), down.Longitude());
	//LOG(_L("left(%f , %f)"), left.Latitude(), left.Longitude());
	//LOG(_L("right(%f , %f)"), right.Latitude(), right.Longitude());
	
	//TODO STL exceptions
	aPoints.push_back(up);
	aPoints.push_back(right);
	aPoints.push_back(down);
	aPoints.push_back(left);
	
	TPosition point;
	point.SetCoordinate(upper_left.Latitude(), upper_left.Longitude());
	aRectangle.push_back(point);
	point.SetCoordinate(upper_left.Latitude(), bottom_right.Longitude());
	aRectangle.push_back(point);
	point.SetCoordinate(bottom_right.Latitude(), bottom_right.Longitude());
	aRectangle.push_back(point);
	point.SetCoordinate(bottom_right.Latitude(), upper_left.Longitude());
	aRectangle.push_back(point);
	}

//TODO - potrzebne? ewentualne wykorzystanie auto_ptr/cleanupstack przy Geos
/*
typedef std::auto_ptr<geos::geom::Geometry> GeometryAPtr;
//from geos\tests\utility.h
template <typename Dst, typename Src>
std::auto_ptr<Dst> dynamic_cast_auto_ptr(std::auto_ptr<Src>& ptr)
{
    Dst* res = dynamic_cast<Dst*>(ptr.get());
    if (0 != res)
    {
        ptr.release();
    }
    return std::auto_ptr<Dst>(res);
}
*/

void CGsmPosEngine::GeomComputeConvexHullL(const vector<TPosition>& aCoords, vector<TPosition>& aPolygon)
	{
	LOG_FUNCTION;

    geos::geom::PrecisionModel geom_precision_model;	/// Creates a PrecisionModel with a default precision of FLOATING.
    geos::geom::GeometryFactory geom_factory(&geom_precision_model,0);

    geos::geom::Geometry* geom_input = GeomConvertTPosition2MultiPointL(geom_factory, aCoords);

    /* FIXME
    {
    std::string test_str = geom_input->toString();
    const char* test_str_c = test_str.c_str();
    LOG_DUMP(TPtrC8((const TUint8*)test_str_c));
    }
    */
    
    //oblicz otoczke wypukla
    geos::geom::Geometry* geom_output = geom_input->convexHull();
    
    /* FIXME
    Well Known Text (WKT) otoczki wypuklej
    {
    std::string test_str = geom_output->toString();
    const char* test_str_c = test_str.c_str();
    LOG_DUMP(TPtrC8((const TUint8*)test_str_c));
    }
    */
    
    //zwroc koordynaty
   	GeomConvertGeometry2TPosition(geom_output, aPolygon);

    //cleanup
    geom_factory.destroyGeometry(geom_input);
    geom_factory.destroyGeometry(geom_output);
	}

/* 
 * TODO czy jest mozliwe ze bedzie wiecej niz 1? jak to obsluzyc?
 * 
 * TODO - traktuj 1szy element jako HomeNetwork (A), nast 2 jako B i C (reszte ignoruj)
 *        nastepnie oblicz  AB=A*B, i AC=A*C 
 *        nastepnie oblicz  ABC=AB*AC
 *        a) jesli niepusty: zwroc ABC, to jest najlepszy wynik
 *        b) jesli pusty... zwroc kazdy niepusty: 
 *        		- AB i/lub AC
 *        c) jesli AB i AC puste -> zwroc A (zwroc caly HomeNetwork, to jest najgorszy wynik)
       
 * TODO - GEOS exception handling
 * TODO - STL eception handling
 * TODO - Centroid - nierowzwiazana sytuacja gdy zwraca 2 rozlaczne obszary AB i AC
 * TODO - centroid - obliczanie odleglosci od najdalszego wierzcholka
 */
TPosition* CGsmPosEngine::GeomComputeIntersectionL(const vector<const vector<TPosition>*>& aConvexHulls, vector<vector<TPosition>*>* aPolygon, TDes* aDescription)
	{
	LOG(_L("CGsmPosEngine::GeomComputeIntersectionL(), aConvexHulls.size() = %d"), aConvexHulls.size());

    geos::geom::PrecisionModel geom_precision_model;	/// Creates a PrecisionModel with a default precision of FLOATING.
    geos::geom::GeometryFactory geom_factory(&geom_precision_model,0);

    geos::geom::Geometry* geom_a = NULL;
    geos::geom::Geometry* geom_b = NULL;
    geos::geom::Geometry* geom_c = NULL;

    geos::geom::Geometry* geom_ab = NULL;
    geos::geom::Geometry* geom_ac = NULL;
    geos::geom::Geometry* geom_abc = NULL;
    
    TPosition* centroid = NULL;
    TPtrC description;
    try
		{
		//TODO - czy trzeba liczyc ConvexHull dla Geometry::intersection?... chyba tak, zastap wiec oddzielna metoda
		if (aConvexHulls.size() >= 1)
			geom_a = GeomConvertTPosition2PolygonL(geom_factory, *(aConvexHulls[0]));
		if (aConvexHulls.size() >= 2)
			{
			geom_b = GeomConvertTPosition2PolygonL(geom_factory, *(aConvexHulls[1]));
			geom_ab = geom_a->intersection(geom_b);
			}
		if (aConvexHulls.size() >= 3)
			{
			geom_c = GeomConvertTPosition2PolygonL(geom_factory, *(aConvexHulls[2]));
			geom_ac = geom_a->intersection(geom_c);
			if (!geom_ab->isEmpty() && !geom_ac->isEmpty())
				geom_abc = geom_ab->intersection(geom_ac);
			}
		if (aConvexHulls.size() >= 4)
			{
			LOG_ERROR(_L("CGsmPosEngine::GeomComputeIntersectionL(), only 3 polygons supported, ignore others"));
			}

		if (geom_abc && !geom_abc->isEmpty())
			{
			description.Set(_L("A^B^C"));
			
			if (aPolygon)
				{
				vector<TPosition>* intersection = new (ELeave) vector<TPosition>(); 
				GeomConvertGeometry2TPosition(geom_abc, *intersection);
				aPolygon->push_back(intersection);		//transfer ownership
				}
			
			centroid = GeomComputeCentroidL(geom_factory, geom_abc);
			}
		else
			if ( (geom_ab && !geom_ab->isEmpty()) || (geom_ac && !geom_ac->isEmpty()) )
				{
				if (geom_ab && !geom_ab->isEmpty())
					{
					description.Set(_L("A^B"));
					
					if (aPolygon)
						{
						vector<TPosition>* intersection = new (ELeave) vector<TPosition>(); 
						GeomConvertGeometry2TPosition(geom_ab, *intersection);
						aPolygon->push_back(intersection);		//transfer ownership
						}
					
					centroid = GeomComputeCentroidL(geom_factory, geom_ab);
					}
				if (geom_ac && !geom_ac->isEmpty())
					{
					description.Set(_L("A^C"));
					
					if (aPolygon)
						{
						vector<TPosition>* intersection = new (ELeave) vector<TPosition>(); 
						GeomConvertGeometry2TPosition(geom_ac, *intersection);
						aPolygon->push_back(intersection);		//transfer ownership
						}
					
					if (centroid == NULL)
						{
						centroid = GeomComputeCentroidL(geom_factory, geom_ac);
						}
					else
						{
						description.Set(_L("A^B ? A^C"));
						
						delete centroid;
						centroid = NULL;
						}
					}
				}
		else
			{
			description.Set(_L("A"));
			
			if (aPolygon)
				{
				vector<TPosition>* intersection = new (ELeave) vector<TPosition>(); 
				GeomConvertGeometry2TPosition(geom_a, *intersection);
				aPolygon->push_back(intersection);		//transfer ownership
				}
			
			centroid = GeomComputeCentroidL(geom_factory, geom_a);
			}
		}	

	catch (geos::util::GEOSException& e)
		{
		LOG_ERROR(_L8("GEOSException: %s"), e.what());
		LOG_INFOPRINT(_L("GEOSException"));
		}

	LOG(description);
	
	if (aDescription)
		aDescription->Copy(description);
	
    //cleanup
   	geom_factory.destroyGeometry(geom_a);
   	geom_factory.destroyGeometry(geom_b);
   	geom_factory.destroyGeometry(geom_c);
   	geom_factory.destroyGeometry(geom_ab);
   	geom_factory.destroyGeometry(geom_ac);
   	geom_factory.destroyGeometry(geom_abc);
   	
   	return centroid;
	}

//TODO symbian leave
TPosition* CGsmPosEngine::GeomComputeCentroidL(geos::geom::GeometryFactory& aGeomFactory, const geos::geom::Geometry* aGeometry)
	{
	LOG_FUNCTION;
	
	TPosition* position = NULL;
	if (aGeometry->getNumPoints())
		{
		geos::geom::Point* centroid = aGeometry->getCentroid();
		if (centroid)
			{
			const geos::geom::Coordinate* centroid_coord = centroid->getCoordinate();
			
			TReal64 latitude(centroid_coord->y), longitude(centroid_coord->x);
			position = new (ELeave) TPosition(TLocality(TCoordinate(latitude,longitude), 0), TTime(0));
			
			TReal max_dist = 0.0;
			TInt idx = -1;
			geos::geom::CoordinateSequence* seq = aGeometry->getCoordinates();
			for (TInt i = 0; i < seq->getSize(); i++)
				{
				TReal dist = seq->getAt(i).distance(*centroid_coord);
				if (dist > max_dist)
					{
					max_dist = dist;
					idx = i;
					}
				}
			if (idx >= 0)
				{
				TCoordinate max_dist_coord(seq->getAt(idx).y, seq->getAt(idx).x);
				TReal32 dist_in_meters = 0.0;
				if (position->Distance(max_dist_coord, dist_in_meters) == KErrNone)
					{
					LOG(_L("centroid:\tlat: %f\t\tlong: %f\t\taccuracy: %f"), latitude, longitude, dist_in_meters);
					
					position->SetHorizontalAccuracy(dist_in_meters);
					}
				}
			delete seq;
			aGeomFactory.destroyGeometry(centroid);
			}
		}
	return position;
	}

void CGsmPosEngine::GeomConvertGeometry2TPosition(const geos::geom::Geometry* aGeometry, vector<TPosition>& aCoords)
	{
	if (aGeometry->getNumPoints())
		{
		aCoords.reserve(aGeometry->getNumPoints());
		geos::geom::CoordinateSequence* seq = aGeometry->getCoordinates();			//TODO auto_ptr
		for (TInt i = 0; i < seq->getSize(); i++)
			{
			TReal64 latitude(seq->getY(i)), longitude(seq->getX(i));
			aCoords.push_back(TPosition(TLocality(TCoordinate(latitude,longitude), 0), TTime(0)));
			}
		delete seq;
		}
	}

geos::geom::Polygon* CGsmPosEngine::GeomConvertTPosition2PolygonL(geos::geom::GeometryFactory& aGeomFactory, const vector<TPosition>& aCoords)
	{
	vector<geos::geom::Coordinate>* coords = new (ELeave) vector<geos::geom::Coordinate>();
	coords->reserve(aCoords.size());
	for (TInt i = 0; i < aCoords.size(); i++)
		{
		coords->push_back(geos::geom::Coordinate(aCoords[i].Longitude(),aCoords[i].Latitude()));
		}
	const geos::geom::CoordinateSequenceFactory* seq_factory = aGeomFactory.getCoordinateSequenceFactory();	
	geos::geom::CoordinateSequence* seq = seq_factory->create(coords);			//takes ownership
	geos::geom::LinearRing* ring = aGeomFactory.createLinearRing(seq);			//takes ownership
	geos::geom::Polygon* polygon = aGeomFactory.createPolygon(ring, NULL);		//takes ownership
	return polygon;
	}

//TODO Symbian cleanupstack (cleanupdelete itp)
geos::geom::MultiPoint* CGsmPosEngine::GeomConvertTPosition2MultiPointL(geos::geom::GeometryFactory& aGeomFactory, const vector<TPosition>& aCoords)
	{
	vector<geos::geom::Coordinate>* coords = new (ELeave) vector<geos::geom::Coordinate>();
	coords->reserve(aCoords.size());
	for (TInt i = 0; i < aCoords.size(); i++)
		{
		coords->push_back(geos::geom::Coordinate(aCoords[i].Longitude(),aCoords[i].Latitude()));
		}
	const geos::geom::CoordinateSequenceFactory* seq_factory = aGeomFactory.getCoordinateSequenceFactory();	
	geos::geom::CoordinateSequence* seq = seq_factory->create(coords);			//takes ownership
	geos::geom::MultiPoint* multipoint = aGeomFactory.createMultiPoint(*seq);
	delete seq;
	return multipoint;
	}

//TODO STL/Symbian exception handling
vector<TPosition>* CGsmPosEngine::GeomComputeGeoCircleL(const TReal& aX, const TReal& aY, const TReal& aRadiusX, const TReal& aRadiusY)
	{
	LOG_FUNCTION;
	
	const TInt N_GONS = 16; 					//TODO - dopasuj ilosc krawedzi aby przypominalo okrag
	
	vector<TPosition>* circle = new (ELeave) vector<TPosition>();
	circle->reserve(N_GONS + 1);
	for (TInt i = 0; i < N_GONS; i++)
		{
		TReal angle = ((TReal) i / (TReal) N_GONS) * 2.0 * 3.14159265358979323846;
		TReal dx,dy;
		Math::Cos(dx, angle);
		dx *= aRadiusX;
		Math::Sin(dy, angle);
		dy *= aRadiusY;
		circle->push_back(TPosition(TLocality(TCoordinate(aY + dy, aX + dx), 0), TTime(0)));
		}
	circle->push_back(circle->at(0));
	
	return circle;
	}

void CGsmPosEngine::ComputeCurrentPositionAndWriteToLogL()
	{
	LOG_FUNCTION_LEAVE;

	if (!iGpsDataValid)
		return;

	vector<CTelephony::TNetworkInfoV1> current_info;
	NetworkInfo(ETrue, current_info);
	
	if (current_info.empty())
		return;
	
	//ilosc terminali skoreluj z iloscia danych z DB (roznica bedzie wskazywac ze brak pokrycia radiowego w tym miejscu)
	TInt cellid_in_db = current_info.size();

	vector<const vector<TPosition>*> convex_hulls;
	for (TInt i = 0; i < current_info.size(); i++)
		{
		const CTelephony::TNetworkInfoV1 info = current_info[i];

		vector<TPosition> coords;
		TInt err = Database()->GetAllCoordinates(info, coords);
		if (err != KErrNone)
			{
			LOG_ERROR(_L("CGsmPosEngine::ComputeCurrentPositionAndWriteToLogL(), database error = %d"), err);

			for (TInt i = 0; i < convex_hulls.size(); i++)
				delete convex_hulls[i];
			return;
			}
		
		if (coords.empty())
			cellid_in_db--;
		else
			{
			vector<TPosition>* polygon = new (ELeave) vector<TPosition> ();
			GeomComputeConvexHullL(coords, *polygon);
			convex_hulls.push_back(polygon); //transfer ownership
			}
		}
	
	if (convex_hulls.empty())
		return;

	_LIT(KFormatCSV, "csv; %f; %f; %f; %f; %f; %f; %f; %S; %d; %d");
	
	TBuf<16> description;
	TPosition* pos = GeomComputeIntersectionL(convex_hulls, NULL, &description);
	if (pos)
		{
		TReal32 dist = 0.0f;
		(void) iPosition.Distance(*pos, dist);

		LOG(KFormatCSV, iPosition.Latitude(), iPosition.Longitude(), iPosition.HorizontalAccuracy(),
						pos->Latitude(), pos->Longitude(), pos->HorizontalAccuracy(), dist, &description, current_info.size(), cellid_in_db);
		delete pos;
		}
	else
		{
		LOG(KFormatCSV, iPosition.Latitude(), iPosition.Longitude(), iPosition.HorizontalAccuracy(),
						0.0f, 0.0f, 0.0f, 0.0f, &description, current_info.size(), cellid_in_db);
		}

	for (TInt i = 0; i < convex_hulls.size(); i++)
		delete convex_hulls[i];
	}
