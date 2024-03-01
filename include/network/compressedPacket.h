#include "SFML/Network.hpp"
#include <SFML/Network/Packet.hpp>
#include "../../gzip/compress.hpp"
#pragma once
struct compressedPacket : public sf::Packet
{
private:
    std::string compressedPacket;//this will keep the c_str pointer alive as long as it's needed
    virtual const void* onSend(std::size_t& size) override
    {
        const void* srcData = getData();
        std::size_t srcSize = getDataSize();
        compressedPacket = gzip::compress((const char*)srcData, srcSize, 1);
        size = compressedPacket.size();
        return compressedPacket.data();

    }
    virtual void onReceive(const void* data, std::size_t size) override
    {
        std::string result = gzip::decompress((const char*)data, size);
        append(result.data(), result.size());
    }
};