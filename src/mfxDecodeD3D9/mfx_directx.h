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
		/// ����MFXHDL
		/// </summary>
		/// <param name="session">�Ự</param>
		/// <param name="deviceHandle">�յľ��</param>
		/// <param name="window">���ھ��������Ϊnullptr</param>
		/// <returns>�ɹ�����MFX_ERR_NONE</returns>
		mfxStatus CreateHWDevice(mfxSession session, mfxHDL* deviceHandle, HWND window);
		
		/// <summary>
		/// ������
		/// </summary>
		void CleanupHWDevice();

		/// <summary>
		/// �����ڴ� 
		/// �ο� https://github.com/Intel-Media-SDK/MediaSDK/blob/master/doc/mediasdk-man.md#Memory_Allocation
		/// </summary>
		/// <param name="pthis">���ھ��</param>
		/// <param name="request">��������</param>
		/// <param name="response">����ظ�</param>
		/// <returns>����mfxStatus</returns>
		mfxStatus mfx_alloc(mfxHDL pthis, mfxFrameAllocRequest* request, mfxFrameAllocResponse* response);

		/// <summary>
		/// ���� 
		/// �ο� https://github.com/Intel-Media-SDK/MediaSDK/blob/master/doc/mediasdk-man.md#Memory_Allocation
		/// </summary>
		/// <param name="pthis">���ھ��</param>
		/// <param name="request">��������</param>
		/// <param name="response">����ظ�</param>
		/// <returns>����mfxStatus</returns>
		mfxStatus mfx_lock(mfxHDL pthis, mfxMemId mid, mfxFrameData* ptr);

		/// <summary>
		/// ���� 
		/// �ο� https://github.com/Intel-Media-SDK/MediaSDK/blob/master/doc/mediasdk-man.md#Memory_Allocation
		/// </summary>
		/// <param name="pthis">���ھ��</param>
		/// <param name="mid">֡��ID</param>
		/// <param name="ptr">���֡��ָ��</param>
		/// <returns></returns>
		mfxStatus mfx_unlock(mfxHDL pthis, mfxMemId mid, mfxFrameData* ptr);

		/// <summary>
		/// ��þ�� 
		/// �ο� https://github.com/Intel-Media-SDK/MediaSDK/blob/master/doc/mediasdk-man.md#Memory_Allocation
		/// </summary>
		/// <param name="pthis">���ھ��</param>
		/// <param name="mid">֡��ID</param>
		/// <param name="handle">���</param>
		/// <returns>����mfxStatus</returns>
		mfxStatus mfx_gethdl(mfxHDL pthis, mfxMemId mid, mfxHDL* handle);

		/// <summary>
		/// �ͷ� 
		/// �ο� https://github.com/Intel-Media-SDK/MediaSDK/blob/master/doc/mediasdk-man.md#mfxFrameAllocator
		/// </summary>
		/// <param name="pthis">���ھ��</param>
		/// <param name="response">����ظ�</param>
		/// <returns>����mfxStatus</returns>
		mfxStatus mfx_free(mfxHDL pthis, mfxFrameAllocResponse* response);
	    

	}
}