#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QMap>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    void start();
    void startServer();
    void handleClient(QTcpSocket *clientSocket);
private:
    QMap<QString, QMap<QString, QVariant>> m_bankUsers;
    QTcpServer* m_server;
    QFile m_usersFile;
    QTextStream m_usersStream;
    QMap<QTcpSocket*, QMap<QString, QString>> m_users;

    void loadBankUsers();
    bool login(QTcpSocket* socket, const QString& username, const QString& password);
    QString transferMoney(QTcpSocket* sender, const QString& receiverUsername, double amount);

private slots:
    void newConnection();
    void readyRead();
};

#endif // SERVER_H
