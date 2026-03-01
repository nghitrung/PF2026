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



int main(int argc, const char * argv[]) {

    char character[FIXED_CHARACTER][MAX_NAME]; 
    int hp[FIXED_CHARACTER];
    int skill[FIXED_CHARACTER];
    int shipHP = 0;
    int repairCost = 0;

    //string filename = "opw_tc_01_input";
    string filename = "opw_tc_02_input";
    
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
    
    return 0;
}
