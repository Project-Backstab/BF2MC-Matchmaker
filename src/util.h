#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>
#include <map>

/**
 * @brief Forward declaration of the MYSQL_TIME structure.
 * 
 * This forward declaration allows the use of the MYSQL_TIME structure in contexts where only the structure name needs to be known,
 * such as declaring pointers or references to MYSQL_TIME objects, without the need to include the full definition of the structure.
 * 
 * MYSQL_TIME is typically used to represent date and time values in MySQL-related operations.
 */
struct MYSQL_TIME;

namespace Util
{
	namespace Buffer
	{
		/**
		 * @brief Read a string from a buffer.
		 * 
		 * This function reads a string from a buffer at the specified offset.
		 * 
		 * @param buffer The buffer to read from.
		 * @param offset The offset in the buffer to start reading from.
		 * @param str The string to store the read value.
		 * @return True if the string was successfully read, false otherwise.
		 */
		bool ReadString(const std::vector<char>& buffer, size_t& offset, std::string &str);

		/**
		 * @brief Read a string from a buffer of unsigned chars.
		 * 
		 * This function reads a string from a buffer of unsigned chars at the specified offset.
		 * 
		 * @param buffer The buffer to read from.
		 * @param offset The offset in the buffer to start reading from.
		 * @param str The string to store the read value.
		 * @return True if the string was successfully read, false otherwise.
		 */
		bool ReadString(const std::vector<unsigned char>& buffer, size_t& offset, std::string &str);
		
		/**
		 * @brief Convert a buffer of chars to a string.
		 * 
		 * This function converts a buffer of chars to a string.
		 * 
		 * @param buffer The buffer of chars to convert.
		 * @return The resulting string.
		 */
		std::string ToString(const std::vector<char>& buffer);

		/**
		 * @brief Convert a buffer of unsigned chars to a string.
		 * 
		 * This function converts a buffer of unsigned chars to a string.
		 * 
		 * @param buffer The buffer of unsigned chars to convert.
		 * @return The resulting string.
		 */
		std::string ToString(const std::vector<unsigned char>& buffer);
	}
	
	namespace Url
	{
		/**
		 * @brief Type definition for URL variables.
		 */
		typedef std::map<std::string, std::string> Variables;
		
		/**
		 * @brief Extract elements from a URL.
		 * 
		 * This function extracts the URL base and variables from a given URL.
		 * 
		 * @param url The URL to extract elements from.
		 * @param url_base The base URL.
		 * @param url_variables The URL variables as a map.
		 */
		void GetElements(const std::string& url, std::string& url_base, Util::Url::Variables& url_variables);
		
		/**
		 * @brief Encode a string for URL.
		 * 
		 * This function encodes a string for URL usage.
		 * 
		 * @param str The string to encode.
		 * @return The encoded string.
		 */
		std::string Encode(const std::string& str);

		/**
		 * @brief Decode a URL-encoded string.
		 * 
		 * This function decodes a URL-encoded string.
		 * 
		 * @param str The URL-encoded string to decode.
		 * @return The decoded string.
		 */
		std::string Decode(const std::string& str);
	}
	
	namespace Time
	{
		/**
		 * @brief Get the current date and time.
		 * 
		 * This function retrieves the current date and time in the specified format.
		 * 
		 * @param format The format string for the date and time.
		 * @return The current date and time as a string.
		 */
		std::string GetNowDateTime(const std::string& format);

		/**
		 * @brief Get a formatted date and time from MYSQL_TIME.
		 * 
		 * This function formats a MYSQL_TIME structure into a date and time string.
		 * 
		 * @param datetime The MYSQL_TIME structure to format.
		 * @return The formatted date and time as a string.
		 */
		std::string GetDateTime(MYSQL_TIME datetime);

		/**
		 * @brief Get the current time zone.
		 * 
		 * This function retrieves the current time zone.
		 * 
		 * @return The current time zone as a string.
		 */
		std::string GetTimeZone();
	}
	
	namespace UTF8
	{
		/**
		 * @brief Checks if a given string is a valid UTF-8 encoded string.
		 *
		 * This function iterates over each byte in the string and verifies that
		 * it follows the UTF-8 encoding rules:
		 * - ASCII characters (0x00-0x7F) occupy a single byte.
		 * - Multi-byte sequences:
		 *   - 2-byte sequence starts with 110xxxxx followed by 10xxxxxx.
		 *   - 3-byte sequence starts with 1110xxxx followed by two 10xxxxxx.
		 *   - 4-byte sequence starts with 11110xxx followed by three 10xxxxxx.
		 *
		 * @param str The input string to be checked.
		 * @return true if the string is a valid UTF-8 encoded string, false otherwise.
		 */
		bool isValid(const std::string& str);
	}

	/**
	 * @brief Calculate the MD5 hash of a string.
	 * 
	 * @param input The input string to hash.
	 * @return The MD5 hash as a string.
	 */
	std::string MD5hash(const std::string& input);
	
	/**
	 * @brief Convert a list of integers to a string.
	 * 
	 * @param list The list of integers to convert.
	 * @return The converted string.
	 */
	std::string ToString(const std::vector<int>& list);

	/**
	 * @brief Generates a random challenge string.
	 * 
	 * This function generates a random challenge string of length 10 using a set of alphanumeric characters.
	 * 
	 * @return A randomly generated challenge string.
	 */
	std::string generateRandomChallenge();

	/**
	 * @brief Generates a random authentication token.
	 * 
	 * This function generates a random authentication token of length 24 using a set of alphanumeric characters.
	 * 
	 * @return A randomly generated authentication token.
	 */
	std::string generateRandomAuthtoken();

	/**
	 * @brief Convert a profile ID string to a vector of integers.
	 * 
	 * @param input The profile ID string to convert.
	 * @return The converted vector of integers.
	 */
	std::vector<int> convertProfileIdToVector(const std::string& input);

	/**
	 * @brief Count the number of set bits in a 32-bit integer.
	 * 
	 * @param num The input number.
	 * @return The number of set bits.
	 */
	int countSetBits(uint32_t num);

	std::string tolower(const std::string &str);
}

#endif // UTIL_H
