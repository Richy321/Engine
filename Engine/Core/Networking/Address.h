#pragma once
namespace networking
{
	class Address
	{
	public:

		unsigned int address;
		unsigned short port;

		Address()
		{
			address = 0;
			port = 0;
		}

		Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port)
		{
			this->address = (a << 24) | (b << 16) | (c << 8) | d;
			this->port = port;
		}

		Address(unsigned int address, unsigned short port)
		{
			this->address = address;
			this->port = port;
		}

		//accessors
		unsigned int GetAddress() const { return address; }
		unsigned char GetA() const{ return (unsigned char)(address >> 24); }
		unsigned char GetB() const{ return (unsigned char)(address >> 16); }
		unsigned char GetC() const{ return (unsigned char)(address >> 8); }
		unsigned char GetD() const{ return (unsigned char)address; }
		unsigned short GetPort() const { return port; }

		//comparators
		bool operator == (const Address & other) const
		{
			return address == other.address && port == other.port;
		}

		bool operator != (const Address & other) const
		{
			return !(*this == other);
		}

		std::string toString() const
		{
			char buffer[50];
			sprintf_s(buffer, "%d.%d.%d.%d:%d\n",
				GetA(), GetB(), GetC(), GetD(), GetPort());
			return std::string(buffer);
		}
	};
}

