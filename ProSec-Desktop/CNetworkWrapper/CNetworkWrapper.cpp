#include "CNetworkWrapper.h"

void CNetworkWrapper::ConnectToServer()
{
    std::thread connection_thread(&CNetworkWrapper::ConnectFunctionForThread, this);
    connection_thread.detach();
}

void CNetworkWrapper::ConnectFunctionForThread()//function for connection thread only!
{
    while(true)
    {
    if(!m_socket->is_open())
   {
        try
        {
            m_socket->connect(m_endpoint);
        }
        catch (std::exception& l_error)
        {
            m_NetworkErrorSTR = l_error.what();

            qDebug() << "[RL SERVER ERROR]: " << l_error.what();
        }
    }
    else
        return;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

CNetworkWrapper::CNetworkWrapper()
{
    m_NetworkErrorSTR   = "All Clear";
    m_RedLineSite       = "redline-software.xyz";
    this->ConnectToServer();
}

bool CNetworkWrapper::CheckRedLineConnection()
{
    return m_socket->is_open();
}

bool CNetworkWrapper::CheckUserLogin(std::string data)
{
    try
    {        
        boost::system::error_code error;
        if(CheckRedLineConnection())
        {
            ConnectToServer();
            m_error_message = "No connection to server";
            return false;
        }

        boost::asio::write(*m_socket, boost::asio::buffer(m_AuthKey + data + "\n"), error);

        if (error)
        {
            qDebug()  << "send failed: error occured";
            m_error_message = error.message();
            qDebug()  << QString::fromStdString(m_error_message);
            return false;
        }

        // getting response from server
        std::array<char, 128> buffer;
        std::fill(buffer.begin(), buffer.end(), 0);

        std::string respond;

        if(!CheckRedLineConnection())
        {
            ConnectToServer();
            m_error_message = "No connection to server";
            return false;
        }

        m_socket->read_some(boost::asio::buffer(buffer), error);

        if (error && error != boost::asio::error::eof)
        {
            qDebug()  << "receive failed: error occured";
            m_error_message = error.message();
            return false;
        }
        else
        {
            respond = "";
            for (char a : buffer)
                if (a != '\n' && a != 0) respond = respond + a;

            if (respond.find(m_AuthKey) == 0)
            {
                client.name = respond.substr(m_AuthKey.size(), respond.find('\t') - m_AuthKey.size());
                client.clientID = respond.substr(respond.find('\t'), respond.size() - 1 - respond.find('\t'));
                qDebug() << QString::fromStdString("Correct: " + respond);
                return true;
            }

            qDebug() << QString::fromStdString("Incorrect: " + respond);
            m_error_message = "Incorrect login or password";
            return false;
        }
    }

    catch (std::exception &error_ec)
    {
        qDebug() << "[DATA SEND]: " << error_ec.what();
        return false;
    }

    return true;
}

// SHitCode!
std::string CNetworkWrapper::ReadJsonFormSite(std::string target_ip, std::string data_reference)
{
    std::string response = "NA";

    try
    {
        boost::asio::io_context l_InOutContext;
        boost::asio::ip::tcp::resolver l_Resolver(l_InOutContext);
        boost::asio::ip::tcp::socket l_ConnectionSocket(l_InOutContext);

        boost::asio::connect(l_ConnectionSocket, l_Resolver.resolve(target_ip, "80"));
        boost::beast::http::request<boost::beast::http::string_body> Request(boost::beast::http::verb::get, target_ip + data_reference, 11);

        Request.set(boost::beast::http::field::host, target_ip);
        Request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        boost::beast::http::write(l_ConnectionSocket, Request);

        boost::beast::flat_buffer l_FlatBuffer;
        boost::beast::http::response<boost::beast::http::dynamic_body> l_ResponseResult;
        boost::beast::http::read(l_ConnectionSocket, l_FlatBuffer, l_ResponseResult);

        response = boost::beast::buffers_to_string(l_ResponseResult.body().data());

        l_ConnectionSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    }
    catch (std::exception &error_ec)
    {
        qDebug() << "[DATA JSON ERROR]: " << error_ec.what();
        return error_ec.what();
    }

    return "[DATA JSON]: " + response;
}



std::string CNetworkWrapper::GetErrorMessage()
{
    return m_error_message;
}
CNetworkWrapper::~CNetworkWrapper()
{
    m_NetworkErrorSTR = "All Clear";
}

