#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

// structure
typedef struct {
    unsigned short id;
    char name[10];
    int model;
    int price;
    unsigned short qty;
    unsigned short qty_sold;
}car;

typedef struct {
    unsigned short model_id;
    float amount_paid;
    unsigned short qty_sold;
    unsigned short was_discounted;
    unsigned short age;
    char name[201];
}sales_record;

// function declaration


bool hasValidLicense();
bool isValidLicenseChoices(char has_license);
bool isEligibleForDiscount(short age);
bool canPurchase(unsigned short age);
bool isAdult(unsigned short age);

void addCarData();
short getCarsRemaining(unsigned short index);

// Menu
void showMainMenu();
void showCars();
void showSalesSummary();
void buyCars();
void showSales();

// File
void openFile(char fileName[201], char mode[4]);
void closeFile();
void readDataFromFile();
void getDataFromFile();
void writeDataToFile(unsigned short total_sales);
void saveDataToFile(unsigned short total_sales);


// User Inputs
unsigned short get_unsigned_short_from_console(char message[201]);
void get_string_from_console(char message[201], char* str);

// Helpers
void empty_stdin (void);
void pause_program(char user_choice);
int compare(const void *s1, const void *s2);
int order(const void *s1, const void *s2);

car temp_array[3];
car cars[3];
sales_record sales_records[100];
unsigned short total_sales = 0;

#define CSV_FILE "../data.csv"
#define FILE_OPENED 0
#define FILE_CLOSED 1
#define FILE_ERROR 2

#define DISCOUNT_PERCENTAGE 0.20f

FILE * file;
char fileStatus = FILE_CLOSED;
FILE *createFile(char fileName[201]) {
    file = fopen(fileName, "w");
    if (file != NULL) {
        fclose(file);
    }
    return file;
}

void main() {
    addCarData();
    getDataFromFile();

    char menu_choice;

    do {
        showMainMenu();
        //for detection of valid and invalid choices of user:
        printf("Please choose from above options (a, b, c or x)");
        scanf("\n%c", &menu_choice);

        switch (menu_choice) {
            case 'a':
                showCars();
                pause_program("_");
                break;
            case 'b':
                buyCars();
                pause_program("_");
                break;
            case 'c':
                showSalesSummary();
                pause_program("_");
                break;
            case 'd':
                showSales();
                pause_program("_");
                break;
            case 'x':
                printf("Thank you for using this program \n GOOD__BYE!!");
                break;

            default:
                printf(" Invalid choice please try again!\n");

        }
    } while (menu_choice != 'x');
}

void addCarData() {
    strcpy(cars[0].name, "Honda");
    cars[0].id = 1;
    cars[0].price = 10000;
    cars[0].model = 2020;
    cars[0].qty = 80;
    cars[0].qty_sold = 0;

    strcpy(cars[1].name, "Suzuki");
    cars[1].id = 2;
    cars[1].price = 3500;
    cars[1].model = 2021;
    cars[1].qty = 70;
    cars[1].qty_sold = 0;

    strcpy(cars[2].name, "Toyota");
    cars[2].id = 3;
    cars[2].price = 5000;
    cars[2].model = 2019;
    cars[2].qty = 60;
    cars[2].qty_sold = 0;
}

// https://stackoverflow.com/questions/6105513/need-help-using-qsort-with-an-array-of-structs
int compare(const void * a, const void * b)
{
    car *orderA = (car *)a;
    car *orderB = (car *)b;

    return ( (orderB->qty - orderB->qty_sold)
             > (orderA->qty - orderA->qty_sold) );
}

int order(const void * a, const void * b)
{
    car *orderA = (car *)a;
    car *orderB = (car *)b;

    return ( (orderB->qty_sold * orderB->price ) > (orderA->qty_sold * orderA->price ) );
}

void showCars() {
    // copying structs array into a new array for sorting
    memcpy(temp_array, cars, sizeof(cars));
    qsort (temp_array, 3, sizeof(car), compare);
    for (int i = 0; i < 3; i++) {
        printf("%s\tqty Available\t%d\n", temp_array[i].name, temp_array[i].qty - temp_array[i].qty_sold);
    }
}

