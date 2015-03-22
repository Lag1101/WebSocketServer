#pragma once

#include <bitset>
#include <vector>


class Frame
{
public:
	typedef std::vector<unsigned char> data_t;

	static Frame decode(const data_t & data);
	static Frame createTextFrame(const std::string & msg);
	data_t getRaw() const;


	inline bool							getFin() const			{return fin;}
	inline std::bitset<3>				getRsv() const			{return rsv;}
	inline unsigned char				getOpcode() const		{return opcode;}
	inline bool							getMask() const			{return mask;}
	inline unsigned long long			getBodyLength() const	{return bodyLength;}
	inline unsigned int					getMaskKey() const		{return maskKey;}
	inline const data_t &				getBody() const			{return body;}

	friend std::ostream & operator << (std::ostream & out, const Frame & frame);


private:
	Frame() : fin(true), opcode(1), mask(false), bodyLength(0), maskKey(0) {}

	template<class T>
	inline void setBody(const T & data)
	{
		body.resize(0);
		body.assign(data.cbegin(), data.cend());
		setBodyLength(body.size());
	}
	inline void setBodyLength(unsigned long long bodyLength) {this->bodyLength = bodyLength;}


	bool fin;
	std::bitset<3> rsv;
	unsigned char opcode;
	bool mask;
	unsigned long long bodyLength;
	unsigned int maskKey;
	data_t	body;
};

