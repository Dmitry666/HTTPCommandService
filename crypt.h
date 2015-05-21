#pragma once

#include "common.h"

#ifdef WITH_WEBSOCKET
#include <openssl/sha.h>
#endif

namespace crypt
{
	class SHA1Crypt
	{
	public:
        static std::string computeHash(const std::string& text)
		{
#ifdef WITH_WEBSOCKET
			unsigned char digest[SHA_DIGEST_LENGTH];
			char buf[SHA_DIGEST_LENGTH*2+1];


			SHA1(reinterpret_cast<const unsigned char*>(text.c_str()), text.size(), digest);

            for (uint32_t i = 0; i < SHA_DIGEST_LENGTH; i++)
				sprintf(&buf[i], "%c", digest[i]);
			//	sprintf(&buf[i*2], "%02x", digest[i]);
			
			return buf;
#else
            return text;
#endif
		}
	};
}
