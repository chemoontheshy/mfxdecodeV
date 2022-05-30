#include "mfx_decode.h"


namespace vsnc
{
	namespace mfx
	{
		static constexpr size_t PROBE_SIZE = 1024 * 64;
		static constexpr mfxU32 DEC_WAIT_INTERVAL = 300000;
		static size_t num = 0;
	}
}


vsnc::mfx::MFXDecoder::MFXDecoder(bool hardward) :
	m_iIndex(0),
	m_bHardware(hardward),
	m_bInitialized(false)
{
	mfxIMPL imp;
	(m_bHardware) ? imp = MFX_IMPL_HARDWARE : imp = MFX_IMPL_AUTO;
	mfxVersion ver = { {4,1} };
	throw_if_negative(m_iMfxSession.Init(imp, &ver));
	m_iMfxDEC = std::make_unique<MFXVideoDECODE>(m_iMfxSession);
	m_iBitStreamAlloc.reserve(PROBE_SIZE);
	memset(&m_iMfxBitStream, 0, sizeof(m_iMfxBitStream));
	m_iMfxBitStream.Data = m_iBitStreamAlloc.data();
	m_iMfxBitStream.MaxLength = static_cast<mfxU32>(m_iBitStreamAlloc.max_size());
	memset(&m_iMfxVideoParams, 0, sizeof(m_iMfxVideoParams));
	//m_iMfxVideoParams.mfx.CodecId = MFX_CODEC_AVC;
	m_iMfxVideoParams.mfx.CodecId = MFX_CODEC_HEVC;
	/*
	* enum {
	*   /// <summary>输入显存</summary>
	*	MFX_IOPATTERN_IN_VIDEO_MEMORY   = 0x01,
	*   /// <summary>输入系统内存</summary>
	*	MFX_IOPATTERN_IN_SYSTEM_MEMORY  = 0x02,
	*   /// <summary>输入到任何一个系统内存或者视频内存表面</summary>
	*	MFX_IOPATTERN_IN_OPAQUE_MEMORY  = 0x04,
	*   /// <summary>输出显存</summary>
	*	MFX_IOPATTERN_OUT_VIDEO_MEMORY  = 0x10,
	*   /// <summary>输出系统内存</summary>
	*	MFX_IOPATTERN_OUT_SYSTEM_MEMORY = 0x20,
	*   /// <summary>输出到任何一个系统内存或者视频内存表面</summary>
	*	MFX_IOPATTERN_OUT_OPAQUE_MEMORY = 0x40
	*	};
	*/
	(m_bHardware) ? m_iMfxVideoParams.IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY : m_iMfxVideoParams.IOPattern = MFX_IOPATTERN_OUT_SYSTEM_MEMORY;
	//缓存多少帧
	m_iMfxVideoParams.AsyncDepth = 1;

	//解出一帧马上输出
	m_iMfxVideoParams.mfx.DecodedOrder = true;

	//创建DIrectX device 上下文
	throw_if_negative(CreateHWDevice(m_iMfxSession, &m_iMfxDeviceHandle, nullptr));
	throw_if_negative(m_iMfxSession.SetHandle(MFX_HANDLE_DIRECT3D_DEVICE_MANAGER9, m_iMfxDeviceHandle));
	m_iMfxAllocator.Alloc = mfx_alloc;
	m_iMfxAllocator.Free = mfx_free;
	m_iMfxAllocator.Lock = mfx_lock;
	m_iMfxAllocator.Unlock = mfx_unlock;
	m_iMfxAllocator.GetHDL = mfx_gethdl;
	m_iMfxSession.SetFrameAllocator(&m_iMfxAllocator);
}


vsnc::mfx::MFXDecoder::~MFXDecoder()
{

	CleanupHWDevice();
	for (size_t i = 0; i < m_u16NumSurfaces; i++)
	{
		delete m_pMfxFrameSurFaceHDL[i];
	}
	if (m_pMfxFrameSurFaceHDL)
	{
		delete[] m_pMfxFrameSurFaceHDL;
	}
	m_iMfxDEC->Close();
}


