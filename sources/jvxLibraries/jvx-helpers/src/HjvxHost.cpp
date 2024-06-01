#include "jvx-helpers.h"

void activate_default_components_host(const char** lst_ModulesOnStart[JVX_COMPONENT_ALL_LIMIT], IjvxHost* hHost, jvxBool systemComponent, jvxBool verboseOutput)
{
	jvxState stat = JVX_STATE_NONE;
	jvxSize num = 0;
	std::string name;
	jvxSize i, j, k;
	jvxSize numA = 0;
	jvxApiString strL;
	jvxErrorType res;
	std::string elm_notfound;
	jvxSize limitRunStart = 1;
	jvxSize limitRunStop = JVX_MAIN_COMPONENT_LIMIT;
	int mod = 1;
	if (systemComponent)
	{
		// The system componets are started fromhigher to lower number
		limitRunStop = JVX_MAIN_COMPONENT_LIMIT-1;
		limitRunStart = JVX_COMPONENT_ALL_LIMIT-1;
		mod = -1;
	}

	for (i = limitRunStart; i != limitRunStop; i+= mod)
	{
		const char** ptrComp = lst_ModulesOnStart[i];

		if (ptrComp)
		{
			if (verboseOutput)
			{
				std::cout << "==== " << __FUNCTION__ << ": Found entries for component type <" << jvxComponentType_txt(i) << ">." << std::endl;
			}

			// Step I: Check if current state is already correct
			jvxBool stateIsCorrect = true;
			jvxApiString astr;
			jvxSize szSlots = 0;
			jvxSize szSubSlots = 0;
			jvxComponentType tpP;
			jvxComponentType tpC;
			jvxSize numSlotsMax = 0;
			jvxSize numSubSlotsMax = 0;
			jvxSize cnt = 0;
			jvxSize cntRequired = 0;
			jvxSize cntCorrect = 0;

			jvxComponentIdentification tpIdN((jvxComponentType)i);
			hHost->role_component_system(tpIdN.tp, &tpP, &tpC, nullptr);
			hHost->number_slots_component_system(tpIdN, &szSlots, &szSubSlots, &numSlotsMax, &numSubSlotsMax);
			if (tpP == JVX_COMPONENT_UNKNOWN)
			{
				// Primary component

				// Part one: deactivate all "wrong" modules
				cnt = 0;
				cntRequired = 0;

				while(1)
				{
					if (ptrComp[cnt])
					{
						cntRequired++;
						cnt++;
						if (cnt > numSlotsMax)
						{
							break;
						}
					}
					else
					{
						break;
					}
				}
				if (cntRequired > numSlotsMax)
				{
					if (verboseOutput)
					{
						std::cout << "==== " << __FUNCTION__ << ": Error: More modules requested than slots avaiable, " << 
							cntRequired << " required whereas there are only " << numSlotsMax << " slots available." << std::endl;
					}

					// Problem: more modules requested that space available
					res = JVX_ERROR_INVALID_SETTING;
					break;
				}

				// Check all active slots
				for (j = 0; j < szSlots; j++)
				{
					tpIdN.slotid = j;
					if (ptrComp[j] == NULL)
					{
						if (verboseOutput)
						{
							std::cout << "==== ==== " << __FUNCTION__ << ": Configuration with nullptr triggers unselection of component in slot id <" << 
								j << ">." << std::endl;
						}

						// Supposed to be none..
						// -> Deactivate component
						res = hHost->unselect_selected_component(tpIdN);
						assert(res == JVX_NO_ERROR);
					}
					else
					{
						if (verboseOutput)
						{
							std::cout << "==== ==== " << __FUNCTION__ << ": Trying to find component with module name <" << 
								ptrComp[j] << ">." << std::endl;
						}

						jvxSize selId = JVX_SIZE_UNSELECTED;
						jvxComponentAccessType acTp = JVX_COMPONENT_ACCESS_UNKNOWN;
						tpIdN.slotid = j;
						res = hHost->selection_component(tpIdN, &selId);
						if (JVX_CHECK_SIZE_SELECTED(selId))
						{
							if (verboseOutput)
							{
								std::cout << "==== ==== ==== " << __FUNCTION__ << ": A component is already there i slot <" << j << ">." << std::endl;
							}
							res = hHost->module_reference_selected_component(tpIdN, &astr, &acTp);
							assert(res == JVX_NO_ERROR);
							if (res == JVX_NO_ERROR)
							{
								// std::cout << __FUNCTION__ << ": Checking module <" << astr.std_str() << "> vs <" << ptrComp[j] << ">." << std::endl;
								if (astr.std_str() == ptrComp[j])
								{
									if (verboseOutput)
									{
										std::cout << "==== ==== ==== " << __FUNCTION__ << ": Correct module already activate." << std::endl;
									}
									res = hHost->state_selected_component(tpIdN, &stat);
									assert(res == JVX_NO_ERROR);
									if (stat == JVX_STATE_ACTIVE)
									{
										cntCorrect++;
									}
								}
								else
								{
									if (verboseOutput)
									{
										std::cout << "==== ==== ==== " << __FUNCTION__ << ": Incorrect module with name < " << astr.std_str() << "> is unselected." << std::endl;
									}

									res = hHost->unselect_selected_component(tpIdN);
									assert(res == JVX_NO_ERROR);
								}
							}
						} // if (JVX_CHECK_SIZE_SELECTED(selId))
						else
						{
							// No component selected within this slot. This is vaid and will be 
							// handled properly afterwards
						}
					}
				}

				// Check that all modules are in right state
				if(cntCorrect != cntRequired)
				{
					// Part two: activate all "right" modules
					for (j = 0; j < cntRequired; j++)
					{
						assert(ptrComp[j]); // <- this was counted before
						if (ptrComp[j] != (std::string)"__skip__")
						{

							if (verboseOutput)
							{
								std::cout << "==== ==== " << __FUNCTION__ << ": Activating component < " << ptrComp[j] << ">." << std::endl;
							}

							stat = JVX_STATE_NONE;
							jvxBool runme = true;
							jvxSize numC = 0;
							tpIdN.slotid = j;
							while (runme)
							{
								elm_notfound = "";
								res = hHost->state_selected_component(tpIdN, &stat);
								switch (stat)
								{
								case JVX_STATE_NONE:
									res = hHost->number_components_system(tpIdN, &numC);
									{
										for (k = 0; k < numC; k++)
										{
											jvxComponentAccessType accTp = JVX_COMPONENT_ACCESS_UNKNOWN;;
											res = hHost->module_reference_component_system(tpIdN, k, &astr, &accTp);
											// std::cout << __FUNCTION__ << ": Checking module <" << astr.std_str() << "> vs <" << ptrComp[j] << ">." << std::endl;
											if (astr.std_str() == ptrComp[j])
											{
												res = hHost->state_selected_component(tpIdN, &stat);
												assert(res == JVX_NO_ERROR);

												res = hHost->select_component(tpIdN, k);
												assert(res == JVX_NO_ERROR);

												break;
											}
										}
										res = hHost->state_selected_component(tpIdN, &stat);
										if (stat == JVX_STATE_NONE)
										{
											// Problem: module not found
											res = JVX_ERROR_ELEMENT_NOT_FOUND;
											elm_notfound = ptrComp[j];
											runme = false;
										}
										break;
								case JVX_STATE_SELECTED:
									res = hHost->activate_selected_component(tpIdN);
									if (res != JVX_NO_ERROR)
									{
										elm_notfound = ptrComp[j];
										runme = false;
										break;
									}
									res = hHost->state_selected_component(tpIdN, &stat);
									break;
								case JVX_STATE_ACTIVE:
									runme = false;
									break;
								default:
									assert(0);
									}
								}

								
								switch (res)
									{
									case JVX_ERROR_ELEMENT_NOT_FOUND:
										std::cout << jvx::coutColor(jvx::coutColor::cc::FG_RED) << "#### " << __FUNCTION__ << ": During default activation of modules for component type "
											<< jvxComponentIdentification_txt(tpIdN) << ": Activation of module " << elm_notfound << " failed: " << jvxErrorType_descr(res) << "."
											<< jvx::coutColor(jvx::coutColor::cc::FG_DEFAULT) << std::endl;
										// jvx::coutColor(jvx::coutColor::cc::BG_RED)   jvx::coutColor()
										// assert(0);
										break;
									case JVX_ERROR_INVALID_SETTING:
										std::cout << "#### " << __FUNCTION__ << ": During default activation of modules for component type " << jvxComponentIdentification_txt(tpIdN) << ": Not enough slots available to activate all required modules." << std::endl;
										// assert(0); 
										break;
									default:
										if (res != JVX_NO_ERROR)
										{
											std::cout << "#### " << __FUNCTION__ << ": During default activation of modules for component type " << jvxComponentIdentification_txt(tpIdN) << ": Error <" << jvxErrorType_descr(res) << "> on activation of required modules." << std::endl;
											// assert(0); 
										}
										break;
									}
								
							}
						}
					} // for (j = 0; j < cntRequired; j++)
				} // if(cntCorrect == cntRequired)
			}
			else
			{
				if (verboseOutput)
				{
					std::cout << "==== " << __FUNCTION__ << ": Activation of secondary component is not supported." << std::endl;
				}

				// Secondary component - it does not make sense to activate the secondary component 
			}

			//if (verboseOutput)
			//{
				
			//}
		} // if (ptrComp)

	} // for (i = 1; i < JVX_COMPONENT_LIMIT; i++)
}

