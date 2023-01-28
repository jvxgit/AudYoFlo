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
	memset(events, 0, sizeof(struct epoll_event) * 2);

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
	epollfd = JVX_INVALID_FILE_DESCRIPTOR_VALUE;
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
	
	struct epoll_event ev, evdown;
	int status;
	int nfds;
		
	epollfd = epoll_create1(0);
	assert(epollfd >= 0);
	
	ev.data.fd = hstdin;
	ev.events = EPOLLIN | EPOLLHUP; // no event EPOLLOUT here
	status = epoll_ctl(epollfd, EPOLL_CTL_ADD, hstdin, &ev);
	assert(status >= 0);
		
	evdown.data.fd = hstdend;
	evdown.events = EPOLLIN | EPOLLET;
	status = epoll_ctl(epollfd, EPOLL_CTL_ADD, hstdend, &evdown);
	assert(status >= 0);
		
	while (1)
	{
		if (posi_avail == posi_read)
		{

#ifdef VERBOSE_CONSOLE_OUT
			std::cout << "Entering Write" << std::endl;
#endif

			// This call is compatible with call in Mac OS X
			nfds = JVX_EVENT_WAIT_2WAIT(epollfd, NULL, 0, events, 2,
						    JVX_SIZE_INT(JVX_INFINITE_MS));

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
					if (events[i].data.fd == hstdin)
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
					if (events[i].data.fd == hstdend)
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

