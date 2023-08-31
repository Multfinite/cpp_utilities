#ifndef LIBSODIUM_HPP
#define LIBSODIUM_HPP

#include <string>
#include <cmath>

#include <sodium.h>

#include "exceptions.hpp"

namespace Utilities
{
class libsodium
{
	public:
		struct init_failed : public Exceptions::base_error
		{
			int Code;
	
			init_failed(std::string function, std::string file, int line, int code) :Exceptions::base_error("Libsodium init failed", function, file, line), Code(code) {}
			init_failed(std::string msg, std::string function, std::string file, int line, int code) : Exceptions::base_error(msg, function, file, line), Code(code) { }
		};
	private:
		int _code;
	public:
		libsodium()
		{
			_code = sodium_init();
			if (_code < 0)
				throw new_base_error_args_no_msg(init_failed, _code);
	
			auto pk_len = crypto_box_PUBLICKEYBYTES;
			auto sk_len = crypto_box_SECRETKEYBYTES;
		}
		~libsodium() = default;
	
		/* https://doc.libsodium.org/public-key_cryptography/sealed_boxes */
	
		void generate_key_pair(std::string& publicKey, std::string& privateKey)
		{
			publicKey	.resize(crypto_box_PUBLICKEYBYTES);
			privateKey	.resize(crypto_box_SECRETKEYBYTES);
			crypto_box_keypair((uint8_t*) publicKey.data(), (uint8_t*) privateKey.data());
		}
	
		std::string encrypt(std::string& message, std::string& publicKey)
		{
			std::string encryptedMessage{};
			encryptedMessage.resize(crypto_box_SEALBYTES + message.size());
	
			auto code = crypto_box_seal(
				reinterpret_cast<unsigned char*>			(encryptedMessage.data())	, 
				reinterpret_cast<const unsigned char*>	(message.c_str())				, message.size(),
				reinterpret_cast<const unsigned char*>	(publicKey.c_str())
			);
	
			return encryptedMessage;
		}
		std::string decrypt(std::string& encrypredMessage, std::string& publicKey, std::string& secretKey)
		{
			std::string decryptedMessage{};
			decryptedMessage.resize(encrypredMessage.size() - crypto_box_SEALBYTES);
	
			auto code = crypto_box_seal_open(
				reinterpret_cast<unsigned char*>			(decryptedMessage.data()),
				reinterpret_cast<const unsigned char*>	(encrypredMessage.c_str()), encrypredMessage.size(),
				reinterpret_cast<const unsigned char*>	(publicKey.c_str()),
				reinterpret_cast<const unsigned char*>	(secretKey.c_str())
			);
	
			if (code != 0)
				throw "TODO: IMPLEMENT LIBSODIUM DECRYPTION ERROR";
	
			return decryptedMessage;
		}
	
		/* https://doc.libsodium.org/advanced/ed25519-curve25519 */
	
		std::string sign(std::string& message, std::string& publicKey) { throw;  }
	
		std::string to_base64(std::string binary, int var)
		{
			std::string b64; 
			b64.resize(sodium_base64_encoded_len(binary.size(), var));
			auto encoded = sodium_bin2base64(b64.data(), b64.size(), (uint8_t*) binary.data(), binary.length(), var);
			if (encoded == nullptr)
				throw "TODO: IMPLEMENT LOBSODIUM BASE64 ENCODE ERROR";
			return b64;
		}
		std::string from_base64(std::string b64, int var)
		{
			size_t binMaxSize = std::ceil(3 * b64.size() / 4);
	
			std::string binary;
			binary.resize(binMaxSize);
	
			size_t binLen = 0;
			auto decoded = sodium_base642bin(
				(uint8_t*) binary.data(), binary.size(),
				b64.c_str(), b64.size(),
				"", &binLen, nullptr,
				var
			);
	
			binary.resize(binLen);
			return binary;
		}
	
		int get_init_code() const { return _code; }
	
		void test(std::string msg)
		{
			std::string pk, sk;
			generate_key_pair(pk, sk);
			auto enc = encrypt(msg, pk);
			auto dec = decrypt(enc, pk, sk);
	
			if (msg != dec)
				throw "LIBSODIUM: INVALID CRYPTOGRAPHY RESULT!";
	
			auto b64 = to_base64(enc, sodium_base64_VARIANT_URLSAFE);
			auto bin = from_base64(b64, sodium_base64_VARIANT_URLSAFE);
	
			if (bin != enc)
				throw "LIBSODIUM: INVALID BASE64 ENCODE/DECODE RESULT!";
		}
	};
}

#endif