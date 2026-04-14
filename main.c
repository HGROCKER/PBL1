#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define _N_ 256

struct ResultWay {
    char name;
    struct ResultWay* next;
};

struct Result {
    struct ResultWay* path;
    int best;
};

 struct Data
 {
        int weight[_N_][_N_];
        int numCount;
        int indexMap[_N_];
        char nameIndex[_N_];
        char startNode;

};

struct Data* DataNow;
void getIndex(char name) {
    if (DataNow->indexMap[name] == -1) {
        DataNow->indexMap[name] = DataNow->numCount;
        DataNow->nameIndex[DataNow->numCount] = name;
        DataNow->numCount++;
    }
}
void readInput(const char *filename, struct Data *data) {
    DataNow = data;
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Khong thu mo file data.txt\n");
        exit(1);
    }
    memset(data->weight, 0, sizeof(data->weight));
    memset(data->indexMap, -1, sizeof(data->indexMap));
    data->numCount = 0;
    fscanf(file, " %c", &data->startNode);
    getIndex(data->startNode);
    char u, v;
    int w;
    while (fscanf(file, " %c %c %d", &u, &v, &w) == 3) {
        getIndex(u);
        getIndex(v);
        int iu = data->indexMap[u];
        int iv = data->indexMap[v];
        data->weight[iu][iv] = data->weight[iv][iu] = data->weight[iu][iv] > w ? data->weight[iu][iv] : w;
    }
    fclose(file);
}

void writeResult(const char *filename, struct Result* result, char startNode) {
    FILE *out = fopen(filename, "w");
    if (!out) out = stdout;
    if (result->best < 0) {
        fprintf(out, "\nKhong co chu trinh Hamilton bat dau tai %c\n", startNode);
        printf("\nKhong co chu trinh Hamilton bat dau tai %c\n", startNode);
    } else {
        fprintf(out, "\nHanh trinh toi uu (%d mat): (QHD + BMask)\n", result->best);
        printf("\nHanh trinh toi uu (%d diem): (QHD + BMask)\n ", result->best);
        struct ResultWay* current = result->path;
        while(current->name != startNode) {
            fprintf(out, "%c -> ", current->name);
            printf("%c -> ", current->name);
            current = current->next;
        }
        do {
            fprintf(out, "%c -> ", current->name);
            printf("%c -> ", current->name);
            current = current->next;
        } while (current->name != startNode);
        fprintf(out, "%c\n", result->path->name);
        printf("%c\n", result->path->name);
    }
}

void solve_QDH_BMask(struct Data* data, struct Result *result) {
    int n = data->numCount;
    int (*weight)[_N_] = data->weight;
    int *indexMap = data->indexMap;
    char *nameIndex = data->nameIndex;
    char startNode = data->startNode;

    if (n < 3) {printf("Khong the tao chu trinh\n"); return ;}
    int fullMask = 1 << (n - 1), mask, j, k;
    int **dp = (int **)malloc(fullMask * sizeof(int *));
    int **parent = (int **)malloc(fullMask * sizeof(int *));
    if (!dp || !parent) {
        printf("Khong du bo nho de xu li\n");
        exit(1);
    }

    for (mask = 0; mask < fullMask; mask++) {
        dp[mask] = (int *)malloc(n * sizeof(int));
        parent[mask] = (int *)malloc(n * sizeof(int));
        if (!dp[mask] || !parent[mask]) {
            printf("Khong du bo nho de xu li\n");
            exit(1);
        }
        for (j = 0; j < n; j++) {
            dp[mask][j] = INT_MIN;
            parent[mask][j] = -1;
        }
    }

    for (j = 1; j < n; j++) {
        if (weight[0][j] > 0) {
            mask = 1 << (j - 1);
            dp[mask][j] = weight[0][j];
            parent[mask][j] = 0;
        }
    }

    for (mask = 0; mask < fullMask; mask++) {
        int nextMask, cand;
        for (j = 1; j < n; j++) {
            if (!(mask & (1 << (j - 1))) || dp[mask][j] == INT_MIN) continue;

            for (k = 1; k < n; k++) {
                if (mask & (1 << (k - 1)) || weight[j][k] == 0) continue;
                nextMask = mask | (1 << (k - 1));
                cand = dp[mask][j] + weight[j][k];
                if (cand > dp[nextMask][k]) {
                    dp[nextMask][k] = cand;
                    parent[nextMask][k] = j;
                }
            }
        }
    }

    result->best = INT_MIN;
    int last = -1, total;
    fullMask--;
    for (j = 1; j < n; j++) {
        if (dp[fullMask][j] == INT_MIN || weight[j][0] == 0) continue;
        total = dp[fullMask][j] + weight[j][0];
        if (total > result->best) {
            result->best = total;
            last = j;
        }
    }
    if (last == -1) {
        for (mask = 0; mask <= fullMask; mask++) {
            free(dp[mask]);
            free(parent[mask]);
        }
        free(dp);
        free(parent);
        return ;
    }

    mask = fullMask;
    result->path->name = nameIndex[0];;
    struct ResultWay *current = result->path;
    int cur = last, temp;
    while (cur != 0) {
        current->next = (struct ResultWay *)malloc(sizeof(struct ResultWay));
        current = current->next;
        current->name = nameIndex[cur];
        temp = parent[mask][cur];
        mask ^= (1 << (cur - 1));
        cur = temp;
    }
    current->next = result->path; // tạo chu trình
    for (mask = 0; mask <= fullMask; mask++) {
        free(dp[mask]);
        free(parent[mask]);
    }
    free(dp);
    free(parent);
    writeResult("output.txt", result, startNode);
    return ;
}
//END TT


