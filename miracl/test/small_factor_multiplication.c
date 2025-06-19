extern "C"
{
#include "miracl.h"
#include "mirdef.h"
#include <stdlib.h>
#include <time.h>
}

#include <iostream>
#include <chrono>
using namespace std;

miracl* mip = mirsys(2000, 16);   //2000位16进制系统
double execution_time = 0;

void tem_ecc_point_addition(big x, big y, big x1, big y1, big a, big p) {
    big lambda, temp, num, denom;
    lambda = mirvar(0);
    temp = mirvar(0);
    num = mirvar(0);
    denom = mirvar(0);
    big char2 = mirvar(2);
    big char3 = mirvar(3);
    
    if (compare(x, x1) == 0 && compare(y, y1) == 0) {
        // P == Q 的情况：lambda = (3 * x^2 + a) / (2 * y)
        powmod(x, char2, p, num);           // num = x^2 mod p
        multiply(num, char3, num);           // num = 3 * x^2
        add(num, a, num);                        // num = 3 * x^2 + a
        add(y, y, denom);                      // denom = 2 * y
        xgcd(denom, p, denom, denom, denom);     // denom = 1 / denom mod p
        multiply(num, denom, lambda);            // lambda = (3 * x^2 + a) / (2 * y)
        divide(lambda, p, p); // lambda = lambda mod p
    }
    else {
        // P != Q 的情况：lambda = (y1 - y) / (x1 - x)
        subtract(y1, y, num);                   // num = y1 - y
        subtract(x1, x, denom);                 // denom = x1 - x
        xgcd(denom, p, denom, denom, denom);     // denom = 1 / denom mod p
        multiply(num, denom, lambda);            // lambda = (y1 - y) / (x1 - x)
        divide(lambda, p, p); // lambda = lambda mod p
    }

    // 计算 x3 = lambda^2 - x - x1
    powmod(lambda, char2, p, temp);            // temp = lambda^2 mod p
    subtract(temp, x, temp);                        // temp = lambda^2 - x
    subtract(temp, x1, x);                        // x = lambda^2 - x - x1
    divide(x, p, p); // x = x mod p

    // 计算 y = lambda * (x1 - x) - y1
    subtract(x1, x, temp);                      // temp = x1 - x
    multiply(lambda, temp, y);                  // y = lambda * temp
    subtract(y, y1, y);                        // y = y - y1
    divide(y, p, p); // y = y mod p

    // 清理内存
    mirkill(lambda);
    mirkill(temp);
    mirkill(num);
    mirkill(char2);
    mirkill(char3);
    mirkill(denom);
}

void small_factor_multiplication_test() {
    big a = mirvar(0), p = mirvar(0);
    big x = mirvar(0), y = mirvar(0);
    big resultx = mirvar(0), resulty = mirvar(0);
    big tempx = mirvar(0), tempy = mirvar(0);
    
    // 初始化椭圆曲线参数
    cinstr(p, (char*)"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F");
    cinstr(a, (char*)"1");
    
    // 设置基点
    cinstr(x, (char*)"79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798");
    cinstr(y, (char*)"483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8");
    
    // 小因子s (2-16之间的随机小数)
    srand(time(NULL));
    int s = rand() % 15 + 2;
    
    // 初始化结果为无穷远点 (设为(0,0)作为标识)
    zero(resultx);
    zero(resulty);
    bool is_first = true;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 使用重复加法实现小因子乘法 s*P
    for (int i = 0; i < s; i++) {
        if (is_first) {
            copy(x, resultx);
            copy(y, resulty);
            is_first = false;
        } else {
            tem_ecc_point_addition(resultx, resulty, x, y, a, p);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    execution_time = elapsed.count() * 1e-6;
    
    cout << "Multiplication with small factor run time:" << execution_time << " ms" << endl;
    
    // 清理内存
    mirkill(a);
    mirkill(p);
    mirkill(x);
    mirkill(y);
    mirkill(resultx);
    mirkill(resulty);
    mirkill(tempx);
    mirkill(tempy);
}

int main() {
    small_factor_multiplication_test();
    mirexit();
    return 0;
}