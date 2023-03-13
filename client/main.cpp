#include "qapplication.h"
#include <QCoreApplication>
#include <QTcpSocket>
#include <iostream>
#include <QInputDialog>
#include <QStringList>
#include <QApplication>
#include <QWidget>

using namespace std;

// Sunucudan gelen mesaj kodları
#define LOGIN_RESULT "1"
#define BALANCE "2"
#define WITHDRAW_RESULT "3"
#define DEPOSIT_RESULT "4"
#define TRANSFER_RESULT "5"

// Kullanıcı tarafından gönderilen mesaj kodları
#define LOGIN "1"
#define CHECK_BALANCE "2"
#define WITHDRAW "3"
#define DEPOSIT "4"
#define TRANSFER "5"

int client_main(int argc, char *argv[])
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

            case 3: {
                // Para Çekme İşlemleri
                QString amountStr = QInputDialog::getText(nullptr, "Para Çekme", "Miktar:");

                bool ok;
                int amount = amountStr.toInt(&ok);
                if (!ok) {
                    cout << "Hatalı giriş yaptınız!" << endl;
                    break;
                }

                QString withdrawMsg = QString("%1:%2").arg(WITHDRAW).arg(amount);
                socket.write(withdrawMsg.toUtf8());

                if (socket.waitForReadyRead()) {
                    QString response = socket.readAll();
                    if (response == "OK") {
                        cout << amount << " TL hesabınızdan başarıyla çekildi." << endl;
                    } else {
                        cout << "Para çekme işlemi başarısız oldu." << endl;
                    }
                }

                break;
            }
        case 4: {
            double amount;
            std::cout << "Enter the amount to deposit: ";
            std::cin >> amount;

            // Send the deposit message to the server
            QString depositMsg = QString("%1:%2").arg(DEPOSIT).arg(amount);
            socket.write(depositMsg.toUtf8());

            if (socket.waitForReadyRead()) {
                QString response = socket.readAll();
                if (response == "OK") {
                    std::cout << "Deposit successful!" << std::endl;
                } else {
                    std::cout << "Deposit failed: " << response.toStdString() << std::endl;
                }
            } else {
                std::cout << "No response from server!" << std::endl;
            }

            break;
        }
}
}
else {
std::cout << "Connection error!" << std::endl;
}

return app.exec(); // QApplication nesnesi çalıştırılır
}
