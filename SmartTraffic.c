#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIGNAL_FILE "signals.dat"
#define EMERGENCY_FILE "emergency.dat"
#define ACCIDENT_FILE "accidents.dat"
#define PARKING_FILE "parking.dat"
#define VIOLATION_FILE "violations.dat"

#define MAX_PARKING 20

/* ========== STRUCTURES ========== */

typedef struct {
    int id;
    char road[50];
    int vehicles;
    int density;
    int greenTime;
    char signal[15];
} Traffic;

typedef struct {
    int id;
    char vehicleType[30];
    char location[80];
    char details[150];
    char status[20];
} Emergency;

typedef struct {
    int id;
    char location[80];
    char vehicle[30];
    char description[150];
    char severity[20];
} Accident;

typedef struct {
    int id;
    char vehicleNumber[30];
    char vehicleType[20];
    int active;
} Parking;

typedef struct {
    int id;
    char vehicleNumber[30];
    char violationType[50];
    float fine;
} Violation;

/* ========== COMMON FUNCTIONS ========== */

void readLine(char str[], int size) {
    if (fgets(str, size, stdin) != NULL) {
        str[strcspn(str, "\n")] = '\0';
    }
}

int readInt() {
    char str[50];
    readLine(str, sizeof(str));
    return atoi(str);
}

float readFloat() {
    char str[50];
    readLine(str, sizeof(str));
    return (float)atof(str);
}

void line() {
    printf("\n========================================\n");
}

void pauseScreen() {
    printf("\nPress Enter to continue...");
    getchar();
}

/* ========== ID GENERATORS ========== */

int getNextID(const char *filename, int typeSize, int start) {
    FILE *fp = fopen(filename, "rb");
    int id, max = start - 1;

    if (fp == NULL)
        return start;

    while (fread(&id, sizeof(int), 1, fp) == 1) {
        if (id > max)
            max = id;

        fseek(fp, typeSize - sizeof(int), SEEK_CUR);
    }

    fclose(fp);
    return max + 1;
}

/* ========== TRAFFIC MANAGEMENT ========== */

void manageTraffic() {
    FILE *fp = fopen(SIGNAL_FILE, "ab");
    Traffic t;
    int choice;

    if (fp == NULL) {
        printf("File Error!\n");
        return;
    }

    line();
    printf("       SMART TRAFFIC MANAGEMENT\n");
    line();

    printf("1. North Road\n");
    printf("2. South Road\n");
    printf("3. East Road\n");
    printf("4. West Road\n");

    printf("\nSelect Road: ");
    choice = readInt();

    switch (choice) {
        case 1: strcpy(t.road, "North Road"); break;
        case 2: strcpy(t.road, "South Road"); break;
        case 3: strcpy(t.road, "East Road"); break;
        case 4: strcpy(t.road, "West Road"); break;
        default:
            printf("Invalid Road!\n");
            fclose(fp);
            return;
    }

    printf("Enter Number of Vehicles: ");
    t.vehicles = readInt();

    if (t.vehicles < 0) {
        printf("Invalid vehicle count!\n");
        fclose(fp);
        return;
    }

    t.id = getNextID(SIGNAL_FILE, sizeof(Traffic), 1000);

    /* Rule-based intelligent traffic calculation */

    if (t.vehicles <= 10) {
        t.density = 1;
        t.greenTime = 20;
        strcpy(t.signal, "LOW");
    }
    else if (t.vehicles <= 30) {
        t.density = 2;
        t.greenTime = 40;
        strcpy(t.signal, "MEDIUM");
    }
    else {
        t.density = 3;
        t.greenTime = 60;
        strcpy(t.signal, "HIGH");
    }

    fwrite(&t, sizeof(Traffic), 1, fp);
    fclose(fp);

    line();
    printf("       SMART SIGNAL RESULT\n");
    line();

    printf("Road: %s\n", t.road);
    printf("Vehicle Count: %d\n", t.vehicles);
    printf("Traffic Density: %s\n", t.signal);
    printf("Green Signal Time: %d Seconds\n", t.greenTime);

    printf("\nSignal Timing Calculated Successfully!\n");
}

void viewTrafficRecords() {
    FILE *fp = fopen(SIGNAL_FILE, "rb");
    Traffic t;
    int found = 0;

    line();
    printf("       TRAFFIC RECORDS\n");
    line();

    if (fp == NULL) {
        printf("No Traffic Records Found!\n");
        return;
    }

    while (fread(&t, sizeof(Traffic), 1, fp) == 1) {
        printf("\nID: %d\n", t.id);
        printf("Road: %s\n", t.road);
        printf("Vehicles: %d\n", t.vehicles);
        printf("Density: %s\n", t.signal);
        printf("Green Time: %d Seconds\n", t.greenTime);
        found = 1;
    }

    if (!found)
        printf("No records available.\n");

    fclose(fp);
}

