#include "ayf-embedding-proxy.h"

#include "ayf-embedding-libs.h"

#define EMBEDDED_HOST_SELECTION "EMBEDDED_HOST"
#define EMBEDDED_HOST_SO_ASSIGN_MIN_HOST "DLL_MIN_HOST"
#define EMBEDDED_HOST_SO_ASSIGN_EMB_HOST "DLL_EMB_HOST"
#define EMBEDDED_HOST_SO_ASSIGN_EMB_ARGS "DLL_EMB_ARG"
#define EMBEDDED_HOST_OPEN_CONSOLE "OPEN_CONSOLE"


std::map<jvxSize, std::string> registeredInstances;
JVX_HMODULE modHostPart = JVX_HMODULE_INVALID;
ayfHostBindingType bindTypeFirstModule = ayfHostBindingType::AYF_HOST_BINDING_NONE;
jvxSize uId = 1;
jvxBool consoleAttached = false;

#define Q(x) #x
#define Quotes(x) Q(x)
#define SET_DLL_REFERENCE(ret, entry) ret-> ## entry ## _call = (entry) JVX_GETPROCADDRESS(modHostPart, Quotes(entry))

static void registerMinHost(ayfHostBindingReferences* retReferences, JVX_HMODULE modHostPart)
{
	SET_DLL_REFERENCE(retReferences, ayf_register_module_host);
	SET_DLL_REFERENCE(retReferences, ayf_unregister_module_host);
	SET_DLL_REFERENCE(retReferences, ayf_load_config_content);
	SET_DLL_REFERENCE(retReferences, ayf_release_config_content);
	SET_DLL_REFERENCE(retReferences, ayf_attach_component_module);
	SET_DLL_REFERENCE(retReferences, ayf_detach_component_module);
	SET_DLL_REFERENCE(retReferences, ayf_forward_text_command);
	retReferences->bindType = ayfHostBindingType::AYF_HOST_BINDING_MIN_HOST;
}

static void registerEmbHost(ayfHostBindingReferences* retReferences, JVX_HMODULE modHostPart)
{
	SET_DLL_REFERENCE(retReferences, ayf_register_module_host);
	SET_DLL_REFERENCE(retReferences, ayf_unregister_module_host);
	SET_DLL_REFERENCE(retReferences, ayf_load_config_content);
	SET_DLL_REFERENCE(retReferences, ayf_release_config_content);
	SET_DLL_REFERENCE(retReferences, ayf_attach_component_module);
	SET_DLL_REFERENCE(retReferences, ayf_detach_component_module);
	SET_DLL_REFERENCE(retReferences, ayf_forward_text_command);
	retReferences->bindType = ayfHostBindingType::AYF_HOST_BINDING_FULL_HOST;
}

