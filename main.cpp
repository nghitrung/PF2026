/*
* Ho Chi Minh City University of Technology
* Faculty of Computer Science and Engineering
* Initial code for Assignment 1
* Programming Fundamentals Spring 2026
* Date: 27.01.2026
*/

//The library here is concretely set, students are not allowed to include any other libraries.

#include "water_seven.h"

using namespace std;

char character[FIXED_CHARACTER][MAX_NAME]; 
int hp[FIXED_CHARACTER];
int skill[FIXED_CHARACTER];
int shipHP = 0;
int repairCost = 0;

int main(int argc, const char * argv[]) {
    
    testReadInput();

    testDamageEvaluation();

    testConflictSimulation();

    testResolveDuel();

    testdecodeCP9Message();

    return 0;
}

bool readFile() {
    //string filename = "opw_tc_01_input";
    string filename = "opw_tc_02_input";
    
    bool readable = readInput(filename, character, hp, skill, shipHP, repairCost);

    if (!readable) return false;

    return true;
}

void testReadInput() {
    cout << "====================================================" << endl;
    cout << "   READ INPUT (TASK 0)          " << endl;

    if (readFile()) {
        cout << left << setw(15) << "Name" << setw(10) << "HP" << setw(10) << "Skill" << endl;

        for (int i = 0; i < FIXED_CHARACTER; i++) {
            cout << left << setw(15) << character[i] << setw(10) << hp[i] << setw(10) << skill[i] << endl;
        }

        cout << "INFORMATION OF MERRY: " << endl;
        cout << "- HP Ship: " << shipHP << endl;
        cout << "- Repair Ship Cost: " << repairCost << endl;
    } else {
        cout << "READING UNSUCCESSFUL" << endl;
    }
}

void testDamageEvaluation() {
    cout << "====================================================" << endl;
    cout << "   DAMAGE EVALUATION (TASK 1)          " << endl;
    cout << left << setw(10) << "STT" << setw(12) << "shipHP" << setw(12) << "Repair" << setw(12) << "Output" << "Expected" << endl;
    cout << string(60, '-') << endl;

    // Danh sach cac test cases
    struct TestCase {
        int shipHP;
        int repairCost;
        int expected;
    } 
    tests[] = {
        // Truong hop 1: shipHP < 455
        {300, 1000, 1000},
        {454, 100,  100},
        
        // Truong hop 2: Tong chu so la so hoan hao (6, 28, ...)
        {123, 500,  750}, 
        {411, 253, 380},
        
        // Truong hop 3: Bien (Edge cases)
        {500, -10,  -10},
        {-50, 100,  100}
    };

    int n = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < n; i++) {
        int result = damageEvaluation(tests[i].shipHP, tests[i].repairCost);
        cout << left << setw(10) << i 
             << setw(12) << tests[i].shipHP 
             << setw(12) << tests[i].repairCost 
             << setw(12) << result 
             << setw(12) << tests[i].expected << endl;

    }
}

void testConflictSimulation() {
    cout << "====================================================" << endl;
    cout << "   CONFLICT SIMULATION (TASK 2)        " << endl;
    

    if (!readFile) cout << "ERROR!" << endl;

    cout << "ConflictIndex: " << conflictSimulation(character, hp, skill, shipHP, repairCost) << endl;

}

void testResolveDuel() {    
    cout << "====================================================" << endl;
    cout << "       RESOLVE DUEL (TASK 3)          " << endl;

    if(!readFile()) cout << "ERROR!" << endl;

    char duelResult[FIXED_CHARACTER][MAX_NAME];

    for (int i = 0; i < FIXED_CHARACTER; i++) {
        duelResult[i][0] = '\0';
    }
    
    resolveDuel(character, hp, skill, 120, 1500, duelResult);
    
    cout << endl;
}

void testdecodeCP9Message() {
    cout << "====================================================" << endl;
    cout << "       DECODE CP9 MESSAGE (TASK 4)          " << endl;

    if (!readFile()) cout << "ERROR!" << endl;

    char cipherText[] = "AQ 7XK#96";
    char resultText[CHAR_MAX] = "";

    decodeCP9Message(character, hp, skill, 120, 1500, cipherText, resultText);

    cout << "Input: " << cipherText << endl;
    cout << "Ouput: " << resultText << endl;
}
