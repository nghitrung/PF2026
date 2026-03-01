#include "water_seven.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////
//Task 0
bool readInput(
    const string &filename,
    char character[FIXED_CHARACTER][MAX_NAME], int hp[FIXED_CHARACTER], 
    int skill[FIXED_CHARACTER], int &shipHP, int &repairCost){
        // TODO 
        ifstream ifs(filename);

        if (!ifs.is_open()) return false;

        string name;
        int val1, val2; 
        int charIdx = 0;
        bool foundShip = false;

        while (ifs >> name >> val1 >> val2) {
            if (name == "GOING_MERRY") {
                if (val1 >= 0 && val1 <= 1000) {
                    shipHP = val1;
                } else {
                    if (val1 <= 0) {
                        shipHP = 0;
                    } else {
                        shipHP = 1000;
                    }
                }

                if (val2 >= 0 && val2 <= 3000) {
                    repairCost = val2;
                } else {
                    if (val2 <= 0) {
                        repairCost = 0;
                    } else {
                        repairCost = 3000;
                    }
                }

                foundShip = true;
            } else {
                if (charIdx < FIXED_CHARACTER) {
                    strncpy(character[charIdx], name.c_str(), MAX_NAME - 1);
                    character[charIdx][MAX_NAME - 1] = '\0';

                    if (val1 >= 0 && val1 <= 1000) {
                        hp[charIdx] = val1;
                    } else {
                        if (val1 <= 0) {
                            hp[charIdx] = 0;
                        } else {
                            hp[charIdx] = 1000;
                        }
                    }

                    if (val2 >= 0 && val2 <= 100) {
                        skill[charIdx] = val2;
                    } else {
                        if (val2 <= 0) {
                            skill[charIdx] = 0;
                        } else {
                            skill[charIdx] = 100;
                        }
                    }

                    charIdx++;
                }
            }
        }

        ifs.close();

        if (charIdx == FIXED_CHARACTER && foundShip) {
            return true;
        }

        return false;
}



// Task 1
int damageEvaluation(int shipHP, int repairCost){
    // TODO
    return 0;
}

// Task 2
int conflictSimulation(
    char character[FIXED_CHARACTER][MAX_NAME], int hp[FIXED_CHARACTER], int skill[FIXED_CHARACTER],
    int shipHP, int repairCost){
        // TODO
        return 0;
    }

// Task 3
void resolveDuel(
    char character[FIXED_CHARACTER][MAX_NAME], int hp[FIXED_CHARACTER], int skill[FIXED_CHARACTER],
    int conflictIndex, int repairCost, char duel[FIXED_CHARACTER][MAX_NAME]){
        //TODO: Output assign to duel parameter
    }

// Task 4
void decodeCP9Message(char character[FIXED_CHARACTER][MAX_NAME], 
    int hp[FIXED_CHARACTER], int skill[FIXED_CHARACTER], int conflictIndex, 
    int repairCost, char cipherText[], char resultText[]){
        //TODO: Output assign to resultText parameter
    }

// Task 5
int analyzeDangerLimit(int grid[MAX_GRID][MAX_GRID], int rows, int cols){
    // TODO
    return 0;
}


bool evaluateRoute(int grid[MAX_GRID][MAX_GRID], int rows, int cols, int dangerLimit){
    // TODO
    return false;
}


////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////