/* ========== EMERGENCY MANAGEMENT ========== */

void reportEmergency() {
    FILE *fp = fopen(EMERGENCY_FILE, "ab");
    Emergency e;

    if (fp == NULL) {
        printf("File Error!\n");
        return;
    }

    line();
    printf("       EMERGENCY VEHICLE REPORT\n");
    line();

    e.id = getNextID(EMERGENCY_FILE, sizeof(Emergency), 2000);

    printf("Emergency ID: %d\n", e.id);

    printf("Vehicle Type (Ambulance/Fire/Police): ");
    readLine(e.vehicleType, sizeof(e.vehicleType));

    printf("Current Location: ");
    readLine(e.location, sizeof(e.location));

    printf("Emergency Details: ");
    readLine(e.details, sizeof(e.details));

    strcpy(e.status, "Pending");

    fwrite(&e, sizeof(Emergency), 1, fp);
    fclose(fp);

    printf("\nEmergency Report Saved!\n");
    printf("Priority: HIGH\n");
    printf("Emergency ID: %d\n", e.id);
}

void viewEmergencies() {
    FILE *fp = fopen(EMERGENCY_FILE, "rb");
    Emergency e;
    int found = 0;

    line();
    printf("       EMERGENCY REPORTS\n");
    line();

    if (fp == NULL) {
        printf("No Emergency Records!\n");
        return;
    }

    while (fread(&e, sizeof(Emergency), 1, fp) == 1) {
        printf("\nID: %d\n", e.id);
        printf("Vehicle: %s\n", e.vehicleType);
        printf("Location: %s\n", e.location);
        printf("Details: %s\n", e.details);
        printf("Status: %s\n", e.status);
        found = 1;
    }

    if (!found)
        printf("No reports available.\n");

    fclose(fp);
}

