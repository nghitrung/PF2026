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
    bool isUsed[FIXED_CHARACTER] = {false};

    while (supIdx < SUPPORT_CHARACTER) {
        for (int i = 0; i < FIXED_CHARACTER; i++) {
            if (!isUsed[i] && supChar[supIdx][0] == skill[i] && strcmp(character[i], "LUFFY") != 0 && strcmp(character[i], "USOPP") != 0) {
                strcpy(supCharName[supIdx], character[i]);
                isUsed[i] = true;
                supIdx++;
                break;
            }
        }
    }

    int U = skill_usopp + (conflictIndex / 20) + (repairCost / 500);

    int currLuffySkill = skill_luffy;

    int count = 0;

    char result[FIXED_CHARACTER][MAX_NAME];

    if (currLuffySkill < U) {
        for (int i = 0; i < SUPPORT_CHARACTER; i++) {
            currLuffySkill += supChar[i][0];
            strncpy(result[count], supCharName[i], MAX_NAME - 1);
            result[count][MAX_NAME - 1] = '\0';
            count++;
            if (currLuffySkill >= U) break;
        }
    }

    memcpy(duel, result, sizeof(result));
}

// Task 4
void decodeCP9Message(char character[FIXED_CHARACTER][MAX_NAME], int hp[FIXED_CHARACTER], int skill[FIXED_CHARACTER], int conflictIndex, int repairCost, char cipherText[], char resultText[]){
    //TODO: Output assign to resultText parameter
    int key = (conflictIndex + repairCost) % 26;
    int B = (key % 5) + 4;

    int lenCipherText = strlen(cipherText);

    if(lenCipherText <= 2) {
        strcpy(resultText, "");
        return;
    }

    int lenMess = lenCipherText - 2;
    char mess[CHAR_MAX];
    strncpy(mess, cipherText, lenMess);
    mess[lenMess] = '\0';

    int XY = (cipherText[lenCipherText - 2] - '0') * 10 + (cipherText[lenCipherText - 1] - '0');
    int checkSum = 0;
    for (int i = 0; i < lenMess; i++) {
        checkSum += (int)(mess[i]);
    }

    checkSum = checkSum % 100;

    if (checkSum != XY) {
        strcpy(resultText, "");
        return;
    }

    // two pointer
    for (int i = 0; i < lenMess; i += B) {
        int l = i;
        int r = (r + B - 1 < lenCipherText) ? (r + B - 1) : (lenCipherText - 1);

        while (l < r) {
            char temp = mess[l];
            mess[l] = mess[r];
            mess[r] = temp;

            l++;
            r++;
        }
    }

    for (int i = 0; i < lenMess; i++) {
        if (mess[i] > 'A' && mess[i] <= 'Z') {
            mess[i] = (mess[i] - 'A' - key % 26 + 26) % 26 + 'A';
        } else if (mess[i] >= '0' && mess[i] < 'A') {
            int keyMod10 = key%10;
            mess[i] = (mess[i] - '0' - keyMod10 + 10) % 10 + '0';
        }
    }

    strcpy(resultText, mess);
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

    quickSort(supChar, 0, SUPPORT_CHARACTER - 1);

}

int partition(int arr[SUPPORT_CHARACTER][SUPPORT_DETAIL], int l, int r) {
    int pivot = arr[r][1];

    int i = l - 1;

    for (int j = l; j <= r - 1; j++) {
        if (arr[j][1] < pivot) {
            i++;
            swap(arr[i][0], arr[j][0]);
            swap(arr[i][1], arr[j][1]);
        }
    }

    swap(arr[i + 1][0], arr[r][0]);
    swap(arr[i + 1][1], arr[r][1]);

    return (i + 1);
}

void quickSort(int arr[SUPPORT_CHARACTER][SUPPORT_DETAIL], int l, int r) {
    if (l < r) {
        int pi = partition(arr, l, r);

        quickSort(arr, l, pi - 1);
        quickSort(arr, pi + 1, r);
    }
}



////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////
