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

void emitCodeIn(unsigned char* m, const std::string password)
{
	unsigned char code[] =
	{
		0x55, 0x89, 0xe5, 0x8b, 0x4d, 0x08, 0x01, 0xc9, 0x89, 0xc8, 0x89, 0xec, 0x5d, 0xc3,			//long add(long)
		0x55, 0x89, 0xe5, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x89, 0xec, 0x5d, 0xc3,						//long sub(long)
		0x55, 0x89, 0xe5, 0x8b, 0x4d, 0x08, 0x0f, 0xaf, 0xc9, 0x89, 0xc8, 0x89, 0xec, 0x5d, 0xc3,	//long mul(long)
		0x55, 0x89, 0xe5, 0xb8, 0x01, 0x00, 0x00, 0x00, 0x89, 0xec, 0x5d, 0xc3,						//long div(long)
		0x55, 0x89, 0xe5, 0x8b, 0x4d, 0x08, 0x03, 0x4d, 0x0c, 0x89, 0xc8, 0x89, 0xec, 0x5d, 0xc3,	//long add(long, long)
		0x55, 0x89, 0xe5, 0x8b, 0x4d, 0x08, 0x2b, 0x4d, 0x0c, 0x89, 0xc8, 0x89, 0xec, 0x5d, 0xc3	//long sub(long, long)
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

	int count1;
	std::cout << "Count 1: " << std::endl;
	std::cin >> count1;

	int count2;
	std::cout << "Count 2: " << std::endl;
	std::cin >> count2;

	int d = square(count1);

	// encrypt code
	void* memory1 = allocExecutableMemory(1024);
	emitCodeIn(static_cast<unsigned char*>(memory1), password);

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

	system("pause");
	return 0;
}