void showSalesSummary() {
    // copying structs array into a new array for sorting
    memcpy(temp_array, cars, sizeof(cars));
    qsort (temp_array, 3, sizeof(car), order);
    for (int i =0; i < 3 ; i++){
        printf("%s\t qty available %d\t qty sold %hd \t sales figure %d GBP\n", temp_array[i].name, temp_array[i].qty - temp_array[i].qty_sold , temp_array[i].qty_sold, temp_array[i].qty_sold * temp_array[i].price);
    }
}

void showMainMenu()
{
    printf("Hello, Welcome to the UK's NO '1' Car sales company\n");
    printf("Menu:\n");
    printf("a)View Cars\n");
    printf("b)Buy Cars\n");
    printf("c)Sales Summary\n");
    printf("d)Sales Record\n");
    printf("x)Exit\n");
}

void buyCars()
{
    char choice;
    float total_bill;
    unsigned short qty_bought;
    unsigned short total_qty = cars[0].qty + cars[1].qty + cars[2].qty;
    unsigned short user_age;
    char user_name[201];

    user_age = get_unsigned_short_from_console("how old are you?(only 18+ can buy cars)");

    if (!canPurchase(user_age)) {
        printf("Sorry you cannot buy any cars because you dont have a valid license or are under age\n");
        return;
    }

    get_string_from_console("What is your name?: ", sales_records[total_sales].name);

    // Cars quantity,model information and selection:
    printf("Dear %s! There are %d total cars available of following models:\n",sales_records[total_sales].name,total_qty);

    while(1) {
        choice = get_unsigned_short_from_console("0.Honda\n1.Suzuki\n2.Toyota\nYour choice(write any number from above options):");

        if (choice >= 0 && choice <= 2) {
            break;
        }
        printf("Invalid choice please try again\n");
    }

    // dynamic variable
    printf("There are %d %s cars available each car costs %d GBP\n", getCarsRemaining(choice), cars[choice].name,
           cars[choice].price);

    do {
        qty_bought = get_unsigned_short_from_console("How many cars do you want to buy?");

        if (qty_bought > getCarsRemaining(choice)) {
            printf("Sorry we only have %d cars of this model\n", getCarsRemaining(choice));
            continue;
        }
    } while (qty_bought < 0 || qty_bought > getCarsRemaining(choice));

    total_bill = qty_bought * cars[choice].price;

    if (isEligibleForDiscount(user_age)) {
        total_bill *= (1 - DISCOUNT_PERCENTAGE);
        printf("You got a discount\n");
    } else {
        printf("You did not get a discount\n");
    }

    printf("Congratulations you have paid for %hd cars and your bill is %f GBP. You can come to our warehouse to collect your car(s)\n",qty_bought, total_bill);
    printf("Thanks for shopping with us\n");

    cars[choice].qty_sold += qty_bought;

    sales_records[total_sales].model_id = (short) choice;
    sales_records[total_sales].amount_paid = total_bill;
    sales_records[total_sales].qty_sold = qty_bought;
    sales_records[total_sales].was_discounted = isEligibleForDiscount(user_age);
    sales_records[total_sales].age = user_age;

    saveDataToFile(total_sales);
    total_sales++;
}

void showSales() {
    for (int i = 0; i < total_sales; i++){
        printf("Name: %s\t Model: %s\t qty: %hd \t amount: %f GBP \t discounted: %hd \n",
               sales_records[i].name,
               cars[sales_records[i].model_id].name,
               sales_records[i].qty_sold,
               sales_records[i].amount_paid,
               sales_records[i].was_discounted
        );
    }
}

bool isAdult(unsigned short age) {
    return age >= 18;
}

bool canPurchase(unsigned short age) {
    return isAdult(age) && hasValidLicense();
}

bool hasValidLicense() {
    char has_license;

    do {
        printf("Do you have a valid driving license?(y/Y or n/N)\n");
        printf("Note: To buy car(s) you need a driving license\n");
        scanf("\n%c", &has_license);
        has_license = tolower(has_license);

        if (!isValidLicenseChoices(has_license)) {
            printf("Invalid answer, please select y or n only!\n");
        }
    } while (!isValidLicenseChoices(has_license));

    // ternary operator, like if/else but shorter syntax
    return has_license == 'n' ? 0 : 1;
}

bool isValidLicenseChoices(char has_license)
{
    return has_license == 'y'
        || has_license == 'n';
}

bool isEligibleForDiscount(short age)
{
    return age >=18 && age <= 25;
}

