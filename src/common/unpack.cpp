#include "unpack.h"
#include <fstream>

vsnc::vpack::__VPacket vsnc::vpack::unPack(const CodecID& codec_id, uint8_t* const stream, const int size)
{
	/// <summary>
	/// RTP头结构体：RTPHeader
	/// </summary>
	typedef struct __RTPHeader
	{
#ifdef  __BIGENDIAN__
		/// <summary>版本</summary>
		uint8_t version : 2;
		/// <summary>用于验证，不使用</summary>
		uint8_t padding : 1;
		/// <summary>是否有拓展字段</summary>
		uint8_t extension : 1;
		/// <summary>数据源</summary>
		uint8_t csrccount : 4;

		/// <summary>是否一帧结束</summary>
		uint8_t marker : 1;
		/// <summary>负载类型</summary>
		uint8_t payloadtype : 7;
#else
		/// <summary>数据源</summary>
		uint8_t csrccount : 4;
		/// <summary>是否有拓展字段</summary>
		uint8_t extension : 1;
		/// <summary>用于验证，不使用</summary>
		uint8_t padding : 1;
		/// <summary>版本</summary>
		uint8_t version : 2;

		/// <summary>负载类型</summary>
		uint8_t payloadtype : 7;
		/// <summary>是否一帧结束</summary>
		uint8_t marker : 1;
#endif //  __BIGENDIAN__
		/// <summary>数据包序号</summary>
		uint16_t sequencenumber;
		/// <summary>时间戳</summary>
		uint32_t timestamp;
		/// <summary>标识源</summary>
		uint32_t ssrc;

	} __RTPHeader;

	/// <summary>
	/// RTP拓展字段头结构体: RTPEXHeader
	/// </summary>
	typedef struct __RTPExtensionHeader
	{
		/// <summary>拓展字段标识源</summary>
		uint16_t extid;
		/// <summary>拓展字段长度，单位：32-bit</summary>
		uint16_t length;
	} __RTPExtensionHeader;


	__VPacket packet;
	// 少于等于12个字节头直接返回。
	if (size <= 12) {
		packet.Head = nullptr;
		packet.Size = -1;
		packet.Timestamp = -1;
		return packet;
	}

	// 填充RTPHeader
	__RTPHeader* rtpHeader = reinterpret_cast<__RTPHeader*>(stream);

	// 获取时间戳
	packet.Timestamp = rtpHeader->timestamp;
	packet.seq = rtpHeader->sequencenumber;
	packet.mark = rtpHeader->marker;
	//return packet;
	// 去掉RTPHeader
	auto data = stream + 12;
	auto len = size - 12;
	/*static constexpr uint32_t TEST_START_CODE = 0x01000000;
	(*reinterpret_cast<uint32_t*>(data - 4)) = TEST_START_CODE;
	packet.Head = data - 3;
	packet.Size = len + 3;
	return packet;*/
	//return packet;
	// 如果还有拓展字段，去掉拓展字段：拓展字段头(4字节：32-bit)+拓展字段长度*4;
	if (rtpHeader->extension == 1) {
		auto extLen = reinterpret_cast<__RTPExtensionHeader*>(data)->length * 4;
		data = data + 4 + extLen;
		len = len - 4 - extLen;
	}

	switch (codec_id)
	{
		// H264解包
	case CodecID::H264:
	{
		/// <summary>
	/// H264FU头
	/// </summary>
		typedef struct __H264Indicator
		{
#ifdef __BIGENDIAN__
			uint8_t flag : 1;
			uint8_t nal_ref_idc : 2;
			uint8_t nal_uint_type : 5;
#else
			/// <summary>NAL Unit类型</summary>
			uint8_t nal_uint_type : 5;
			/// <summary>保留</summary>
			uint8_t nal_ref_idc : 2;
			/// <summary>为0</summary>
			uint8_t flag : 1;
#endif // __BIGENDIAN__

		} __H264Indicator;

		typedef struct __H264FUHeader
		{
#ifdef __BIGENDIAN__
			/// <summary>开启标志</summary>
			uint8_t s : 1;
			/// <summary>结束标志</summary>
			uint8_t e : 1;
			/// <summary>保留设置为0</summary>
			uint8_t r : 1;
			/// <summary>NAL Unit类型</summary>
			uint8_t type : 5;
#else
			/// <summary>NAL Unit类型</summary>
			uint8_t type : 5;
			/// <summary>保留设置为0</summary>
			uint8_t r : 1;
			/// <summary>结束标志</summary>
			uint8_t e : 1;
			/// <summary>开启标志</summary>
			uint8_t s : 1;
#endif // __BIGENDIAN__

		} __H264FUHeader;

		static constexpr uint32_t START_CODE = 0x01000000;

		if (len < 2) {
			throw std::runtime_error("stream is too short to unpack");
		}

		__H264Indicator* h264Indicator = reinterpret_cast<__H264Indicator*>(data);
		__H264FUHeader* h264FUHeader = reinterpret_cast<__H264FUHeader*>(data + 1);
		if (0x1C == h264Indicator->nal_uint_type) { // 判断NAL的类型为0x1C，说明是FU-A分片
			if (1 == h264FUHeader->s) { // 如果是一帧的开始
				unsigned char nal_hdr = (((*data) & 0xE0) | ((*(data + 1)) & 0x1F));
				int offset = -3;
				(*reinterpret_cast<uint32_t*>(data + offset)) = START_CODE;
				if ((6 != nal_hdr) && (7 != nal_hdr) && (8 != nal_hdr)) {
					offset = -2;
				}
				(*(data + 1)) = nal_hdr;
				packet.Head = data + offset;
				packet.Size = len - offset;
				return packet;
			}
			// 如果是一帧中间或结束
			packet.Head = data + 2;
			packet.Size = len - 2;
			return packet;
		}
		// 单包数据
		int offset = -4;
		(*reinterpret_cast<uint32_t*>(data + offset)) = START_CODE;
		if ((6 != h264Indicator->nal_uint_type) && (7 != h264Indicator->nal_uint_type) && (8 != h264Indicator->nal_uint_type)) {
			offset = -3;
		}
		packet.Head = data + offset;
		packet.Size = len - offset;
		return packet;
}
	// HEVC解包
	case CodecID::HEVC:
	{

		uint8_t* BufOut = nullptr;
		uint8_t** pBufOut = &BufOut;
		std::size_t OutLen = 0;
		std::size_t* pOutLen = &OutLen;

		unsigned char* src = reinterpret_cast<unsigned char*>(data);
		unsigned char  head1 = *src; // 获取第一个字节
		unsigned char  head2 = *(src + 1); // 获取第二个字节
		unsigned char  head3 = *(src + 2);
		unsigned char  nal_flag = head1 & 0x7e;
		unsigned char  nal = head3 & 0x3f;
		unsigned char  flag = head3 & 0x80;
		unsigned char  nal1 = (head1 & 0x81) + (nal << 1);
		unsigned char  nal2 = head2;

		if (nal_flag == 98) {
			if (flag != 128) {
				*pBufOut = src + 3;
				*pOutLen = static_cast<size_t>(len) - 3;
				packet.Head = BufOut;
				packet.Size = OutLen;
				return packet;
			}
			if (0x13 == nal) {
				*pBufOut = src - 2;
				*(*pBufOut) = 0;
				*(*pBufOut + 1) = 0;
				*(*pBufOut + 2) = 1;
				*(*pBufOut + 3) = nal1;
				*(*pBufOut + 4) = nal2;
				*pOutLen = static_cast<size_t>(len) + 2;
			}
			else {
				*pBufOut = src - 3;
				*(*pBufOut) = 0;
				*(*pBufOut + 1) = 0;
				*(*pBufOut + 2) = 0;
				*(*pBufOut + 3) = 1;
				*(*pBufOut + 4) = nal1;
				*(*pBufOut + 5) = nal2;
				*pOutLen = static_cast<size_t>(len) + 3;
			}
		}
		else {
			*pBufOut = src - 4;
			*(*pBufOut) = 0;
			*(*pBufOut + 1) = 0;
			*(*pBufOut + 2) = 0;
			*(*pBufOut + 3) = 1;
			*(*pBufOut + 4) = head1;
			*(*pBufOut + 5) = head2;
			*pOutLen = static_cast<size_t>(len) + 4;
		}
		packet.Head = BufOut;
		packet.Size = OutLen;
		return packet;
	}
	default:
		throw std::runtime_error("the CodecID is wrong format.");
	}

	return packet;
}
