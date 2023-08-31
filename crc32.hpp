#ifndef CRC32_HPP
#define CRC32_HPP

#include <array>
#include <istream>

namespace Utilities
{
	/*
	* @author Was taken from:
	* @author https://github.com/Ares-Developers/Syringe/blob/master/CRC32.h
	* @author https://github.com/Ares-Developers/Syringe/blob/master/CRC32.cpp
	* @brief A simple CRC32 computator.
	*/
	class CRC32 final
	{
		static constexpr auto create_crc_table() noexcept
		{
			std::array<unsigned int, 256> ret{};

			for (size_t i = 0u; i < 256u; ++i) {
				auto value = i;

				for (auto j = 8u; j; --j) {
					// bit-reverse 0x04C11DB7U;
					auto const polynomial = (value & 1u) ? 0xEDB88320u : 0u;
					value = (value >> 1u) ^ polynomial;
				}

				ret[i] = value;
			}

			return ret;
		}
	public:
		unsigned int compute(void const* buffer, long long length) noexcept
		{
			static constexpr auto const crc_table = create_crc_table();

			auto const data = static_cast<unsigned char const*>(buffer);

			for (auto i = data; i < &data[length]; ++i) {
				auto const index = static_cast<unsigned char>((_value & 0xFFu) ^ *i);
				_value = (_value >> 8u) ^ crc_table[index];
			}

			return ~_value;
		}

		unsigned int value() const noexcept { return ~_value; }
		void reset() noexcept { _value = 0xFFFFFFFFU; }

		void dispose() { delete this; }
	private:
		unsigned int _value{ 0xFFFFFFFFU };
	public:
		static unsigned int compute_stream(std::istream& is)
		{
			CRC32 crc;
			is.clear(); // C++11 will automatically clear EOF bit when seekg calls, but C++17 and above willn't.

			is.seekg(0, std::istream::end);
			auto size = static_cast<size_t>(is.tellg());
			is.seekg(0, std::istream::beg);

			char buffer[0x1000];
			while (auto const read = is.read(buffer, std::size(buffer)).gcount())
				crc.compute(buffer, read);
			return crc.value();
		}
	};
}

#endif //UTILITIES_CRC32_HPP