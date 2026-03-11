#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ACCOUNTS 100
#define ACCOUNTS_FILE "accounts.txt"
#define LOG_FILE "transactions.txt"

// ─── STRUCT ───────────────────────────────────────────────
typedef struct {
    int accNo;
    char name[50];
    float balance;
} Account;

typedef struct {
    int accNo;
    char type[20];   // "DEPOSIT" or "WITHDRAW"
    float amount;
    char timestamp[30];
} Transaction;

// ─── FUNCTION DECLARATIONS ────────────────────────────────
void createAccount();
void depositMoney();
void withdrawMoney();
void viewAllAccounts();
void searchAccount();
void viewLastTransactions();
int loadAccounts(Account accounts[], int *count);
void saveAccounts(Account accounts[], int count);
void logTransaction(int accNo, const char *type, float amount);
void displayMenu();

// ─── MAIN ─────────────────────────────────────────────────
int main() {
    int choice;

    printf("====================================\n");
    printf("   WELCOME TO MINI BANKING SYSTEM   \n");
    printf("====================================\n");

    while (1) {
        displayMenu();
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n'); // flush
            continue;
        }

        switch (choice) {
            case 1: createAccount();        break;
            case 2: depositMoney();         break;
            case 3: withdrawMoney();        break;
            case 4: viewAllAccounts();      break;
            case 5: searchAccount();        break;
            case 6: viewLastTransactions(); break;
            case 7:
                printf("\nThank you for using Mini Banking System. Goodbye!\n");
                exit(0);
            default:
                printf("Invalid choice! Please select between 1 and 7.\n");
        }
    }
    return 0;
}

// ─── DISPLAY MENU ─────────────────────────────────────────
void displayMenu() {
    printf("\n========== MAIN MENU ===========\n");
    printf(" 1. Create Account\n");
    printf(" 2. Deposit Money\n");
    printf(" 3. Withdraw Money\n");
    printf(" 4. View All Accounts\n");
    printf(" 5. Search Account by AccNo\n");
    printf(" 6. View Last 5 Transactions\n");
    printf(" 7. Exit\n");
    printf("=================================\n");
}

// ─── LOAD ACCOUNTS FROM FILE ──────────────────────────────
int loadAccounts(Account accounts[], int *count) {
    FILE *fp = fopen(ACCOUNTS_FILE, "r");
    *count = 0;
    if (fp == NULL) return 0;

    while (fscanf(fp, "%d %s %f",
        &accounts[*count].accNo,
        accounts[*count].name,
        &accounts[*count].balance) == 3) {
        (*count)++;
        if (*count >= MAX_ACCOUNTS) break;
    }
    fclose(fp);
    return 1;
}

// ─── SAVE ACCOUNTS TO FILE ────────────────────────────────
void saveAccounts(Account accounts[], int count) {
    FILE *fp = fopen(ACCOUNTS_FILE, "w");
    if (fp == NULL) {
        printf("Error: Could not save accounts.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d %s %.2f\n",
            accounts[i].accNo,
            accounts[i].name,
            accounts[i].balance);
    }
    fclose(fp);
}

// ─── LOG TRANSACTION ──────────────────────────────────────
void logTransaction(int accNo, const char *type, float amount) {
    FILE *fp = fopen(LOG_FILE, "a");
    if (fp == NULL) {
        printf("Warning: Could not write to transaction log.\n");
        return;
    }

    time_t now = time(NULL);
    char timeStr[30];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d_%H:%M:%S", localtime(&now));

    fprintf(fp, "%d %s %.2f %s\n", accNo, type, amount, timeStr);
    fclose(fp);
}

// ─── CREATE ACCOUNT ───────────────────────────────────────
void createAccount() {
    Account accounts[MAX_ACCOUNTS];
    int count = 0;
    loadAccounts(accounts, &count);

    Account newAcc;

    printf("\n--- Create New Account ---\n");
    printf("Enter Account Number: ");
    if (scanf("%d", &newAcc.accNo) != 1 || newAcc.accNo <= 0) {
        printf("Invalid account number!\n");
        while (getchar() != '\n');
        return;
    }

    // Check duplicate
    for (int i = 0; i < count; i++) {
        if (accounts[i].accNo == newAcc.accNo) {
            printf("Account number already exists!\n");
            return;
        }
    }

    printf("Enter Name (no spaces): ");
    scanf("%49s", newAcc.name);

    printf("Enter Initial Balance: ");
    if (scanf("%f", &newAcc.balance) != 1 || newAcc.balance < 0) {
        printf("Invalid balance! Must be 0 or more.\n");
        while (getchar() != '\n');
        return;
    }

    accounts[count] = newAcc;
    count++;
    saveAccounts(accounts, count);
    logTransaction(newAcc.accNo, "CREATED", newAcc.balance);

    printf("Account created successfully!\n");
    printf("AccNo: %d | Name: %s | Balance: %.2f\n",
        newAcc.accNo, newAcc.name, newAcc.balance);
}

