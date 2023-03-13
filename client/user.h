#ifndef USER_H
#define USER_H

#include <QString>

class User
{
public:
    User();
    User(QString username, QString password, QString name, QString surname, QString bank, QString accountNo, double balance);

    QString getUsername() const;
    QString getPassword() const;
    QString getName() const;
    QString getSurname() const;
    QString getBank() const;
    QString getAccountNo() const;
    double getBalance() const;

    void setUsername(QString username);
    void setPassword(QString password);
    void setName(QString name);
    void setSurname(QString surname);
    void setBank(QString bank);
    void setAccountNo(QString accountNo);
    void setBalance(double balance);

    bool isValid() const;

private:
    QString m_username;
    QString m_password;
    QString m_name;
    QString m_surname;
    QString m_bank;
    QString m_accountNo;
    double m_balance;
};

#endif // USER_H
