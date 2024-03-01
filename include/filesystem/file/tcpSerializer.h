#pragma once
#include <SFML/Network/TcpSocket.hpp>
#include "optimization/debug.h"
#include <network/compressedPacket.h>
struct tcpSerializer {
	sf::TcpSocket* socket = nullptr;
	compressedPacket receivingPacket = compressedPacket();
	compressedPacket sendingPacket = compressedPacket();
	const char* receivingPacketPos = nullptr;
	const char* receivingPacketEndPos = nullptr;
	inline bool write(const char* const& value, const size_t& size)
	{
		sendingPacket.append(value, size);
		return true;
	}
	inline bool read(char* const& value, const size_t& size)
	{
		const auto newDataPos = receivingPacketPos + size;
		assumeInRelease(newDataPos <= receivingPacketEndPos);
		std::copy(receivingPacketPos, newDataPos, value);
		receivingPacketPos = newDataPos;
		return true;
	}
	sf::Socket::Status sendPacket() {
		const sf::Socket::Status& status = socket->send(sendingPacket);
		sendingPacket = compressedPacket();
		return status;
	}
	sf::Socket::Status receivePacket() {
		const sf::Socket::Status& status = socket->receive(receivingPacket);
		receivingPacketPos = (const char*)receivingPacket.getData();
		receivingPacketEndPos = receivingPacketPos + receivingPacket.getDataSize();
		return status;
	}
	//inline bool write(const char* const& value, const size_t& size) const
	//{
	//	size_t sentSize;
	//	if (socket->send(value, size, sentSize) == sf::Socket::Status::Done) {
	//		if (isDebugging && sentSize != size)
	//		{
	//			throw "received different data";
	//		}
	//		return true;
	//	}
	//	return false;
	//}
	//inline bool read(char* const& value, const size_t& size) const
	//{
	//	size_t receivedSize;
	//	if (socket->receive(value, size, receivedSize) == sf::Socket::Status::Done) {
	//		if (isDebugging && receivedSize != size)
	//		{
	//			throw "received different data";
	//		}
	//		return true;
	//	}
	//	return false;
	//}
};