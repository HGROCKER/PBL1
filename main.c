#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define __N 256
#define MAX 100

int weight[__N][__N];       // ma trận trọng số
int indexMap[__N];          // ánh xạ ký tự -> chỉ số
char nameIndex[__N];      // danh sách ký tự theo chỉ số
int numCount = 0;

void getIndex(char uc) {
    if (indexMap[uc] == -1) {
        indexMap[uc] = numCount;
        nameIndex[numCount] = uc;
        numCount++;
    }
    return ;
}

struct Result {
    char name;
    struct Result* next;
};

void writeResult(const char *filename, struct Result *path, int best, char startNode) {
    FILE *out = fopen(filename, "w");
    if (!out) out = stdout;
    if (best < 0) {
        fprintf(out, "\nKhong co chu trinh Hamilton bat dau tai %c\n", startNode);
        printf("\nKhong co chu trinh Hamilton bat dau tai %c\n", startNode);
    } else {
        fprintf(out, "\nHanh trinh toi uu (%d mat): (QHD + BMask)\n", best);
        printf("\nHanh trinh toi uu (%d diem): (QHD + BMask)\n ", best);
        struct Result *current = path;
        do {
            fprintf(out, "%c -> ", current->name);
            printf("%c -> ", current->name);
            current = current->next;
        } while (current != path);
        fprintf(out, "%c\n", path->name);
        printf("%c\n", path->name);
    }
}

void readInput(const char *filename, char *startNode) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Khong thu mo file data.txt\n");
        exit(1);
    }

    memset(weight, -1, sizeof(weight));
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

int solve_QDH_BMask(struct Result *outPath) {
    int n = numCount;
    if (n < 3) return -1;

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
        if (weight[0][j] >= 0) {
            int mask = 1 << (j - 1);
            dp[mask][j] = weight[0][j];
            parent[mask][j] = 0;
        }
    }

    for (int mask = 0; mask < fullMask; mask++) {
        for (int j = 1; j < n; j++) {
            if (!(mask & (1 << (j - 1))) || dp[mask][j] == INT_MIN) continue;
            for (int k = 1; k < n; k++) {
                if (mask & (1 << (k - 1)) || weight[j][k] < 0) continue;
                int nextMask = mask | (1 << (k - 1));
                int cand = dp[mask][j] + weight[j][k];
                if (cand > dp[nextMask][k]) {
                    dp[nextMask][k] = cand;
                    parent[nextMask][k] = j;
                }
            }
        }
    }

    int best = INT_MIN;
    int last = -1;
    fullMask--;
    for (int j = 1; j < n; j++) {
        if (dp[fullMask][j] == INT_MIN || weight[j][0] < 0) continue;
        int total = dp[fullMask][j] + weight[j][0];
        if (total > best) {
            best = total;
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
        return INT_MIN;
    }

    int mask = fullMask;
    int idx = n;
    outPath->name = nameIndex[0];
    struct Result *current = outPath;
    int cur = last;
    while (cur != 0) {
        current->next = (struct Result *)malloc(sizeof(struct Result));
        current = current->next;
        current->name = nameIndex[cur];
        int temp = parent[mask][cur];
        mask ^= (1 << (cur - 1));
        cur = temp;
    }
    current->next = outPath; // tạo chu trình
    for (int m = 0; m <= fullMask; m++) {
        free(dp[m]);
        free(parent[m]);
    }
    free(dp);
    free(parent);

    return best;
}
//END TT


// ================= BACKTRACKING =================


int n1;
int c[MAX][MAX], visited[MAX], x[MAX];
int best1, best_path[MAX][MAX], count_best;
char name[MAX];

int get_index(char ch){
    for(int i = 1; i <= n1; i++){
        if(name[i] == ch) return i;
    }
    n1++;
    name[n1] = ch;
    return n1;
}

void out_bt(FILE *g){
    fprintf(g, "\nHanh trinh toi uu (%d mat): (Quay lui)\n", best1);
    for(int i = 0; i < count_best; i++){
        for(int j = 1; j <= n1; j++){
            fprintf(g, "%c -> ", name[best_path[i][j]]);
        }
        fprintf(g, "%c\n", name[best_path[i][1]]);
    }
    printf("\nHanh trinh toi uu (%d mat): (Quay lui)\n", best1);
    for(int i = 0; i < count_best; i++){
        for(int j = 1; j <= n1; j++){
            printf("%c -> ", name[best_path[i][j]]);
        }
        printf("%c\n", name[best_path[i][1]]);
    }
}

void Try(int i, int sum){
    for(int v = 1; v <= n1; v++){
        if(!visited[v] && c[x[i-1]][v] > 0){
            visited[v] = 1;
            x[i] = v;

            int new_sum = sum + c[x[i-1]][v];

            if(i == n1){
                if(c[v][x[1]] > 0){
                    new_sum += c[v][x[1]];

                    if(new_sum > best1){
                        best1 = new_sum;
                        count_best = 0;
                    }
                    if(new_sum == best1){
                        for(int j = 1; j <= n1; j++){
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

    n1 = 0; best1 = 0; count_best = 0;

    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++) c[i][j] = 0;
        visited[i] = 0;
    }

    char start, u, v;
    int w;

    fscanf(f, " %c", &start);

    while(fscanf(f, " %c %c %d", &u, &v, &w) != EOF){
        int i = get_index(u);
        int j = get_index(v);
        c[i][j] = c[j][i] = w;
    }

    int start_idx = get_index(start);
    x[1] = start_idx;
    visited[start_idx] = 1;

    Try(2, 0);
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

void printMatrix(int weight[__N][__N], int numCount) {
    printf("Ma tran trong so:\n");
    for (int i = 0; i < numCount; i++) {
        for (int j = 0; j < numCount; j++) {
            printf("%4d ", weight[i][j]);
        }
        printf("\n");
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
    struct Result *path = (struct Result*)malloc(sizeof(struct Result));
    int best = solve_QDH_BMask(path);

    //output
    writeResult("output.txt", path, best, startNode);

    //free memory
    free(path);



    return 0;
}
