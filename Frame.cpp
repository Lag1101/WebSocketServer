#include "Frame.h"

#include <ostream>
#include <iterator>
#include <limits>

/*!
 * 	\brief this is comment
 */
Frame Frame::decode(const data_t & data)
{
	Frame frame;

	frame.fin = (data[0] & (1<<7)) != 0;

	frame.rsv[0] = (data[0] & (1<<6)) != 0;
	frame.rsv[1] = (data[0] & (1<<5)) != 0;
	frame.rsv[2] = (data[0] & (1<<4)) != 0;

	frame.opcode = (unsigned char) (data[0] & 0x0f);
	frame.mask = (data[1] & (1<<7)) != 0;

	frame.bodyLength = (unsigned long long int) (data[1] & 0x7f);

	size_t shift = 2;

	if(frame.bodyLength == 126)
	{
		frame.bodyLength = *(unsigned short*)&data[2];
		shift += 2;
	}
	else if(frame.bodyLength == 127)
	{
		frame.bodyLength = *(unsigned short*)&data[2];
		shift += 8;
	}

	if(frame.mask)
	{
		frame.maskKey = *(unsigned int*)&data[shift];
		shift += 4;
	}

	auto begin = data.cbegin() + shift;
	frame.body = data_t(begin, begin + frame.bodyLength);

	if(frame.mask)
	{
		const unsigned char * maskPtr = (unsigned char *)&(frame.maskKey);
		for(size_t i = 0; i < frame.body.size(); i++)
			frame.body[i] ^= maskPtr[i % 4];
	}

	return frame;
}

Frame Frame::createTextFrame(const std::string & msg)
{
	Frame frame;
	frame.opcode = 0x1;
	frame.setBody(msg);
	return frame;
}

Frame::data_t Frame::getRaw() const
{

	data_t result(2);

	{
		result[0] = opcode;

		if(fin)		result[0] |= (1<<7);
		if(rsv[0])	result[0] |= (1<<6);
		if(rsv[1])	result[0] |= (1<<5);
		if(rsv[2])	result[0] |= (1<<4);
	}
	{
		result[1] = (unsigned char) bodyLength;

		if(bodyLength >= (2<<7) && bodyLength < (2<<16))
		{
			result[1] = 126;
			result.resize(4);
			*(unsigned short*)&result[2] = (unsigned short) bodyLength;
		} 
		else if(bodyLength < (std::numeric_limits<unsigned long long>::max()))
		{
			result[1] = 127;
			result.resize(10);
			*(unsigned long long*)&result[2] = bodyLength;
		}

		if(mask) result[1] |= (1<<7);
	}
	{
		if(mask)
		{
			const unsigned char * maskPtr = (unsigned char *)&maskKey;
			std::copy(maskPtr, maskPtr + 4, std::back_inserter(result));
		}
	}

	{
		std::copy(body.cbegin(), body.cend(), std::back_inserter(result));
		if(mask)
		{
			const unsigned char * maskPtr =  (unsigned char *)&maskKey;
			for(int i = 0; i < bodyLength; i++)
				result[result.size() - bodyLength + i] ^= maskPtr[i % 4];
		}
	}

	return result;
}

std::ostream & operator << (std::ostream & out, const Frame & frame)
{
	out << std::endl;
	out << "FIN:" << frame.getFin() << std::endl;
	out << "RSV:" << frame.getRsv() << std::endl;
	out << "OPCODE:" << (int)frame.getOpcode() << std::endl;

	out << "MASK:" << frame.getMask() << std::endl;
	out << "BODY_LENGTH:" << frame.getBodyLength() << std::endl;

	if(frame.getMask())
		out << "MASK_KEY:" << frame.getMaskKey() << std::endl;

	const auto & body = frame.getBody();
	out << "BODY:" << std::string(body.cbegin(),body.cend()) << std::endl;
	return out;
}