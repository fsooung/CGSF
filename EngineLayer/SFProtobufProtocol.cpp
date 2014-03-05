#include "stdafx.h"
#include "SFProtobufProtocol.h"
#include "SFProtobufPacketImpl.h"
#include <EngineInterface/ISession.h>

#define SignatureStart 16018
#define SignatureEnd 16108


#define nReceiveBufferSize 1024 * 8/*8192*/
#define nSendBufferSize 1024 * 8/*8192*/
SFProtobufProtocol::SFProtobufProtocol(void)
	:oBuffer(nSendBufferSize), m_Buffer(nReceiveBufferSize)
{
	Initialize();
}


SFProtobufProtocol::~SFProtobufProtocol(void)
{

}

BOOL SFProtobufProtocol::Initialize()
{

	return TRUE;
}

BOOL SFProtobufProtocol::AddTransferredData(char* pBuffer, DWORD dwTransferred)
{
	m_Buffer.Append(pBuffer,dwTransferred);

	return TRUE;
}

BOOL SFProtobufProtocol::Reset()
{
	return TRUE;
}

/*BOOL SFProtobufProtocol::SendRequest(ISession* pSession, BasePacket* pPacket)
{
	unsigned int uWrittenBytes = 0;
	int iResult = serializeOutgoingPacket(*pPacket, oBuffer, uWrittenBytes);
	if (iResult!= SFProtocol::Success)
	{
		return false;
	}

	unsigned int uSize = oBuffer.GetDataSize();
	pSession->SendInternal(oBuffer.GetBuffer(), uSize);

	oBuffer.Pop(uSize);

	return true;
}*/

bool SFProtobufProtocol::GetPacketData(BasePacket* pPacket, char* buffer, const int BufferSize, unsigned int& writtenSize)
{
	unsigned int uWrittenBytes = 0;
	int iResult = serializeOutgoingPacket(*pPacket, oBuffer, uWrittenBytes);
	
	if (iResult!= SFProtocol::Success)
	{
		return false;
	}

	unsigned int uSize = oBuffer.GetDataSize();

	if(uSize == 0)
		return false;

	memcpy(buffer, oBuffer.GetBuffer(), uSize);
	writtenSize = uSize;

	oBuffer.Pop(uSize);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
int SFProtobufProtocol::decodeIncomingPacket( BasePacket* pPacket, int& serviceId )
{
	return SFProtocol::Success;
}

void SFProtobufProtocol::disposeOutgoingPacket( BasePacket* pPacket )
{
	delete pPacket;
}

void SFProtobufProtocol::disposeIncomingPacket( BasePacket* pPacket )
{
	delete pPacket;
}

int SFProtobufProtocol::tryDeframeIncomingPacket( DataBuffer& Buffer, BasePacket*& pPacket, int& PacketId, unsigned int& nExtractedBytes )
{
	if(Buffer.GetDataSize() < 8)
		return SFProtocol::eIncompletePacket;


    char* pBuffer = Buffer.GetBuffer();

    unsigned int sStart = 0;
    unsigned int packetLen = 0;
    PacketId = 0;
    unsigned int sEnd = 0;

    for (int i=0; i<2; i++)
    {
        *((BYTE*)(&sStart)+i)=pBuffer[i];
        *((BYTE*)(&packetLen)+i)=pBuffer[i+2];
        *((BYTE*)(&PacketId)+i)=pBuffer[i+4];
    }

    if (sStart!=SignatureStart)
        return SFProtocol::eCorruptPacket;

    if (packetLen > Buffer.GetDataSize())
        return SFProtocol::eIncompletePacket;

    for (int i=0; i<2; i++)
        *((BYTE*)(&sEnd)+i)=pBuffer[packetLen-2+i];

    if(sEnd!=SignatureEnd)
        return SFProtocol::eCorruptPacket;

    char* pData = pBuffer + 6;
    unsigned int dataSize = packetLen - 8;

    nExtractedBytes = packetLen;

	pPacket = CreateIncomingPacketFromPacketId(PacketId);

	if(pPacket==NULL)
		return SFProtocol::eUndefinedFailure;

	if(!pPacket->Decode(pData, dataSize))
	{
		disposeIncomingPacket(pPacket);
		return SFProtocol::eDecodingFailure;
	}

	return SFProtocol::Success;
}

BasePacket* SFProtobufProtocol::GetPacket(int& ErrorCode)
{
	//The Processing Loop.
	int uCommandID;
	BasePacket* pPacket = NULL;
	int iResult;
	unsigned int uExtractedBytes;

	ErrorCode = 0;

	//watch.GetElapsedTime(false);
	iResult = tryDeserializeIncomingPacket(m_Buffer , pPacket, uCommandID, uExtractedBytes);
	//iResult = pProtocol->tryDeframeIncomingPacket(pChannel->GetReceiveBuffer().GetBuffer(), pChannel->GetReceiveBuffer().GetDataSize(),uCommandID, pPacket, uExtractedBytes);
	//wcout << L"Packet deframed : " << watch.GetElapsedTime(false) << std::endl;
	if (iResult == SFProtocol::Success)
	{
		m_Buffer.Pop(uExtractedBytes);
		pPacket->SetPacketID(uCommandID);
	}
	else if (iResult == SFProtocol::eDecodingFailure)
	{
		m_Buffer.Pop(uExtractedBytes);
		ErrorCode = -1;

		return NULL;
	}
	
	return pPacket;

}

int SFProtobufProtocol::encodeOutgoingPacket( BasePacket& packet )
{
	SFProtobufPacketImpl& gPacket = (SFProtobufPacketImpl&) packet;

	return gPacket.Encode() ? SFProtocol::Success : SFProtocol::eEncodingFailure;
}

int SFProtobufProtocol::frameOutgoingPacket( BasePacket& packet, DataBuffer& buffer, unsigned int& nWrittenBytes )
{
	SFProtobufPacketImpl& gPacket = (SFProtobufPacketImpl&) packet;

	nWrittenBytes = 6 + gPacket.getEncodedStream()->size() + 2;

	if(nWrittenBytes > buffer.getRemainingSize())
		return SFProtocol::eInsufficientBuffer;
	//
	unsigned int sStart = SignatureStart;
	unsigned int packetLen = gPacket.getEncodedStreamSize() + 8;
	unsigned int commandID = gPacket.getServiceId();
	unsigned int sEnd = SignatureEnd;

	buffer.Append((char*) &sStart, 2);
	buffer.Append((char*)&packetLen, 2);
	buffer.Append((char*)&commandID, 2);

	buffer.Append((char*)gPacket.getEncodedStream()->c_str(), gPacket.getEncodedStreamSize());

	buffer.Append((char*)&sEnd, 2);


	return SFProtocol::Success;
	
}