bool vsnc::mfx::MFXDecoder::GetPacket(D3DLOCKED_RECT& rect)
{
	if (!m_bInitialized)
	{
		if (!__init_decoder()) return false;
	}
	mfxFrameSurface1* surfaceOut = nullptr;
	mfxSyncPoint syncp;
	mfxStatus sts = MFX_ERR_NONE;
	do 
	{
		if (!m_bHardware)
		{
			sts = m_iMfxDEC->DecodeFrameAsync(&m_iMfxBitStream, &m_iMfxFrameSurface, &surfaceOut, &syncp);
		}
		else
		{
			sts = m_iMfxDEC->DecodeFrameAsync(&m_iMfxBitStream, m_pMfxFrameSurFaceHDL[m_iIndex], &surfaceOut, &syncp);
		}
		
	} while (MFX_ERR_NONE < sts);
	switch (sts)
	{
		case MFX_ERR_NONE:
		{

			if (MFX_ERR_NONE == m_iMfxSession.SyncOperation(syncp, DEC_WAIT_INTERVAL))
			{
				num++;
				//std::cout << "finish decode " << num << " frame" << std::endl;
				//出来的是NV12格式的图像
				if (!m_bHardware)
				{
					rect.pBits= m_iMfxFrameSurface.Data.Y;
					rect.Pitch = m_iMfxFrameSurface.Data.Pitch;
				}
				else
				{
					//std::cout << "finish decode " << num << " frame" << std::endl;
					//出来的是NV12格式的图像
					m_iMfxAllocator.Lock(m_iMfxAllocator.pthis, surfaceOut->Data.MemId, &(surfaceOut->Data));

					rect.pBits = surfaceOut->Data.Y;
					rect.Pitch = surfaceOut->Data.Pitch;

					m_iMfxAllocator.Unlock(m_iMfxAllocator.pthis, surfaceOut->Data.MemId, &(surfaceOut->Data));
				}
			}
			return true;
		}
		case MFX_ERR_MORE_SURFACE:
		{
			if (m_bHardware)
			{
				auto __get_free_surface = [](mfxFrameSurface1** pSurfacesPool, mfxU16 nPoolSize)->int
				{
					if (pSurfacesPool)
						for (mfxU16 i = 0; i < nPoolSize; i++)
							if (0 == pSurfacesPool[i]->Data.Locked)
								return i;
					return MFX_ERR_NOT_FOUND;
				};
				m_iIndex = __get_free_surface(m_pMfxFrameSurFaceHDL, m_u16NumSurfaces);
				if (MFX_ERR_NOT_FOUND == m_iIndex)
				{
					std::cout << "no more buff" << std::endl;
					exit(-1);
				}
			}
			return false;
		}
		default:
		{
			break;
		}
		std::cout << "decode failed. sts" << sts << std::endl;
		exit(-1);
	}
	return false;
}

bool vsnc::mfx::MFXDecoder::GetPacketHDL(D3DLOCKED_RECT& rect)
{

	if (!m_bInitialized)
	{
		if (!__init_decoder()) return false;
	}
	mfxFrameSurface1* surfaceOut = nullptr;
	mfxSyncPoint syncp;
	mfxStatus sts = MFX_ERR_NONE;
	do
	{
		sts = m_iMfxDEC->DecodeFrameAsync(&m_iMfxBitStream, m_pMfxFrameSurFaceHDL[m_iIndex], &surfaceOut, &syncp);

	} while (MFX_ERR_NONE < sts );
	switch (sts)
	{
	case MFX_ERR_NONE:
	{

		if (MFX_ERR_NONE == m_iMfxSession.SyncOperation(syncp, DEC_WAIT_INTERVAL))
		{
			num++;
			//std::cout << "finish decode " << num << " frame" << std::endl;
			//出来的是NV12格式的图像
			m_iMfxAllocator.Lock(m_iMfxAllocator.pthis, surfaceOut->Data.MemId, &(surfaceOut->Data));

			rect.pBits = surfaceOut->Data.Y;
			rect.Pitch = surfaceOut->Data.Pitch;

			m_iMfxAllocator.Unlock(m_iMfxAllocator.pthis, surfaceOut->Data.MemId, &(surfaceOut->Data));

		}
		return true;
	}
	case MFX_ERR_MORE_DATA:
	{
		return false;
	}
	case MFX_ERR_MORE_SURFACE:
	{
		auto __get_free_surface = [](mfxFrameSurface1** pSurfacesPool, mfxU16 nPoolSize)->int
		{
			if (pSurfacesPool)
				for (mfxU16 i = 0; i < nPoolSize; i++)
					if (0 == pSurfacesPool[i]->Data.Locked)
						return i;
			return MFX_ERR_NOT_FOUND;
		};
		m_iIndex = __get_free_surface(m_pMfxFrameSurFaceHDL, m_u16NumSurfaces);
		if (MFX_ERR_NOT_FOUND == m_iIndex)
		{
			std::cout << "no more buff" << std::endl;
			exit(-1);
		}
		return false;
	}
	default:
	{
		break;
	}
	std::cout << "decode failed. sts" << sts << std::endl;
	exit(-1);
	}
	return false;
}

