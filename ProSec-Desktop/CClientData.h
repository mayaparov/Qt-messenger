#ifndef CCLIENTDATA_H
#define CCLIENTDATA_H

#pragma once
#endif // CCLIENTDATA_H
#include <string>
#include <vector>
class Client
{
public:
    std::string name;
    std::string clientID;
    std::string job_title;

    std::vector<std::string> dialogs;
};
