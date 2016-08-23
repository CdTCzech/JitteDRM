#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <random>
#include <vector>

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		return 1;
	}

	std::random_device randomDevice;
	std::default_random_engine randomEngine(randomDevice());
	std::uniform_int_distribution<int> randomDistribution(0x21, 0x7E);

	std::ifstream inputStream(argv[1], std::ifstream::binary);
	std::ofstream outputStream("HEX.txt", std::ofstream::binary);

	if (inputStream)
	{
		// donn't skip newlines
		inputStream.unsetf(std::ios::skipws);

		// get file size
		inputStream.seekg(0, std::ios::end);
		auto fileSize = inputStream.tellg();
		inputStream.seekg(0, std::ios::beg);

		// reserve capacity
		std::vector<unsigned char> originalBinary;
		originalBinary.reserve(static_cast<size_t>(fileSize));

		// read the data:
		originalBinary.insert(originalBinary.begin(),
			std::istream_iterator<unsigned char>(inputStream),
			std::istream_iterator<unsigned char>());

		// strip 0x66 and 0x67 to new vector
		std::vector<unsigned char> binary;
		binary.reserve(static_cast<size_t>(fileSize));
		for (const auto character : originalBinary)
		{
			if (character != 0x66 && character != 0x67)
			{
				binary.push_back(character);
			}
		}

		// initialize key vector
		std::vector<unsigned char> key;
		key.resize(binary.size());
		std::generate(key.begin(), key.end(), [&]() { return randomDistribution(randomEngine); });

		// print original binary in hex, key and encrypted binary in hex
		outputStream << "ORIGINAL" << "\r\n";
		for (size_t i = 0; i < binary.size() - 1; ++i)
		{
			outputStream << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(binary[i]) << ", ";
				
			if (binary[i] == 0xC3)
			{
				outputStream << "\r\n";
			}
		}
		outputStream << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(binary[binary.size() - 1]);

		outputStream << "\r\n" << "KEY" << "\r\n";
		for (const auto keyChar : key)
		{
			outputStream << keyChar;
		}

		outputStream << "\r\n" << "ENCRYPTED" << "\r\n";
		for (size_t i = 0; i < binary.size() - 1; ++i)
		{
			outputStream << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(binary[i] ^ key[i]) << ", ";

			if ((i + 1) % 20 == 0)
			{
				outputStream << "\r\n";
			}
		}
		outputStream << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(binary[binary.size() - 1] ^ key[key.size() - 1]);
	}

	outputStream.close();
	inputStream.close();
	return 0;
}
