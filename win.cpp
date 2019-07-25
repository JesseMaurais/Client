// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifdef _WIN32

#include "win.hpp"

namespace sys::win
{
	void impl::perror(char const *prefix)
	{
		constexpr auto flag = FORMAT_MESSAGE_ALLOCATE_BUFFER
		                    | FORMAT_MESSAGE_FROM_SYSTEM 
							| FORMAT_MESSAGE_IGNORE_INSERTS;

		LPSTR message = nullptr;
		auto const data = reinterpret_cast<LPSTR>(&message);
		auto const code = GetLastError();
		auto const size = FormatMessage
		(
		 flag,    // style
		 nullptr, // source
		 code,    // message
		 lang,    // language
		 data,    // buffer
		 0,       // size
		 nullptr  // arguments
		);
		
		if (0 < size)
		{
			fprintf("%s: %s", prefix, message);
			LocalFree(message);
		}
	}
}

#endif // _WIN32
