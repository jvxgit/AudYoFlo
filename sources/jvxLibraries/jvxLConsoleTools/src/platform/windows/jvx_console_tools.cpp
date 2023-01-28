#include "jvx_console_tools.h"

void 
jvxConsoleHdl::prepare(HANDLE& hdlStop)
{
	DWORD fdwSaveOldMode = 0;
	DWORD fdwMode = 0;

	// get the console handle
	hstdin = GetStdHandle(STD_INPUT_HANDLE);
	hstdend = hdlStop;
	handles[0] = hstdin;
	handles[1] = hstdend;

	// Save the current input mode, to be restored on exit.
	GetConsoleMode(hstdin, &fdwSaveOldMode);

	// Enable the window and mouse input events.
	fdwMode = ENABLE_WINDOW_INPUT; // | ENABLE_MOUSE_INPUT;
	SetConsoleMode(hstdin, fdwMode);
}

int 
jvxConsoleHdl::get_single_console_character()
{
	jvxSize i;
	DWORD numRead = 0;
	int retC = 0;
	jvxBool leaveFunc = false;
	while (1)
	{
		if (posi_avail == posi_read)
		{
			JVX_WAIT_RESULT resW = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
			if (resW == WAIT_OBJECT_0)
			{
				numRead = 0;
				ReadConsoleInput(hstdin,
					irInBuf,
					numMax,
					&numRead);
				if (numRead)
				{
					posi_avail = numRead;
					posi_read = 0;
				}
			}
			else
			{
				retC = -2;
				return retC;
			}
		}

		for (i = posi_read; i < posi_avail; i++)
		{
			switch (irInBuf[i].EventType)
			{
			case KEY_EVENT:
				if (irInBuf[i].Event.KeyEvent.bKeyDown)
				{
					switch (irInBuf[i].Event.KeyEvent.wVirtualKeyCode)
					{
					case VK_LEFT:
						if (in_sequence_count == 0)
						{
							in_sequence_count = 1;
							retC = JVX_CHARACTER_START_SPECIAL_CHAR_V1;
						}
						else
						{
							in_sequence_count--;
							retC = JVX_ASCI_CODE_ARROW_LEFT_TTY;
						}
						leaveFunc = true;
						break;
					case VK_RIGHT:
						if (in_sequence_count == 0)
						{
							in_sequence_count = 1;
							retC = JVX_CHARACTER_START_SPECIAL_CHAR_V1;
						}
						else
						{
							in_sequence_count--;
							retC = JVX_ASCI_CODE_ARROW_RIGHT_TTY;
						}
						leaveFunc = true;
						break;
					case VK_UP:
						if (in_sequence_count == 0)
						{
							in_sequence_count = 1;
							retC = JVX_CHARACTER_START_SPECIAL_CHAR_V1;
						}
						else
						{
							in_sequence_count--;
							retC = JVX_ASCI_CODE_ARROW_UP_TTY;
						}
						leaveFunc = true;
						break;
					case VK_DOWN:   // if any arrow key was pressed break here
						if (in_sequence_count == 0)
						{
							in_sequence_count = 1;
							retC = JVX_CHARACTER_START_SPECIAL_CHAR_V1;
						}
						else
						{
							in_sequence_count--;
							retC = JVX_ASCI_CODE_ARROW_DOWN_TTY;
						}
						leaveFunc = true;
						break;
					case VK_HOME:
						if (in_sequence_count == 0)
						{
							in_sequence_count = 1;
							retC = JVX_CHARACTER_START_SPECIAL_CHAR_V1;
						}
						else
						{
							in_sequence_count--;
							retC = JVX_ASCI_CODE_BEGINLINE_TTY;
						}
						leaveFunc = true;
						break;

					case VK_END:
						if (in_sequence_count == 0)
						{
							in_sequence_count = 1;
							retC = JVX_CHARACTER_START_SPECIAL_CHAR_V1;
						}
						else
						{
							in_sequence_count--;
							retC = JVX_ASCI_CODE_ENDLINE_TTY;
						}
						leaveFunc = true;
						break;
					case VK_DELETE:
						if (in_sequence_count == 0)
						{
							in_sequence_count = 1;
							retC = JVX_CHARACTER_START_SPECIAL_CHAR_V1;
						}
						else
						{
							in_sequence_count--;
							retC = JVX_ASCI_CODE_DELETE_TTY;
						}
						leaveFunc = true;
						break;

					case VK_ESCAPE:
						retC = 0;
						break;

					default:
						leaveFunc = true;
						retC = irInBuf[i].Event.KeyEvent.uChar.UnicodeChar;
						break;
					}
					
					break;
				}
				break;
			case MOUSE_EVENT:
				break;

			case WINDOW_BUFFER_SIZE_EVENT:
				break;

			case FOCUS_EVENT:
				break;

			case MENU_EVENT:
				break;
			}
			if (leaveFunc)
			{
				if (in_sequence_count == 0)
				{
					i++; // Manual forward of event queue due to break
				}
				break;
			}
		}
		posi_read = i;
		if (leaveFunc)
		{
			break;
		}
	}
	return retC;
}

void
jvxConsoleHdl::postprocess()
{
	memset(irInBuf, 0, sizeof(irInBuf));
	numMax = 128;
	posi_read = 0;
	posi_avail = 0;
	in_sequence_count = 0;
	hstdin = JVX_INVALID_FILE_DESCRIPTOR_VALUE;
	hstdend = JVX_INVALID_FILE_DESCRIPTOR_VALUE;
	handles[0] = JVX_INVALID_HANDLE_VALUE;
	handles[1] = JVX_INVALID_HANDLE_VALUE;
}


