#include <fstream>
#include <iomanip>
#include <iterator>
#include <vector>

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		return 1;
	}

	std::ifstream is(argv[1], std::ifstream::binary);
	std::ofstream os("HEX.txt", std::ofstream::binary);

	if (is)
	{
		// Stop eating new lines in binary mode!!!
		is.unsetf(std::ios::skipws);

		// get its size:
		std::streampos fileSize;

		is.seekg(0, std::ios::end);
		fileSize = is.tellg();
		is.seekg(0, std::ios::beg);

		// reserve capacity
		std::vector<unsigned char> vec;
		vec.reserve(static_cast<size_t>(fileSize));

		// read the data:
		vec.insert(vec.begin(),
			std::istream_iterator<unsigned char>(is),
			std::istream_iterator<unsigned char>());


		for (size_t i = 0; i < vec.size() - 1; ++i)
		{
			if (vec[i] == 0x66 || vec[i] == 0x67)
			{
				continue;
			}

			os << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(vec[i]) << ", ";
				
			if (vec[i] == 0xC3)
			{
				os << "\r\n";
			}
		}
		os << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(vec[vec.size() - 1]);
	}

	os.close();
	is.close();
	return 0;
}
