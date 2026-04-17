#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define _N_ 256
#define MAX 10
#define INF 1e9


char *nameFileOut = "output.txt";
char *nameFileIn = "data.txt";
int n;
int (*weight)[_N_], *indexMap;
char *nameIndex, startNode;

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


void printMatrix() {
    printf("Ma tran trong so:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%4d ", weight[i][j]);
        }
        printf("\n");
    }
}

void getIndex(char name, int *n) {
    if (indexMap[name] == -1) {
        indexMap[name] = *n;
        nameIndex[*n] = name;
        (*n)++;
    }
}

void ChoiceData(struct Data* data){
    n = data->numCount;
    weight = data->weight;
    indexMap = data->indexMap;
    nameIndex = data->nameIndex;
    startNode = data->startNode;
}

void readInput(const char *filename, struct Data *data) {
    ChoiceData(data);
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Khong thu mo file data.txt\n");
        exit(1);
    }
    memset(data->weight, -1, sizeof(data->weight));
    memset(data->indexMap, -1, sizeof(data->indexMap));
    data->numCount = 0;
    fscanf(file, " %c", &data->startNode);
    getIndex(data->startNode,&data->numCount);
    char u, v;
    int w;
    while (fscanf(file, " %c %c %d", &u, &v, &w) == 3) {
        getIndex(u,&data->numCount);
        getIndex(v,&data->numCount);
        int iu = data->indexMap[u];
        int iv = data->indexMap[v];
        data->weight[iu][iv] = data->weight[iv][iu] = data->weight[iu][iv] > w ? data->weight[iu][iv] : w;
    }
    fclose(file);
}

void writeResult(const char *filename, struct Result* result, char startNode) {
    FILE *out = fopen(filename, "a");
    if (!out) out = stdout;
    if (result->best < 0) {
        fprintf(out, "\nKhong co chu trinh Hamilton bat dau tai %c\n", startNode);
        printf("\nKhong co chu trinh Hamilton bat dau tai %c\n", startNode);
    } else {
        fprintf(out, "\nHanh trinh toi uu (%d mat): \n", result->best);
        printf("\nHanh trinh toi uu (%d diem):\n ", result->best);
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

void kTraMTran() {
    for (int i = 0; i < n; i++) {
        int deg = 0;
        for (int j = 0; j < n; j++) {
            if (weight[i][j] != -1) {
                deg++;
            }
        }
        if (deg<2){printf("Khong co loi giai do khong tao thanh chu trinh duong di\n"); exit(1);}
        }
    return ;
}



void solve_QDH_BMask(struct Data* data, struct Result *result) {
    ChoiceData(data);
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
                if (mask & (1 << (k - 1)) || weight[j][k] <= 0) continue;
                nextMask = mask | (1 << (k - 1));
                cand = dp[mask][j] + weight[j][k];
                if (cand > dp[nextMask][k]) {
                    dp[nextMask][k] = cand;
                    parent[nextMask][k] = j;
                }
            }
        }
    }
    result->best = -1;
    int last = -1, total;
    fullMask--;
    for (j = 1; j < n; j++) {
        if (dp[fullMask][j] == INT_MIN || weight[j][0] <= 0) continue;
        total = dp[fullMask][j] + weight[j][0];
        if (total > result->best) {
            result->best = total;
            last = j;
        }
    }
    if (last == -1) {
        writeResult(nameFileOut, result, startNode);
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
    printf("\nQHD+BMash");
    writeResult(nameFileOut, result, startNode);
    return ;
}
//END TT


// ================= BACKTRACKING =================

int visited, x[_N_];
int best1=0, best_path[_N_][_N_], count_best=0;

void out_bt(FILE *g){
    if(best1 < 1){
        printf("Khong co loi giai do khong tao thanh chu trinh duong di (Quay lui)\n");
        return ;
    }
    fprintf(g, "\nHanh trinh toi uu (%d mat): (Quay lui)\n", best1);
    for(int i = 0; i < count_best; i++){
        for(int j = 0; j < n; j++){
            fprintf(g, "%c -> ", nameIndex[best_path[i][j]]);
        }
        fprintf(g, "%c\n", nameIndex[best_path[i][0]]);
    }
    printf("\nHanh trinh toi uu (%d mat): (Quay lui)\n", best1);
    for(int i = 0; i < count_best; i++){
        for(int j = 0; j < n; j++){
            printf("%c -> ", nameIndex[best_path[i][j]]);
        }
        printf("%c\n", nameIndex[best_path[i][0]]);
    }
}

