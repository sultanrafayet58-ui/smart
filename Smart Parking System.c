 #include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SLOTS 10
#define MAX_HISTORY 100
#define DATA_FILE "parking_data.dat"

struct Vehicle {
    char number[20];
    char owner[50];
    char type[15];
    time_t entryTime;
    int occupied;
};

struct History {
    char number[20];
    char owner[50];
    char type[15];
    double hours;
    double fee;
    time_t entryTime;
    time_t exitTime;
};

struct ParkingData {
    struct Vehicle slots[MAX_SLOTS];
    struct History history[MAX_HISTORY];
    int historyCount;
    int totalVehicles;
    double totalRevenue;
};

struct ParkingData parking;

/* Read a complete line of input */
void readLine(const char *message, char text[], int size) {
    int ch;
    size_t len;

    printf("%s", message);

    if (fgets(text, size, stdin) == NULL) {
        text[0] = '\0';
        return;
    }

    len = strlen(text);

    if (len > 0 && text[len - 1] == '\n') {
        text[len - 1] = '\0';
    } else {
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* Clear extra input */
        }
    }
}

/* Read an integer safely */
int readInt(const char *message) {
    char input[50];
    char *end;
    long value;

    while (1) {
        readLine(message, input, sizeof(input));

        value = strtol(input, &end, 10);

        while (*end == ' ' || *end == '\t') {
            end++;
        }

        if (input[0] != '\0' && end != input &&
            *end == '\0' && value >= 0 && value <= 1000000) {
            return (int)value;
        }

        printf("Invalid input! Enter a valid number.\n");
    }
}

/* Save all data to a file */
void saveData(void) {
    FILE *fp = fopen(DATA_FILE, "wb");

    if (fp == NULL) {
        printf("Warning: Could not save data!\n");
        return;
    }

    if (fwrite(&parking, sizeof(parking), 1, fp) != 1) {
        printf("Warning: Data could not be written completely.\n");
    }

    fclose(fp);
}

/* Load previously saved data */
void loadData(void) {
    FILE *fp = fopen(DATA_FILE, "rb");

    memset(&parking, 0, sizeof(parking));

    if (fp == NULL) {
        printf("Starting with a new parking database.\n");
        return;
    }

    if (fread(&parking, sizeof(parking), 1, fp) != 1) {
        memset(&parking, 0, sizeof(parking));
        printf("Saved data unavailable or invalid. Starting fresh.\n");
    } else {
        printf("Previous parking data loaded successfully.\n");
    }

    fclose(fp);
}

/* Find a vehicle by its number */
int findVehicle(const char number[]) {
    int i;

    for (i = 0; i < MAX_SLOTS; i++) {
        if (parking.slots[i].occupied == 1 &&
            strcmp(parking.slots[i].number, number) == 0) {
            return i;
        }
    }

    return -1;
}

/* Update a vehicle using a structure pointer */
void updateVehicle(struct Vehicle *v,
                   const char number[],
                   const char owner[],
                   const char type[]) {

    strcpy(v->number, number);
    strcpy(v->owner, owner);
    strcpy(v->type, type);

    v->entryTime = time(NULL);
    v->occupied = 1;
}

/* Calculate hourly parking charge */
double calculateFee(const char type[], double hours) {
    double rate;

    if (strcmp(type, "Car") == 0) {
        rate = 50.0;
    } else if (strcmp(type, "Bike") == 0) {
        rate = 20.0;
    } else {
        rate = 100.0;
    }

    return rate * hours;
}

/* Show the available parking slots */
void viewSlots(void) {
    int i;
    int available = 0;

    printf("\n========== PARKING STATUS ==========\n");

    for (i = 0; i < MAX_SLOTS; i++) {
        if (parking.slots[i].occupied == 1) {
            printf("Slot %d: OCCUPIED | %s | %s\n",
                   i + 1,
                   parking.slots[i].number,
                   parking.slots[i].type);
        } else {
            printf("Slot %d: EMPTY\n", i + 1);
            available++;
        }
    }

    printf("------------------------------------\n");
    printf("Total slots     : %d\n", MAX_SLOTS);
    printf("Available slots : %d\n", available);
    printf("Occupied slots  : %d\n", MAX_SLOTS - available);
}

