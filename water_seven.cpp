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

    bool condition1 = (shipHP < 455);

    bool condition2 = isPerfect(sumDigits(shipHP));

    if (condition1 && condition2) {
        return (repairCost + (int)round(repairCost * 0.5));
    } else {
        return repairCost;
    }
    
}

// Task 2
int conflictSimulation(
    char character[FIXED_CHARACTER][MAX_NAME], int hp[FIXED_CHARACTER], int skill[FIXED_CHARACTER],
    int shipHP, int repairCost) {
    // TODO
    int listEvent[6] = {255, 20, 50, 70, 90, 100};

    int luffyIdx = -1;
    int usoppIdx = -1;

    for (int i = 0; i < FIXED_CHARACTER; i++) {
        if (strcmp(character[i], "LUFFY") == 0) luffyIdx = i;
        if (strcmp(character[i], "USOPP") == 0) usoppIdx = i;
    }

    int skill_luffy = -1;
    int skill_usopp = -1;

    if (luffyIdx != -1 && usoppIdx != -1) {
        skill_luffy = skill[luffyIdx];
        skill_usopp = skill[usoppIdx];
    } 

    int numEvent = 0; 
    int conflictIndex = skill_luffy - skill_usopp + (repairCost / 100) + ((500 - shipHP) / 50);
    int id = -1; 

    while (numEvent < 10 && conflictIndex < 255) {
        id = conflictIndex % 6;

        conflictIndex += listEvent[id];

        numEvent++;
    }

    return conflictIndex;
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

// HELPER FUNCTION

int sumDigits(int n) {
    int sum = 0;
    n = abs(n);

    while (n > 0) {
        sum += n % 10;
        n /= 10;
    }

    return sum;
}

bool isPerfect(int n) {
    if (n <= 1) return false;

    int sum = 0;
    for (int i = 1; i <= n / 2; i++) {
        if (n % i == 0) sum += i;
    }

    return (sum == n);
}

////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////
