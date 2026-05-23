#include "Comp_card.hpp"
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <climits>

using namespace std;

// Comp_card.cpp - 复合卡实现：正余额高利率收益，负余额罚息
// 利率0.6/分钟为刻意设计，使利益计算体现更直观
// 声明 system.cpp 中的彩蛋图片显示函数
void showRealImage(const char* imagePath);

long long Comp_card::getCurrentTimestamp()
{
    return time(nullptr);
}

long long Comp_card::parseTimeToTimestamp(const string& timeStr)
{
    struct tm tm = {0};
    istringstream ss(timeStr);
    ss >> get_time(&tm, "%Y-%m-%d %H:%M:%S");
    return mktime(&tm);
}

// 核心：根据当前余额正负分支处理
// 正余额：利息 = 余额 * 0.6/分钟 * 经过分钟数（高收益）
// 负余额：罚息 = 透支金额 * 0.0001/分钟 * 经过分钟数
void Comp_card::settleInterestAndPenalty()
{
    long long now = getCurrentTimestamp();
    long long last = parseTimeToTimestamp(lastInterestTime);

    long long minutesPassed = (now - last) / 60;

    if (minutesPassed > 0) {
        if (balance >= 0) {
            // 正余额：按高利率（0.6/分钟）计算收益
            double interest = balance * interestRatePerMinute * minutesPassed;
            if (interest > 0) {
                balance += interest;
                cout << "距离上次动账经过 " << minutesPassed << " 分钟，获得利息 "
                     << interest << " 元，当前余额：" << balance << endl;
            }
        } else {
            // 透支：按罚息率计算并增加负债
            double overdueAmount = -balance;
            double penalty = overdueAmount * penaltyRatePerMinute * minutesPassed;
            if (penalty > 0) {
                balance -= penalty;
                cout << "此卡已透支 " << overdueAmount << " 元，"
                     << "经过 " << minutesPassed << " 分钟未还，"
                     << "产生罚息 " << penalty << " 元，"
                     << "当前负债：" << (-balance) << " 元" << endl;
            }
        }
    }

    time_t now_t = now;
    tm* tm_now = localtime(&now_t);
    char newtime[20];
    strftime(newtime, sizeof(newtime), "%Y-%m-%d %H:%M:%S", tm_now);
    lastInterestTime = newtime;
}

Comp_card::Comp_card(string cardId, double balance, string lastInterestTime)
{
    this->cardId = cardId;
    this->balance = balance;
    this->lastInterestTime = lastInterestTime;
}

Comp_card::~Comp_card() {}

string Comp_card::getcardType() const
{
    return cardType;
}

bool Comp_card::deposit(double amount)
{
    settleInterestAndPenalty();
    
    if (amount > 0) {
        balance += amount;
        if (balance >= 0) {
            cout << "存款成功！当前余额：" << balance << endl;
        } else {
            cout << "存款成功！当前仍透支：" << (-balance) << " / " << creditLimit << " 元" << endl;
        }
        return true;
    } else if (amount == 0) {
        cout << "存0元？何意味" << endl;
        return true;
    } else {
        cout << "存款金额必须大于0" << endl;
        return false;
    }
}

bool Comp_card::withdraw(double amount)
{
    settleInterestAndPenalty();
    
    if (amount > 0) {
        // 彩蛋：取款金额超过long long最大值时弹出图片
        if (amount > static_cast<double>(LLONG_MAX)) {
            showRealImage("caidan.png");
            return false;
        }
        
        if (amount <= balance + creditLimit) {
            balance -= amount;
            if (balance >= 0) {
                cout << "取款成功！当前余额：" << balance << endl;
            } else {
                cout << "取款成功！当前透支：" << (-balance) << " / " << creditLimit << " 元，记得还" << endl;
            }
            return true;
        }
    } else if (amount == 0) {
        cout << "取0元？何意味" << endl;
        return true;
    } else {
        cout << "越取钱越多是吧" << endl;
        return false;
    }
    
    return false;
}

double Comp_card::getBalance() const
{
    return balance;
}

double Comp_card::calculateInterest() const
{
    long long now = getCurrentTimestamp();
    long long last = parseTimeToTimestamp(lastInterestTime);
    long long minutesPassed = (now - last) / 60;
    
    if (minutesPassed > 0) {
        if (balance >= 0) {
            return balance * interestRatePerMinute * minutesPassed;
        } else {
            double overdueAmount = -balance;
            return overdueAmount * penaltyRatePerMinute * minutesPassed;
        }
    }
    return 0.0;
}

string Comp_card::getcardId() const
{
    return cardId;
}

string Comp_card::getLastInterestTime() const
{
    return lastInterestTime;
}

void Comp_card::updateInterest()
{
    settleInterestAndPenalty();
}

void Comp_card::showcardInfo() const
{
    cout << "卡类型：" << cardType << endl;
    cout << "卡号：" << cardId << endl;
    if (balance >= 0) {
        cout << "余额：" << balance << endl;
        cout << "利率：" << interestRatePerMinute << " /分钟" << endl;
    } else {
        cout << "透支：" << (-balance) << " / " << creditLimit << endl;
        cout << "罚息率：" << penaltyRatePerMinute << " /分钟" << endl;
    }
}