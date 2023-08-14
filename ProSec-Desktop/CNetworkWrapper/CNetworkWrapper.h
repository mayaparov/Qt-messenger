#pragma once

//#include <SDKDDKVer.h> // Boost asking us about Win compile Version...
#define _WIN32_WINNT 0x0601

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>
#include <boost/beast.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio.hpp>
#include <QMessageBox>
#include <thread>
#include <chrono>
#include <iostream>

#include "CClientData.h"

class CNetworkWrapper
{	
public:

    Client client;
	CNetworkWrapper();
	~CNetworkWrapper();

    std::string ReadJsonFormSite(std::string target_ip, std::string data_reference);

    std::string GetServerIP();

    std::string GetErrorMessage();

    bool CheckRedLineConnection();
    bool CheckUserLogin(std::string data);

    std::string m_RedLineSite;
    std::string m_NetworkErrorSTR;

private:

    std::string m_error_message;

    boost::asio::io_service io_service;

    boost::asio::ip::tcp::socket* m_socket = new boost::asio::ip::tcp::socket(io_service);

    void ConnectToServer();
    void ConnectFunctionForThread();
    const std::string     m_AuthKey     = "AUTHKEY";
    const std::string     m_MessageKey  = "MESSAGEKEY";
    const std::string     m_RegKey      = "REGKEY";
    const boost::asio::ip::tcp::endpoint m_endpoint = *new
            boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("193.105.114.182"), 40334);




};


