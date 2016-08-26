#include <Windows.h>

#include <iostream>
#include <string>

// decrypted function pointers
typedef long(*l_l_func)(long);
typedef long(*l_l_l_func)(long, long);


void* allocExecutableMemory(size_t size)
{
	void* ptr = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (ptr == nullptr)
	{
		std::cout << "Allocation error!" << std::endl;
		return nullptr;
	}
	return ptr;
}

__declspec(noinline) long square(long num)
{
	return num * num;
}

bool emitCodeIn(unsigned char* m, const std::string password)
{
	unsigned char code[] =
	{
		0x15, 0xc5, 0xa6, 0xf0, 0x17, 0x4e, 0x28, 0xa7, 0xff, 0x8f, 0xa4, 0xbe, 0x2c, 0x9b, 0x31, 0xbe, 0xd0, 0xdb, 0x37, 0x3c,
		0x71, 0x22, 0xcf, 0xc2, 0x76, 0xbb, 0x2c, 0xa5, 0xc6, 0xaf, 0x76, 0x38, 0x6a, 0xff, 0x98, 0xcc, 0xe6, 0xab, 0x9b, 0x35,
		0x82, 0x04, 0xa0, 0xca, 0x81, 0x51, 0x36, 0x79, 0x2a, 0xde, 0x82, 0x70, 0x9e, 0x68, 0xd9, 0x81, 0xd7, 0x17, 0x79, 0x6d,
		0x08, 0x5c, 0xa0, 0xf7, 0xc7, 0xb2, 0x38, 0xf1, 0x7c, 0xb4, 0xae, 0xb2, 0x62, 0x43, 0x16, 0x6c, 0x74, 0xf7, 0xa1, 0xd7,
		0x96, 0x0a, 0xb2
	};

	size_t codeSize = sizeof(code);

	if (codeSize == password.size()) {
		for (size_t i = 0; i < codeSize; ++i)
		{
			code[i] ^= password[i];
		}
	}
	else
	{
		std::cout << "Invalid password length! Exiting..." << std::endl;
		return false;
	}

	memcpy(m, code, codeSize);
	return true;
}

void printHeader()
{
	std::cout << R"(JitteDRM v.20160826 compiled with Microsoft Visual Studio "15" )" << _MSC_FULL_VER << std::endl;
}

int main()
{
	printHeader();

	std::string password;
	std::cout << "Password: " << std::endl;
	std::getline(std::cin, password);

	// encrypt code
	void* memory1 = allocExecutableMemory(1024);
	if (emitCodeIn(static_cast<unsigned char*>(memory1), password))
	{
		int count1;
		std::cout << "Count 1: " << std::endl;
		std::cin >> count1;

		int count2;
		std::cout << "Count 2: " << std::endl;
		std::cin >> count2;

		int d = square(count1);

		// get memory pointers
		void* memory2 = static_cast<unsigned char*>(memory1) + 14;
		void* memory3 = static_cast<unsigned char*>(memory2) + 12;
		void* memory4 = static_cast<unsigned char*>(memory3) + 15;
		void* memory5 = static_cast<unsigned char*>(memory4) + 12;
		void* memory6 = static_cast<unsigned char*>(memory5) + 15;

		// get functions
		l_l_func addF = static_cast<l_l_func>(memory1);
		l_l_func subF = static_cast<l_l_func>(memory2);
		l_l_func mulF = static_cast<l_l_func>(memory3);
		l_l_func divF = static_cast<l_l_func>(memory4);
		l_l_l_func add2F = static_cast<l_l_l_func>(memory5);
		l_l_l_func sub2F = static_cast<l_l_l_func>(memory6);

		int result1, result2, result3, result4, result5, result6;
		__asm
		{
			push	count1
			call	addF
			pop		edx
			mov		result1, eax

			push	count1
			call	subF
			pop		edx
			mov		result2, eax

			push	count1
			call	mulF
			pop		edx
			mov		result3, eax

			push	count1
			call	divF
			pop		edx
			mov		result4, eax

			push	count2
			push	count1
			call	add2F
			pop		edx
			pop		edx
			mov		result5, eax

			push	count2
			push	count1
			call	sub2F
			pop		edx
			pop		edx
			mov		result6, eax
		}
		std::cout << count1 << " + " << count1 << " = " << result1 << std::endl;
		std::cout << count1 << " - " << count1 << " = " << result2 << std::endl;
		std::cout << count1 << " * " << count1 << " = " << result3 << std::endl;
		std::cout << count1 << " / " << count1 << " = " << result4 << std::endl;
		std::cout << count1 << " + " << count2 << " = " << result5 << std::endl;
		std::cout << count1 << " - " << count2 << " = " << result6 << std::endl;
		std::cout << d;
	}

	system("pause");
	return 0;
}