#include "jvx_console_tools.h"

/*
https://stackoverflow.com/questions/26948723/checking-the-stdin-buffer-if-its-empty
*/

void 
jvxConsoleHdl::prepare(int& hdlStop)
{
	JVX_PREPARE_UNBUFFERED_CHAR
	// get the console handle
	hstdin = STDIN_FILENO;
	hstdend = hdlStop;
	memset(events, 0, sizeof(struct kevent) * 2);

	std::cout << __FUNCTION__ << std::endl;

}

void 
jvxConsoleHdl::postprocess()
{
#ifdef VERBOSE_CONSOLE_OUT
	std::cout << __FUNCTION__ << std::endl;
#endif

	posi_read = 0;
	posi_avail = 0;
	hstdin = JVX_INVALID_FILE_DESCRIPTOR_VALUE;
	hstdend = JVX_INVALID_FILE_DESCRIPTOR_VALUE;
	kqueuefd = JVX_INVALID_FILE_DESCRIPTOR_VALUE;
	JVX_POSTPROCESS_UNBUFFERED_CHAR;
}

int 
jvxConsoleHdl::get_single_console_character()
{
#ifdef VERBOSE_CONSOLE_OUT
	std::cout << __FUNCTION__ << std::endl;
#endif

	jvxSize i;
	jvxSize numRead = 0;
	int retC = 0;
	jvxBool leaveFunc = false;
	
	struct kevent triggered[2] = {0};
	
	int status;
	int nfds;
		
	kqueuefd = kqueue();
	assert(kqueuefd != -1);

	EV_SET(&events[0], hstdin, EVFILT_READ, EV_ADD, 0,0,0);
	EV_SET(&events[1], hstdend, EVFILT_READ, EV_ADD, 0,0,0);
			
	while (1)
	{
		if (posi_avail == posi_read)
		{

#ifdef VERBOSE_CONSOLE_OUT
			std::cout << "Entering Write" << std::endl;
#endif

			nfds = JVX_EVENT_WAIT_2WAIT(kqueuefd, events, 2, triggered, 2, NULL);

#ifdef VERBOSE_CONSOLE_OUT
			std::cout << "Leaving Write, nfds = " << nfds << std::endl;
#endif

			if (nfds < 0)
			{
					// This indicates an error
			}
			else if(nfds == 0)
			{
				// No file descriptor fired, timeout!
				retC = -2;
				return retC;
			}
			else 
			{
				for(i = 0; i < nfds; i++)
				{
					if (events[i].ident == hstdin)
					{

#ifdef VERBOSE_CONSOLE_OUT
						std::cout << "Enterig IO Ctrl" << nfds << std::endl;
#endif

						numRead = 0;
						int resIo = ioctl(0, FIONREAD, &numRead);

#ifdef VERBOSE_CONSOLE_OUT
						std::cout << "Leaving IO Ctrl, numRead = " << numRead << std::endl;
#endif

						if (resIo == 0)
						{
							if (numRead > 0)
							{
								posi_avail = numRead;
								posi_read = 0;
							}
						}
					}
					if (events[i].ident == hstdend)
					{
						// Indicates end trigger
#ifdef VERBOSE_CONSOLE_OUT
						std::cout << __FUNCTION__ << ": end triggered." << std::endl;
#endif
						retC = -2;
						return retC;
					}
				}
			}
		}

		for (i = posi_read; i < posi_avail; i++)
		{
#ifdef VERBOSE_CONSOLE_OUT
			std::cout << "Enterig IO Read" << nfds << std::endl;
#endif
			retC = JVX_GET_UNBUFFERED_CHAR(); // is that function sufficient?

#ifdef VERBOSE_CONSOLE_OUT
			std::cout << "Leaving IO Read, data = " << retC << std::endl;
#endif

			posi_read = i;
			return retC;
		}
	}
	return retC;
}

