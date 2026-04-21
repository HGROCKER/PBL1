#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <windows.h>


#define __MAX_N 32
#define __MAX_CHAR 256
#define __MAX_Way 1000
#define INF 1e9


void printMenu() {
    #define RESET   "\033[0m"
    #define RED     "\033[31m"
    #define GREEN   "\033[32m"
    #define YELLOW  "\033[33m"
    #define BLUE    "\033[34m"
    #define CYAN    "\033[36m"

    printf(CYAN "=============================================================\n" RESET);

    printf(GREEN "| %-61s|\n" RESET, "TRUONG DAI HOC BACH KHOA - DAI HOC DA NANG");
    printf(GREEN "| %-61s|\n" RESET, "KHOA: CONG NGHE THONG TIN");

    printf(YELLOW "| %-61s|\n" RESET, "PBL1: DO AN LAP TRINH TINH TOAN");
    printf(YELLOW "| %-61s|\n" RESET, "DE TAI: BAI TOAN ONG TIM MAT");

    printf(BLUE "| %-61s|\n" RESET, "GIAO VIEN HUONG DAN: TS. NGUYEN VAN HIEU");

    printf(RED "| %-61s|\n" RESET, "SINH VIEN: TRAN VAN KHANH HUNG - 25T_KHDL");
    printf(RED "| %-61s|\n" RESET, "           PHAM PHU HUNG - 25T_KHDL");

    printf(CYAN "=============================================================\n\n" RESET);
}

void printMatrix(int weight[__MAX_N][__MAX_N], int n) {
    printf("Ma tran trong so:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%4d ", weight[i][j]);
        }
        printf("\n");
    }
}

void clearDataFile(char *filename){
    FILE *out = fopen(filename, "w");
    fclose(out);
    return ;
}

char *nameFileOut = "output.txt", *nameFileIn = "data.txt";

struct ResultWay {
    char name;
    struct ResultWay* next;
};

void readInput(const char *filename, int weight[__MAX_N][__MAX_N], int indexMap[__MAX_CHAR], char nameIndex[__MAX_N], int *n, char *startNode) {
    FILE *file = fopen(filename, "r");
    int i,j;
    if (!file) {
        fprintf(stderr, "Khong thu mo file %s\n", filename);
        exit(1);
    }

    for(i=0; i<__MAX_N; i++) for(j=0; j<__MAX_N; j++) weight[i][j] = -1;
    for(i=0; i<__MAX_CHAR; i++)indexMap[i]=-1;
    *n = 0;
    fscanf(file, " %c", startNode);
    indexMap[(unsigned char)*startNode] = *n;
    nameIndex[*n] = *startNode;
    (*n)++;

    char u, v;
    int w;
    while (fscanf(file, " %c %c %d", &u, &v, &w) == 3) {
        if (indexMap[(unsigned char)u] == -1) {
            indexMap[(unsigned char)u] = *n;
            nameIndex[*n] = u;
            (*n)++;
        }
        if (indexMap[(unsigned char)v] == -1) {
            indexMap[(unsigned char)v] = *n;
            nameIndex[*n] = v;
            (*n)++;
        }
        int iu = indexMap[(unsigned char)u];
        int iv = indexMap[(unsigned char)v];
        if (weight[iu][iv] == -1 || w > weight[iu][iv]) {
            weight[iu][iv] = weight[iv][iu] = w;
        }
    }
    fclose(file);
}

void writeResult(const char *filename, struct ResultWay* result, int best, char startNode, const char *nameTypeSolve) {
    FILE *out = fopen(filename, "a");
    if (!out) out = stdout;
    
    if (best <= 0) {
        fprintf(out, "\nKhong co chu trinh di qua tat ca cac dinh \n");
        printf("\nKhong co chu trinh di qua tat ca cac dinh \n");
    } else {        
        if (result) {
            fprintf(out, "\nHanh trinh toi uu (%d mat) (%s): \n", best, nameTypeSolve);
            printf("\nHanh trinh toi uu (%d diem) (%s):\n ", best, nameTypeSolve);
            while(result->name!=startNode)result=result->next;      
            do {
                fprintf(out, "%c -> ", result->name);
                printf("%c -> ", result->name);
                result = result->next;
            } while (result->name != startNode);
            fprintf(out, "%c\n", result->name);
            printf("%c\n", result->name);
        }
        else printf("\nERROR: ResultWay = NULL\n");
    }
    if (out != stdout) fclose(out);
}

