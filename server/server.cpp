#include <QObject>
#include "server.h"
#include "qcoreapplication.h"
#include <QDataStream>
#include <iostream>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>



using namespace std;
void Server::handleClient(QTcpSocket *clientSocket){
    qDebug() << "New client from:" << clientSocket->peerAddress().toString();

    // Read the incoming data from the client
    QByteArray requestData = clientSocket->readAll();
    QJsonDocument document = QJsonDocument::fromJson(requestData);
    QJsonObject requestObject = document.object();

    // Get the request type from the JSON object
    QString requestType = requestObject["request_type"].toString();

    // Check if the request type is "login"
    if (requestType == "login") {
        QString username = requestObject["username"].toString();
        QString password = requestObject["password"].toString();

        // Check if the username exists in the bank users map
        if (m_bankUsers.contains(username)) {
            qDebug() << "Found user:" << username;
            // Check if the password is correct
            if (m_bankUsers[username]["password"] == password) {
                // Login successful
                qDebug() << "Login successful for:" << username;

                // Send a login response to the client
                QJsonObject response;
                response["response_type"] = "login";
                response["status"] = "success";
                response["message"] = "Login successful!";
                QByteArray responseJson = QJsonDocument(response).toJson();
                clientSocket->write(responseJson);
            } else {
                // Password is incorrect
                qDebug() << "Invalid password for:" << username;

                // Send a login response to the client
                QJsonObject response;
                response["response_type"] = "login";
                response["status"] = "error";
                response["message"] = "Invalid username or password!";
                QByteArray responseJson = QJsonDocument(response).toJson();
                clientSocket->write(responseJson);
            }
        } else {
            // Username does not exist
            qDebug() << "Invalid username:" << username;

            // Send a login response to the client
            QJsonObject response;
            response["response_type"] = "login";
            response["status"] = "error";
            response["message"] = "Invalid username or password!";
            QByteArray responseJson = QJsonDocument(response).toJson();
            clientSocket->write(responseJson);
        }
    }

    // Check if the request type is "transfer"
    else if (requestType == "transfer") {
        QString senderUsername = requestObject["sender_username"].toString();
        QString receiverUsername = requestObject["receiver_username"].toString();
        double amount = requestObject["amount"].toDouble();

        // Check if the sender and receiver usernames exist in the bank users map
        if (m_bankUsers.contains(senderUsername) && m_bankUsers.contains(receiverUsername)) {
            // Check if the sender has sufficient balance
            double senderBalance = m_bankUsers[senderUsername]["balance"].toDouble();
            if (amount > senderBalance) {
                // Insufficient funds
                qDebug() << "Insufficient funds for transfer from:" << senderUsername;

                // Send a transfer response to the client
                QJsonObject response;
                response["response_type"] = "transfer";
                response["status"] = "error";
                response["message"] = "Insufficient funds!";
                QByteArray responseJson = QJsonDocument(response).toJson();
                clientSocket->write(responseJson);

            } else {
                // Sufficient funds
                double receiverBalance = m_bankUsers[receiverUsername]["balance"].toDouble();
                senderBalance -= amount;
                receiverBalance += amount;

                m_bankUsers[senderUsername]["balance"] = QString::number(senderBalance, 'f', 2);
                m_bankUsers[receiverUsername]["balance"] = QString::number(receiverBalance, 'f', 2);

                // Send a transfer response to the client
                QJsonObject response;
                response["response_type"] = "transfer";
                response["status"] = "success";
                response["message"] = QString("Transfer of %1 from %2 to %3 successful.").arg(QString::number(amount, 'f', 2), senderUsername, receiverUsername);
                QByteArray responseJson = QJsonDocument(response).toJson();
                clientSocket->write(responseJson);
            }
}
}
}

void Server::start() {
    m_server->listen(QHostAddress::Any, 1234);
    QObject::connect(m_server, &QTcpServer::newConnection, [&]() {
        while (m_server->hasPendingConnections()) {
            QTcpSocket *clientSocket = m_server->nextPendingConnection();
            handleClient(clientSocket);
        }
    });
}