// ================= BACKTRACKING =================

int visited, x[_N_];
int best1, best_path[_N_][_N_], count_best;

void out_bt(FILE *g){
    fprintf(g, "\nHanh trinh toi uu (%d mat): (Quay lui)\n", best1);
    for(int i = 0; i < count_best; i++){
        for(int j = 0; j < DataNow->numCount; j++){
            fprintf(g, "%c -> ", DataNow->nameIndex[best_path[i][j]]);
        }
        fprintf(g, "%c\n", DataNow->nameIndex[best_path[i][1]]);
    }
    printf("\nHanh trinh toi uu (%d mat): (Quay lui)\n", best1);
    for(int i = 0; i < count_best; i++){
        for(int j = 0; j < DataNow->numCount; j++){
            printf("%c -> ", DataNow->nameIndex[best_path[i][j]]);
        }
        printf("%c\n", DataNow->nameIndex[best_path[i][1]]);
    }
}

void Try(int i, int sum) {
    for(int v = 0; v < DataNow->numCount; v++){
        if(!(visited & (1 << v)) && DataNow->weight[x[i-1]][v] > 0){
            visited |= (1 << v) ;
            x[i] = v;

            int new_sum = sum + DataNow->weight[x[i-1]][v];

            if(i == DataNow->numCount - 1){
                if(DataNow->weight[v][x[0]] > 0){
                    new_sum += DataNow->weight[v][x[0]];

                    if(new_sum > best1){
                        best1 = new_sum;
                        count_best = 0;
                    }
                    if(new_sum == best1){
                        for(int j = 0; j < DataNow->numCount; j++){
                            best_path[count_best][j] = x[j];
                        }
                        count_best++;
                    }
                }   
            } else {
                Try(i + 1, new_sum);
            }
            visited ^= (1 << v);
        }
    }
}

void solve_backtracking(struct Data* data) {
    DataNow = data;
    FILE *f = fopen("data.txt", "r");
    FILE *g = fopen("output.txt", "a");

    best1 = 0; count_best = 0;
    visited = 0;
    char start, u, v;
    int w;

    fscanf(f, " %c", &start);
    int start_idx = DataNow->indexMap[start];
    x[0] = start_idx;
    visited |= (1 << start_idx);

    Try(1, 0);
    out_bt(g);

    fclose(f);
    fclose(g);
}

//END BACKTRACKING

//Nhanh canh


//END TT

void kTraMTran(int weight[_N_][_N_], int numCount) {
    for (int i = 0; i < numCount; i++) {
        int deg = 0;
        for (int j = 0; j < numCount; j++) {
            if (weight[i][j] != -1) {
                deg++;
            }
        }
        if (deg<2){printf("Khong co loi giai do khong tao thanh chu trinh duong di\n"); exit(1);}
        }
    return ;
}

void printMatrix(int weight[_N_][_N_], int numCount) {
    printf("Ma tran trong so:\n");
    for (int i = 0; i < numCount; i++) {
        for (int j = 0; j < numCount; j++) {
            printf("%4d ", weight[i][j]);
        }
        printf("\n");
    }
}

void MenuChoice() {
    char choice[_N_];
    printf("Chon phuong phap giai:\n");
    scanf("%s", choice);
        if (strcmp(choice, "QHD") == 0) {
            // Gọi hàm giải thuật QHD + BMask
        } else if (strcmp(choice, "Backtracking") == 0) {
            // Gọi hàm giải thuật Backtracking
        } else {
            printf("Lua chon khong hop le. Vui long chon lai.\n");
            MenuChoice();
        }
}

int main(void) {
    //input
    char startNode;
    struct Data DataIn;
    readInput("data.txt", &DataIn);
    printMatrix(DataIn.weight, DataIn.numCount);
    kTraMTran(DataIn.weight, DataIn.numCount);


    if(DataIn.numCount > 25) {
        printf("Số lượng đỉnh quá lớn, không thể giải bằng QHD+BMask\n");
        return 0;
    }
    struct Result *result= (struct Result*)malloc(sizeof(struct Result));
    result->path = (struct ResultWay*)malloc(sizeof(struct ResultWay));

    solve_QDH_BMask(&DataIn, result);
    //free memory
    free(result);


    //solve backtracking
    solve_backtracking(&DataIn);

    //solve
    
    return 0;
}
