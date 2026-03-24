/*
 * MINI BANKING SYSTEM - WEB VERSION
 */
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define MAX_ACCOUNTS 100
#define ACCOUNTS_FILE "accounts.txt"
#define TRANSACTIONS_FILE "transactions.txt"

/* ================= STRUCTS ================= */

typedef struct {
    char accNo[20];
    char name[60];
    double balance;
} Account;

typedef struct {
    char accNo[20];
    char type[15];
    double amount;
    char timestamp[25];
} Transaction;

/* ================= GLOBALS ================= */

Account accounts[MAX_ACCOUNTS];
int accountCount = 0;

/* ================= UTIL FUNCTIONS ================= */

void getTimestamp(char *buf, int size)
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

void loadAccounts(void)
{
    accountCount = 0;
    FILE *f = fopen(ACCOUNTS_FILE, "r");
    if (!f) return;

    char line[200];

    while (fgets(line, sizeof(line), f)) {
        char *p1 = strchr(line, '|');
        char *p2 = strchr(p1 + 1, '|');

        *p1 = '\0';
        *p2 = '\0';

        strcpy(accounts[accountCount].accNo, line);
        strcpy(accounts[accountCount].name, p1 + 1);
        accounts[accountCount].balance = atof(p2 + 1);

        accountCount++;
    }
    fclose(f);
}

void saveAccounts(void)
{
    FILE *f = fopen(ACCOUNTS_FILE, "w");
    if (!f) {
        printf("ERROR writing file\n");
        return;
    }

    for (int i = 0; i < accountCount; i++) {
        fprintf(f, "%s|%s|%.2f\n",
                accounts[i].accNo,
                accounts[i].name,
                accounts[i].balance);
    }

    fclose(f);
}

int findAccount(const char *accNo)
{
    for (int i = 0; i < accountCount; i++) {
        if (strcmp(accounts[i].accNo, accNo) == 0)
            return i;
    }
    return -1;
}

void logTransaction(const char *accNo, const char *type, double amount)
{
    FILE *f = fopen(TRANSACTIONS_FILE, "a");
    if (!f) return;

    char ts[25];
    getTimestamp(ts, sizeof(ts));

    fprintf(f, "%s|%s|%.2f|%s\n", accNo, type, amount, ts);
    fclose(f);
}

/* ================= UI FUNCTIONS ================= */

EMSCRIPTEN_KEEPALIVE
void ui_create_account(char* accNo, char* name, double balance)
{
    loadAccounts();

    if (findAccount(accNo) >= 0) {
        printf("ERROR: Account already exists\n");
        return;
    }

    strcpy(accounts[accountCount].accNo, accNo);
    strcpy(accounts[accountCount].name, name);
    accounts[accountCount].balance = balance;
    accountCount++;

    saveAccounts();
    logTransaction(accNo, "CREATE", balance);

    printf("SUCCESS: Account Created\n");
}

EMSCRIPTEN_KEEPALIVE
void ui_deposit(char* accNo, double amount)
{
    loadAccounts();
    int idx = findAccount(accNo);

    if (idx < 0) {
        printf("ERROR: Account not found\n");
        return;
    }

    accounts[idx].balance += amount;
    saveAccounts();
    logTransaction(accNo, "DEPOSIT", amount);

    printf("SUCCESS: Deposited %.2f\n", amount);
    printf("New Balance: %.2f\n", accounts[idx].balance);
}

EMSCRIPTEN_KEEPALIVE
void ui_withdraw(char* accNo, double amount)
{
    loadAccounts();
    int idx = findAccount(accNo);

    if (idx < 0) {
        printf("ERROR: Account not found\n");
        return;
    }

    if (amount > accounts[idx].balance) {
        printf("ERROR: Insufficient balance\n");
        return;
    }

    accounts[idx].balance -= amount;
    saveAccounts();
    logTransaction(accNo, "WITHDRAW", amount);

    printf("SUCCESS: Withdrawn %.2f\n", amount);
    printf("New Balance: %.2f\n", accounts[idx].balance);
}

EMSCRIPTEN_KEEPALIVE
void ui_summary()
{
    loadAccounts();

    if (accountCount == 0) {
        printf("No accounts found\n");
        return;
    }

    printf("\n--- ACCOUNT SUMMARY ---\n");

    for (int i = 0; i < accountCount; i++) {
        printf("%s | %s | %.2f\n",
               accounts[i].accNo,
               accounts[i].name,
               accounts[i].balance);
    }
}

/* ================= MAIN (FIXED) ================= */

int main(void)
{
    setbuf(stdout, NULL);
    printf("Vault Bank Web Version Loaded\n");
    return 0;
}