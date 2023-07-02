// RG Bremen.h: Hauptheaderdatei für die RG Bremen-DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

//#include "resource.h"		// Hauptsymbole
#include "RG Bremen Plugin.h"


// CRGBremenApp
// Informationen zur Implementierung dieser Klasse finden Sie unter RG Bremen.cpp.
//

class CRGBremenApp : public CWinApp
{
public:
	CRGBremenApp();

// Überschreibungen
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
