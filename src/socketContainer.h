#include <filesystem/file/tcpSerializer.h>
#include "filesystem/file/endian.h"
#include "filesystem/serializer.h"
constexpr endianness defaultEndianness = endianness::littleEndian;
#pragma once
struct socketContainer :serializer<tcpSerializer> {
	tcpSerializer s = tcpSerializer();
	socketContainer() : serializer(s, false, defaultEndianness){}
};