void Try(int i, int sum) {
    for(int v = 0; v < n; v++){
        if(!(visited & (1 << v)) && weight[x[i-1]][v] > 0){
            visited |= (1 << v) ;
            x[i] = v;

            int new_sum = sum + weight[x[i-1]][v];

            if(i == n - 1){
                if(weight[v][x[0]] > 0){
                    new_sum += weight[v][x[0]];

                    if(new_sum > best1){
                        best1 = new_sum;
                        count_best = 0;
                    }
                    if(new_sum == best1){
                        for(int j = 0; j < n; j++){
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
    ChoiceData(data);
    FILE *f = fopen(nameFileIn, "r");
    FILE *g = fopen(nameFileOut, "a");

    best1 = 0; count_best = 0;
    visited = 0;
    char start, u, v;
    int w;

    fscanf(f, " %c", &start);
    int start_idx = indexMap[start];
    x[0] = start_idx;
    visited |= (1 << start_idx);

    Try(1, 0);
    out_bt(g);

    fclose(f);
    fclose(g);
}

int final_cost = INF;
int best_edges[MAX][2]; 
int current_edges[MAX][2];

int reduce_matrix(int matrix[MAX][MAX]) {
    int reduction_sum = 0;
    for (int i = 0; i < n; i++) {
        int min_val = INF;
        for (int j = 0; j < n; j++)
            if (matrix[i][j] < min_val) min_val = matrix[i][j];
        if (min_val != INF && min_val != 0) {
            for (int j = 0; j < n; j++)
                if (matrix[i][j] != INF) matrix[i][j] -= min_val;
            reduction_sum += min_val;
        }
    }
    for (int j = 0; j < n; j++) {
        int min_val = INF;
        for (int i = 0; i < n; i++)
            if (matrix[i][j] < min_val) min_val = matrix[i][j];
        if (min_val != INF && min_val != 0) {
            for (int i = 0; i < n; i++)
                if (matrix[i][j] != INF) matrix[i][j] -= min_val;
            reduction_sum += min_val;
        }
    }
    return reduction_sum;
}

void find_best_edge(int matrix[MAX][MAX], int *r, int *s) {
    int max_regret = -1;
    *r = -1; *s = -1;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (matrix[i][j] == 0) {
                int min_row = INF, min_col = INF;
                for (int k = 0; k < n; k++) {
                    if (k != j && matrix[i][k] < min_row) min_row = matrix[i][k];
                    if (k != i && matrix[k][j] < min_col) min_col = matrix[k][j];
                }
                int regret = (min_row == INF ? 0 : min_row) + (min_col == INF ? 0 : min_col);
                if (regret > max_regret) {
                    max_regret = regret;
                    *r = i; *s = j;
                }
            }
        }
    }
}

void copyMatran(int in[MAX][MAX], int out[MAX][MAX]){
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) out[i][j] = in[i][j];
    return ;
}


void createCycle(struct ResultWay* res, char nameIndex[]) {
    int start_node_idx = 0; 
    int current_node = start_node_idx;

    struct ResultWay* current_way = res;
    struct ResultWay* prev_way = NULL;
    for (int i = 0; i < n; i++) {
        current_way->name = nameIndex[current_node];
        int next_node = -1;
        for (int j = 0; j < n; j++) {
            if (best_edges[j][0] == current_node) {
                next_node = best_edges[j][1];
                break;
            }
        }
        if (i == n - 1) {
            current_way->next = res; 
        } else {
            current_way->next = (struct ResultWay*)malloc(sizeof(struct ResultWay));
            current_way = current_way->next;
        }

        current_node = next_node;
    }
}



void branch_and_bound(int matrix[MAX][MAX], int bound, int edge_count) {
    if (bound >= final_cost) return;

    if (edge_count == n) {
        final_cost = bound;
        for (int i = 0; i < n; i++) {
            best_edges[i][0] = current_edges[i][0];
            best_edges[i][1] = current_edges[i][1];
        }
        return;
    }

    int r, s;
    find_best_edge(matrix, &r, &s);
    if (r == -1) return;

    // --- Nhánh 1: CHỌN cạnh (r, s) ---
    int next_matrix[MAX][MAX];
    copyMatran(matrix,next_matrix);

    for (int k = 0; k < n; k++) {
        next_matrix[r][k] = INF;
        next_matrix[k][s] = INF;
    }
    next_matrix[s][r] = INF; // Chống chu trình ngược 2 điểm

    current_edges[edge_count][0] = r;
    current_edges[edge_count][1] = s;

    int res_reduction = reduce_matrix(next_matrix);
    branch_and_bound(next_matrix, bound + res_reduction, edge_count + 1);

    // --- Nhánh 2: KHÔNG CHỌN cạnh (r, s) ---
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) next_matrix[i][j] = matrix[i][j];
    
    next_matrix[r][s] = INF;
    res_reduction = reduce_matrix(next_matrix);
    branch_and_bound(next_matrix, bound + res_reduction, edge_count);
}


int createMatraXuli(int C[MAX][MAX]){
    int M=-1;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (C[i][j] == -1 || i == j) C[i][j] = INF;
            if(M<C[i][j])M=C[i][j];
        }
    }
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            if(C[i][j]!=INF)C[i][j]=M-C[i][j];
        }
    }
    return M;
}



void NhanhCanh(struct Data* data,struct Result* res){
    ChoiceData(data);
    int matrix[MAX][MAX];
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++)matrix[i][j]=weight[i][j];
    }
    int M;
    M = createMatraXuli(matrix);
    int initial_bound = reduce_matrix(matrix);
    branch_and_bound(matrix, initial_bound, 0);
    res->best=final_cost>=INF?-1:M*n - final_cost;
    // if(res->best!=-1)print_cycle();
    createCycle(res->path, data->nameIndex);
    printf("\nNhanhCanh");
    writeResult(nameFileOut,res,data->startNode);
}
//END TT




int main(void) {
    //input
    struct Data DataIn;
    readInput(nameFileIn, &DataIn);
    n=DataIn.numCount;
    ChoiceData(&DataIn);
    printMatrix();
    kTraMTran();

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
    struct Result *result2= (struct Result*)malloc(sizeof(struct Result));
    result2->path = (struct ResultWay*)malloc(sizeof(struct ResultWay));
    NhanhCanh(&DataIn,result2);
    free(result2);



    return 0;
}
