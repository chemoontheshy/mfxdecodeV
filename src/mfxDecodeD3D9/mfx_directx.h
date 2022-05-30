#pragma once
#include <initguid.h>
#include <d3d9.h>
#include <dxva2api.h>
#include "mediaSDK/mfxvideo++.h"
#include "../common/vsnc_common_utils.h"



namespace vsnc
{
	namespace mfx
	{
		/// <summary>
		/// 创建MFXHDL
		/// </summary>
		/// <param name="session">会话</param>
		/// <param name="deviceHandle">空的句柄</param>
		/// <param name="window">窗口句柄，可以为nullptr</param>
		/// <returns>成功返回MFX_ERR_NONE</returns>
		mfxStatus CreateHWDevice(mfxSession session, mfxHDL* deviceHandle, HWND window);
		
		/// <summary>
		/// 清理句柄
		/// </summary>
		void CleanupHWDevice();

		/// <summary>
		/// 分配内存 
		/// 参考 https://github.com/Intel-Media-SDK/MediaSDK/blob/master/doc/mediasdk-man.md#Memory_Allocation
		/// </summary>
		/// <param name="pthis">窗口句柄</param>
		/// <param name="request">分配请求</param>
		/// <param name="response">分配回复</param>
		/// <returns>返回mfxStatus</returns>
		mfxStatus mfx_alloc(mfxHDL pthis, mfxFrameAllocRequest* request, mfxFrameAllocResponse* response);

		/// <summary>
		/// 锁定 
		/// 参考 https://github.com/Intel-Media-SDK/MediaSDK/blob/master/doc/mediasdk-man.md#Memory_Allocation
		/// </summary>
		/// <param name="pthis">窗口句柄</param>
		/// <param name="request">分配请求</param>
		/// <param name="response">分配回复</param>
		/// <returns>返回mfxStatus</returns>
		mfxStatus mfx_lock(mfxHDL pthis, mfxMemId mid, mfxFrameData* ptr);

		/// <summary>
		/// 解锁 
		/// 参考 https://github.com/Intel-Media-SDK/MediaSDK/blob/master/doc/mediasdk-man.md#Memory_Allocation
		/// </summary>
		/// <param name="pthis">窗口句柄</param>
		/// <param name="mid">帧面ID</param>
		/// <param name="ptr">输出帧面指针</param>
		/// <returns></returns>
		mfxStatus mfx_unlock(mfxHDL pthis, mfxMemId mid, mfxFrameData* ptr);

		/// <summary>
		/// 获得句柄 
		/// 参考 https://github.com/Intel-Media-SDK/MediaSDK/blob/master/doc/mediasdk-man.md#Memory_Allocation
		/// </summary>
		/// <param name="pthis">窗口句柄</param>
		/// <param name="mid">帧面ID</param>
		/// <param name="handle">句柄</param>
		/// <returns>返回mfxStatus</returns>
		mfxStatus mfx_gethdl(mfxHDL pthis, mfxMemId mid, mfxHDL* handle);

		/// <summary>
		/// 释放 
		/// 参考 https://github.com/Intel-Media-SDK/MediaSDK/blob/master/doc/mediasdk-man.md#mfxFrameAllocator
		/// </summary>
		/// <param name="pthis">窗口句柄</param>
		/// <param name="response">分配回复</param>
		/// <returns>返回mfxStatus</returns>
		mfxStatus mfx_free(mfxHDL pthis, mfxFrameAllocResponse* response);
	    

	}
}