#include <QCoreApplication>
#include <QTcpSocket>
#include <iostream>
#include <QTextStream>
#include <constants.h>
#include <QtWidgets/QInputDialog>
#include <QCoreApplication>
#include <QDebug>
#include <QTcpSocket>
#include <QApplication>
#include <QWidget>
#include "constants.h"
#define CHECK_BALANCE "CHECK_BALANCE"
#define BALANCE "BALANCE"
#define TRANSFER "TRANSFER"
#define TRANSFER_RESULT "TRANSFER_RESULT"
#define WITHDRAW "WITHDRAW"
#define WITHDRAW_RESULT "WITHDRAW_RESULT"
#define DEPOSIT "DEPOSIT"
#define DEPOSIT_RESULT "DEPOSIT_RESULT"



using namespace std;

int main(int argc, char *argv[])
{   QApplication app(argc, argv);
    QTcpSocket socket;
    socket.connectToHost("localhost", 1234);

    if (socket.waitForConnected()) {
        QString username = QInputDialog::getText(nullptr, "Login", "Enter username:");
        QString password = QInputDialog::getText(nullptr, "Login", "Enter password:", QLineEdit::Password);
        QString loginMsg = QString("%1:%2:%3").arg(LOGIN).arg(username).arg(password);
        socket.write(loginMsg.toUtf8());
        // Kullanıcı adı ve şifre kontrolü (yukarı kısım)
        if (socket.waitForReadyRead()) {
            QString response = socket.readAll();
            if (response == "OK") {
                cout << "Welcome, " << username.toStdString() << "!" << endl;
                return 0;
            } else {
                cout << "Invalid username or password!" << endl;
                return 1;
            }
        }
        cout << "1. Check Balance" << endl;
        cout << "2. Transfer Money" << endl;
        cout << "3. Withdraw Money" << endl;
        cout << "4. Deposit Money" << endl;
        cout << "5. Exit" << endl;

        int choice = 0;
        cin >> choice;

        switch (choice) {
            case 1:
                // Bakiye sorgulama işlemleri
                socket.write(QByteArray().append(CHECK_BALANCE).append("\n"));
                if (socket.waitForReadyRead()) {
                    QString response = socket.readAll();
                    if (response.startsWith(BALANCE)) {
                        QString balanceStr = response.mid(response.indexOf(":") + 1);
                        double balance = balanceStr.toDouble();
                        cout << "Your balance is: " << balance << endl;
                    } else {
                        cout << "Invalid response from server!" << endl;
                    }
                }
                break;

            case 2:
                // Para transferi işlemleri
                {
                    int accountNumber;
                    double amount;
                    cout << "Enter account number: ";
                    cin >> accountNumber;
                    cout << "Enter amount to transfer: ";
                    cin >> amount;

                    QString transferMsg = QString("%1:%2:%3").arg(TRANSFER).arg(accountNumber).arg(amount);
                    socket.write((QString(DEPOSIT) + ":" + QString::number(amount) + "\n").toUtf8());
                    if (socket.waitForReadyRead()) {
                        QString response = socket.readAll();
                        if (response.startsWith(TRANSFER_RESULT)) {
                            QString resultStr = response.mid(response.indexOf(":") + 1);
                            int result = resultStr.toInt();
                            if (result == 0) {
                                cout << "Transfer successful!" << endl;
                            } else if (result == 1) {
                                cout << "Insufficient funds!" << endl;
                            } else if (result == 2) {
                                cout << "Invalid account number!" << endl;
                            } else {
                                cout << "Unknown error!" << endl;
                            }
                        } else {
                            cout << "Invalid response from server!" << endl;
                        }
                    }
                }
                break;

        case 3:
        {
            double amount;
            cout << "Enter amount to withdraw: ";
            cin >> amount;

            socket.write((QString(WITHDRAW) + ":" + QString::number(amount) + "\n").toUtf8());
            if (socket.waitForReadyRead()) {
                QString response = socket.readAll();
                if (response.startsWith(WITHDRAW_RESULT)) {
                    QString resultStr = response.mid(response.indexOf(":") + 1);
                    int result = resultStr.toInt();
                    if (result == 0) {
                        cout << "Withdrawal successful!" << endl;
                    } else if (result == 1) {
                        cout << "Insufficient funds!" << endl;
                    } else {
                        cout << "Unknown error occurred!" << endl;
                    }
                } else {
                    cout << "Invalid response from server!" << endl;
                }
            }
            break;
        }

        case 4:
        {
            double amount;
            cout << "Enter amount to deposit: ";
            cin >> amount;

            socket.write((QString(DEPOSIT) + ":" + QString::number(amount) + "\n").toUtf8());
            if (socket.waitForReadyRead()) {
                QString response = socket.readAll();
                if (response.startsWith(DEPOSIT_RESULT)) {
                    QString resultStr = response.mid(response.indexOf(":") + 1);
                    int result = resultStr.toInt();
                    if (result == 0) {
                        cout << "Deposit successful!" << endl;
                    } else {
                        cout << "Unknown error occurred!" << endl;
                    }
                } else {
                    cout << "Invalid response from server!" << endl;
                }
            }
            break;
        }

        case 5:
        {
            socket.write(QString(EXIT).toUtf8());
            cout << "Goodbye!" << endl;
            return 0;
        }

        default:
        {
            cout << "Invalid choice!" << endl;
            break;
        }
        }
        } else {
        cout << "Failed to connect to server!" << endl;
        }

        return app.exec();
        }