// ─── DEPOSIT MONEY ────────────────────────────────────────
void depositMoney() {
    Account accounts[MAX_ACCOUNTS];
    int count = 0;
    loadAccounts(accounts, &count);

    if (count == 0) {
        printf("No accounts found. Please create an account first.\n");
        return;
    }

    int accNo;
    float amount;

    printf("\n--- Deposit Money ---\n");
    printf("Enter Account Number: ");
    if (scanf("%d", &accNo) != 1) {
        printf("Invalid input!\n");
        while (getchar() != '\n');
        return;
    }

    for (int i = 0; i < count; i++) {
        if (accounts[i].accNo == accNo) {
            printf("Enter Deposit Amount: ");
            if (scanf("%f", &amount) != 1 || amount <= 0) {
                printf("Invalid amount! Must be greater than 0.\n");
                while (getchar() != '\n');
                return;
            }
            accounts[i].balance += amount;
            saveAccounts(accounts, count);
            logTransaction(accNo, "DEPOSIT", amount);
            printf("Deposited %.2f successfully!\n", amount);
            printf("New Balance: %.2f\n", accounts[i].balance);
            return;
        }
    }
    printf("Account not found!\n");
}

// ─── WITHDRAW MONEY ───────────────────────────────────────
void withdrawMoney() {
    Account accounts[MAX_ACCOUNTS];
    int count = 0;
    loadAccounts(accounts, &count);

    if (count == 0) {
        printf("No accounts found. Please create an account first.\n");
        return;
    }

    int accNo;
    float amount;

    printf("\n--- Withdraw Money ---\n");
    printf("Enter Account Number: ");
    if (scanf("%d", &accNo) != 1) {
        printf("Invalid input!\n");
        while (getchar() != '\n');
        return;
    }

    for (int i = 0; i < count; i++) {
        if (accounts[i].accNo == accNo) {
            printf("Current Balance: %.2f\n", accounts[i].balance);
            printf("Enter Withdrawal Amount: ");
            if (scanf("%f", &amount) != 1 || amount <= 0) {
                printf("Invalid amount! Must be greater than 0.\n");
                while (getchar() != '\n');
                return;
            }
            if (amount > accounts[i].balance) {
                printf("Insufficient balance! Cannot withdraw %.2f\n", amount);
                return;
            }
            accounts[i].balance -= amount;
            saveAccounts(accounts, count);
            logTransaction(accNo, "WITHDRAW", amount);
            printf("Withdrawn %.2f successfully!\n", amount);
            printf("Remaining Balance: %.2f\n", accounts[i].balance);
            return;
        }
    }
    printf("Account not found!\n");
}

// ─── VIEW ALL ACCOUNTS ────────────────────────────────────
void viewAllAccounts() {
    Account accounts[MAX_ACCOUNTS];
    int count = 0;
    loadAccounts(accounts, &count);

    if (count == 0) {
        printf("No accounts found.\n");
        return;
    }

    printf("\n--- All Accounts ---\n");
    printf("%-10s %-20s %-10s\n", "AccNo", "Name", "Balance");
    printf("------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-10d %-20s %-10.2f\n",
            accounts[i].accNo,
            accounts[i].name,
            accounts[i].balance);
    }
    printf("Total Accounts: %d\n", count);
}

// ─── SEARCH ACCOUNT ───────────────────────────────────────
void searchAccount() {
    Account accounts[MAX_ACCOUNTS];
    int count = 0;
    loadAccounts(accounts, &count);

    int accNo;
    printf("\n--- Search Account ---\n");
    printf("Enter Account Number: ");
    if (scanf("%d", &accNo) != 1) {
        printf("Invalid input!\n");
        while (getchar() != '\n');
        return;
    }

    for (int i = 0; i < count; i++) {
        if (accounts[i].accNo == accNo) {
            printf("\nAccount Found!\n");
            printf("AccNo   : %d\n", accounts[i].accNo);
            printf("Name    : %s\n", accounts[i].name);
            printf("Balance : %.2f\n", accounts[i].balance);
            return;
        }
    }
    printf("Account number %d not found!\n", accNo);
}

// ─── VIEW LAST 5 TRANSACTIONS ─────────────────────────────
void viewLastTransactions() {
    int accNo;
    printf("\n--- Last 5 Transactions ---\n");
    printf("Enter Account Number: ");
    if (scanf("%d", &accNo) != 1) {
        printf("Invalid input!\n");
        while (getchar() != '\n');
        return;
    }

    FILE *fp = fopen(LOG_FILE, "r");
    if (fp == NULL) {
        printf("No transaction records found.\n");
        return;
    }

    Transaction all[1000];
    int total = 0;

    Transaction t;
    while (fscanf(fp, "%d %s %f %s",
        &t.accNo, t.type, &t.amount, t.timestamp) == 4) {
        if (t.accNo == accNo) {
            all[total++] = t;
        }
    }
    fclose(fp);

    if (total == 0) {
        printf("No transactions found for account %d.\n", accNo);
        return;
    }

    int start = (total > 5) ? total - 5 : 0;
    printf("\nLast %d transaction(s) for Account %d:\n", total - start, accNo);
    printf("%-12s %-10s %-12s\n", "Type", "Amount", "Timestamp");
    printf("------------------------------------------\n");
    for (int i = start; i < total; i++) {
        printf("%-12s %-10.2f %-20s\n",
            all[i].type,
            all[i].amount,
            all[i].timestamp);
    }
}
