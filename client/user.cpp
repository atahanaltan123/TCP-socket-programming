#include "user.h"

User::User()
{
    m_balance = 0.0;
}

User::User(QString username, QString password, QString name, QString surname, QString bank, QString accountNo, double balance)
{
    m_username = username;
    m_password = password;
    m_name = name;
    m_surname = surname;
    m_bank = bank;
    m_accountNo = accountNo;
    m_balance = balance;
}

QString User::getUsername() const
{
    return m_username;
}

QString User::getPassword() const
{
    return m_password;
}

QString User::getName() const
{
    return m_name;
}

QString User::getSurname() const
{
    return m_surname;
}

QString User::getBank() const
{
    return m_bank;
}

QString User::getAccountNo() const
{
    return m_accountNo;
}

double User::getBalance() const
{
    return m_balance;
}

void User::setUsername(QString username)
{
    m_username = username;
}

void User::setPassword(QString password)
{
    m_password = password;
}

void User::setName(QString name)
{
    m_name = name;
}

void User::setSurname(QString surname)
{
    m_surname = surname;
}

void User::setBank(QString bank)
{
    m_bank = bank;
}

void User::setAccountNo(QString accountNo)
{
    m_accountNo = accountNo;
}

void User::setBalance(double balance)
{
    m_balance = balance;
}
