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

void zq_multiplication_test() {
    big q = mirvar(0);
    big a = mirvar(0);
    big b = mirvar(0);
    big result = mirvar(0);
    
    // 设置大素数q (SECP256K1的阶)
    cinstr(q, (char*)"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141");
    
    // 设置两个大数进行乘法运算
    cinstr(a, (char*)"1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF");
    cinstr(b, (char*)"FEDCBA0987654321FEDCBA0987654321FEDCBA0987654321FEDCBA0987654321");
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 在Zq*中进行模乘运算: (a * b) mod q
    multiply(a, b, result);
    divide(result, q, q);  // result = result mod q
    
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    execution_time = elapsed.count() * 1e-6;
    
    cout << "Multiplication in Zq:" << execution_time << " ms" << endl;
 
    // 清理内存
    mirkill(q);
    mirkill(a);
    mirkill(b);
    mirkill(result);
}

int main() {
    zq_multiplication_test();
    mirexit();
    return 0;
}