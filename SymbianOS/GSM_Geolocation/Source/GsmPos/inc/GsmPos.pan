
#ifndef GSMPOS_PAN_H
#define GSMPOS_PAN_H

/** GsmPos application panic codes */
enum TGsmPosPanics
	{
	EGsmPosUi = 1,
	// add further panics here
	EGsmPosAssert,
	EGsmPosImproperApiUsage,
	EWebClientLeave				//wystapil leave w kodzie obslugi bledow 
	};

inline void Panic(TGsmPosPanics aReason)
	{
	_LIT(applicationName,"GsmPos");
	User::Panic(applicationName, aReason);
	}

#endif // GSMPOS_PAN_H
