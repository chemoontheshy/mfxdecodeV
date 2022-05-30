#ifndef __GET_SOCKET_DATA_H__
#define __GET_SOCKET_DATA_H__
#include <Windows.h>
#include "unpack.h"
#include "vsnc_common_utils.h"

using vsnc::stucture::Packet;
namespace vsnc
{
	namespace vget
	{
		class VSocket
		{
		public:
			VSocket(const size_t port);
			~VSocket();

			size_t getData(uint8_t* ptr, size_t& size);

			size_t getData(Packet& packet);
		private:
			SOCKET            m_iSocktFD;
			char*	          m_cRBuf;
			sockaddr_in       m_iClient;
			int               m_iClientLen;
			bool              m_bFlag;
		};
	}
}

#endif // !__GET_SOCKET_DATA_H__

