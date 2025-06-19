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

void ecc_point_addition() {
    big a = mirvar(0), b = mirvar(0), p = mirvar(0), x1 = mirvar(0), y1 = mirvar(0), x2 = mirvar(0),
        y2 = mirvar(0), lambda = mirvar(0), temp = mirvar(0);
   
    cinstr(p, (char*)"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F"); // 素数 p (通常使用SECP256K1)
    cinstr(a, (char*)"1");  // 曲线参数 a
    cinstr(b, (char*)"7");  // 曲线参数 b

    // 设置点 P1(x1, y1) 和 P2(x2, y2) 的坐标
    cinstr(x1, (char*)"79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798");//64*4bits
    cinstr(y1, (char*)"483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8");

    cinstr(x2, (char*)"C6047F9441ED7D6D3045406E95C07CD85A28E4A9592D69C1736BEE187E9C5829");
    cinstr(y2, (char*)"1AE168FEA63DC339A3C58419466CEAEEF7F632653266D0E8FDF4672DFA6835E7");

    auto start = std::chrono::high_resolution_clock::now();
    tem_ecc_point_addition(x1, y1, x2, y2, a, p);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    execution_time = elapsed.count() * 1e-6;
    
    cout << "Point addition run time:" << execution_time << " ms" << endl;
    
    mirkill(a);
    mirkill(b);
    mirkill(p);
    mirkill(x1);
    mirkill(y1);
    mirkill(x2);
    mirkill(y2);
    mirkill(lambda);
    mirkill(temp);
}

int main() {
    ecc_point_addition();
    mirexit();
    return 0;
}