void updateEmergency() {
    FILE *fp = fopen(EMERGENCY_FILE, "rb");
    FILE *temp = fopen("temp.dat", "wb");
    Emergency e;
    int id, found = 0, choice;

    if (fp == NULL || temp == NULL) {
        printf("File Error!\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    printf("Enter Emergency ID: ");
    id = readInt();

    while (fread(&e, sizeof(Emergency), 1, fp) == 1) {
        if (e.id == id) {
            printf("1. Pending\n");
            printf("2. Responding\n");
            printf("3. Resolved\n");
            printf("Select Status: ");

            choice = readInt();

            if (choice == 1)
                strcpy(e.status, "Pending");
            else if (choice == 2)
                strcpy(e.status, "Responding");
            else if (choice == 3)
                strcpy(e.status, "Resolved");
            else
                printf("Invalid choice. Status unchanged.\n");

            found = 1;
        }

        fwrite(&e, sizeof(Emergency), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    remove(EMERGENCY_FILE);
    rename("temp.dat", EMERGENCY_FILE);

    if (found)
        printf("Emergency Status Updated!\n");
    else
        printf("Emergency ID Not Found!\n");
}

/* ========== ACCIDENT MANAGEMENT ========== */

void reportAccident() {
    FILE *fp = fopen(ACCIDENT_FILE, "ab");
    Accident a;
    int choice;

    if (fp == NULL) {
        printf("File Error!\n");
        return;
    }

    line();
    printf("       ACCIDENT REPORTING SYSTEM\n");
    line();

    a.id = getNextID(ACCIDENT_FILE, sizeof(Accident), 3000);

    printf("Accident ID: %d\n", a.id);

    printf("Location: ");
    readLine(a.location, sizeof(a.location));

    printf("Vehicle Type: ");
    readLine(a.vehicle, sizeof(a.vehicle));

    printf("Accident Description: ");
    readLine(a.description, sizeof(a.description));

    printf("\n1. Minor\n2. Moderate\n3. Critical\n");
    printf("Select Severity: ");
    choice = readInt();

    if (choice == 1)
        strcpy(a.severity, "Minor");
    else if (choice == 2)
        strcpy(a.severity, "Moderate");
    else if (choice == 3)
        strcpy(a.severity, "Critical");
    else {
        printf("Invalid severity!\n");
        fclose(fp);
        return;
    }

    fwrite(&a, sizeof(Accident), 1, fp);
    fclose(fp);

    printf("\nAccident Report Saved Successfully!\n");
}

void viewAccidents() {
    FILE *fp = fopen(ACCIDENT_FILE, "rb");
    Accident a;
    int found = 0;

    line();
    printf("       ACCIDENT RECORDS\n");
    line();

    if (fp == NULL) {
        printf("No Accident Records!\n");
        return;
    }

    while (fread(&a, sizeof(Accident), 1, fp) == 1) {
        printf("\nID: %d\n", a.id);
        printf("Location: %s\n", a.location);
        printf("Vehicle: %s\n", a.vehicle);
        printf("Description: %s\n", a.description);
        printf("Severity: %s\n", a.severity);
        found = 1;
    }

    if (!found)
        printf("No records available.\n");

    fclose(fp);
}

/* ========== PARKING MANAGEMENT ========== */

void parkingEntry() {
    FILE *fp;
    Parking p;
    int occupied = 0;

    fp = fopen(PARKING_FILE, "rb");

    if (fp != NULL) {
        while (fread(&p, sizeof(Parking), 1, fp) == 1) {
            if (p.active == 1)
                occupied++;
        }
        fclose(fp);
    }

    if (occupied >= MAX_PARKING) {
        printf("Parking is Full!\n");
        return;
    }

    printf("\nAvailable Slots: %d\n", MAX_PARKING - occupied);

    printf("Enter Vehicle Number: ");
    readLine(p.vehicleNumber, sizeof(p.vehicleNumber));

    fp = fopen(PARKING_FILE, "ab");

    if (fp == NULL) {
        printf("File Error!\n");
        return;
    }

    p.id = getNextID(PARKING_FILE, sizeof(Parking), 4000);

    printf("Enter Vehicle Type: ");
    readLine(p.vehicleType, sizeof(p.vehicleType));

    p.active = 1;

    fwrite(&p, sizeof(Parking), 1, fp);
    fclose(fp);

    printf("\nParking Entry Successful!\n");
    printf("Parking ID: %d\n", p.id);
}

void parkingExit() {
    FILE *fp = fopen(PARKING_FILE, "rb");
    FILE *temp = fopen("temp.dat", "wb");
    Parking p;
    char number[30];
    int found = 0;

    if (fp == NULL || temp == NULL) {
        printf("File Error!\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    printf("Enter Vehicle Number: ");
    readLine(number, sizeof(number));

    while (fread(&p, sizeof(Parking), 1, fp) == 1) {
        if (strcmp(p.vehicleNumber, number) == 0 &&
            p.active == 1) {
            p.active = 0;
            found = 1;
        }

        fwrite(&p, sizeof(Parking), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    remove(PARKING_FILE);
    rename("temp.dat", PARKING_FILE);

    if (found)
        printf("Vehicle Exit Successful!\n");
    else
        printf("Active Vehicle Not Found!\n");
}

void viewParking() {
    FILE *fp = fopen(PARKING_FILE, "rb");
    Parking p;
    int occupied = 0;

    line();
    printf("       PARKING MANAGEMENT\n");
    line();

    if (fp != NULL) {
        while (fread(&p, sizeof(Parking), 1, fp) == 1) {
            if (p.active == 1) {
                printf("\nParking ID: %d\n", p.id);
                printf("Vehicle: %s\n", p.vehicleNumber);
                printf("Type: %s\n", p.vehicleType);
                occupied++;
            }
        }
        fclose(fp);
    }

    printf("\nTotal Slots: %d\n", MAX_PARKING);
    printf("Occupied Slots: %d\n", occupied);
    printf("Available Slots: %d\n", MAX_PARKING - occupied);
}

/* ========== TRAFFIC VIOLATION ========== */

void reportViolation() {
    FILE *fp = fopen(VIOLATION_FILE, "ab");
    Violation v;
    int choice;

    if (fp == NULL) {
        printf("File Error!\n");
        return;
    }

    line();
    printf("       TRAFFIC VIOLATION SYSTEM\n");
    line();

    v.id = getNextID(VIOLATION_FILE, sizeof(Violation), 5000);

    printf("Violation ID: %d\n", v.id);

    printf("Enter Vehicle Number: ");
    readLine(v.vehicleNumber, sizeof(v.vehicleNumber));

    printf("\n1. Signal Violation - 1000 Taka\n");
    printf("2. Overspeeding - 1500 Taka\n");
    printf("3. Wrong Parking - 500 Taka\n");
    printf("4. No Helmet - 800 Taka\n");

    printf("Select Violation: ");
    choice = readInt();

    switch (choice) {
        case 1:
            strcpy(v.violationType, "Signal Violation");
            v.fine = 1000;
            break;

        case 2:
            strcpy(v.violationType, "Overspeeding");
            v.fine = 1500;
            break;

        case 3:
            strcpy(v.violationType, "Wrong Parking");
            v.fine = 500;
            break;

        case 4:
            strcpy(v.violationType, "No Helmet");
            v.fine = 800;
            break;

        default:
            printf("Invalid Violation Type!\n");
            fclose(fp);
            return;
    }

    fwrite(&v, sizeof(Violation), 1, fp);
    fclose(fp);

    printf("\nViolation Recorded Successfully!\n");
    printf("Fine Amount: %.2f Taka\n", v.fine);
}

void viewViolations() {
    FILE *fp = fopen(VIOLATION_FILE, "rb");
    Violation v;
    int count = 0;
    float total = 0;

    line();
    printf("       TRAFFIC VIOLATION RECORDS\n");
    line();

    if (fp == NULL) {
        printf("No Violation Records!\n");
        return;
    }

    while (fread(&v, sizeof(Violation), 1, fp) == 1) {
        printf("\nID: %d\n", v.id);
        printf("Vehicle: %s\n", v.vehicleNumber);
        printf("Violation: %s\n", v.violationType);
        printf("Fine: %.2f Taka\n", v.fine);

        total += v.fine;
        count++;
    }

    printf("\nTotal Violations: %d\n", count);
    printf("Total Fine: %.2f Taka\n", total);

    fclose(fp);
}

/* ========== ADMIN DASHBOARD ========== */

void adminDashboard() {
    int choice;

    do {
        line();
        printf("       SMART TRAFFIC ADMIN PANEL\n");
        line();

        printf("1. View Traffic Records\n");
        printf("2. View Emergency Reports\n");
        printf("3. Update Emergency Status\n");
        printf("4. View Accident Reports\n");
        printf("5. View Parking Records\n");
        printf("6. View Traffic Violations\n");
        printf("0. Logout\n");

        printf("\nEnter Choice: ");
        choice = readInt();

        switch (choice) {
            case 1: viewTrafficRecords(); break;
            case 2: viewEmergencies(); break;
            case 3: updateEmergency(); break;
            case 4: viewAccidents(); break;
            case 5: viewParking(); break;
            case 6: viewViolations(); break;
            case 0: printf("Admin Logged Out!\n"); break;
            default: printf("Invalid Choice!\n");
        }

        if (choice != 0)
            pauseScreen();

    } while (choice != 0);
}

void adminLogin() {
    char username[30], password[30];

    line();
    printf("           ADMIN LOGIN\n");
    line();

    printf("Username: ");
    readLine(username, sizeof(username));

    printf("Password: ");
    readLine(password, sizeof(password));

    if (strcmp(username, "admin") == 0 &&
        strcmp(password, "admin123") == 0) {

        printf("\nLogin Successful!\n");
        adminDashboard();
    }
    else {
        printf("\nInvalid Username or Password!\n");
    }
}

/* ========== MAIN MENU ========== */

int main() {
    int choice;

    do {
        line();
        printf("    AI-BASED SMART TRAFFIC & ROAD\n");
        printf("          MANAGEMENT SYSTEM\n");
        line();

        printf("1. Smart Traffic Signal Management\n");
        printf("2. Emergency Vehicle Report\n");
        printf("3. Accident Reporting\n");
        printf("4. Parking Entry\n");
        printf("5. Parking Exit\n");
        printf("6. View Parking Availability\n");
        printf("7. Report Traffic Violation\n");
        printf("8. Admin Login\n");
        printf("0. Exit\n");

        printf("\nEnter Your Choice: ");
        choice = readInt();

        switch (choice) {
            case 1:
                manageTraffic();
                pauseScreen();
                break;

            case 2:
                reportEmergency();
                pauseScreen();
                break;

            case 3:
                reportAccident();
                pauseScreen();
                break;

            case 4:
                parkingEntry();
                pauseScreen();
                break;

            case 5:
                parkingExit();
                pauseScreen();
                break;

            case 6:
                viewParking();
                pauseScreen();
                break;

            case 7:
                reportViolation();
                pauseScreen();
                break;

            case 8:
                adminLogin();
                pauseScreen();
                break;

            case 0:
                printf("\nThank You for Using Smart Traffic System!\n");
                break;

            default:
                printf("\nInvalid Choice! Try Again.\n");
                pauseScreen();
        }

    } while (choice != 0);

    return 0;
}
