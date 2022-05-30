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
		/// Nalu���ṹ��
		/// </summary>
		typedef struct Nalu
		{
			/// <summary>ָ��ͷ</summary>
			ptr Head = nullptr;
			/// <summary>����</summary>
			len Length = 0;
		}Nalu;

		/// <summary>
		/// ����H264/H264�ļ����ֽ��һ����nalu��
		/// </summary>
		class Parser
		{
		public:
			/// <summary>
			/// ���캯��
			/// </summary>
			/// <param name="filename">����h264/h265�ļ���ַ</param>
			Parser(std::string filename);

			/// <summary>
			/// ��������
			/// </summary>
			~Parser() noexcept;
			
			/// <summary>
			/// ���nalu��ͷ
			/// </summary>
			/// <param name="head">��ǰָ��ͷ</param>
			/// <returns>���һ֡��ͷ�Ƿ���4�������һ֡�м䣬���ص���3�����û�з���-1</returns>
			int        CheckNaluHead(const ptr& head) noexcept;

			/// <summary>
			/// ��ȡ��һ��nalu��
			/// </summary>
			/// <returns>��һ��nalu��</returns>
			Nalu            GetNextNalu();

			/// <summary>
			/// ��ȡ�ļ��ܳ���
			/// </summary>
			/// <returns>�����ļ��ܳ���</returns>
			len          GetTotalLength() noexcept { return m_u32FileSize; }

		private:
			/// <summary>
			/// Ѱ����һ��nalu��
			/// </summary>
			/// <param name="head">��ǰ��ָ��</param>
			/// <param name="size">ʣ��ĳ���</param>
			/// <returns>������µ�nalu����������nalu����ͷָ�룬���򷵻ؿ�</returns>
			ptr             FindNextStartCode(ptr head, len size) noexcept;

		private:

			
			/// <summary>�ļ�������</summary>
			len             m_u32FileSize;

			/// <summary>�ļ�������</summary>
			char*           m_pBuffer;

			/// <summary>��ǰָ��</summary>
			ptr             m_pCurrentPos;

			/// <summary>��ͷָ��</summary>
			ptr             m_pStartPos;

			/// <summary>��βָ��</summary>
			ptr             m_pEndPos;
		};
	}
}

#endif // !__SPLITNALU_H_