void kTraMTran(int weight[__MAX_N][__MAX_N], int n) {
    if (n < 3) {
            printf("Khong co loi giai do khong du dinh de tao chu trinh (n phai >= 3)\n");
            exit(1);
    }
    int i,j,deg;
    for (i = 0; i < n; i++) {
        deg = 0;
        for (j = 0; j < n; j++) {
            if (weight[i][j] != -1) deg++;
        }
        if (deg < 2) {
            printf("Khong co loi giai do dinh %d khong du bac de tao chu trinh\n", i);
            exit(1);
        }
    }
}

void solve_QDH_BMask(int weight[__MAX_N][__MAX_N], int n, char nameIndex[__MAX_N], char startNode, int *best, struct ResultWay *result) {
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
    *best = -1;
    int last = -1, total;
    fullMask--;
    for (j = 1; j < n; j++) {
        if (dp[fullMask][j] == INT_MIN || weight[j][0] <= 0) continue;
        total = dp[fullMask][j] + weight[j][0];
        if (total > *best) {
            *best = total;
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
    result->name = nameIndex[0];;
    struct ResultWay *current = result;
    int cur = last, temp;
    while (cur != 0) {
        current->next = (struct ResultWay *)malloc(sizeof(struct ResultWay));
        current = current->next;
        current->name = nameIndex[cur];
        temp = parent[mask][cur];
        mask ^= (1 << (cur - 1));
        cur = temp;
    }
    current->next = result;
    
    for (mask = 0; mask <= fullMask; mask++) {
        free(dp[mask]);
        free(parent[mask]);
    }
    free(dp);
    free(parent);
    return ;
}

// BACKTRACKING 
int visited_bt, x_bt[__MAX_N], n_bt;
int (*w_bt)[__MAX_N];
int best_bt = 0, best_path_bt[__MAX_Way][__MAX_N], count_best_bt = 0;

void Try(int i, int sum) {
    int v;
    for(v = 0; v < n_bt; v++) {
        if(!(visited_bt & (1 << v)) && w_bt[x_bt[i-1]][v] > 0) {
            visited_bt |= (1 << v);
            x_bt[i] = v;
            int new_sum = sum + w_bt[x_bt[i-1]][v];
            if(i == n_bt - 1) {
                if(w_bt[v][x_bt[0]] > 0) {
                    new_sum += w_bt[v][x_bt[0]];
                    if(new_sum > best_bt) {
                        best_bt = new_sum;
                        count_best_bt = 0;
                    }
                    if(new_sum == best_bt && count_best_bt < __MAX_Way) {
                        int j;
                        for(j = 0; j < n_bt; j++) best_path_bt[count_best_bt][j] = x_bt[j];
                        count_best_bt++;
                    }
                }
            } else Try(i + 1, new_sum);
            visited_bt ^= (1 << v);
        }
    }
}

void solve_backtracking(int n, int weight[__MAX_N][__MAX_N], int indexMap[__MAX_CHAR], char nameIndex[__MAX_N] , char startNode, char *filename) {
    n_bt = n;
    w_bt = weight;
    best_bt = 0; count_best_bt = 0;
    visited_bt = (1 << indexMap[(unsigned char)startNode]);
    x_bt[0] = indexMap[(unsigned char)startNode];

    Try(1, 0);
    FILE *out = fopen( filename,  "a");
    if(best_bt < 1) {
        fprintf(out,"Quay lui: Khong tim thay chu trinh.\n");
        printf("Quay lui: Khong tim thay chu trinh.\n");
        return;
    }
    printf("\nQuay lui: Hanh trinh toi uu (%d diem)\n", best_bt);
    fprintf(out,"\nQuay lui: Hanh trinh toi uu (%d diem)\n", best_bt);

    int i,j;
    for(i = 0; i < count_best_bt; i++) {
        for(j = 0; j < n; j++) {
            fprintf(out,"%c -> ", nameIndex[best_path_bt[i][j]]);
            printf("%c -> ", nameIndex[best_path_bt[i][j]]);
        }
        fprintf(out,"%c\n", nameIndex[best_path_bt[i][0]]);
        printf("%c\n", nameIndex[best_path_bt[i][0]]);
    }
    fclose(out);
}

//Nhanh canh
int final_cost = INF;
int best_edges[__MAX_N][2];
int current_edges[__MAX_N][2];

int reduce_matrix(int matrix[__MAX_N][__MAX_N], int n) {
    int reduction_sum = 0, i, j, min_val;
    for (i = 0; i < n; i++) {
        min_val = INF;
        for (j = 0; j < n; j++)
            if (matrix[i][j] < min_val) min_val = matrix[i][j];
        if (min_val != INF && min_val != 0) {
            for (j = 0; j < n; j++)
                if (matrix[i][j] != INF) matrix[i][j] -= min_val;
            reduction_sum += min_val;
        }
    }
    for (j = 0; j < n; j++) {
        min_val = INF;
        for (i = 0; i < n; i++)
            if (matrix[i][j] < min_val) min_val = matrix[i][j];
        if (min_val != INF && min_val != 0) {
            for (i = 0; i < n; i++)
                if (matrix[i][j] != INF) matrix[i][j] -= min_val;
            reduction_sum += min_val;
        }
    }
    return reduction_sum;
}

void find_best_edge(int matrix[__MAX_N][__MAX_N], int n, int *r, int *s) {
    int max_regret = -1, i, j, min_row, min_col, regret;
    *r = -1; *s = -1;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (matrix[i][j] == 0) {
                min_row = INF, min_col = INF;
                for (int k = 0; k < n; k++) {
                    if (k != j && matrix[i][k] < min_row) min_row = matrix[i][k];
                    if (k != i && matrix[k][j] < min_col) min_col = matrix[k][j];
                }
                regret = (min_row == INF ? 0 : min_row) + (min_col == INF ? 0 : min_col);
                if (regret > max_regret) {
                    max_regret = regret;
                    *r = i; *s = j;
                }
            }
        }
    }
}


