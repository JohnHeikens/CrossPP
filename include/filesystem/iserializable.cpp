#include "iserializable.h"
#include "fileio.h"

void ISerializable::clone(ISerializable& cloneTo)
{
	std::stringstream* buf = new std::stringstream();
	streamSerializer s = streamSerializer(*buf, true);
	serialize(s);//save in string stream
	s.write = false;
	buf->seekp(0);//reset position
	cloneTo.serialize(s);//read from string stream
	delete buf;
}
bool ISerializable::serialize(const std::wstring& path, cbool& write, const endianness& fileEndianness)
{
	std::shared_ptr<char[]> buffer;
	std::fstream stream = getAcceleratedFstream(path, write, buffer, 0x1000);
	if(stream.good())
	{
		streamSerializer s = streamSerializer(stream, write, fileEndianness);
		serialize(s);
		stream.close();
		return true;
	}
	else 
	{
		return false;
	}
}