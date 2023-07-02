// RG Bremen.cpp: Definiert die Initialisierungsroutinen für die DLL.
//

#include "pch.h"
#include "framework.h"
#include "RG Bremen.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: Wenn diese DLL dynamisch mit MFC-DLLs verknüpft ist,
//		muss für alle aus dieser DLL exportierten Funktionen, die in
//		MFC aufgerufen werden, das AFX_MANAGE_STATE-Makro
//		am Anfang der Funktion hinzugefügt werden.
//
//		Beispiel:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// Hier normaler Funktionsrumpf
//		}
//
//		Es ist sehr wichtig, dass dieses Makro in jeder Funktion
//		vor allen MFC-Aufrufen angezeigt wird.  Dies bedeutet,
//		dass es als erste Anweisung innerhalb der
//		Funktion angezeigt werden muss, sogar vor jeglichen Deklarationen von Objektvariablen,
//		da ihre Konstruktoren Aufrufe in die MFC-DLL generieren
//		könnten.
//
//		Siehe Technische Hinweise für MFC 33 und 58 für weitere
//		Details.
//

// CRGBremenApp

BEGIN_MESSAGE_MAP(CRGBremenApp, CWinApp)
END_MESSAGE_MAP()


// CRGBremenApp-Erstellung

CRGBremenApp::CRGBremenApp()
{
	// TODO: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance positionieren
}


// Das einzige CRGBremenApp-Objekt

CRGBremenApp theApp;


// CRGBremenApp-Initialisierung

BOOL CRGBremenApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

CRGBremenPlugin * gpMyPlugin = NULL;

void __declspec (dllexport) EuroScopePlugInInit(EuroScopePlugIn::CPlugIn ** ppPlugInInstance)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		* ppPlugInInstance = gpMyPlugin = new CRGBremenPlugin();
}

void __declspec(dllexport) EuroScopePlugInExit()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		delete gpMyPlugin;
}