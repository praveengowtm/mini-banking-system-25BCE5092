#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ACCOUNTS 100
#define MAX_TRANSACTIONS 1000
#define MAX_NAME 50
#define MAX_ACCNO 20
#define MAX_DATETIME 30

// ===== Structures =====

typedef struct {
    char type[10];       // "Deposit" or "Withdraw"
    double amount;
    int transactionNo;
    char dateTime[MAX_DATETIME];
} Transaction;

typedef struct {
    char accNo[MAX_ACCNO];
    char name[MAX_NAME];
    double balance;      // private concept: only modified via deposit/withdraw
    Transaction history[MAX_TRANSACTIONS];
    int txnCount;
    int nextTxnNo;
} Account;

typedef struct {
    Account accounts[MAX_ACCOUNTS];
    int accountCount;
} BankSystem;

// ===== Transaction Functions =====

void Transaction_init(Transaction *t, const char *type, double amount, int txnNo) {
    strcpy(t->type, type);
    t->amount = amount;
    t->transactionNo = txnNo;
    time_t now = time(NULL);
    strftime(t->dateTime, MAX_DATETIME, "%Y-%m-%d %H:%M:%S", localtime(&now));
}

void Transaction_initWithDate(Transaction *t, const char *type, double amount, int txnNo, const char *dt) {
    strcpy(t->type, type);
    t->amount = amount;
    t->transactionNo = txnNo;
    strcpy(t->dateTime, dt);
}

// ===== Account Functions =====

void Account_init(Account *acc, const char *accNo, const char *name, double balance) {
    strcpy(acc->accNo, accNo);
    strcpy(acc->name, name);
    acc->balance = balance;
    acc->txnCount = 0;
    acc->nextTxnNo = 1;
}

double Account_getBalance(const Account *acc) {
    return acc->balance;
}

int Account_deposit(Account *acc, double amount) {
    if (amount <= 0) {
        printf("  [Error] Deposit amount must be positive.\n");
        return 0;
    }
    acc->balance += amount;
    Transaction_init(&acc->history[acc->txnCount], "Deposit", amount, acc->nextTxnNo++);
    acc->txnCount++;
    printf("  [Success] Deposited %.2f. New balance: %.2f\n", amount, acc->balance);
    return 1;
}

int Account_withdraw(Account *acc, double amount) {
    if (amount <= 0) {
        printf("  [Error] Withdrawal amount must be positive.\n");
        return 0;
    }
    if (amount > acc->balance) {
        printf("  [Error] Insufficient funds. Balance: %.2f\n", acc->balance);
        return 0;
    }
    acc->balance -= amount;
    Transaction_init(&acc->history[acc->txnCount], "Withdraw", amount, acc->nextTxnNo++);
    acc->txnCount++;
    printf("  [Success] Withdrew %.2f. New balance: %.2f\n", amount, acc->balance);
    return 1;
}

void Account_showLast5(const Account *acc) {
    if (acc->txnCount == 0) {
        printf("  No transactions found.\n");
        return;
    }
    int start = acc->txnCount - 5;
    if (start < 0) start = 0;
    int count = acc->txnCount - start;
    printf("  Last %d transactions:\n", count);
    printf("  %-6s %-12s %-12s %s\n", "TxnNo", "Type", "Amount", "Date/Time");
    printf("  --------------------------------------------------\n");
    for (int i = start; i < acc->txnCount; i++) {
        printf("  %-6d %-12s %-12.2f %s\n",
               acc->history[i].transactionNo,
               acc->history[i].type,
               acc->history[i].amount,
               acc->history[i].dateTime);
    }
}

// ===== BankSystem Functions =====

void BankSystem_init(BankSystem *bs) {
    bs->accountCount = 0;
}

Account* BankSystem_find(BankSystem *bs, const char *accNo) {
    for (int i = 0; i < bs->accountCount; i++) {
        if (strcmp(bs->accounts[i].accNo, accNo) == 0)
            return &bs->accounts[i];
    }
    return NULL;
}

void BankSystem_saveToFiles(BankSystem *bs) {
    FILE *aFile = fopen("accounts.txt", "w");
    if (aFile) {
        for (int i = 0; i < bs->accountCount; i++) {
            fprintf(aFile, "%s|%s|%.2f\n",
                    bs->accounts[i].accNo,
                    bs->accounts[i].name,
                    bs->accounts[i].balance);
        }
        fclose(aFile);
    }

    FILE *tFile = fopen("transactions.txt", "w");
    if (tFile) {
        for (int i = 0; i < bs->accountCount; i++) {
            for (int j = 0; j < bs->accounts[i].txnCount; j++) {
                fprintf(tFile, "%s|%s|%.2f|%d|%s\n",
                        bs->accounts[i].accNo,
                        bs->accounts[i].history[j].type,
                        bs->accounts[i].history[j].amount,
                        bs->accounts[i].history[j].transactionNo,
                        bs->accounts[i].history[j].dateTime);
            }
        }
        fclose(tFile);
    }
}

