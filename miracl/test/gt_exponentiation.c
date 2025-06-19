extern "C"
{
#include "miracl.h"
#include "mirdef.h"
#include <stdlib.h>
}

#define AES_SECURITY 128
#define MR_PAIRING_SSP


#include <iostream>
#include <chrono>
using namespace std;

miracl* mip = mirsys(2000, 16);   //2000位16进制系统
double execution_time = 0;

void gt_exponentiation_test() {
    // 初始化配对环境
    PFC pfc(AES_SECURITY);
    
    big p = mirvar(0), a = mirvar(0), b = mirvar(0);
    big x1 = mirvar(0), y1 = mirvar(0), x2 = mirvar(0), y2 = mirvar(0);
    big exponent = mirvar(0);
    
    // 设置椭圆曲线参数
    cinstr(p, (char*)"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F");
    cinstr(a, (char*)"1");
    cinstr(b, (char*)"7");
    
    // 设置两个点用于配对
    cinstr(x1, (char*)"79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798");
    cinstr(y1, (char*)"483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8");
    cinstr(x2, (char*)"C6047F9441ED7D6D3045406E95C07CD85A28E4A9592D69C1736BEE187E9C5829");
    cinstr(y2, (char*)"1AE168FEA63DC339A3C58419466CEAEEF7F632653266D0E8FDF4672DFA6835E7");
    
    // 设置指数
    cinstr(exponent, (char*)"1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF");
    
    // 初始化椭圆曲线
    ecurve_init(a, b, p, MR_PROJECTIVE);
    
    epoint* P = epoint_init();
    epoint* Q = epoint_init();
    epoint_set(x1, y1, 0, P);
    epoint_set(x2, y2, 0, Q);
    
    // GT群元素
    GT z, result;
    G1 g1_P, g1_Q;
    
    // 转换为G1类型
    g1_P.g.set(x1, y1);
    g1_Q.g.set(x2, y2);
    
    // 先计算配对 e(P,Q)
    z = pfc.pairing(g1_P, g1_Q);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 在GT群中进行指数运算: z^exponent
    result = pfc.power(z, exponent);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    execution_time = elapsed.count() * 1e-6;
    
    cout << "Exponentiation run time:" << execution_time << " ms" << endl;
    
    // 清理内存
    mirkill(p);
    mirkill(a);
    mirkill(b);
    mirkill(x1);
    mirkill(y1);
    mirkill(x2);
    mirkill(y2);
    mirkill(exponent);
    epoint_free(P);
    epoint_free(Q);
}

int main() {
    gt_exponentiation_test();
    mirexit();
    return 0;
}