#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <chrono>
#include <map>
#include <mutex>
#include <cstdlib> // for rand and srand
#include <ctime>   // for time

using namespace std;

// Mock Bank Database
class BankDatabase {
public:
    // Constructor initializing accounts with some balance
    BankDatabase() {
        accounts["A123"] = 1000.00; // Payer's account
        accounts["B456"] = 200.00;  // Payee's account
    }

    // Function to get balance of an account
    double getBalance(const string &account_id) {
        lock_guard<mutex> guard(db_mutex);
        if (accounts.find(account_id) != accounts.end()) {
            return accounts[account_id];
        }
        throw runtime_error("Account not found.");
    }

    // Function to update balance of an account
    void updateBalance(const string &account_id, double amount) {
        lock_guard<mutex> guard(db_mutex);
        if (accounts.find(account_id) != accounts.end()) {
            accounts[account_id] += amount;
        } else {
            throw runtime_error("Account not found.");
        }
    }

private:
    map<string, double> accounts;
    mutex db_mutex;
};

// function to simulate bank API calls
bool callBankAPI() {
    // Simulate a random API call outcome (success or failure)
    return rand() % 2 == 0; 
}

// Payer Bank (Bank A)
class PayerBank {
public:
    PayerBank(BankDatabase &db) : db(db) {}

    // Function to debit amount from the payer's account
    bool debit(const string &account_id, double amount) {
        double current_balance = db.getBalance(account_id);
        if (current_balance >= amount) {
            cout << "Debiting " << amount << " from account " << account_id << endl;
            db.updateBalance(account_id, -amount);
            return true;
        } else {
            cerr << "Transaction failed: Insufficient funds in account " << account_id << endl;
            return false; 
        }
    }

private:
    BankDatabase &db;
};

// Payee Bank (Bank B)
class PayeeBank {
public:
    PayeeBank(BankDatabase &db) : db(db) {}

    // Function to credit amount to the payee's account
    bool credit(const string &account_id, double amount) {
        cout << "Crediting " << amount << " to account " << account_id << endl;
        db.updateBalance(account_id, amount);
        return true;
    }

private:
    BankDatabase &db;
};

// Transfer Service (Mediator)
class TransferService {
public:
    TransferService(PayerBank &payer_bank, PayeeBank &payee_bank)
        : payer_bank(payer_bank), payee_bank(payee_bank) {}

    // Function to transfer amount from payer to payee
    bool transfer(const string &payer_account, const string &payee_account, double amount) {
        
        cout << "Starting Transaction..." << endl;

        //  Debit from payer's account
        cout << "Phase 1: Initiating debit..." << endl;
        if (!payer_bank.debit(payer_account, amount)) {
            cerr << "Transaction failed: Debit failed. Aborting transaction." << endl;
            return false; // Debit failure
        }

        // Simulate API call for crediting the payee's account
        cout << "Simulating API call for credit..." << endl;
        if (!callBankAPI()) {
            cerr << "API call failed during crediting. Rolling back debit." << endl;
            payer_bank.debit(payer_account, -amount); // Revert debit
            return false; // API call failure
        }

        //  Credit to payee's account
        cout << "Phase 2: Initiating credit..." << endl;
        if (!payee_bank.credit(payee_account, amount)) {
            cerr << "Transaction failed: Credit failed. Rolling back debit." << endl;
            payer_bank.debit(payer_account, -amount); // Revert debit
            return false; // Credit failure
        }

        cout << "Transaction completed successfully." << endl;
        return true;
    }

private:
    PayerBank &payer_bank;
    PayeeBank &payee_bank;
};


int main() {
    
    srand(static_cast<unsigned int>(time(0)));

    try {
        
        BankDatabase bank_db;

        PayerBank payer_bank(bank_db);
        PayeeBank payee_bank(bank_db);

        TransferService transfer_service(payer_bank, payee_bank);
 
        transfer_service.transfer("A123", "B456", 50.00);

        cout << "\nFinal Balances:" << endl;
        cout << "Payer Account A123: " << bank_db.getBalance("A123") << endl;
        cout << "Payee Account B456: " << bank_db.getBalance("B456") << endl;

    } catch (const std::exception &ex) {
        cerr << "Error: " << ex.what() << endl;
    }

    return 0;
}
