#include <stdio.h>
#include <string.h>
#include <time.h>

struct Account
{
    int accNo;
    char name[50];
    float balance;
};

void logTransaction(int accNo, char type[], float amount)
{
    FILE *fp = fopen("transactions.txt","a");

    time_t t = time(NULL);

    fprintf(fp,"%d %s %.2f %s\n",accNo,type,amount,ctime(&t));

    fclose(fp);
}

int createAccount(int accNo, char name[], float balance)
{
    struct Account a;

    a.accNo = accNo;
    strcpy(a.name,name);
    a.balance = balance;

    FILE *fp = fopen("accounts.dat","ab");

    fwrite(&a,sizeof(a),1,fp);

    fclose(fp);

    return 1;
}

float deposit(int accNo, float amount)
{
    struct Account a;

    FILE *fp = fopen("accounts.dat","rb+");

    while(fread(&a,sizeof(a),1,fp))
    {
        if(a.accNo==accNo)
        {
            a.balance += amount;

            fseek(fp,-sizeof(a),SEEK_CUR);
            fwrite(&a,sizeof(a),1,fp);

            fclose(fp);

            logTransaction(accNo,"Deposit",amount);

            return a.balance;
        }
    }

    fclose(fp);

    return -1;
}

float withdraw(int accNo, float amount)
{
    struct Account a;

    FILE *fp = fopen("accounts.dat","rb+");

    while(fread(&a,sizeof(a),1,fp))
    {
        if(a.accNo==accNo)
        {
            if(a.balance < amount)
            {
                fclose(fp);
                return -2;
            }

            a.balance -= amount;

            fseek(fp,-sizeof(a),SEEK_CUR);
            fwrite(&a,sizeof(a),1,fp);

            fclose(fp);

            logTransaction(accNo,"Withdraw",amount);

            return a.balance;
        }
    }

    fclose(fp);

    return -1;
}

float searchAccount(int accNo)
{
    struct Account a;

    FILE *fp = fopen("accounts.dat","rb");

    while(fread(&a,sizeof(a),1,fp))
    {
        if(a.accNo==accNo)
        {
            fclose(fp);
            return a.balance;
        }
    }

    fclose(fp);

    return -1;
}

void accountSummary()
{
    struct Account a;

    FILE *fp = fopen("accounts.dat","rb");

    printf("\nAccount Summary\n");

    while(fread(&a,sizeof(a),1,fp))
    {
        printf("AccNo: %d Name: %s Balance: %.2f\n",
        a.accNo,a.name,a.balance);
    }

    fclose(fp);
}

void lastTransactions(int accNo)
{
    FILE *fp = fopen("transactions.txt","r");

    char line[200];
    int count=0;

    while(fgets(line,sizeof(line),fp))
    {
        int id;
        sscanf(line,"%d",&id);

        if(id==accNo)
        {
            printf("%s",line);
            count++;

            if(count==5)
            break;
        }
    }

    fclose(fp);
}