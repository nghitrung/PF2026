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

    int shipTerm = round((500.0 - shipHP) / 50.0);
    int repairTerm = round(repairCost / 100.0);

    int numEvent = 0; 
    int conflictIndex = skill_luffy - skill_usopp + repairTerm + shipTerm;

    while (numEvent < 10 && conflictIndex < 255) {
        int id = abs(conflictIndex) % 6;

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

    for (int i = 0; i < FIXED_CHARACTER; i++) duel[i][0] = '\0';

    if (currLuffySkill < U) {
        for (int i = 0; i < SUPPORT_CHARACTER; i++) {
            currLuffySkill += supChar[i][0];
            strncpy(duel[count], supCharName[i], MAX_NAME - 1);
            duel[count][MAX_NAME - 1] = '\0';
            count++;
            if (currLuffySkill >= U) break;
        }
    }

    return;
}

// Task 4
void decodeCP9Message(char character[FIXED_CHARACTER][MAX_NAME], int hp[FIXED_CHARACTER], int skill[FIXED_CHARACTER], int conflictIndex, int repairCost, char cipherText[], char resultText[]){
    //TODO: Output assign to resultText parameter
    int key = (conflictIndex + repairCost) % 26;
    int B = (key % 5) + 4;

    char message[CHAR_MAX] = "";
    int readChecksum = -1;
    char* sharpPos = strchr(cipherText, '#');

    if (sharpPos != NULL) {
        int lenMess = sharpPos - cipherText;
        strncpy(message, cipherText, lenMess);
        message[lenMess] = '\0';
        readChecksum = atoi(sharpPos + 1);
    } else {
        strcpy(message, cipherText);
    }

    int lenCipherText = strlen(cipherText);

    if(lenCipherText < 2) {
        strcpy(resultText, "");
        return;
    }

    int calculateSum = 0;
    for (int i = 0; i < strlen(message); i++) {
        calculateSum += (int)message[i];
    }
    calculateSum %= 100;

    if (calculateSum != readChecksum) {
        resultText[0] = '\0';
        return;
    }

    int lenMess = strlen(message);
    for (int i = 0; i < lenMess; i += B) {
        int l = i;
        int r = (i + B - 1 < lenMess) ? (i + B - 1) : (lenMess - 1);

        while (l < r) {
            char temp = message[l];
            message[l] = message[r];
            message[r] = temp;

            l++;
            r--;
        }
    }

    for (int i = 0; i < lenMess; i++) {
        if (message[i] >= 'A' && message[i] <= 'Z') {
            message[i] = (message[i] - 'A' - (key % 26) + 26) % 26 + 'A';
        } else if (message[i] >= '0' && message[i] <= '9') {
            message[i] = (message[i] - '0' - (key % 10) + 10) % 10 + '0';
        }
    }

    strcpy(resultText, message);
    bool hasCP9 = (strstr(message, "CP9") != NULL);

    if (calculateSum == readChecksum && hasCP9) {
        strcat(resultText, "_TRUE");
    } else {
        strcat(resultText, "");
    }

    return;
}

// Task 5
int analyzeDangerLimit(int grid[MAX_GRID][MAX_GRID], int rows, int cols){
    int maxRowSum = 0;
    int maxCell = -1e9;

    for (int i = 0; i < rows; i++) {
        int currentRowSum = 0;
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] > maxCell) {
                maxCell = grid[i][j];
            }
            if (grid[i][j] >= 0) {
                currentRowSum += grid[i][j];
            }
        }
        if (currentRowSum > maxRowSum) {
            maxRowSum = currentRowSum;
        }
    }

    return maxRowSum + maxCell;
}


bool evaluateRoute(int grid[MAX_GRID][MAX_GRID], int rows, int cols, int dangerLimit){
    if (grid[0][0] == -1 || grid[rows - 1][cols - 1] == -1) return false;

    int dp[MAX_GRID][MAX_GRID];
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) dp[i][j] = 1e9; 
    }

    dp[0][0] = grid[0][0];

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] == -1 || dp[i][j] == 1e9) continue;

            if (j + 1 < cols && grid[i][j + 1] != -1) {
                if (dp[i][j] + grid[i][j + 1] < dp[i][j + 1]) {
                    dp[i][j + 1] = dp[i][j] + grid[i][j + 1];
                }
            }

            if (i + 1 < rows && grid[i + 1][j] != -1) {
                if (dp[i][j] + grid[i + 1][j] < dp[i + 1][j]) {
                    dp[i + 1][j] = dp[i][j] + grid[i + 1][j];
                }
            }
        }
    }

    int minTotalDanger = dp[rows - 1][cols - 1];

    return (minTotalDanger != 1e9 && minTotalDanger <= dangerLimit);
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

}



////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////
