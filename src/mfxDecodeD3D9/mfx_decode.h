#pragma once
#include <iostream>
#include <vector>
#include "mfx_directx.h"

using vsnc::stucture::Packet;
namespace vsnc
{
	namespace mfx
	{
		/// <summary>
		/// mfx����
		/// </summary>
		class MFXDecoder
		{
			using __dec_type = std::unique_ptr<MFXVideoDECODE>;
		public:
			/// <summary>
			/// ���캯��
			/// </summary>
			/// <param name="hardward">�Ƿ���Ӳ���룬Ĭ�������</param>
			MFXDecoder(bool hardward =false);

			/// <summary>
			/// ��������
			/// </summary>
			~MFXDecoder();
			/// <summary>
			/// ��ȡһ֡ͼ��
			/// </summary>
			/// <param name="rect">ͼ��</param>
			/// <returns>����true���ܻ�ȡͼ�񣬷���false��Ҫ����µ�����</returns>
			bool               GetPacket(D3DLOCKED_RECT& rect);
			/// <summary>
			/// ͬ�ϣ�������
			/// </summary>
			/// <param name="rect">ͼ��</param>
			/// <returns>����true���ܻ�ȡͼ�񣬷���false��Ҫ����µ�����</returns>
			bool               GetPacketHDL(D3DLOCKED_RECT& rect);

			/// <summary>
			/// ����µ�����
			/// </summary>
			/// <param name="packet">���ݰ�</param>
			/// <returns>����true</returns>
			bool               FillMFXBitStream(const Packet& packet);
		private:
			/// <summary>
			/// ��ʼ��������
			/// </summary>
			/// <returns>�ɹ�����true�����󷵻�false</returns>
			bool               __init_decoder();
		private:
			/// <summary>�Ƿ�ɹ���ʼ��</summary>
			bool                  m_bInitialized;
			
			/// <summary>�Ự</summary>
			MFXVideoSession       m_iMfxSession;

			/// <summary>������</summary>
			__dec_type            m_iMfxDEC;

			/// <summary>���������������������������</summary>
			mfxBitstream          m_iMfxBitStream;

			/// <summary>�����һ֡ͼ��Ļ�����</summary>
			mfxFrameSurface1      m_iMfxFrameSurface;
			
			/// <summary>һ֡ͼ��Ļ��������ڴ���䣬���new,delete ����Ҫ����</summary>
			std::vector<mfxU8>    m_iFrameAlloc;

			/// <summary>�����һ֡ͼ��Ļ����������new,delete,����Ҫ����</summary>
			std::vector<mfxU8>    m_iBitStreamAlloc;
			
			/// <summary>����������</summary>
			mfxVideoParam         m_iMfxVideoParams;
		private:
			/// <summary>�Ƿ���Ӳ����</summary>
			bool                  m_bHardware;
			/// <summary>Ӳ���뻺����</summary>
			mfxFrameSurface1**    m_pMfxFrameSurFaceHDL;
			/// <summary>Ӳ�����پ��</summary>
			mfxHDL                m_iMfxDeviceHandle;
			/// <summary>Ӳ������Ҫ�Լ����������ڴ�ο�https://github.com/Intel-Media-SDK/MediaSDK/blob/master/doc/mediasdk-man.md#Memory_Allocation</summary>
			mfxFrameAllocator     m_iMfxAllocator;
			/// <summary>�����˳�ʼ��������������������ƵԤ������ʱ�Ķ��֡����</summary>
			mfxFrameAllocResponse m_iMfxResponse;
			/// <summary>��Ҫ��֡�������</summary>
			mfxU16                m_u16NumSurfaces;
			/// <summary>Surfaces��ID</summary>
			int                   m_iIndex;
		};
	}
}