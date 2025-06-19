extern "C"
{
#include "miracl.h"
#include "mirdef.h"
#include <stdlib.h>
}

#include <iostream>
#include <chrono>
using namespace std;

miracl* mip = mirsys(2000, 16);   //2000位16进制系统
double execution_time = 0;

void hash_example() {
    char input[] = "plaintextdata123plaintextdata123"; //明文 32*8bits
    sha256 sh;
    char hash_value[32];

    // 初始化 SHA-256
    shs256_init(&sh);

    // 逐字节处理输入
    for (int i = 0; i < 32; i++) {
        shs256_process(&sh, (int)input[i]);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    // 计算最终哈希值
    shs256_hash(&sh, hash_value);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    execution_time = elapsed.count() * 1e-6;
    
    cout << "Hash run time:" << execution_time << " ms" << endl;


int main() {
    hash_example();
    mirexit();
    return 0;
}