void copyMatran(int in[__MAX_N][__MAX_N], int out[__MAX_N][__MAX_N], int n){
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) out[i][j] = in[i][j];
    return ;
}

void branch_and_bound(int matrix[__MAX_N][__MAX_N], int n, int bound, int edge_count) {
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
    find_best_edge(matrix, n, &r, &s);
    if (r == -1) return;

    // --- Nhánh 1: CHỌN cạnh (r, s) ---

    int next_matrix[__MAX_N][__MAX_N];

    copyMatran(matrix,next_matrix, n);

    for (int k = 0; k < n; k++) {
        next_matrix[r][k] = INF;
        next_matrix[k][s] = INF;
    }

    next_matrix[s][r] = INF; 
    current_edges[edge_count][0] = r;
    current_edges[edge_count][1] = s;

    int res_reduction = reduce_matrix(next_matrix, n);
    branch_and_bound(next_matrix, n, bound + res_reduction, edge_count + 1);

    // --- Nhánh 2: KHÔNG CHỌN cạnh (r, s) ---
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) next_matrix[i][j] = matrix[i][j];
    next_matrix[r][s] = INF;
    res_reduction = reduce_matrix(next_matrix, n);
    branch_and_bound(next_matrix, n, bound + res_reduction, edge_count);

}

