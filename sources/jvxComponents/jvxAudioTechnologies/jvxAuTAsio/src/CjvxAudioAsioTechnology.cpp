#include "CjvxAudioAsioTechnology.h"
#include "asio.h"
#include "asiolist.h"

// =========================================================================================

#define ASIODRV_DESC		"description"
#define INPROC_SERVER		"InprocServer32"
#define ASIO_PATH			"software\\asio"
#define COM_CLSID			"clsid"

// =========================================================================================

CjvxAudioAsioTechnology::CjvxAudioAsioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxAudioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxTechnology*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
}

jvxErrorType
CjvxAudioAsioTechnology::activate()
{
	jvxErrorType res = CjvxAudioTechnology::activate();

	if(res == JVX_NO_ERROR)
	{
		initializeAsio();

		// update_capabilities();
	}
	return(res);
}


jvxErrorType
CjvxAudioAsioTechnology::deactivate()
{
	jvxSize i;
	jvxErrorType res = _deactivate();
	if(res == JVX_NO_ERROR)
	{
		auto elm = _common_tech_set.lstDevices.begin();
		for (; elm != _common_tech_set.lstDevices.end(); elm++)
		{
			delete(elm->hdlDev);
		}
		_common_tech_set.lstDevices.clear();
	}
	return(res);
}

