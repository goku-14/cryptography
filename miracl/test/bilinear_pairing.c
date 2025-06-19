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

void gt_pairing(miracl* mip, epoint* P, epoint* Q) {
    // 初始化配对环境
    PFC pfc(AES_SECURITY); // 使用默认的AES安全级别

    // 配对结果是一个GT群的元素
    GT z;
    G1 g1_P;
    G1 g1_Q;

    big xP = mirvar(0);
    big yP = mirvar(0);
    big xQ = mirvar(0);
    big yQ = mirvar(0);

    // 从 epoint 提取 (x, y) 坐标
    epoint_get(P, xP, yP);
    epoint_get(Q, xQ, yQ);

    // 将 epoint 转换为 G1 类型
    g1_P.g.set(xP, yP);
    g1_Q.g.set(xQ, yQ);

    auto start = std::chrono::high_resolution_clock::now();

    // 计算配对 e(P, Q)
    z = pfc.pairing(g1_P, g1_Q);

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    execution_time = elapsed.count() * 1e-6;

    mirkill(xP);
    mirkill(yP);
    mirkill(xQ);
    mirkill(yQ);
}

void bilinear_pairing_example() {
    big p = mirvar(0);
    big a = mirvar(0);
    big b = mirvar(0);
    big x = mirvar(0);
    big y = mirvar(0);
    big x1 = mirvar(0);
    big y1 = mirvar(0);
    big order = mirvar(0);

    // 使用MIRACL库中内置的BN曲线参数
    cinstr(p, (char*)"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F"); // 参数 p
    cinstr(a, (char*)"1"); // 参数 a
    cinstr(b, (char*)"7"); // 参数 b
    cinstr(x, (char*)"79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798"); // 基点 x 32*8bits
    cinstr(x1, (char*)"483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8"); // 基点 y 32*8bits

    cinstr(y, (char*)"483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8"); // 基点 y 32*8bits
    cinstr(y1, (char*)"79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798"); // 基点 x 32*8bits
    
    ecurve_init(a, b, p, MR_PROJECTIVE);

    epoint* P = epoint_init(); // 初始化点 P
    epoint_set(x, y, 0, P);    // 将点 P 设置为曲线上的基点

    // 计算 e(P, Q) 的配对值
    epoint* Q = epoint_init();
    epoint_set(x1, y1, 0, Q);

    // 进行配对计算
    gt_pairing(mip, P, Q);
    
    cout << "Bilinear pairing operation run time:" << execution_time << " ms" << endl;

    // 清理资源
    mirkill(p);
    mirkill(a);
    mirkill(b);
    mirkill(x);
    mirkill(y);
    mirkill(x1);
    mirkill(y1);
    mirkill(order);
    epoint_free(P);
    epoint_free(Q);
}

int main() {
    bilinear_pairing_example();
    mirexit();
    return 0;
}