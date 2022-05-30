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
		/// mfx解码
		/// </summary>
		class MFXDecoder
		{
			using __dec_type = std::unique_ptr<MFXVideoDECODE>;
		public:
			/// <summary>
			/// 构造函数
			/// </summary>
			/// <param name="hardward">是否开启硬解码，默认软解码</param>
			MFXDecoder(bool hardward =false);

			/// <summary>
			/// 析构函数
			/// </summary>
			~MFXDecoder();
			/// <summary>
			/// 获取一帧图像
			/// </summary>
			/// <param name="rect">图像</param>
			/// <returns>返回true才能获取图像，返回false需要填充新的码流</returns>
			bool               GetPacket(D3DLOCKED_RECT& rect);
			/// <summary>
			/// 同上，测试用
			/// </summary>
			/// <param name="rect">图像</param>
			/// <returns>返回true才能获取图像，返回false需要填充新的码流</returns>
			bool               GetPacketHDL(D3DLOCKED_RECT& rect);

			/// <summary>
			/// 填充新的码流
			/// </summary>
			/// <param name="packet">数据包</param>
			/// <returns>返回true</returns>
			bool               FillMFXBitStream(const Packet& packet);
		private:
			/// <summary>
			/// 初始化解码器
			/// </summary>
			/// <returns>成功返回true，错误返回false</returns>
			bool               __init_decoder();
		private:
			/// <summary>是否成功初始化</summary>
			bool                  m_bInitialized;
			
			/// <summary>会话</summary>
			MFXVideoSession       m_iMfxSession;

			/// <summary>解码器</summary>
			__dec_type            m_iMfxDEC;

			/// <summary>解码码流缓存区，用于填充码流</summary>
			mfxBitstream          m_iMfxBitStream;

			/// <summary>软解码一帧图像的缓冲区</summary>
			mfxFrameSurface1      m_iMfxFrameSurface;
			
			/// <summary>一帧图像的缓存区的内存分配，替代new,delete 不需要管理</summary>
			std::vector<mfxU8>    m_iFrameAlloc;

			/// <summary>软解码一帧图像的缓冲区，替代new,delete,不需要管理</summary>
			std::vector<mfxU8>    m_iBitStreamAlloc;
			
			/// <summary>解码器参数</summary>
			mfxVideoParam         m_iMfxVideoParams;
		private:
			/// <summary>是否开启硬解码</summary>
			bool                  m_bHardware;
			/// <summary>硬解码缓冲区</summary>
			mfxFrameSurface1**    m_pMfxFrameSurFaceHDL;
			/// <summary>硬件加速句柄</summary>
			mfxHDL                m_iMfxDeviceHandle;
			/// <summary>硬解码需要自己分配额外的内存参考https://github.com/Intel-Media-SDK/MediaSDK/blob/master/doc/mediasdk-man.md#Memory_Allocation</summary>
			mfxFrameAllocator     m_iMfxAllocator;
			/// <summary>描述了初始化编码器、解码器和视频预处理器时的多个帧分配</summary>
			mfxFrameAllocResponse m_iMfxResponse;
			/// <summary>需要的帧面的数量</summary>
			mfxU16                m_u16NumSurfaces;
			/// <summary>Surfaces的ID</summary>
			int                   m_iIndex;
		};
	}
}