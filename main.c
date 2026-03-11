/*
 * ============================================================
 *  MINI BANKING SYSTEM WITH TRANSACTION LOG
 *  Language : Pure C
 *
 *  COMPILE COMMAND:
 *  emcc main.c -o index.html --shell-file shell.html -s ASYNCIFY=1 -s FORCE_FILESYSTEM=1
 *
 *  FILES GENERATED: index.html  index.js  index.wasm
 *
 *  Data files used:
 *    accounts.txt     — stores account records (pipe-delimited)
 *    transactions.txt — stores transaction log
 *
 *  Structs   : Account, Transaction
 *  Functions : 13 user-defined functions
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ============================================================
 *  CONSTANTS
 * ============================================================ */
#define MAX_ACCOUNTS      100
#define ACCOUNTS_FILE     "accounts.txt"
#define TRANSACTIONS_FILE "transactions.txt"

/* ============================================================
 *  STRUCT 1 : Account
 * ============================================================ */
typedef struct {
    char   accNo[20];
    char   name[60];
    double balance;
} Account;

/* ============================================================
 *  STRUCT 2 : Transaction
 * ============================================================ */
typedef struct {
    char   accNo[20];
    char   type[15];   /* CREATE | DEPOSIT | WITHDRAW */
    double amount;
    char   timestamp[25];
} Transaction;

/* ============================================================
 *  GLOBAL ARRAY
 * ============================================================ */
Account accounts[MAX_ACCOUNTS];
int     accountCount = 0;

/* ============================================================
 *  FUNCTION PROTOTYPES
 * ============================================================ */
void clearInputBuffer(void);
void getTimestamp(char *buf, int size);
void loadAccounts(void);
void saveAccounts(void);
int  findAccount(const char *accNo);
void logTransaction(const char *accNo, const char *type, double amount);
void createAccount(void);
void depositFunds(void);
void withdrawFunds(void);
void viewSummary(void);
void searchAccount(void);
void viewLastTransactions(void);
void printMenu(void);

/* ============================================================
 *  FUNCTION 1 : clearInputBuffer
 *  Clears leftover characters in stdin after scanf.
 *  MUST be called after every scanf to avoid ghost inputs.
 * ============================================================ */
void clearInputBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* ============================================================
 *  FUNCTION 2 : getTimestamp
 *  Fills buf with current local date-time "YYYY-MM-DD HH:MM:SS"
 * ============================================================ */
void getTimestamp(char *buf, int size)
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

/* ============================================================
 *  FUNCTION 3 : loadAccounts
 *  Reads accounts.txt into global accounts[] array.
 *  File format (each line): accNo|name|balance
 * ============================================================ */
void loadAccounts(void)
{
    accountCount = 0;
    FILE *f = fopen(ACCOUNTS_FILE, "r");
    if (!f) return;  /* File not created yet - that is OK */

    char line[200];
    while (fgets(line, sizeof(line), f) && accountCount < MAX_ACCOUNTS) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        char *p1 = strchr(line, '|');
        if (!p1) continue;
        char *p2 = strchr(p1 + 1, '|');
        if (!p2) continue;

        *p1 = '\0';
        *p2 = '\0';

        strncpy(accounts[accountCount].accNo,  line,     19);
        strncpy(accounts[accountCount].name,   p1 + 1,   59);
        accounts[accountCount].balance = atof(p2 + 1);
        accountCount++;
    }
    fclose(f);
}

/* ============================================================
 *  FUNCTION 4 : saveAccounts
 *  Overwrites accounts.txt with current accounts[] array.
 * ============================================================ */
