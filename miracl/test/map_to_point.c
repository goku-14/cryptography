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

void map_to_point_example() {
    const char *hash_value;
    hash_value = "1A2B3C4D5E6C7D8E1A2B3C4D5E6C7D8E1A2B3C4D5E6C7D8E1A2B3C4D5E6C7D8E"; //hash 32*8bits
    
    big p = mirvar(0), a = mirvar(0), b = mirvar(0), x = mirvar(0), y = mirvar(0), x_candidate = mirvar(0);
    big char3 = mirvar(3);
    
    // 初始化椭圆曲线参数
    cinstr(p, (char*)"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F"); // p 32*8bits
    cinstr(a, (char*)"1"); // a
    cinstr(b, (char*)"7"); // b
    
    cinstr(x, (char*)"79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798"); // 基点 x 32*8bits
    cinstr(y, (char*)"483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8"); // 基点 y 32*8bits
    
    // 将hash_value转换为一个大整数，作为x的初始候选值
    cinstr(x_candidate, (char*)hash_value);

    auto start = std::chrono::high_resolution_clock::now();
    while (1) {
        // 计算 y^2 = x^3 + ax + b (mod p)
        powmod(x_candidate, char3, p, y);  // y = x^3 mod p
        add(y, b, y);                      // y = x^3 + b mod p
        if (jack(y, p) == 1) {             // 如果 y 是平方剩余
            nroot(y, 2, y);                // y = sqrt(y) mod p
            break;                         // 找到合法的 y 值，退出循环
        }
        incr(x_candidate, 1, x_candidate); // 尝试下一个 x_candidate
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    execution_time = elapsed.count() * 1e-6;
    
    cout << "MapToPoint run time:" << execution_time << " ms" << endl;
    
    // 清理资源
    mirkill(p);
    mirkill(a);
    mirkill(b);
    mirkill(x);
    mirkill(y);
    mirkill(x_candidate);
    mirkill(char3);
}

int main() {
    map_to_point_example();
    mirexit();
    return 0;
}