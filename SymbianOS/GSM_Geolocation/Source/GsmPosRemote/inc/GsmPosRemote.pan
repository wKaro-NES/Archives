
#ifndef GSMPOSREMOTE_PAN_H
#define GSMPOSREMOTE_PAN_H

/** GsmPosRemote application panic codes */
enum TGsmPosRemotePanics
	{
	EGsmPosRemoteUi = 1
	// add further panics here
	};

inline void Panic(TGsmPosRemotePanics aReason)
	{
	_LIT(applicationName,"GsmPosRemote");
	User::Panic(applicationName, aReason);
	}

#endif // GSMPOSREMOTE_PAN_H