void saveAccounts(void)
{
    FILE *f = fopen(ACCOUNTS_FILE, "w");
    if (!f) {
        printf("  ERROR: Cannot write to accounts file.\n");
        fflush(stdout);
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

/* ============================================================
 *  FUNCTION 5 : findAccount
 *  Linear search by accNo. Returns array index or -1.
 * ============================================================ */
int findAccount(const char *accNo)
{
    for (int i = 0; i < accountCount; i++) {
        if (strcmp(accounts[i].accNo, accNo) == 0)
            return i;
    }
    return -1;
}

/* ============================================================
 *  FUNCTION 6 : logTransaction
 *  Appends a transaction line to transactions.txt.
 *  Format: accNo|type|amount|timestamp
 * ============================================================ */
void logTransaction(const char *accNo, const char *type, double amount)
{
    FILE *f = fopen(TRANSACTIONS_FILE, "a");
    if (!f) return;
    char ts[25];
    getTimestamp(ts, sizeof(ts));
    fprintf(f, "%s|%s|%.2f|%s\n", accNo, type, amount, ts);
    fclose(f);
}

/* ============================================================
 *  FUNCTION 7 : createAccount
 *  Prompts for AccNo, Name, Initial Balance. Validates & saves.
 * ============================================================ */
void createAccount(void)
{
    printf("\n  --- CREATE NEW ACCOUNT ---\n");

    char   accNo[20], name[60];
    double balance;

    /* Account Number */
    printf("  Account Number (no spaces, max 19 chars): ");
    fflush(stdout);
    if (scanf("%19s", accNo) != 1) { clearInputBuffer(); return; }
    clearInputBuffer();

    /* Validate: no pipe */
    if (strchr(accNo, '|')) {
        printf("  ERROR: '|' character not allowed.\n\n");
        fflush(stdout);
        return;
    }

    loadAccounts();

    /* Validate: no duplicate */
    if (findAccount(accNo) >= 0) {
        printf("  ERROR: Account '%s' already exists.\n\n", accNo);
        fflush(stdout);
        return;
    }

    /* Validate: capacity */
    if (accountCount >= MAX_ACCOUNTS) {
        printf("  ERROR: Maximum account limit reached.\n\n");
        fflush(stdout);
        return;
    }

    /* Full Name */
    printf("  Full Name: ");
    fflush(stdout);
    if (!fgets(name, sizeof(name), stdin)) return;
    name[strcspn(name, "\r\n")] = '\0';

    if (strlen(name) == 0) {
        printf("  ERROR: Name cannot be empty.\n\n");
        fflush(stdout);
        return;
    }
    if (strchr(name, '|')) {
        printf("  ERROR: '|' character not allowed in name.\n\n");
        fflush(stdout);
        return;
    }

    /* Initial Balance */
    printf("  Initial Deposit Amount (>= 0): ");
    fflush(stdout);
    if (scanf("%lf", &balance) != 1) {
        printf("  ERROR: Invalid amount.\n\n");
        fflush(stdout);
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (balance < 0.0) {
        printf("  ERROR: Balance cannot be negative.\n\n");
        fflush(stdout);
        return;
    }

    /* Save */
    strncpy(accounts[accountCount].accNo,  accNo, 19);
    strncpy(accounts[accountCount].name,   name,  59);
    accounts[accountCount].balance = balance;
    accountCount++;

    saveAccounts();
    logTransaction(accNo, "CREATE", balance);

    printf("\n  [SUCCESS] Account Created!\n");
    printf("  Account No : %s\n", accNo);
    printf("  Name       : %s\n", name);
    printf("  Balance    : %.2f\n\n", balance);
    fflush(stdout);
}

/* ============================================================
 *  FUNCTION 8 : depositFunds
 *  Adds funds to an account. Validates positive amount.
 * ============================================================ */
void depositFunds(void)
{
    printf("\n  --- DEPOSIT FUNDS ---\n");

    char   accNo[20];
    double amount;

    printf("  Account Number: ");
    fflush(stdout);
    if (scanf("%19s", accNo) != 1) { clearInputBuffer(); return; }
    clearInputBuffer();

    loadAccounts();
    int idx = findAccount(accNo);
    if (idx < 0) {
        printf("  ERROR: Account '%s' not found.\n\n", accNo);
        fflush(stdout);
        return;
    }

    printf("  Current Balance : %.2f\n", accounts[idx].balance);
    printf("  Deposit Amount  : ");
    fflush(stdout);
    if (scanf("%lf", &amount) != 1) {
        printf("  ERROR: Invalid amount.\n\n");
        fflush(stdout);
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (amount <= 0.0) {
        printf("  ERROR: Amount must be positive.\n\n");
        fflush(stdout);
        return;
    }

    accounts[idx].balance += amount;
    saveAccounts();
    logTransaction(accNo, "DEPOSIT", amount);

    printf("\n  [SUCCESS] Deposit Done!\n");
    printf("  Deposited   : %.2f\n", amount);
    printf("  New Balance : %.2f\n\n", accounts[idx].balance);
    fflush(stdout);
}

/* ============================================================
 *  FUNCTION 9 : withdrawFunds
 *  Deducts funds. Rejects if balance would go negative.
 * ============================================================ */
void withdrawFunds(void)
{
    printf("\n  --- WITHDRAW FUNDS ---\n");

    char   accNo[20];
    double amount;

    printf("  Account Number: ");
    fflush(stdout);
    if (scanf("%19s", accNo) != 1) { clearInputBuffer(); return; }
    clearInputBuffer();

    loadAccounts();
    int idx = findAccount(accNo);
    if (idx < 0) {
        printf("  ERROR: Account '%s' not found.\n\n", accNo);
        fflush(stdout);
        return;
    }

    printf("  Current Balance   : %.2f\n", accounts[idx].balance);
    printf("  Withdrawal Amount : ");
    fflush(stdout);
    if (scanf("%lf", &amount) != 1) {
        printf("  ERROR: Invalid amount.\n\n");
        fflush(stdout);
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (amount <= 0.0) {
        printf("  ERROR: Amount must be positive.\n\n");
        fflush(stdout);
        return;
    }
    if (amount > accounts[idx].balance) {
        printf("  ERROR: Insufficient balance!\n");
        printf("  Available : %.2f\n\n", accounts[idx].balance);
        fflush(stdout);
        return;
    }

    accounts[idx].balance -= amount;
    saveAccounts();
    logTransaction(accNo, "WITHDRAW", amount);

    printf("\n  [SUCCESS] Withdrawal Done!\n");
    printf("  Withdrawn   : %.2f\n", amount);
    printf("  New Balance : %.2f\n\n", accounts[idx].balance);
    fflush(stdout);
}

/* ============================================================
 *  FUNCTION 10 : viewSummary
 *  Prints a formatted table of all accounts + total balance.
 * ============================================================ */
void viewSummary(void)
{
    loadAccounts();

    printf("\n  --- ACCOUNT SUMMARY ---\n");
    printf("  Total Accounts: %d\n\n", accountCount);

    if (accountCount == 0) {
        printf("  No accounts found.\n\n");
        fflush(stdout);
        return;
    }

    printf("  %-20s %-25s %15s\n", "Account No", "Name", "Balance");
    printf("  %s\n", "----------------------------------------------------------------");

    double total = 0.0;
    for (int i = 0; i < accountCount; i++) {
        printf("  %-20s %-25s %15.2f\n",
               accounts[i].accNo,
               accounts[i].name,
               accounts[i].balance);
        total += accounts[i].balance;
    }

    printf("  %s\n", "----------------------------------------------------------------");
    printf("  %-46s %15.2f\n\n", "TOTAL BALANCE:", total);
    fflush(stdout);
}

/* ============================================================
 *  FUNCTION 11 : searchAccount
 *  Finds and displays one account by AccNo.
 * ============================================================ */
void searchAccount(void)
{
    printf("\n  --- SEARCH ACCOUNT ---\n");

    char accNo[20];
    printf("  Enter Account Number: ");
    fflush(stdout);
    if (scanf("%19s", accNo) != 1) { clearInputBuffer(); return; }
    clearInputBuffer();

    loadAccounts();
    int idx = findAccount(accNo);

    if (idx < 0) {
        printf("  ERROR: Account '%s' not found.\n\n", accNo);
        fflush(stdout);
        return;
    }

    printf("\n  Account No : %s\n", accounts[idx].accNo);
    printf("  Name       : %s\n", accounts[idx].name);
    printf("  Balance    : %.2f\n\n", accounts[idx].balance);
    fflush(stdout);
}

/* ============================================================
 *  FUNCTION 12 : viewLastTransactions
 *  Reads transactions.txt and displays last 5 for an account.
 * ============================================================ */
void viewLastTransactions(void)
{
    printf("\n  --- LAST 5 TRANSACTIONS ---\n");

    char accNo[20];
    printf("  Enter Account Number: ");
    fflush(stdout);
    if (scanf("%19s", accNo) != 1) { clearInputBuffer(); return; }
    clearInputBuffer();

    /* Verify account exists */
    loadAccounts();
    if (findAccount(accNo) < 0) {
        printf("  ERROR: Account '%s' not found.\n\n", accNo);
        fflush(stdout);
        return;
    }

    FILE *f = fopen(TRANSACTIONS_FILE, "r");
    if (!f) {
        printf("  INFO: No transactions on record yet.\n\n");
        fflush(stdout);
        return;
    }

    /* Collect matching lines (up to 500) */
    char   tx_type[500][15];
    double tx_amount[500];
    char   tx_ts[500][25];
    int    count = 0;

    char line[200];
    while (fgets(line, sizeof(line), f) && count < 500) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        char tmp[200];
        strncpy(tmp, line, 199);

        char *p_acc  = strtok(tmp,  "|");
        char *p_type = strtok(NULL, "|");
        char *p_amt  = strtok(NULL, "|");
        char *p_ts   = strtok(NULL, "|");

        if (!p_acc || !p_type || !p_amt || !p_ts) continue;

        if (strcmp(p_acc, accNo) == 0) {
            strncpy(tx_type[count],   p_type, 14);
            tx_amount[count] = atof(p_amt);
            strncpy(tx_ts[count],     p_ts,   24);
            count++;
        }
    }
    fclose(f);

    if (count == 0) {
        printf("  INFO: No transactions found for '%s'.\n\n", accNo);
        fflush(stdout);
        return;
    }

    /* Show last 5 */
    int start = (count > 5) ? count - 5 : 0;
    int shown = count - start;

    printf("\n  Last %d transaction(s) for account: %s\n\n", shown, accNo);
    printf("  %-5s %-12s %12s   %s\n", "No.", "Type", "Amount", "Date & Time");
    printf("  %s\n", "------------------------------------------------------");

    for (int i = start; i < count; i++) {
        printf("  %-5d %-12s %12.2f   %s\n",
               i - start + 1,
               tx_type[i],
               tx_amount[i],
               tx_ts[i]);
    }
    printf("\n");
    fflush(stdout);
}

/* ============================================================
 *  FUNCTION 13 : printMenu
 *  Prints the main menu to stdout.
 * ============================================================ */
void printMenu(void)
{
    printf("  ==============================\n");
    printf("       VAULT BANK SYSTEM        \n");
    printf("  ==============================\n");
    printf("  1. Create Account\n");
    printf("  2. Deposit Funds\n");
    printf("  3. Withdraw Funds\n");
    printf("  4. View Account Summary\n");
    printf("  5. Search Account\n");
    printf("  6. View Last 5 Transactions\n");
    printf("  7. Exit\n");
    printf("  ==============================\n");
    printf("  Enter choice (1-7): ");
    fflush(stdout);
}

/* ============================================================
 *  main() — Menu loop
 * ============================================================ */
int main(void)
{
    int choice;

    printf("\n  Welcome to Vault Bank System!\n\n");
    fflush(stdout);

    while (1) {
        printMenu();

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("  ERROR: Enter a number between 1 and 7.\n\n");
            fflush(stdout);
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: createAccount();        break;
            case 2: depositFunds();         break;
            case 3: withdrawFunds();        break;
            case 4: viewSummary();          break;
            case 5: searchAccount();        break;
            case 6: viewLastTransactions(); break;
            case 7:
                printf("\n  Goodbye! Thank you for using Vault Bank.\n\n");
                fflush(stdout);
                return 0;
            default:
                printf("  ERROR: Invalid choice. Enter 1 to 7.\n\n");
                fflush(stdout);
        }
    }

    return 0;
}