void deactivate_default_components_host(const char** lst_ModulesOnStart[JVX_COMPONENT_ALL_LIMIT], IjvxHost* hHost, jvxBool systemComponent)
{
	jvxSize i,j;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize limitRunStart = 1;
	jvxSize limitRunStop = JVX_MAIN_COMPONENT_LIMIT;
	int mod = 1;
	if (systemComponent)
	{
		// The system components are not deactivated here, they need to be alive until
		// the shutdwn unselects all component in the order of the tp id.
		return;
		//limitRunLow = JVX_MAIN_COMPONENT_LIMIT;
		//limitRunHigh = JVX_COMPONENT_ALL_LIMIT;
	}

	for (i = limitRunStart; i != limitRunStop; i+= mod)	
	{
		const char** ptrComp = lst_ModulesOnStart[i];

		if (ptrComp)
		{
			// Step I: Check if current state is already correct
			jvxBool stateIsCorrect = true;
			jvxApiString astr;
			jvxSize szSlots = 0;
			jvxSize szSubSlots = 0;
			jvxComponentType tpP;
			jvxComponentType tpC;
			jvxSize numSlotsMax = 0;
			jvxSize numSubSlotsMax = 0;
			jvxSize cnt = 0;
			jvxSize cntRequired = 0;
			jvxSize cntCorrect = 0;

			jvxComponentIdentification tpIdN((jvxComponentType)i);
			hHost->role_component_system(tpIdN.tp, &tpP, &tpC, nullptr);
			hHost->number_slots_component_system(tpIdN, &szSlots, &szSubSlots, &numSlotsMax, &numSubSlotsMax);
			if (tpP == JVX_COMPONENT_UNKNOWN)
			{
				for (j = 0; j < szSlots; j++)
				{
					tpIdN.slotid = j;
					if (ptrComp[j] == NULL)
					{
						break;
					}
					else
					{
						res = hHost->module_reference_selected_component(tpIdN, &astr, NULL);
						if (res == JVX_NO_ERROR)
						{
							if (astr.std_str() == ptrComp[j])
							{
								res = hHost->unselect_selected_component(tpIdN);
								assert(res == JVX_NO_ERROR);
							}
							else
							{
								std::cout << __FUNCTION__ << ": Error: Failed to unselect component with module name <" << ptrComp[j] << ">." << std::endl;
								assert(0);
							}
						}
						else
						{
							std::cout << __FUNCTION__ << ": Warning: Failed to get module reference, error code: <" << jvxErrorType_descr(res) << ">." << std::endl;						
						}
					}
				} // for (j = 0; j < szSlots; j++)
			} // if (tpP == JVX_COMPONENT_UNKNOWN)
		} // if (ptrComp)
	} // for (i = 1; i < JVX_COMPONENT_LIMIT; i++)
}
