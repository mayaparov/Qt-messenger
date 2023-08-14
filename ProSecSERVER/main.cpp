//
//  main.cpp
//  ProSecServer
//
//  Created by Егор Хохлов on 11.02.2023.
//  Modified by Матвей Япаров on 10.03.2023
//



#include "CProSecServer/CProSecServer.hpp"

int main(int argc, const char* argv[])
{
    CProSecServer g_ProSecServer;
    try
    {
        g_ProSecServer.StartServer();
    }
    catch (const std::exception& s)
    {
        std::cout << "[CProSecServer ShitCode]: " << s.what() << std::endl;
    }



    return 0;
}
