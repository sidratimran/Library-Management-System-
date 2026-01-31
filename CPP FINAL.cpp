#include <iostream>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <ctime>
#include <cstdlib>
#include <sstream> // C++98 compatibility for logging

using namespace std;

// --- Helper: Transaction Logger ---
void logTransaction(const string& message) {
    ofstream logFile("log.txt", ios::app);
    time_t now = time(0);
    char* dt = ctime(&now);
    dt[strlen(dt)-1] = '\0'; 
    logFile << "[" << dt << "] " << message << endl;
    logFile.close();
}

// --- 1. Abstract Base Class (Abstraction) ---
class LibraryAsset {
public:
    virtual void displayDetails() = 0; 
};

// --- 2. Main Book Class (Base for Inheritance and Binary Data) ---
class Book : public LibraryAsset {
protected:
    int accessionNumber;
    char title[60]; // Fixed size for binary safety
    int yearPublished;

public:
    Book() { 
        accessionNumber = 0; 
        strcpy(title, "N/A"); 
        yearPublished = 0;
    }
    
    Book(int num, const char* t, int year) {
        accessionNumber = num;
        strncpy(title, t, 59);
        title[59] = '\0';
        yearPublished = year;
    }

    // Polymorphic display implementation
    void displayDetails() {
        cout << " [Standard Loan] Acc #:" << accessionNumber 
             << " | Title: " << title 
             << " | Year: " << yearPublished << endl;
    }

    // Binary File Operations
    void writeToFile(ofstream &out) {
        out.write((char*)this, sizeof(*this));
    }

    void readFromFile(ifstream &in) {
        in.read((char*)this, sizeof(*this));
    }
};

// --- 3. Multi-Level Inheritance ---
class ReferenceBook : public Book {
protected:
    char section[20];
public:
    ReferenceBook(int num, const char* t, int year, const char* sec) 
        : Book(num, t, year) {
        strncpy(section, sec, 19);
        section[19] = '\0';
    }
    
    // Overriding display for Polymorphism
    void displayDetails() override {
        cout << " [REFERENCE ONLY] Acc #:" << accessionNumber 
             << " | Title: " << title 
             << " | Section: " << section
             << " | Year: " << yearPublished 
             << " *NO LOAN*" << endl;
    }
};


void addBook() {
    int accNum, year;
    char title[60];

    cout << "\n--- Add New Book ---\n";
    cout << "Enter Accession Number: "; cin >> accNum;
    cout << "Enter Title (No spaces): "; cin >> title;
    cout << "Enter Publication Year: "; cin >> year;
    
    Book newBook(accNum, title, year);

    // Exception Handling for File I/O
    try {
        ofstream outFile("library.dat", ios::binary | ios::app);
        
        if (!outFile) {
            throw runtime_error("Critical Error: File access denied or file missing!");
        }
        
        newBook.writeToFile(outFile);
        outFile.close();
        
        // C++98 COMPATIBLE LOGGING
        stringstream ss;
        ss << "Added Book Acc #" << accNum << " (" << title << ")";
        logTransaction(ss.str());
        
        cout << ">> Book saved successfully.\n";
    }
    catch (exception &e) {
        cout << "\n!!! EXCEPTION CAUGHT: " << e.what() << " !!!\n";
        logTransaction("Failed to add book due to file error.");
    }
}

void randomAccessSearch() {
    ifstream inFile("library.dat", ios::binary);
    Book temp;
    int recordNum;
    
    cout << "\n--- Random Access Search ---\n";
    cout << "Enter Shelf Slot Number (e.g., 1 for 1st book): ";
    cin >> recordNum;
    
    if (recordNum < 1) {
        cout << "Invalid slot number.\n"; 
        return;
    }

    // LOGIC: Random Access calculation
    int position = (recordNum - 1) * sizeof(Book);
    
    // SEEKG: Jump to position
    inFile.seekg(position, ios::beg);
    temp.readFromFile(inFile);
    
    if (inFile) {
        cout << "\nRecord Found at Slot " << recordNum << ":\n";
        // Polymorphism usage: Treating Book as a LibraryAsset
        LibraryAsset* asset = &temp;
        asset->displayDetails();
    } else {
        cout << "Error: Shelf slot number is out of range.\n";
    }
    inFile.close();
}

int main() {
    int choice;
    do {
        cout << "\n====================================\n";
        cout << "   LIBRARY MANAGEMENT SYSTEM   \n";
        cout << "====================================\n";
        cout << "1. Add New Book\n";
        cout << "2. Random Search (by Slot #)\n";
        cout << "3. Exit\n";
        cout << "Select Option: ";
        cin >> choice;

        switch(choice) {
            case 1: addBook(); break;
            case 2: randomAccessSearch(); break;
            case 3: cout << "System Closing...\n"; break;
            default: cout << "Invalid Option.\n";
        }
    } while (choice != 3);

    return 0;
}