void BankSystem_loadFromFiles(BankSystem *bs) {
    FILE *aFile = fopen("accounts.txt", "r");
    if (aFile) {
        char line[256];
        while (fgets(line, sizeof(line), aFile)) {
            line[strcspn(line, "\n")] = 0;
            char accNo[MAX_ACCNO], name[MAX_NAME];
            double bal;
            if (sscanf(line, "%[^|]|%[^|]|%lf", accNo, name, &bal) == 3) {
                Account_init(&bs->accounts[bs->accountCount], accNo, name, bal);
                bs->accountCount++;
            }
        }
        fclose(aFile);
    }

    FILE *tFile = fopen("transactions.txt", "r");
    if (tFile) {
        char line[256];
        while (fgets(line, sizeof(line), tFile)) {
            line[strcspn(line, "\n")] = 0;
            char accNo[MAX_ACCNO], type[10], dt[MAX_DATETIME];
            double amt;
            int txnNo;
            if (sscanf(line, "%[^|]|%[^|]|%lf|%d|%[^\n]", accNo, type, &amt, &txnNo, dt) == 5) {
                Account *acc = BankSystem_find(bs, accNo);
                if (acc) {
                    Transaction_initWithDate(&acc->history[acc->txnCount], type, amt, txnNo, dt);
                    acc->txnCount++;
                    if (txnNo >= acc->nextTxnNo)
                        acc->nextTxnNo = txnNo + 1;
                }
            }
        }
        fclose(tFile);
    }
}

void BankSystem_createAccount(BankSystem *bs, const char *accNo, const char *name, double bal) {
    if (BankSystem_find(bs, accNo)) {
        printf("  [Error] Account %s already exists.\n", accNo);
        return;
    }
    if (bs->accountCount >= MAX_ACCOUNTS) {
        printf("  [Error] Maximum account limit reached.\n");
        return;
    }
    Account_init(&bs->accounts[bs->accountCount], accNo, name, bal);
    bs->accountCount++;
    printf("  [Success] Account %s created for %s.\n", accNo, name);
    BankSystem_saveToFiles(bs);
}

void BankSystem_deposit(BankSystem *bs, const char *accNo, double amount) {
    Account *acc = BankSystem_find(bs, accNo);
    if (!acc) { printf("  [Error] Account not found.\n"); return; }
    if (Account_deposit(acc, amount)) BankSystem_saveToFiles(bs);
}

void BankSystem_withdraw(BankSystem *bs, const char *accNo, double amount) {
    Account *acc = BankSystem_find(bs, accNo);
    if (!acc) { printf("  [Error] Account not found.\n"); return; }
    if (Account_withdraw(acc, amount)) BankSystem_saveToFiles(bs);
}

void BankSystem_balanceEnquiry(BankSystem *bs, const char *accNo) {
    Account *acc = BankSystem_find(bs, accNo);
    if (!acc) { printf("  [Error] Account not found.\n"); return; }
    printf("  Account: %s | Name: %s | Balance: %.2f\n", acc->accNo, acc->name, Account_getBalance(acc));
}

void BankSystem_viewTransactions(BankSystem *bs, const char *accNo) {
    Account *acc = BankSystem_find(bs, accNo);
    if (!acc) { printf("  [Error] Account not found.\n"); return; }
    Account_showLast5(acc);
}

void BankSystem_reportTotal(BankSystem *bs) {
    double total = 0;
    for (int i = 0; i < bs->accountCount; i++)
        total += Account_getBalance(&bs->accounts[i]);
    printf("  Total money across all accounts: %.2f\n", total);
}

void BankSystem_reportLowBalance(BankSystem *bs, double threshold) {
    int found = 0;
    for (int i = 0; i < bs->accountCount; i++) {
        if (Account_getBalance(&bs->accounts[i]) < threshold) {
            printf("  %s - %s - Balance: %.2f\n",
                   bs->accounts[i].accNo, bs->accounts[i].name,
                   Account_getBalance(&bs->accounts[i]));
            found = 1;
        }
    }
    if (!found) printf("  No accounts below threshold %.2f.\n", threshold);
}

// ===== Main =====

int main() {
    BankSystem bank;
    BankSystem_init(&bank);
    BankSystem_loadFromFiles(&bank);

    int choice;
    char accNo[MAX_ACCNO], name[MAX_NAME];
    double amount;

    do {
        printf("\n====== Mini Banking System ======\n");
        printf("1. Create Account\n");
        printf("2. Deposit\n");
        printf("3. Withdraw\n");
        printf("4. Balance Enquiry\n");
        printf("5. Transaction History (Last 5)\n");
        printf("6. Report: Total Money\n");
        printf("7. Report: Low Balance Accounts\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("  Account No: "); scanf("%s", accNo);
                printf("  Name: "); getchar(); fgets(name, MAX_NAME, stdin);
                name[strcspn(name, "\n")] = 0;
                printf("  Initial Balance: "); scanf("%lf", &amount);
                BankSystem_createAccount(&bank, accNo, name, amount);
                break;
            case 2:
                printf("  Account No: "); scanf("%s", accNo);
                printf("  Amount: "); scanf("%lf", &amount);
                BankSystem_deposit(&bank, accNo, amount);
                break;
            case 3:
                printf("  Account No: "); scanf("%s", accNo);
                printf("  Amount: "); scanf("%lf", &amount);
                BankSystem_withdraw(&bank, accNo, amount);
                break;
            case 4:
                printf("  Account No: "); scanf("%s", accNo);
                BankSystem_balanceEnquiry(&bank, accNo);
                break;
            case 5:
                printf("  Account No: "); scanf("%s", accNo);
                BankSystem_viewTransactions(&bank, accNo);
                break;
            case 6:
                BankSystem_reportTotal(&bank);
                break;
            case 7:
                printf("  Threshold: "); scanf("%lf", &amount);
                BankSystem_reportLowBalance(&bank, amount);
                break;
            case 0:
                printf("  Goodbye!\n");
                BankSystem_saveToFiles(&bank);
                break;
            default:
                printf("  Invalid choice.\n");
        }
    } while (choice != 0);

    return 0;
}
