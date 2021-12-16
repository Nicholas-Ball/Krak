#include <iostream>
#include "../Krak/Krak.hpp"

int main()
{
    Krak k;
    std::cout<<k.GetValue("BTC/USDT")<<std::endl;
    return 0;
}