void openFile(char fileName[201], char mode[4]) {
    file = fopen(fileName, mode);
    if (file == NULL) {
        if (createFile(fileName) == NULL) {
            fileStatus = FILE_ERROR;
        }
        else {
            openFile(fileName, mode);
        }
    } else{
        fileStatus = FILE_OPENED;
    }
}

void closeFile(){
    if (fileStatus == FILE_OPENED) {
        fclose(file);
        fileStatus = FILE_CLOSED;
    }
}

void readDataFromFile(){
    while (1) {
        int scanResult = fscanf(
                file,"%hd,%f,%hd,%hd,%hd,%[^\n]s",
                &sales_records[total_sales].model_id,
                &sales_records[total_sales].amount_paid,
                &sales_records[total_sales].qty_sold,
                &sales_records[total_sales].was_discounted,
                &sales_records[total_sales].age,
                &sales_records[total_sales].name
        );

        cars[sales_records[total_sales].model_id].qty_sold+= sales_records[total_sales].qty_sold;

        if (scanResult == EOF) {
            break;
        }
        total_sales++;
    }
}

void getDataFromFile() {
    openFile(CSV_FILE, "r");

    if (fileStatus == FILE_OPENED) {
        readDataFromFile();
    }
    else if (fileStatus == FILE_ERROR) {
        printf("There was an error trying to read from the file %s.", CSV_FILE);
    }

    closeFile();
}

void writeDataToFile(unsigned short total_sales) {
    // this string will be built up bit by bit before being written to the opened file
    char line[201];
    // this string will contain the converted int value
    char data[50];

    // had problems using itoa so snprintf was used
    snprintf(data, 50, "%hd", sales_records[total_sales].model_id);
    strcpy(line, data);
    strcat(line, ",");

    snprintf(data, 50, "%f", sales_records[total_sales].amount_paid);
    strcat(line, data);
    strcat(line, ",");

    snprintf(data, 50, "%hd", sales_records[total_sales].qty_sold);
    strcat(line, data);
    strcat(line, ",");

    snprintf(data, 50, "%d", sales_records[total_sales].was_discounted);
    strcat(line, data);
    strcat(line, ",");

    snprintf(data, 50, "%hd", sales_records[total_sales].age);
    strcat(line, data);
    strcat(line, ",");

    strcat(line, sales_records[total_sales].name);
    strcat(line, "\n");

    fprintf(file, line);
}

void saveDataToFile(unsigned short total_sales) {
    openFile(CSV_FILE, "a+");

    if (fileStatus == FILE_OPENED) {
        writeDataToFile(total_sales);
    }
    else if (fileStatus == FILE_ERROR) {
        printf("There was an error trying to write to the file %s.", CSV_FILE);
    }

    closeFile();
}

short getCarsRemaining(unsigned short index) {
   return cars[index].qty - cars[index].qty_sold;
}

unsigned short get_unsigned_short_from_console(char message[201]) {
    unsigned short userInput;
    int rtn;

    //  https://stackoverflow.com/a/53059527/4641522
    do {
        printf("%s (>= 0)", message);
        rtn = scanf("%hd", &userInput);
        if (rtn == 0) {
            printf("Invalid Input - please enter digits only\n");
            empty_stdin();
            continue;
        }
    } while (userInput < 0);

    return userInput;
}

// simple helper-function to empty stdin
void empty_stdin (void)
{
    int c = getchar();

    while (c != '\n' && c != EOF)
        c = getchar();
}

void get_string_from_console(char message[201], char* str) {
    int length,i;
    unsigned short has_digit = 0;

    while (1) {
        printf("%s", message);
        scanf("\n%[^\n]s", str);
        length = strlen(str);

        for (i = 0; i < length; i++) {
            if (isdigit(str[i])) {
                has_digit = 1;
            }
        }
        if (has_digit == 1) {
            printf("Only letters please\n");
            continue;
        }
        break;
    }
}

void pause_program(char user_choice) {
// give the user a chance to read the previous output, allow them to continue when ready
// customise the output depending on the user's choice
    if (user_choice == "x") {
        printf("\n\nPress Enter to Exit...");
    }
    else {
        printf("\n\nPress Enter to return to the Menu...");
    }

// two of these getchar() are needed to skip the newline character
// that's likely floating around the console window
// however, if the user_choice value is an underscore character,
// then this is only used when pausing after a file error occurred
// so only one getchar() is needed
    if (user_choice != '_') {
        getchar();
    }

    getchar();
}
