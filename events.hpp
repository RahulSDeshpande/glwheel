/*#pragma once
#include <vector>
#include <functional>
#include <iostream>
#if defined(_WIN32)
	#include <windows.h>
	typedef HANDLE pollfd; // :(
#elif defined(ANDROID)
	#include <sys/poll.h>
	inline bool operator==(const pollfd& a, const pollfd& b) { return a.fd == b.fd; }
#else
	#include <sys/poll.h>
	inline bool operator==(const pollfd& a, const pollfd& b) { return a.fd == b.fd; }
#endif

namespace wheel
{
	using namespace std;

	struct eventloop
	{
		int n = 0;
		#ifdef ANDROID
		ALooper *looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
		#else
		pollfd fds[64]; // in honor of Windows
		function<void(pollfd)> fns[64];
		#endif

		void add(pollfd fd, function<void(pollfd)> fn)
		{
		#ifdef ANDROID
			ALooper_addFd(looper, android::act().msgpipe[0], main_id, ALOOPER_EVENT_INPUT, 0, 0);
		#else
			fds[n] = fd;
			fns[n] = fn;
			++n;
		#endif
		}
		void remove(pollfd fd) { for(int i = 0; i < n; ++i) if(fds[i] == fd) { --n; swap(fds[i],fds[n]); swap(fns[i],fns[n]); return; } }

		void process(int timeout)
		{
		#ifdef _WIN32
			DWORD r = 0;
			while((r = MsgWaitForMultipleObjects(n, fds, false, timeout, QS_ALLINPUT|QS_ALLPOSTMESSAGE)) != WAIT_TIMEOUT)
			{
				if(r == WAIT_FAILED) { cerr << "MsgWaitForMultipleObjects() failed" << endl; return; }
				if(r == n)
				{
					MSG msg;
					while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
					{
						if(msg.message == WM_QUIT) break;
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
				else if(0 <= r && r < n) fns[r](fds[r]);
				timeout = 0;
			}
		#else
			while(int r = poll(fds, n, timeout))
			{
				if(r < 0) { cerr << "poll() failed" << endl; return; }
				for(int i = 0; i < n; ++i) if(fds[i].revents) { fns[i](fds[i]); fds[i].revents = 0; }
				timeout = 0;
			}
		#endif
		}
	};
}
*/