/* Register a new vehicle */
void vehicleEntry(void) {
    int i;
    int freeSlot = -1;
    int typeChoice;
    char number[20];
    char owner[50];
    char type[15];

    for (i = 0; i < MAX_SLOTS; i++) {
        if (parking.slots[i].occupied == 0) {
            freeSlot = i;
            break;
        }
    }

    if (freeSlot == -1) {
        printf("\nParking is full!\n");
        return;
    }

    readLine("Enter vehicle number: ", number, sizeof(number));

    if (strlen(number) == 0) {
        printf("Vehicle number cannot be empty!\n");
        return;
    }

    if (findVehicle(number) != -1) {
        printf("This vehicle is already parked!\n");
        return;
    }

    readLine("Enter owner name: ", owner, sizeof(owner));

    if (strlen(owner) == 0) {
        printf("Owner name cannot be empty!\n");
        return;
    }

    printf("\n1. Car (Tk 50 per hour)\n");
    printf("2. Motorcycle (Tk 20 per hour)\n");
    printf("3. Bus (Tk 100 per hour)\n");

    typeChoice = readInt("Choose vehicle type: ");

    switch (typeChoice) {
        case 1:
            strcpy(type, "Car");
            break;

        case 2:
            strcpy(type, "Bike");
            break;

        case 3:
            strcpy(type, "Bus");
            break;

        default:
            printf("Invalid vehicle type!\n");
            return;
    }

    /* Pointer passes the address of the selected slot */
    updateVehicle(&parking.slots[freeSlot],
                  number, owner, type);

    saveData();

    printf("\nVehicle parked successfully!\n");
    printf("Assigned slot : %d\n", freeSlot + 1);
    printf("Vehicle       : %s\n", number);
    printf("Owner         : %s\n", owner);
    printf("Type          : %s\n", type);
}

/* Display date and time */
void showTime(time_t t) {
    struct tm *timeInfo = localtime(&t);

    if (timeInfo != NULL) {
        printf("%02d-%02d-%04d %02d:%02d:%02d",
               timeInfo->tm_mday,
               timeInfo->tm_mon + 1,
               timeInfo->tm_year + 1900,
               timeInfo->tm_hour,
               timeInfo->tm_min,
               timeInfo->tm_sec);
    }
}

/* Process vehicle exit and calculate the fee */
void vehicleExit(void) {
    char number[20];
    int index;
    double elapsed;
    double hours;
    double fee;
    time_t exitTime;

    readLine("Enter vehicle number: ", number, sizeof(number));

    index = findVehicle(number);

    if (index == -1) {
        printf("Vehicle not found in parking!\n");
        return;
    }

    exitTime = time(NULL);

    elapsed = difftime(exitTime,
                       parking.slots[index].entryTime);

    /* Round up to the next hour */
    hours = (elapsed <= 0) ? 1 : (double)((long long)
             ((elapsed + 3599.0) / 3600.0));

    if (hours < 1) {
        hours = 1;
    }

    fee = calculateFee(parking.slots[index].type, hours);

    printf("\n========== PARKING RECEIPT ==========\n");
    printf("Vehicle number : %s\n",
           parking.slots[index].number);
    printf("Owner name     : %s\n",
           parking.slots[index].owner);
    printf("Vehicle type   : %s\n",
           parking.slots[index].type);

    printf("Entry time     : ");
    showTime(parking.slots[index].entryTime);
    printf("\n");

    printf("Exit time      : ");
    showTime(exitTime);
    printf("\n");

    printf("Parking hours  : %.0f\n", hours);
    printf("Total fee      : Tk %.2f\n", fee);
    printf("=====================================\n");

    /* Store the completed parking transaction */
    if (parking.historyCount < MAX_HISTORY) {
        struct History *h =
            &parking.history[parking.historyCount];

        strcpy(h->number, parking.slots[index].number);
        strcpy(h->owner, parking.slots[index].owner);
        strcpy(h->type, parking.slots[index].type);

        h->hours = hours;
        h->fee = fee;
        h->entryTime = parking.slots[index].entryTime;
        h->exitTime = exitTime;

        parking.historyCount++;
    } else {
        printf("Warning: History storage is full.\n");
    }

    parking.totalRevenue += fee;
    parking.totalVehicles++;

    parking.slots[index].occupied = 0;

    saveData();

    printf("Vehicle exit completed successfully!\n");
}

