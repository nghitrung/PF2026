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
    
    //testReadInput();

    testDamageEvaluation();

    //testConflictSimulation();

    return 0;
}

void testReadInput() {
    string filename = "opw_tc_01_input";
    //string filename = "opw_tc_02_input";
    
    cout << "--- READING FILE: " << filename << " ---" << endl;

    bool success = readInput(filename, character, hp, skill, shipHP, repairCost);

    if (success) {
        cout << "READING SUCCESSFULL" << endl;
        cout << "-------------------" << endl;
        cout << left << setw(15) << "Name" << setw(10) << "HP" << setw(10) << "Skill" << endl;
        cout << "-------------------" << endl;

        for (int i = 0; i < FIXED_CHARACTER; i++) {
            cout << left << setw(15) << character[i] << setw(10) << hp[i] << setw(10) << skill[i] << endl;
        }

        cout << "------------------" << endl;
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
    cout << "====================================================" << endl;
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
    cout << "====================================================" << endl << endl;
}

void testConflictSimulation() {
    cout << "====================================================" << endl;
    cout << "   CONFLICT SIMULATION (TASK 2)        " << endl;
    cout << "====================================================" << endl;

    testReadInput();

    int result = -1;
    for (int i = 0; i < FIXED_CHARACTER; i++) {
        result = conflictSimulation(character, hp, skill, shipHP, repairCost);
    }

    cout << "ConflictIndex: " << result << endl;
}