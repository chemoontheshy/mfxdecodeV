/*
 *
 * Copyright (C) chemoontheshy
 * File name: // splitnalu
 * Author: chemoontheshy
 * Version: v1.0
 * Date: 2021.11.2
 * Description: 主要用于把H264/H265分成一个个nalu包
 */
#include "splitnalu.h"
#include <fstream>
using namespace vsnc::vnal;


Parser::Parser(std::string filename)
{
	// 以二进制格式打开
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		std::cout << "can't open the file:" + filename << std::endl;
		exit(-1);
	}
	// 把指针移动到文件最后面
	file.seekg(0, file.end);

	// 获取文件的长度
	m_u32FileSize = file.tellg();
	std::cout << m_u32FileSize << std::endl;
	// 把指针移动到文件最开始
	file.seekg(0, std::ios::beg);

	//把文件写到缓冲区里
	m_pBuffer = new char[m_u32FileSize + 1];
	file.read(&m_pBuffer[0], m_u32FileSize);
	//判读文件是否全部写道缓冲区
	if (file)
	{
		std::cout << "all characters read successfully.";
	}
	else
	{
		std::cout << "error:only " << file.gcount() << "could be read";
	}
	file.close();
	m_pStartPos = m_pCurrentPos = reinterpret_cast<ptr>(m_pBuffer);
	m_pEndPos = m_pStartPos + m_u32FileSize;
}

Parser::~Parser() noexcept
{
	delete[] m_pBuffer;
}

int Parser::CheckNaluHead(const ptr& head) noexcept
{
	//if (*head == 0x00 && *(head + 1) == 0x00 && *(head + 2) == 0x01) return 3;
    if (*head == 0x00 && *(head + 1) == 0x00 && *(head + 2) == 0x00 && *(head + 3) == 0x01) return 4;
	return -1;
}

Nalu Parser::GetNextNalu()
{
	Nalu nalu;
	len currentSize = m_pEndPos - m_pCurrentPos;
	if (currentSize == 0) return nalu;
	//检查是否有新的Nalu
	if (CheckNaluHead(m_pCurrentPos) < 0)
	{
		std::cout << "Parser::GetNextNula() failed :file not start with startcode" << std::endl;
		return nalu;
	}
	auto nextStartCode = FindNextStartCode(m_pCurrentPos + 3, currentSize - 3);
	if (!nextStartCode)
	{
		return nalu;
	}
	len naluSize = static_cast<len>(nextStartCode - m_pCurrentPos);
	nalu.Head = m_pCurrentPos;
	nalu.Length = naluSize;
	m_pCurrentPos += naluSize;
	return nalu;
}

ptr vsnc::vnal::Parser::FindNextStartCode(ptr head, len size) noexcept
{
	for (len i = 0; i < size - 3; i++)
	{  
		if (CheckNaluHead(head) > 0) return head;
		head++;
	}
	return CheckNaluHead(head) == 3 ? head : nullptr;
}
