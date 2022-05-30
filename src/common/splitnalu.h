#ifndef __SPLITNALU_H_
#define __SPLITNALU_H_

#include <iostream>

namespace vsnc
{
	namespace vnal
	{
		typedef uint8_t* ptr;
		typedef int32_t len;
		/// <summary>
		/// Nalu包结构体
		/// </summary>
		typedef struct Nalu
		{
			/// <summary>指针头</summary>
			ptr Head = nullptr;
			/// <summary>长度</summary>
			len Length = 0;
		}Nalu;

		/// <summary>
		/// 解析H264/H264文件，分解成一个个nalu包
		/// </summary>
		class Parser
		{
		public:
			/// <summary>
			/// 构造函数
			/// </summary>
			/// <param name="filename">输入h264/h265文件地址</param>
			Parser(std::string filename);

			/// <summary>
			/// 析构函数
			/// </summary>
			~Parser() noexcept;
			
			/// <summary>
			/// 检查nalu的头
			/// </summary>
			/// <param name="head">当前指针头</param>
			/// <returns>如果一帧开头是返回4，如果是一帧中间，返回的是3，如果没有返回-1</returns>
			int        CheckNaluHead(const ptr& head) noexcept;

			/// <summary>
			/// 获取下一个nalu包
			/// </summary>
			/// <returns>下一个nalu包</returns>
			Nalu            GetNextNalu();

			/// <summary>
			/// 获取文件总长度
			/// </summary>
			/// <returns>返回文件总长度</returns>
			len          GetTotalLength() noexcept { return m_u32FileSize; }

		private:
			/// <summary>
			/// 寻找下一个nalu包
			/// </summary>
			/// <param name="head">当前的指针</param>
			/// <param name="size">剩余的长度</param>
			/// <returns>如果有新的nalu包，返回新nalu包的头指针，否则返回空</returns>
			ptr             FindNextStartCode(ptr head, len size) noexcept;

		private:

			
			/// <summary>文件缓存区</summary>
			len             m_u32FileSize;

			/// <summary>文件缓存区</summary>
			char*           m_pBuffer;

			/// <summary>当前指针</summary>
			ptr             m_pCurrentPos;

			/// <summary>开头指针</summary>
			ptr             m_pStartPos;

			/// <summary>结尾指针</summary>
			ptr             m_pEndPos;
		};
	}
}

#endif // !__SPLITNALU_H_


