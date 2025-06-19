extern "C"
{
#include "miracl.h"
#include "mirdef.h"
#include <stdlib.h>
}

#include <iostream>
#include <chrono>
#include <cstring>
using namespace std;

miracl* mip = mirsys(2000, 16);   //2000位16进制系统
double execution_time = 0;

void symmetric_encryption_example() {
    char key[33] = "secretkey1234567secretkey1234567"; //32*8bits
    char plaintext[33] = "plaintextdata123plaintextdata123"; // 明文块长度必须是AES块大小的倍数
    char original[33];
    strcpy(original, plaintext); // 保存原始明文用于显示
    
    aes a;

    // 初始化AES，使用ECB模式
    aes_init(&a, MR_ECB, 32, key, NULL);
    
    auto start = std::chrono::high_resolution_clock::now();
    // 对plaintext进行加密，结果直接写回plaintext
    aes_encrypt(&a, plaintext);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    execution_time = elapsed.count() * 1e-6;
    
    cout << "AES Symmetric run time:" << execution_time << " ms" << endl;

    // 结束AES操作
    aes_end(&a);
}

int main() {
    symmetric_encryption_example();
    mirexit();
    return 0;
}