#include <Windows.h>

#include <iostream>
#include <string>

// decrypted function pointers
typedef long(*l_l_func)(long);
typedef double(*d_l_l_func)(long, long);


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

__declspec(noinline) double square(long num, long num2)
{
	return (num * num + num2) / num2;
}

void emitCodeIn(unsigned char* m, const std::string password)
{
	unsigned char code[] =
	{
		0x55, 0x89, 0xe5, 0x8b, 0x4d, 0x08, 0x01, 0xc9, 0x89, 0xc8, 0x89, 0xec, 0x5d, 0xc3,
		0x55, 0x89, 0xe5, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x89, 0xec, 0x5d, 0xc3,
		0x55, 0x89, 0xe5, 0x8b, 0x4d, 0x08, 0x0f, 0xaf, 0xc9, 0x89, 0xc8, 0x89, 0xec, 0x5d, 0xc3,
		0x55, 0x89, 0xe5, 0xb8, 0x01, 0x00, 0x00, 0x00, 0x89, 0xec, 0x5d, 0xc3
	};

	//code[0] ^= password[0];
	//code[1] ^= password[1];
	//code[2] ^= password[2];
	//code[3] ^= password[3];
	//code[4] ^= password[4];
	//code[5] ^= password[5];

	memcpy(m, code, sizeof(code));
}


int main()
{
	std::string password;
	std::cout << "Password: " << std::endl;
	std::getline(std::cin, password);

	int count;
	std::cout << "Count: " << std::endl;
	std::cin >> count;

	int d = square(count);

	// encrypt code
	void* memory1 = allocExecutableMemory(1024);
	emitCodeIn(static_cast<unsigned char*>(memory1), password);

	// get memory pointers
	void* memory2 = static_cast<unsigned char*>(memory1) + 14;
	void* memory3 = static_cast<unsigned char*>(memory2) + 12;
	void* memory4 = static_cast<unsigned char*>(memory3) + 15;

	// get functions
	l_l_func addF = static_cast<l_l_func>(memory1);
	l_l_func subF = static_cast<l_l_func>(memory2);
	l_l_func mulF = static_cast<l_l_func>(memory3);
	l_l_func divF = static_cast<l_l_func>(memory4);

	int result1, result2, result3, result4;
	__asm
	{
		push	count
		call	addF
		mov		result1, eax
		push	count
		call	subF
		mov		result2, eax
		push	count
		call	mulF
		mov		result3, eax
		push	count
		call	divF
		mov		result4, eax
	}
	std::cout << "Result1: " << result1 << std::endl;
	std::cout << "Result2: " << result2 << std::endl;
	std::cout << "Result3: " << result3 << std::endl;
	std::cout << "Result4: " << result4 << std::endl;
	std::cout << d;

	system("pause");
	return 0;
}