int serverMain(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    Server server;
    server.start();

    return app.exec();
}
Server::Server(QObject *parent) : QObject(parent), m_server(new QTcpServer(this))
{
    m_usersFile.setFileName("users.txt");
    connect(m_server, &QTcpServer::newConnection, this, &Server::newConnection);

    if(!m_usersFile.exists("users.txt")) {
        m_usersFile.open(QIODevice::WriteOnly | QIODevice::Text);
        m_usersFile.close();
        m_usersFile.open(QIODevice::WriteOnly | QIODevice::Text);
        m_usersFile.close();
    }

    if(!m_usersFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "Failed to open users file!";
        return;
    }

    m_usersStream.setDevice(&m_usersFile);
    loadBankUsers();
}
void Server::loadBankUsers()
{
    while(!m_usersStream.atEnd()) {
        QString line = m_usersStream.readLine();
        QStringList fields = line.split(" ");
        QString username = fields.at(0);
        QString password = fields.at(1);
        double balance = fields.at(2).toDouble();
        m_bankUsers.insert(username, {{"password", password}, {"balance", balance}});
    }
}

void Server::startServer()
{
    if(!m_server->listen(QHostAddress::Any, 1234)) {
        qDebug() << "Failed to start server!";
        return;
    }

    qDebug() << "Server started on port" << m_server->serverPort();
}

bool Server::login(QTcpSocket* socket, const QString& username, const QString& password)
{
    if(m_users.contains(socket)) {
        return false;
    }

    if(!m_bankUsers.contains(username)) {
        return false;
    }

    if(m_bankUsers[username]["password"].toString() != password) {
        return false;
    }

    m_users[socket] = {{"username", username}};
    return true;
}

QString Server::transferMoney(QTcpSocket* sender, const QString& receiverUsername, double amount)
{
    QString senderUsername = m_users[sender]["username"];
    if(senderUsername.isEmpty()) return "You must be logged in to transfer money!";
    if(!m_bankUsers.contains(receiverUsername)) return "User does not exist!";
    if(senderUsername == receiverUsername) return "You cannot transfer money to yourself!";

    double senderBalance = m_bankUsers[senderUsername]["balance"].toDouble();
    double receiverBalance = m_bankUsers[receiverUsername]["balance"].toDouble();

    if(amount > senderBalance) return "Insufficient funds!";

    senderBalance -= amount;
    receiverBalance += amount;

    m_bankUsers[senderUsername]["balance"] = senderBalance;
    m_bankUsers[receiverUsername]["balance"] = receiverBalance;

    m_usersStream.seek(0);
    m_usersStream << "";

    for(auto user : m_bankUsers.keys()) {
        QString line = user + " " + m_bankUsers[user]["password"].toString() + " " + QString::number(m_bankUsers[user]["balance"].toDouble(), 'f', 2) + "\n";
        m_usersStream << line;
    }

    return "";
}

void Server::newConnection()
{
    while(m_server->hasPendingConnections()) {
        QTcpSocket* socket = m_server->nextPendingConnection();

        connect(socket, &QTcpSocket::readyRead, this, &Server::readyRead);
    }
}

void Server::readyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if(!socket) return;

    while(socket->canReadLine())
    {
        QString line = QString::fromUtf8(socket->readLine()).trimmed();
        QStringList parts = line.split(' ');
        QString command = parts[0];

        if(command == "login")
        {
            if(parts.size() != 3)
            {
                socket->write("Error: Invalid arguments for command 'login'!\n");
                continue;
            }

            QString username = parts[1];
            QString password = parts[2];

            if(login(socket, username, password))
            {
                socket->write("Successful login!\n");
            }
            else
            {
                socket->write("Error: Invalid username or password!\n");
            }
        }
        else if(command == "transfer")
        {
            if(parts.size() != 3)
            {
                socket->write("Error: Invalid arguments for command 'transfer'!\n");
                continue;
            }

            QString receiverUsername = parts[1];
            double amount = parts[2].toDouble();

            QString result = transferMoney(socket, receiverUsername, amount);
            socket->write(result.toUtf8());
        }
        else
        {
            socket->write("Error: Invalid command!\n");
        }
}
}