bool vsnc::mfx::MFXDecoder::FillMFXBitStream(const Packet& packet)
{
	if (m_iMfxBitStream.DataOffset)
	{
		memmove(m_iMfxBitStream.Data, m_iMfxBitStream.Data + m_iMfxBitStream.DataOffset, m_iMfxBitStream.DataLength);
		m_iMfxBitStream.DataOffset = 0;
	}
	memcpy(m_iMfxBitStream.Data+m_iMfxBitStream.DataLength, packet.Data, packet.Length);
	m_iMfxBitStream.DataLength += static_cast<mfxU32>(packet.Length);
	return true;
}

bool vsnc::mfx::MFXDecoder::__init_decoder()
{
	if (m_iMfxBitStream.DataLength <= 0) return false;
	auto sts = m_iMfxDEC->DecodeHeader(&m_iMfxBitStream, &m_iMfxVideoParams);
	// "MfxBitStream no enough"
	if (MFX_ERR_NONE != sts) return false;
	
	mfxFrameAllocRequest requeset;
	memset(&requeset, 0, sizeof(requeset));
	throw_if_negative(m_iMfxDEC->QueryIOSurf(&m_iMfxVideoParams, &requeset));
	throw_if_negative(m_iMfxDEC->Init(&m_iMfxVideoParams));

	if (!m_bHardware)
	{
		//内存-start
		auto width = m_iMfxVideoParams.mfx.FrameInfo.Width;
		auto height = m_iMfxVideoParams.mfx.FrameInfo.Height;
		m_iFrameAlloc.reserve(width * height * 3 / 2);
		m_iMfxFrameSurface.Data.Y = m_iFrameAlloc.data();
		m_iMfxFrameSurface.Data.U = m_iMfxFrameSurface.Data.Y + (static_cast<mfxU32>(width) * static_cast<mfxU32>(height));
		m_iMfxFrameSurface.Data.V = m_iMfxFrameSurface.Data.U + 1;
		m_iMfxFrameSurface.Info = m_iMfxVideoParams.mfx.FrameInfo;
		m_iMfxFrameSurface.Data.Pitch = width;
		//内存-end
	}
	else
	{
		throw_if_negative(m_iMfxAllocator.Alloc(m_iMfxAllocator.pthis, &requeset, &m_iMfxResponse));
		m_u16NumSurfaces = m_iMfxResponse.NumFrameActual;

		m_pMfxFrameSurFaceHDL = new mfxFrameSurface1 * [m_u16NumSurfaces];
		throw_if_nullptr(m_pMfxFrameSurFaceHDL);
		for (int i = 0; i < m_u16NumSurfaces; i++)
		{
			m_pMfxFrameSurFaceHDL[i] = new mfxFrameSurface1;
			memset(m_pMfxFrameSurFaceHDL[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(m_pMfxFrameSurFaceHDL[i]->Info), &(m_iMfxVideoParams.mfx.FrameInfo), sizeof(mfxFrameInfo));
			m_pMfxFrameSurFaceHDL[i]->Data.MemId = m_iMfxResponse.mids[i]; // MID (memory id) represent one D3D NV12 surface  
		}
	}
	m_bInitialized = true;
	return true;
}
