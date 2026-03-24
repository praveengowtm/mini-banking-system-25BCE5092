#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <emscripten/emscripten.h>

#define MAX_ACCOUNTS 100

typedef struct {
    char accNo[20];
    char name[60];
    double balance;
} Account;

Account accounts[MAX_ACCOUNTS];
int accountCount = 0;

/* ================= FILE HANDLING ================= */

void loadAccounts()
{
    FILE *f = fopen("accounts.txt", "r");
    accountCount = 0;

    if (!f) return;

    while (fscanf(f, "%[^|]|%[^|]|%lf\n",
        accounts[accountCount].accNo,
        accounts[accountCount].name,
        &accounts[accountCount].balance) != EOF)
    {
        accountCount++;
    }

    fclose(f);
}

void saveAccounts()
{
    FILE *f = fopen("accounts.txt", "w");

    for (int i = 0; i < accountCount; i++) {
        fprintf(f, "%s|%s|%.2f\n",
            accounts[i].accNo,
            accounts[i].name,
            accounts[i].balance);
    }

    fclose(f);
}

int findAccount(char *accNo)
{
    for (int i = 0; i < accountCount; i++) {
        if (strcmp(accounts[i].accNo, accNo) == 0)
            return i;
    }
    return -1;
}

/* ================= UI FUNCTIONS ================= */

EMSCRIPTEN_KEEPALIVE
void ui_create_account(char* accNo, char* name, double balance)
{
    loadAccounts();

    if (findAccount(accNo) != -1) {
        printf("ERROR: Account exists\n");
        return;
    }

    strcpy(accounts[accountCount].accNo, accNo);
    strcpy(accounts[accountCount].name, name);
    accounts[accountCount].balance = balance;
    accountCount++;

    saveAccounts();

    printf("SUCCESS: Account Created\n");
}

EMSCRIPTEN_KEEPALIVE
void ui_deposit(char* accNo, double amount)
{
    loadAccounts();

    int idx = findAccount(accNo);
    if (idx == -1) {
        printf("ERROR: Account not found\n");
        return;
    }

    accounts[idx].balance += amount;
    saveAccounts();

    printf("Deposited %.2f\n", amount);
    printf("New Balance: %.2f\n", accounts[idx].balance);
}

EMSCRIPTEN_KEEPALIVE
void ui_withdraw(char* accNo, double amount)
{
    loadAccounts();

    int idx = findAccount(accNo);
    if (idx == -1) {
        printf("ERROR: Account not found\n");
        return;
    }

    if (amount > accounts[idx].balance) {
        printf("ERROR: Insufficient balance\n");
        return;
    }

    accounts[idx].balance -= amount;
    saveAccounts();

    printf("Withdrawn %.2f\n", amount);
    printf("New Balance: %.2f\n", accounts[idx].balance);
}

EMSCRIPTEN_KEEPALIVE
void ui_summary()
{
    loadAccounts();

    if (accountCount == 0) {
        printf("No accounts\n");
        return;
    }

    printf("\n--- ACCOUNTS ---\n");

    for (int i = 0; i < accountCount; i++) {
        printf("%s | %s | %.2f\n",
            accounts[i].accNo,
            accounts[i].name,
            accounts[i].balance);
    }
}

/* ================= MAIN ================= */

int main()
{
    printf("Vault Bank Loaded\n");
    return 0;
}