/* Search for a currently parked vehicle */
void searchVehicle(void) {
    char number[20];
    int index;

    readLine("Enter vehicle number to search: ",
             number, sizeof(number));

    index = findVehicle(number);

    if (index != -1) {
        printf("\nVehicle found!\n");
        printf("Slot number : %d\n", index + 1);
        printf("Number      : %s\n", parking.slots[index].number);
        printf("Owner       : %s\n", parking.slots[index].owner);
        printf("Type        : %s\n", parking.slots[index].type);

        printf("Entry time  : ");
        showTime(parking.slots[index].entryTime);
        printf("\n");
    } else {
        printf("Vehicle not found in current parking!\n");
    }
}

/* Update the owner name */
void updateOwner(void) {
    char number[20];
    char newOwner[50];
    int index;

    readLine("Enter vehicle number: ", number, sizeof(number));

    index = findVehicle(number);

    if (index == -1) {
        printf("Vehicle not found!\n");
        return;
    }

    readLine("Enter new owner name: ",
             newOwner, sizeof(newOwner));

    if (strlen(newOwner) == 0) {
        printf("Owner name cannot be empty!\n");
        return;
    }

    /* Pointer updates the original structure */
    struct Vehicle *v = &parking.slots[index];

    strcpy(v->owner, newOwner);

    saveData();

    printf("Owner information updated successfully!\n");
}

/* Display completed parking transactions */
void parkingHistory(void) {
    int i;

    if (parking.historyCount == 0) {
        printf("\nNo parking history available.\n");
        return;
    }

    printf("\n============= PARKING HISTORY =============\n");

    for (i = 0; i < parking.historyCount; i++) {
        printf("\nTransaction %d\n", i + 1);
        printf("Vehicle : %s\n", parking.history[i].number);
        printf("Owner   : %s\n", parking.history[i].owner);
        printf("Type    : %s\n", parking.history[i].type);
        printf("Hours   : %.0f\n", parking.history[i].hours);
        printf("Fee     : Tk %.2f\n", parking.history[i].fee);

        printf("Entry   : ");
        showTime(parking.history[i].entryTime);
        printf("\n");

        printf("Exit    : ");
        showTime(parking.history[i].exitTime);
        printf("\n");
    }

    printf("============================================\n");
}

/* Generate a parking report */
void showReport(void) {
    int i;
    int occupied = 0;

    for (i = 0; i < MAX_SLOTS; i++) {
        if (parking.slots[i].occupied == 1) {
            occupied++;
        }
    }

    printf("\n========== PARKING REPORT ==========\n");
    printf("Total parking slots : %d\n", MAX_SLOTS);
    printf("Occupied slots      : %d\n", occupied);
    printf("Available slots     : %d\n", MAX_SLOTS - occupied);
    printf("Vehicles exited     : %d\n", parking.totalVehicles);
    printf("Revenue collected   : Tk %.2f\n",
           parking.totalRevenue);
    printf("Saved transactions  : %d\n",
           parking.historyCount);
    printf("====================================\n");
}

/* Main function */
int main(void) {
    int choice;

    loadData();

    while (1) {
        printf("\n========================================\n");
        printf("     SMART PARKING MANAGEMENT SYSTEM\n");
        printf("========================================\n");
        printf("1. Vehicle Entry\n");
        printf("2. Vehicle Exit\n");
        printf("3. View Parking Slots\n");
        printf("4. Search Vehicle\n");
        printf("5. Update Owner Information\n");
        printf("6. Parking History\n");
        printf("7. Parking Report\n");
        printf("8. Exit Program\n");
        printf("========================================\n");

        choice = readInt("Enter your choice: ");

        switch (choice) {
            case 1:
                vehicleEntry();
                break;

            case 2:
                vehicleExit();
                break;

            case 3:
                viewSlots();
                break;

            case 4:
                searchVehicle();
                break;

            case 5:
                updateOwner();
                break;

            case 6:
                parkingHistory();
                break;

            case 7:
                showReport();
                break;

            case 8:
                saveData();
                printf("Data saved. Thank you!\n");
                return 0;

            default:
                printf("Invalid choice! Select 1 to 8.\n");
                break;
        }
    }
}
