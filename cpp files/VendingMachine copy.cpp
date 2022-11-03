// This program simulates the payment section of a vending machine.
// The vending machine only dispenses sodas which are priced at $1.30

#include <iostream>
using namespace std;

// Create an enumeration data type called Money
enum class Money { nickel = 5, dime = 10, quarter = 25, dollar = 100 };

const int MAXSIZE = 26;
const int PRICE = 130;

//Displays initial menu
void menu();

// Prints price to the screen in the format: $X.XX
// price represents an amount of money, in pennies
void PrintPrice(int price);

// Takes user input (1, 2, 3, 4) and converts it to a Money type
Money ConvertToMoney(int x);

// Prints a word to the screen stating what the Money type is
void PrintMoney(Money x);

// Prints the coins to dispense, which add up to amount
// Note: This will never need to dispense a dollar
void DispenseChange(int amount);

int main()
{
    //Main Program Variables
    char option;
    Money deposit[MAXSIZE];
    int u = 0;
    int amount = 0;

    menu();

    while (amount < PRICE)
    {
        cout << "Enter: >";
        cin >> option;

        switch (option)
        {
        case '1':
            deposit[u] = ConvertToMoney(1);
            u++;
            amount += (int)Money::nickel;
            break;

        case '2':
            deposit[u] = ConvertToMoney(2);
            u++;
            amount += (int)Money::dime;
            break;

        case '3':
            deposit[u] = ConvertToMoney(3);
            u++;
            amount += (int)Money::quarter;
            break;

        case '4':
            deposit[u] = ConvertToMoney(4);
            u++;
            amount += (int)Money::dollar;
            break;

        case '5':
            cout << "Transaction canceled.\n";
            if (u > 0)
            {
                cout << "Returning: \n";
                for (int r = 0; r < u; r++)
                {
                    PrintMoney(deposit[r]);
                    cout << "\n";
                }
            }

            return 0;

        default:
            cout << "Invalid option\n";
        }

        if (PRICE > amount)
        {
            cout << "Amount left to pay: ";
            PrintPrice(PRICE - amount);
            cout << endl;
        }
    }

    cout << "Dispensing soda.\n";

    if (amount > PRICE)
    {
        cout << "Change due: ";
        PrintPrice(amount - PRICE);
        cout << endl;
        DispenseChange(amount);
    }

    cout << endl;
    return 0;
}



void PrintPrice(int price)
{
    // Moves decimal two places to the left
    printf("$%d.%02d", (price / 100), (price % 100));
}

void DispenseChange(int amount)
{
    //variables for amount of each coin needed
    // quarter
    int q;
    //dime
    int d;
    //nickel
    int n;

    //variable to keep track of how much change is needed
    amount = (amount - PRICE);

    while (amount > 0)
    {
        q = amount / (int)Money::quarter;
        amount %= (int)Money::quarter;

        d = amount / (int)Money::dime;
        amount %= (int)Money::dime;

        n = amount / (int)Money::nickel;
        amount %= (int)Money::nickel;
    }

    //array to hold change
    Money change[MAXSIZE];

    //variable to keep track of array
    int used = 0;

    //put items into array for each coin needed
    if (q > 0)
    {
        for (q; q > 0; q--)
        {
            change[used] = Money::quarter;
            used++;
        }
    }
    if (d > 0)
    {
        for (d; d > 0; d--)
        {
            change[used] = Money::dime;
            used++;
        }
    }

    if (n > 0)
    {
        for (n; n > 0; n--)
        {
            change[used] = Money::nickel;
            used++;
        }
    }

    for (int i = 0; i < used; i++)
    {
        PrintMoney(change[i]);
        cout << " ";
    }
}

Money ConvertToMoney(int x)
{
    switch (x)
    {
    case 1:
        return Money::nickel;
    case 2:
        return Money::dime;
    case 3:
        return Money::quarter;
    case 4:
        return Money::dollar;
    }
}

void PrintMoney(Money x)
{
    if (x == Money::nickel)
    {
        cout << "nickel";
    }
    if (x == Money::dime)
    {
        cout << "dime";
    }
    if (x == Money::quarter)
    {
        cout << "quarter";
    }
    if (x == Money::dollar)
    {
        cout << "dollar";
    }
}

void menu()
{
    cout << "***Vending Machine***\n";
    cout << "Deposit ";
    PrintPrice(PRICE);
    cout << " for a soda.\n";
    cout << "1.Deposit nickel 2.Deposit dime 3.Deposit quarter 4.Deposit dollar 5.Cancel Transaction\n";
}