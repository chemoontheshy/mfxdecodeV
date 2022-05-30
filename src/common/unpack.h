#ifndef __UNPACK_H_
#define __UNPACK_H_
#include <iostream>

namespace vsnc
{
	namespace vpack
	{

		/// <summary>编码协议枚举</summary>
		enum class CodecID : int { H264, HEVC };
		/// <summary>
		/// 码流包
		/// </summary>
		typedef struct __VPacket
		{
			/// <summary>头指针</summary>
			uint8_t* Head;
			/// <summary>码流长度</summary>
			int      Size;
			/// <summary>时间戳</summary>
			uint32_t  Timestamp;
			/// <summary>序号</summary>
			int16_t  seq;
			/// <summary>一帧结束标记</summary>
			int      mark;
		} __VPacket;

		/// <summary>
		/// 解RTP包，主要是去掉前12个字节头，还原为h264
		/// </summary>
		/// <param name="codec_id">编码格式:支持h264和hevc</param>
		/// <param name="stream">指针头</param>
		/// <param name="size">长度</param>
		/// <returns></returns>
		__VPacket unPack(const CodecID& codec_id, uint8_t* const stream, const int size);
	}
}

#endif