bool
CjvxAudioAsioTechnology::initializeAsio()
{
	HKEY			hkEnum = 0, hkEnumNewBranch = 0, hkSub = 0, hkPath = 0, hkSubNewBranch = 0;
	char			keyname[MAXDRVNAMELEN];
//	LPASIODRVSTRUCT	pdl;
	LONG 			cr, crListDevices;
	DWORD			index = 0, indexInner = 0;
	BOOL			fin = FALSE;
	char			clsidString[256];
	char			description[256];
	char			dllPath[MAXPATHLEN];
	char			dataBuf[512];
	WORD			wData[100];
	CLSID			clsid;
	DWORD			datatype, datasize;
	bool			found = false;
	HFILE			hFile;
	OFSTRUCT		ofs;

	unsigned idDevice = 0;
	unsigned numdrv		= 0;
	//lpdrvlist	= 0;

	CjvxAudioAsioDevice* ptrDev = NULL;

	crListDevices = RegOpenKey(HKEY_LOCAL_MACHINE,ASIO_PATH,&hkEnum);
	while (crListDevices == ERROR_SUCCESS)
	{
		if ((crListDevices = RegEnumKey(hkEnum,index++,(LPTSTR)keyname,MAXDRVNAMELEN))== ERROR_SUCCESS)
		{
//			std::cout << keyname << std::endl;//lpdrvlist = newDrvStruct (hkEnum,keyname,0,lpdrvlist);

			// Get the key for keyname
			if ((cr = RegOpenKeyEx(hkEnum,(LPCTSTR)keyname,0,KEY_READ,&hkSub)) == ERROR_SUCCESS)
			{
				// Set dataype to NULL terminated string for Registry access
				datatype = REG_SZ;

				// Set datasize to 256
				datasize = 256;

				// Return the access name for COM_CLSID ("clsid") in databuf
				cr = RegQueryValueEx(hkSub,COM_CLSID,0,&datatype,(LPBYTE)clsidString,&datasize);
				if(cr == ERROR_SUCCESS)
				{
					CharLowerBuff(clsidString,(DWORD)strlen(clsidString));

					// Find the subkey for the "clsid" entry in another branch
					if ((cr = RegOpenKey(HKEY_CLASSES_ROOT,COM_CLSID,&hkEnumNewBranch)) == ERROR_SUCCESS)
					{
						indexInner = 0;
						found = false;
						while (cr == ERROR_SUCCESS && !found)
						{
							// Enum the keys under CLSID entry (these are really many....)
							cr = RegEnumKey(hkEnumNewBranch,indexInner++,(LPTSTR)dataBuf,512);
							if (cr == ERROR_SUCCESS)
							{
								// Convert to lower case letters again
								CharLowerBuff(dataBuf,(DWORD)strlen(dataBuf));

								// If the expression matches, continue
								if (!(strcmp(dataBuf,clsidString)))
								{
									// Read the value subkey: This is the node for the hardware CLSID
									if ((cr = RegOpenKeyEx(hkEnumNewBranch,(LPCTSTR)dataBuf,0,KEY_READ,&hkSubNewBranch)) == ERROR_SUCCESS)
									{
										// Now find the subentry for "InProcServer32"
										if ((cr = RegOpenKeyEx(hkSubNewBranch,(LPCTSTR)INPROC_SERVER,0,KEY_READ,&hkPath)) == ERROR_SUCCESS)
										{
											datatype = REG_SZ; datasize = (DWORD)MAXPATHLEN;

											// For this leaf, get the name of the DLL path
											cr = RegQueryValueEx(hkPath,0,0,&datatype,(LPBYTE)dllPath,&datasize);
											if (cr == ERROR_SUCCESS)
											{
												memset(&ofs,0,sizeof(OFSTRUCT));
												ofs.cBytes = sizeof(OFSTRUCT);

												// Check whether file can be opened
												hFile = OpenFile(dllPath,&ofs,OF_EXIST);
												if (hFile)
												{
//													std::cout << dllPath << std::endl;
													ptrDev = new CjvxAudioAsioDevice(keyname, false, 
														(_common_set.theDescriptor + "#" + jvx_size2String(_common_tech_set.lstDevices.size())).c_str(),
														_common_set.theFeatureClass, _common_set.theModuleName.c_str(), 
														JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_AUDIO_DEVICE, "", NULL);
													ptrDev->props_init(dllPath, idDevice++);

													// Convert the string for CLSID into a physical CLSID, use Ansi Code page
													MultiByteToWideChar(CP_ACP, 0, (LPCSTR)clsidString, -1, (LPWSTR)wData, 100);
													if(CLSIDFromString((LPOLESTR)wData, (LPCLSID)&clsid) == S_OK)
													{
														ptrDev->props_clsid(clsid);
														datatype = REG_SZ;
														datasize = 256;
														cr = RegQueryValueEx(hkSubNewBranch, ASIODRV_DESC, 0, &datatype, (LPBYTE)description, &datasize);
														if(cr == ERROR_SUCCESS)
														{
															ptrDev->props_description(description);
														}
														else
														{
															ptrDev->props_description(keyname);
														}
														//ptrDev->initializeDevice(hostRef, &this->defaultSetupForDevices, sizeof(this->defaultSetupForDevices));
														CjvxTechnology::oneDeviceWrapper elm;
														elm.hdlDev = ptrDev;
														_common_tech_set.lstDevices.push_back(elm);
													}
													else
													{
														delete(ptrDev);
														ptrDev = NULL;
													}
												}
											}
											RegCloseKey(hkPath);
										}
										RegCloseKey(hkSubNewBranch);
									}
									found = TRUE;	// break out
								}
							}
						}
						RegCloseKey(hkEnumNewBranch);
					}
				}
				RegCloseKey(hkSub);
			}
		}
	}
	if(hkEnum)
	{
		RegCloseKey(hkEnum);
	}

	// Start COM subsystem
	if(_common_tech_set.lstDevices.size())
	{
		/*
		* ASIO und Flutter, Diskussion mit Claude.AI am 17.2.2025:
		* "Ja, ich stimme Ihnen zu. Wir haben verschiedene Ansätze durchgespielt:
		*  ASIO-Treiberinitialisierung in verschiedenen Thread-Kontexten
		*  Versuche der Thread-Synchronisation über Windows Messages
		*  Alternative Audio-Schnittstellen wie PortAudio und WASAPI
		*  Verschiedene COM-Initialisierungsstrategien
		*  Leider scheint die Kombination aus Flutter's Thread-Management und den strikten Thread-Anforderungen von ASIO nicht vereinbar zu sein.
		*  Da WASAPI zumindest funktioniert, auch wenn es nicht alle gewünschten Funktionen bietet, wäre das vermutlich der pragmatischste Weg vorwärts.
		*  Danke für die interessante technische Diskussion!
		*/
		CoInitialize(0);
	}

	return(true);
}
