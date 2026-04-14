#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define _N_ 256

int weight[_N_][_N_];       // ma trận trọng số
int indexMap[_N_];          // ánh xạ ký tự -> chỉ số 'v' -> index v = 0 ; v[0]=
char nameIndex[_N_];      // danh sách ký tự theo chỉ số
int numCount = 0;

struct ResultWay {
    char name;
    struct ResultWay* next;
};

struct Result {
    struct ResultWay* path;
    int best;
};

void getIndex(char uc) {
    if (indexMap[uc] == -1) {
        indexMap[uc] = numCount;
        nameIndex[numCount] = uc;
        numCount++;
    }
    return ;
}

void readInput(const char *filename, char *startNode) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Khong thu mo file data.txt\n");
        exit(1);
    }

    memset(weight, 0, sizeof(weight));
    memset(indexMap, -1, sizeof(indexMap));
    numCount = 0;

    fscanf(file, " %c", startNode);
    getIndex(*startNode);

    char u, v;
    int w;
    while (fscanf(file, " %c %c %d", &u, &v, &w) == 3) {
        getIndex(u);
        getIndex(v);
        int iu = indexMap[u];
        int iv = indexMap[v];
        weight[iu][iv] = weight[iv][iu] = weight[iu][iv] > w ? weight[iu][iv] : w;
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

void solve_QDH_BMask(struct Result *result, char startNode) {
    int n = numCount;
    if (n < 3) {printf("Khong the tao chu trinh\n"); return ;}

    int fullMask = 1 << (n - 1);
    int **dp = (int **)malloc(fullMask * sizeof(int *));
    int **parent = (int **)malloc(fullMask * sizeof(int *));
    if (!dp || !parent) {
        printf("Khong du bo nho de xu li\n");
        exit(1);
    }

    for (int mask = 0; mask < fullMask; mask++) {
        dp[mask] = (int *)malloc(n * sizeof(int));
        parent[mask] = (int *)malloc(n * sizeof(int));
        if (!dp[mask] || !parent[mask]) {
            printf("Khong du bo nho de xu li\n");
            exit(1);
        }
        for (int j = 0; j < n; j++) {
            dp[mask][j] = INT_MIN;
            parent[mask][j] = -1;
        }
    }

    for (int j = 1; j < n; j++) {
        if (weight[0][j] > 0) {
            int mask = 1 << (j - 1);
            dp[mask][j] = weight[0][j];
            parent[mask][j] = 0;
        }
    }

    for (int mask = 0; mask < fullMask; mask++) {
        for (int j = 1; j < n; j++) {
            if (!(mask & (1 << (j - 1))) || dp[mask][j] == INT_MIN) continue;
            for (int k = 1; k < n; k++) {
                if (mask & (1 << (k - 1)) || weight[j][k] == 0) continue;
                int nextMask = mask | (1 << (k - 1));
                int cand = dp[mask][j] + weight[j][k];
                if (cand > dp[nextMask][k]) {
                    dp[nextMask][k] = cand;
                    parent[nextMask][k] = j;
                }
            }
        }
    }

    result->best = INT_MIN;
    int last = -1;
    fullMask--;
    for (int j = 1; j < n; j++) {
        if (dp[fullMask][j] == INT_MIN || weight[j][0] == 0) continue;
        int total = dp[fullMask][j] + weight[j][0];
        if (total > result->best) {
            result->best = total;
            last = j;
        }
    }

    if (last == -1) {
        for (int mask = 0; mask <= fullMask; mask++) {
            free(dp[mask]);
            free(parent[mask]);
        }
        free(dp);
        free(parent);
        return ;
    }

    int mask = fullMask;
    int idx = n;
    result->path->name = nameIndex[0];;
    struct ResultWay *current = result->path;
    int cur = last;
    while (cur != 0) {
        current->next = (struct ResultWay *)malloc(sizeof(struct ResultWay));
        current = current->next;
        current->name = nameIndex[cur];
        int temp = parent[mask][cur];
        mask ^= (1 << (cur - 1));
        cur = temp;
    }
    current->next = result->path; // tạo chu trình
    for (int m = 0; m <= fullMask; m++) {
        free(dp[m]);
        free(parent[m]);
    }
    free(dp);
    free(parent);

    writeResult("output.txt", result, startNode);
    return ;
}
//END TT


// ================= BACKTRACKING =================

int visited[_N_], x[_N_];
int best1, best_path[_N_][_N_], count_best;

void out_bt(FILE *g){
    fprintf(g, "\nHanh trinh toi uu (%d mat): (Quay lui)\n", best1);
    for(int i = 0; i < count_best; i++){
        for(int j = 0; j < numCount; j++){
            fprintf(g, "%c -> ", nameIndex[best_path[i][j]]);
        }
        fprintf(g, "%c\n", nameIndex[best_path[i][1]]);
    }
    printf("\nHanh trinh toi uu (%d mat): (Quay lui)\n", best1);
    for(int i = 0; i < count_best; i++){
        for(int j = 0; j < numCount; j++){
            printf("%c -> ", nameIndex[best_path[i][j]]);
        }
        printf("%c\n", nameIndex[best_path[i][1]]);
    }
}

void Try(int i, int sum){
    for(int v = 0; v < numCount; v++){
        if(!visited[v] && weight[x[i-1]][v] > 0){
            visited[v] = 1;
            x[i] = v;

            int new_sum = sum + weight[x[i-1]][v];

            if(i == numCount - 1){
                if(weight[v][x[0]] > 0){
                    new_sum += weight[v][x[0]];

                    if(new_sum > best1){
                        best1 = new_sum;
                        count_best = 0;
                    }
                    if(new_sum == best1){
                        for(int j = 0; j < numCount; j++){
                            best_path[count_best][j] = x[j];
                        }
                        count_best++;
                    }
                }   
            } else {
                Try(i + 1, new_sum);
            }

            visited[v] = 0;
        }
    }
}

void solve_backtracking(){
    FILE *f = fopen("data.txt", "r");
    FILE *g = fopen("output.txt", "a");

    best1 = 0; count_best = 0;

    memset(visited, 0, sizeof(visited));

    char start, u, v;
    int w;

    fscanf(f, " %c", &start);

    int start_idx = indexMap[start];
    x[0] = start_idx;
    visited[start_idx] = 1;

    Try(1, 0);
    out_bt(g);

    fclose(f);
    fclose(g);
}


//END TT

void kTraMTran() {
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
    readInput("data.txt", (char*)&startNode);
    printMatrix(weight, numCount);
    kTraMTran();

    //solve backtracking
    solve_backtracking();

    //solve
    if(numCount > 20) {
        printf("Số lượng đỉnh quá lớn, không thể giải bằng QHD+BMask\n");
        return 0;
    }
    struct Result *result= (struct Result*)malloc(sizeof(struct Result));
    struct ResultWay *path = (struct ResultWay*)malloc(sizeof(struct ResultWay));
    result->path = path;
    solve_QDH_BMask(result, startNode);

    //free memory
    free(result);



    return 0;
}