int createMatraXuli(int C[__MAX_N][__MAX_N], int n){
    int M=-1,i,j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (C[i][j] == -1 || i == j) C[i][j] = INF;
            if(M<C[i][j])M=C[i][j];
        }
    }
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            if(C[i][j]!=INF)C[i][j]=M-C[i][j];
        }
    }
    return M;
}

void NhanhCanh(int weight[__MAX_N][__MAX_N],int n, char nameIndex[__MAX_N], char startNode , int *best, struct ResultWay* res){
    int i,j;
    int matrix[__MAX_N][__MAX_N];
    for(i=0;i<n;i++){
        for(j=0;j<n;j++)matrix[i][j]=weight[i][j];
    }
    int M;
    M = createMatraXuli(matrix, n);
    int initial_bound = reduce_matrix(matrix, n);
    branch_and_bound(matrix, n, initial_bound, 0);
    *best=final_cost>=INF?-1:M*n - final_cost;

    int cur = 0;
    struct ResultWay* cur_way = res;
    n--;
    for (i = 0; i < n; i++) {
        cur_way->name = nameIndex[cur];
        for (j = 0; j <= n; j++) {
            if (best_edges[j][0] == cur) {
                cur = best_edges[j][1];
                break;
            }
        }
        cur_way->next = (struct ResultWay*)malloc(sizeof(struct ResultWay));
        cur_way = cur_way->next;
    }
    cur_way->name = nameIndex[cur];
    cur_way->next = res;
    n++;
    return ;

}
//END TT

int main(void) {
    int weight[__MAX_N][__MAX_N], indexMap[__MAX_CHAR], n;
    char nameIndex[__MAX_N], startNode;
    struct ResultWay *result = (struct ResultWay*)malloc(sizeof(struct ResultWay));
    result->next = NULL;
    int best;

    clearDataFile(nameFileOut);
    readInput(nameFileIn, weight, indexMap, nameIndex, &n, &startNode);
    kTraMTran(weight, n);
    printMenu();

    int choice;

do {
    printf("\n============= MENU =============\n");
    printf("1. QHD + Bitmask\n");
    printf("2. Backtracking (Quay lui)\n");
    printf("3. Nhanh canh (Branch & Bound)\n");
    printf("0. Thoat\n");
    printf("================================\n");
    printf("Nhap lua chon: ");
    scanf("%d", &choice);
    LARGE_INTEGER tic, toc, freq;
    QueryPerformanceFrequency(&freq);


    switch(choice) {
        case 1: {
            printMatrix(weight,n);
            QueryPerformanceCounter(&tic);
            solve_QDH_BMask(weight, n, nameIndex, startNode, &best, result);
            QueryPerformanceCounter(&toc);
            writeResult(nameFileOut, result, best, startNode,"QDH+BitMask");
            printf("Thoi gian chinh xac: %.6f ms (mili giay)\n", (double)(toc.QuadPart - tic.QuadPart) * 1000.0 / freq.QuadPart);
            break;
            }

        case 2:
            printMatrix(weight,n);

            QueryPerformanceCounter(&tic);
            solve_backtracking(n, weight, indexMap, nameIndex, startNode, nameFileOut);
            QueryPerformanceCounter(&toc);

            printf("Thoi gian chinh xac: %.6f ms (mili giay)\n", (double)(toc.QuadPart - tic.QuadPart) * 1000.0 / freq.QuadPart);
            break;

        case 3: {
            QueryPerformanceCounter(&tic);
            NhanhCanh(weight, n, nameIndex, startNode, &best, result);
            QueryPerformanceCounter(&toc);
            writeResult(nameFileOut, result, best, startNode,"Nhanh Canh");
            printf("Thoi gian chinh xac: %.6f ms (mili giay)\n", (double)(toc.QuadPart - tic.QuadPart) * 1000.0 / freq.QuadPart);
            break;
        }

        case 0:
            printf("\nThoat chuong trinh.\n");
            break;

        default:
            printf("\nLua chon khong hop le!\n");
    }

} while (choice != 0);    
    return 0;
}
