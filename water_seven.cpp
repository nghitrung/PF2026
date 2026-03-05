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

    bool condition2 = isPerfect(shipHP);

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

    takeValuation(character, hp, skill, shipHP, repairCost);

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
void resolveDuel(char character[FIXED_CHARACTER][MAX_NAME], int hp[FIXED_CHARACTER], int skill[FIXED_CHARACTER], int conflictIndex, int repairCost, char duel[FIXED_CHARACTER][MAX_NAME]){
    //TODO: Output assign to duel parameter
    takeValuation(character, hp, skill, 0, repairCost);
    char supCharName[SUPPORT_CHARACTER][MAX_NAME];

    int supIdx = 0;
    int count = 0;

    while (supIdx < SUPPORT_CHARACTER) {
        if (supChar[supIdx][0] == skill[count]) {
            if (strcmp(character[count], "LUFFY") != 0 && strcmp(character[count], "USOPP") != 0) {
                //for (int i = 0; i < SUPPORT_CHARACTER; i++) {
                    //if (strcmp(supCharName[count], supCharName[i]) != 0) {
                        strcpy(supCharName[supIdx],character[count]);
                        supIdx++;
                        count = 0;
                    // }
                //}
                cout << "in if - " << supIdx << endl;

            } else {
                count++;
            }
        } else {
            //cout << "in else - " << supIdx << endl;
            count++;
        }
    }

    cout << left << setw(10) << "Name" << setw(10) << "Support" << setw(10) << "Cost" << endl;
    for (int i = 0; i < SUPPORT_CHARACTER; i++) {
        cout << left << setw(10)<< supCharName[i] << setw(10) << supChar[i][0] 
             << setw(10) << supChar[i][1] << endl;
    }

    int U = skill_usopp + (conflictIndex / 20) + (repairCost / 500);

    return;
}

// Task 4
void decodeCP9Message(char character[FIXED_CHARACTER][MAX_NAME], int hp[FIXED_CHARACTER], int skill[FIXED_CHARACTER], int conflictIndex, int repairCost, char cipherText[], char resultText[]){
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
bool isPerfect(int n) {
    int sum = 0;
    n = abs(n);

    while (n > 0) {
        sum += n % 10;
        n /= 10;
    }

    if (sum <= 1) return false;

    int nums = 0;
    for (int i = 1; i <= sum / 2; i++) {
        if (sum % i == 0) nums += i;
    }

    return (nums == sum);
}

void takeValuation(char character[FIXED_CHARACTER][MAX_NAME], int hp[FIXED_CHARACTER], int skill[FIXED_CHARACTER],int shipHP, int repairCost) {
    int charIdx = 0;

    for (int i = 0; i < FIXED_CHARACTER; i++) {
        if (strcmp(character[i], "LUFFY") == 0){
            skill_luffy = skill[i];
        } else if (strcmp(character[i], "USOPP") == 0){
            skill_usopp = skill[i];
        } else {
            if (charIdx < SUPPORT_CHARACTER) {
                supChar[charIdx][0] = skill[i];
                supChar[charIdx][1] = (hp[i] % 10) + 1;        
                charIdx++;            
            }
        }
    }

    sort();
}

void sort() {
    for (int i = 0; i < SUPPORT_CHARACTER - 1; i++) {
        int min = i;
        for (int j = i + 1; j < SUPPORT_CHARACTER; j++) {
            if (supChar[j][1] < supChar[min][1]){
                min = j;
            }
        }

        if (min != i) {
            for (int k = 0; k < SUPPORT_CHARACTER; k++) {
                int temp = supChar[i][k];
                supChar[i][k] = supChar[min][k];
                supChar[min][k] = temp;
            }
        }
    }
}



////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////