extern "C"
{
	jvxErrorType ayf_embedding_proxy_init(const char* nm, jvxSize* idRegistered, ayfHostBindingReferences* retReferences, const char* fNameIniPath)
	{
		std::list<std::string> messagesConsole;

		jvxBool openConsole = false;
		std::string loadDllNameMinHost = MIN_HOST_DLL_NAME;
		std::string loadDllNameEmbHost = EMB_HOST_DLL_NAME;
		std::string argsExp; 
		std::vector<std::string> lstArgs;

		// std::string loadDllNameFullHost = ""; // This one should not be loaded, it should be there already
		std::string loadDllName = loadDllNameMinHost;
		int embedId = (int)ayfHostBindingType::AYF_HOST_BINDING_MIN_HOST;
		// Check what is desired from first caller

		if (registeredInstances.size() == 0)
		{
			if (fNameIniPath)
			{
				std::string readText;
				std::string fNameIni = fNameIniPath;
				fNameIni += JVX_SEPARATOR_DIR;
				fNameIni += "ayf-proxy.ini";

#ifdef JVX_OS_WINDOWS
				fNameIni = jvx_replaceDirectorySeparators_toWindows(fNameIni, JVX_SEPARATOR_DIR_CHAR, JVX_SEPARATOR_DIR_CHAR_THE_OTHER);
#endif
				if (jvx_readContentFromFile(fNameIni, readText) == JVX_NO_ERROR)
				{
					messagesConsole.push_back("Arguments in file <" + fNameIni + ">:");
					jvxSize startIdx = 0;
					while(1)
					{
						size_t posNewLine = readText.find('\n', startIdx);
						std::string commandText = readText.substr(startIdx, posNewLine- startIdx);
						if (commandText.size())
						{
							// Remove leading \r if given
							jvx_tokenRemoveCharLeftRight(commandText, '\r', false, true);
							jvx_tokenRemoveCharLeftRight(commandText, ' ', true, false);
							jvx_tokenRemoveCharLeftRight(commandText, '\t', true, false);

							messagesConsole.push_back(" -- "  + commandText + " -- ");

							// This is a comment <# .....>
							if (commandText.size() && commandText[0] != '#')
							{
								std::vector<std::string> lstStr;
								jvx_parseCommandLineOneToken(commandText, lstStr, '=');
								if (lstStr.size() >= 2)
								{
									std::string assToken = lstStr[0];
									std::string assValue = lstStr[1];
									jvx_tokenRemoveCharLeftRight(assToken);
									jvx_tokenRemoveCharLeftRight(assValue);
									if (assToken == EMBEDDED_HOST_SO_ASSIGN_MIN_HOST)
									{
										loadDllNameMinHost = assValue;
									}

									if (assToken == EMBEDDED_HOST_SO_ASSIGN_MIN_HOST)
									{
										loadDllNameMinHost = assValue;
									}

									if (assToken == EMBEDDED_HOST_OPEN_CONSOLE)
									{
										if (assValue == "yes")
										{
											openConsole = true;
										}
									}
									if (assToken == EMBEDDED_HOST_SELECTION)
									{
										if (assValue == "MIN")
										{
											embedId = (int)ayfHostBindingType::AYF_HOST_BINDING_MIN_HOST;
											loadDllName = loadDllNameMinHost;
										}
										if (assValue == "EMB")
										{
											embedId = (int)ayfHostBindingType::AYF_HOST_BINDING_EMBEDDED_HOST;
											loadDllName = loadDllNameEmbHost;
										}						
										if (assValue == "NONE")
										{
											embedId = (int)ayfHostBindingType::AYF_HOST_BINDING_NONE;
										}
									}

									if (assToken == EMBEDDED_HOST_SO_ASSIGN_EMB_ARGS)
									{
										argsExp = assValue;
										jvx_parseCommandLineOneToken(argsExp, lstArgs, ',');
									}
								}
							}
						}
					
						if (posNewLine == std::string::npos)
						{
							break;
						}
						else
						{
							startIdx = posNewLine + 1;
						}
					}					
				}
				else
				{
					messagesConsole.push_back("Failed to open ini file <" + fNameIni + ">.");
					openConsole = true;
				}
			}
			
			if (openConsole)
			{
#ifdef JVX_OS_WINDOWS
				AllocConsole();
				freopen("conin$", "r", stdin);
				freopen("conout$", "w", stdout);
				freopen("conout$", "w", stderr);
				printf("Attached Debugging Window:\n");
				for (auto elm : messagesConsole)
				{
					std::cout << elm << std::endl;
				}
				consoleAttached = true;
#endif
			}

			jvxBool runLoop = true;
			while (runLoop)
			{
				switch (embedId)
				{
				case (int)ayfHostBindingType::AYF_HOST_BINDING_EMBEDDED_HOST:

					std::cout << "Trying to open emb host <" << loadDllName << ">." << std::endl;
					modHostPart = JVX_LOADLIBRARY(loadDllName.c_str());
					if (modHostPart != JVX_HMODULE_INVALID)
					{
						std::cout << " --> Opening successful!" << std::endl;
						registerEmbHost(retReferences, modHostPart);
						runLoop = false;
					}
					break;

				case (int)ayfHostBindingType::AYF_HOST_BINDING_MIN_HOST:

					std::cout << "Trying to open min host <" << loadDllName << ">." << std::endl;
					modHostPart = JVX_LOADLIBRARY(loadDllName.c_str());
					if (modHostPart != JVX_HMODULE_INVALID)
					{
						std::cout << " --> Opening successful!" << std::endl;
						registerMinHost(retReferences, modHostPart);
						runLoop = false;
					}
					break;
				case (int)ayfHostBindingType::AYF_HOST_BINDING_NONE:
					std::cout << " --> Running with no binding." << std::endl;
					retReferences->bindType = ayfHostBindingType::AYF_HOST_BINDING_NONE;
					runLoop = false;
					break;
				}

				embedId--;
			}

			bindTypeFirstModule = retReferences->bindType;
			registeredInstances[uId] = nm;
			if (idRegistered) *idRegistered = uId;
			uId++;
		} // if (registeredInstances.size() == 0)
		else
		{
			switch (bindTypeFirstModule)
			{
			case ayfHostBindingType::AYF_HOST_BINDING_FULL_HOST:
				registerEmbHost(retReferences, modHostPart);
				break;

			case ayfHostBindingType::AYF_HOST_BINDING_MIN_HOST:
				registerMinHost(retReferences, modHostPart);
				break;
			}

			registeredInstances[uId] = nm;
			uId++;
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType ayf_embedding_proxy_terminate(jvxSize idRegistered)
	{
		auto elm = registeredInstances.find(idRegistered);
		if (elm == registeredInstances.end())
		{
			return JVX_ERROR_ELEMENT_NOT_FOUND;
		}

		registeredInstances.erase(elm);

		if (registeredInstances.size() == 0)
		{
			if (modHostPart != JVX_HMODULE_INVALID)
			{
				JVX_UNLOADLIBRARY(modHostPart);
				modHostPart = JVX_HMODULE_INVALID;

				if (consoleAttached)
				{
					BOOL freeResult = FreeConsole();
					if (!freeResult)
					{
						DWORD lErr = GetLastError();
					}					
				}
				consoleAttached = false;
			}
		}
		return JVX_NO_ERROR;
	}

	// =======================================================================================

	jvxErrorType ayf_embedding_proxy_host_set(IjvxHost* hostRefOnInit)
	{
		return JVX_NO_ERROR;
	}

	jvxErrorType ayf_embedding_proxy_host_reset()
	{
		return JVX_NO_ERROR;
	}
}


