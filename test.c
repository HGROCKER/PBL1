#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <windows.h>
#define __MAX_N 25
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
    printf(CYAN " =============================================================\n" RESET);
    printf(GREEN "| %-61s|\n" RESET, "TRUONG DAI HOC BACH KHOA - DAI HOC DA NANG");
    printf(GREEN "| %-61s|\n" RESET, "KHOA: CONG NGHE THONG TIN");

    printf(YELLOW "| %-61s|\n" RESET, "PBL1: DO AN LAP TRINH TINH TOAN");
    printf(YELLOW "| %-61s|\n" RESET, "DE TAI: BAI TOAN ONG TIM MAT");

    printf(BLUE "| %-61s|\n" RESET, "GIAO VIEN HUONG DAN: TS. NGUYEN VAN HIEU");

    printf(RED "| %-61s|\n" RESET, "SINH VIEN: TRAN VAN KHANH HUNG - 25T_KHDL");
    printf(RED "| %-61s|\n" RESET, "           PHAM PHU HUNG - 25T_KHDL");

    printf(CYAN " =============================================================\n\n" RESET);
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

void readInput(FILE *file, int weight[__MAX_N][__MAX_N], int indexMap[__MAX_CHAR], char nameIndex[__MAX_N], int *n, char *startNode) {
    int i,j;

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
        if(u == '?');
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
    if(file != stdin){
        fclose(file);
    }
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
            printf("Khong co loi giai do khong du dinh de tao chu trinh (n phai  >= 3)\n");
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

int show_steps_qhd = 0;
int step_count_qhd = 0;
int max_steps_qhd = 0;

void print_dp_table(int **dp, int fullMask, int n, const char *title) {
    if (!show_steps_qhd || step_count_qhd > max_steps_qhd) return;
    printf("%s\n", title);
    printf("    ");
    for (int j = 0; j < n; j++) printf("%5d ", j);
    printf("\n");
    for (int mask = 0; mask <= fullMask; mask++) {
        printf("%3d:", mask);
        for (int j = 0; j < n; j++) {
            if (dp[mask][j] == INT_MIN) printf(" -INF ");
            else printf("%5d ", dp[mask][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void solve_QDH_BMask(int weight[__MAX_N][__MAX_N], int n, char nameIndex[__MAX_N], char startNode, int *best, struct ResultWay *result, int choice) {
    show_steps_qhd = choice;
    step_count_qhd = 0;
    max_steps_qhd = 0;
    
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
            dp[1 << (j - 1)][j] = weight[0][j];
            parent[1 << (j - 1)][j] = 0;
        }
    }
    if (show_steps_qhd) {
        printf("\n" YELLOW "=== PHUONG PHAP QHD + BITMASK ===" RESET "\n");
        printf("Nhap so buoc toi da muon hien thi (0 = hien thi tat ca): ");
        scanf("%d", &max_steps_qhd);
        if (max_steps_qhd == 0) max_steps_qhd = INF;
        
        printf("\nBuoc khoi tao: Dat dp[1<<(j-1)][j] = weight[0][j] cho cac dinh j tu 1 den %d\n", n - 1);
        printf("Cac canh khoi tao:\n");
        for (j = 1; j < n; j++) {
            if (weight[0][j] > 0) {
                printf("  Canh tu dinh 0 (%c) den dinh %d (%c): %d\n", nameIndex[0], j, nameIndex[j], weight[0][j]);
            }
        }
        print_dp_table(dp, fullMask - 1, n, "Bang dp sau khoi tao:");
        step_count_qhd++;
    }
    
    int countLoop = 0;
    if (show_steps_qhd) {
        printf("Tong so mask can tinh: %d (tu 1 den %d)\n", fullMask - 1, fullMask - 1);
        printf("Nhap so mask ban muon hien thi (0 = hien thi tat ca): ");
        scanf("%d", &countLoop);
        if (countLoop > fullMask - 1) countLoop = fullMask - 1;
        if (countLoop == 0) countLoop = fullMask - 1;
    } else {
        countLoop = fullMask;
    }
    
    for (mask = 1; mask < fullMask; mask++) {
        int nextMask, cand;
        
        if (show_steps_qhd && step_count_qhd <= max_steps_qhd && mask <= countLoop) {
            printf("\nBuoc %d: Tinh toan cho mask = %d (", step_count_qhd, mask);
            for (int b = n - 2; b >= 0; b--) {
                if (mask & (1 << b)) printf("1");
                else printf("0");
            }
            printf(")\n");
            step_count_qhd++;
        }
        
        for (j = 1; j < n; j++) {
            if (!(mask & (1 << (j - 1))) || dp[mask][j] == INT_MIN) continue;
            
            if (show_steps_qhd && step_count_qhd <= max_steps_qhd && mask <= countLoop) {
                printf("  Xet dinh %d (%c) voi dp[%d][%d] = %d\n", j, nameIndex[j], mask, j, dp[mask][j]);
            }
            
            for (k = 1; k < n; k++) {
                if (mask & (1 << (k - 1)) || weight[j][k] <= 0) continue;
                
                nextMask = mask | (1 << (k - 1));
                cand = dp[mask][j] + weight[j][k];
                
                if (show_steps_qhd && step_count_qhd <= max_steps_qhd && mask <= countLoop) {
                    printf("    -> Dinh %d (%c): dp[%d][%d] + canh(%c->%c) = %d + %d = %d",
                        k, nameIndex[k], mask, j, nameIndex[j], nameIndex[k], dp[mask][j], weight[j][k], cand);
                }
                
                if (cand > dp[nextMask][k]) {

                 dp[nextMask][k] = cand;

                parent[nextMask][k] = j;

                if (show_steps_qhd &&
                step_count_qhd <= max_steps_qhd &&
                mask <= countLoop) {

                printf(" [CAP NHAT]\n");

                printf("       dp[%d][%c] = %d\n",
                nextMask,
                nameIndex[k],
                cand);
                printf("       parent[%d][%c] = %c\n",
                nextMask,
                nameIndex[k],
                nameIndex[j]);
                }
                }
                 else {
                    if (show_steps_qhd && step_count_qhd <= max_steps_qhd && mask <= countLoop) {
                        printf(" [KHONG CAP NHAT]\n");
                    }
                }
            }
        }
        
        if (show_steps_qhd && step_count_qhd <= max_steps_qhd && mask <= countLoop) {
            printf("  Ket thuc mask %d\n", mask);
        }
    }

    *best = -1;
    int last = -1, total;
    fullMask--;
    
    if (show_steps_qhd) {
        printf("\n" YELLOW "=== TIM DUONG DI TOI UU ===" RESET "\n");
        printf("Xet toan bo cac duong di ket thuc tai dinh j va quay ve dinh 0:\n");
    }
    
    for (j = 1; j < n; j++) {
        if (dp[fullMask][j] == INT_MIN || weight[j][0] <= 0) {
            if (show_steps_qhd) {
                printf("Dinh %d (%c): Khong the quay ve diem xuat phat (dp[%d][%d] = %d)\n",
                    j, nameIndex[j], fullMask, j, dp[fullMask][j]);
            }
            continue;
        }
        
        total = dp[fullMask][j] + weight[j][0];
        
        if (show_steps_qhd) {
            printf("Dinh %d (%c): dp[%d][%d] + canh(%c->%c) = %d + %d = %d",
                j, nameIndex[j], fullMask, j, nameIndex[j], nameIndex[0], dp[fullMask][j], weight[j][0], total);
        }
        
        if (total > *best) {
            *best = total;
            last = j;
            if (show_steps_qhd) printf(" [TOI UU]\n");
        } else {
            if (show_steps_qhd) printf("\n");
        }
    }

    if (show_steps_qhd) {
        printf("\nKet qua: best = %d, last = %d (%c)\n", *best, last, last >= 0 ? nameIndex[last] : '?');
    }

    if (*best == -1) {
        for (mask = 0; mask <= fullMask; mask++) {
            free(dp[mask]);
            free(parent[mask]);
        }
        free(dp);
        free(parent);
        return;
    }

    mask = fullMask;

    int path[__MAX_N];
    int cnt = 0;

    int cur = last;
    int temp;

    if (show_steps_qhd) {
    printf("\n=== TRUY VET KET QUA ===\n");
    printf("Bat dau tu dinh %d (%c), mask = %d\n", last, nameIndex[last], fullMask);
    }

    while (cur != 0 && cur != -1) {

    if (show_steps_qhd) {
        printf("Dinh %d (%c) <- parent[%d][%d] = %d (%c)\n",
            cur,
            nameIndex[cur],
            mask,
            cur,
            parent[mask][cur],
            nameIndex[parent[mask][cur]]);
    }

    path[cnt++] = cur;
    temp = parent[mask][cur];
    mask ^= (1 << (cur - 1));
    cur = temp;
     }
    result->name = nameIndex[0];

    struct ResultWay *current = result;

    for (int i = cnt - 1; i >= 0; i--) {

    current->next = (struct ResultWay *)malloc(sizeof(struct ResultWay));

    current = current->next;

    current->name = nameIndex[path[i]];
    }

    current->next = result;

    if (show_steps_qhd) {
    printf("\nTruy vet hoan tat!\n");
    }
    for (mask = 0; mask <= fullMask; mask++) {
        free(dp[mask]);
        free(parent[mask]);
    }
    free(dp);
    free(parent);
    show_steps_qhd = 0;
    return;
}

int final_cost = INF;
int best_edges[__MAX_N][2];
int current_edges[__MAX_N][2];
int show_steps_bb = 0;  
int step_count = 0;     
int max_steps_display = 0; 

void print_matrix_bb(int matrix[__MAX_N][__MAX_N], int n, const char *title) {
    if (!show_steps_bb || step_count > max_steps_display) return;
    printf("%s\n", title);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (matrix[i][j] == INF) printf("  INF ");
            else printf("%5d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int reduce_matrix(int matrix[__MAX_N][__MAX_N], int n) {
    int reduction_sum = 0, i, j, min_val;
    for (i = 0; i < n; i++) {
        min_val = INF;
        for (j = 0; j < n; j++)
            if (matrix[i][j] < min_val) min_val = matrix[i][j];
        if (min_val != INF && min_val != 0) {
            for (j = 0; j < n; j++)
                if (matrix[i][j] != INF) matrix[i][j] -= min_val;
            if (show_steps_bb && step_count <= max_steps_display)
                printf("    Giam hang %d (tru %d)\n", i, min_val);
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
            if (show_steps_bb && step_count <= max_steps_display)
                printf("    Giam cot %d (tru %d)\n", j, min_val);
            reduction_sum += min_val;
        }
    }
    return reduction_sum;
}

void find_best_edge(int matrix[__MAX_N][__MAX_N], int n, int *r, int *s, char nameIndex[__MAX_N]) {
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
                if (show_steps_bb && step_count <= max_steps_display) {
                printf("    Zero (%c -> %c): regret = %d\n",  nameIndex[i],nameIndex[j],regret);         
}
                if (regret > max_regret) {
                    max_regret = regret;
                    *r = i; *s = j;
                }
            }
        }
    }
    if (show_steps_bb && step_count <= max_steps_display && *r != -1)
        printf("  Canh tot nhat: (%c -> %c) voi regret = %d\n", nameIndex[*r], nameIndex[*s], max_regret);
}

void copyMatran(int in[__MAX_N][__MAX_N], int out[__MAX_N][__MAX_N], int n){
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) out[i][j] = in[i][j];
    return ;
}

void branch_and_bound(int matrix[__MAX_N][__MAX_N], int n, int bound, int edge_count, char nameIndex[__MAX_N]) {
    if (bound >= final_cost) {
        if (show_steps_bb && step_count <= max_steps_display)
            printf("  Cat nhanh: bound (%d) >= final_cost (%d)\n", bound, final_cost);
        return;
    }
    
    if (edge_count == n) {
        if (show_steps_bb && step_count <= max_steps_display)
            printf("  Tim thay giai phap: final_cost = %d\n\n", bound);
        final_cost = bound;
        for (int i = 0; i < n; i++) {
            best_edges[i][0] = current_edges[i][0];
            best_edges[i][1] = current_edges[i][1];
        }
        if (show_steps_bb && step_count <= max_steps_display) {

        printf("  best_edges:\n");

        for (int t = 0; t < n; t++) {

        printf("    best_edges[%d][0] = %d, [1] = %d  => (%c -> %c)\n",
            t,
            best_edges[t][0],
            best_edges[t][1],
            nameIndex[best_edges[t][0]],
            nameIndex[best_edges[t][1]]);
        }
        }
        return;
    }

    int r, s;
    find_best_edge(matrix, n, &r, &s, nameIndex);
    if (r == -1) return;

    if (show_steps_bb && step_count <= max_steps_display) {
        printf("\nBuoc %d: CHON canh (%c -> %c)\n", step_count + 1, nameIndex[r], nameIndex[s]);
        step_count++;
    }

    int next_matrix[__MAX_N][__MAX_N];
    copyMatran(matrix, next_matrix, n);

    for (int k = 0; k < n; k++) {
        next_matrix[r][k] = INF;
        next_matrix[k][s] = INF;
    }

    next_matrix[s][r] = INF; 
    current_edges[edge_count][0] = r;
    current_edges[edge_count][1] = s;
    if (show_steps_bb && step_count <= max_steps_display) {

    printf("  current_edges:\n");

    for (int t = 0; t <= edge_count; t++) {

        printf("    current_edges[%d][0] = %d, [1] = %d  => (%c -> %c)\n",
            t,
            current_edges[t][0],
            current_edges[t][1],
            nameIndex[current_edges[t][0]],
            nameIndex[current_edges[t][1]]);
    }
    }
    if (show_steps_bb && step_count <= max_steps_display) {

    printf("  Tap canh hien tai: ");

    for (int t = 0; t <= edge_count; t++) {

        printf("(%c -> %c) ",
            nameIndex[current_edges[t][0]],
            nameIndex[current_edges[t][1]]);
    }

    printf("\n");
    }
    if (show_steps_bb && step_count <= max_steps_display) {
        printf("  Khoa hang %d, cot %d, va vi tri (%c -> %c):\n", r, s, nameIndex[s], nameIndex[r]);
        print_matrix_bb(next_matrix, n, "  Ma tran sau khi chon canh:");
    }

    int res_reduction = reduce_matrix(next_matrix, n);

    
    
    if (show_steps_bb && step_count <= max_steps_display)
        printf("  Tong giam: %d, bound moi: %d\n", res_reduction, bound + res_reduction);
    
    branch_and_bound(next_matrix, n, bound + res_reduction, edge_count + 1, nameIndex);

    if (show_steps_bb && step_count <= max_steps_display) {
        printf("\nBuoc %d: KHONG CHON canh (%c -> %c)\n", step_count + 1, nameIndex[r], nameIndex[s]);
        step_count++;
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) next_matrix[i][j] = matrix[i][j];
    next_matrix[r][s] = INF;

    if (show_steps_bb && step_count <= max_steps_display) {
        printf("  Khoa canh (%c -> %c):\n", nameIndex[r], nameIndex[s]);
        print_matrix_bb(next_matrix, n, "  Ma tran sau khi khong chon canh:");
    }

    res_reduction = reduce_matrix(next_matrix, n);
    
    if (show_steps_bb && step_count <= max_steps_display)
        printf("  Tong giam: %d, bound moi: %d\n", res_reduction, bound + res_reduction);

    branch_and_bound(next_matrix, n, bound + res_reduction, edge_count, nameIndex);
}
int createMatraXuli(int C[__MAX_N][__MAX_N], int n){
    int M=-1,i,j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if(M<C[i][j])M=C[i][j];
            if (C[i][j] == -1 || i == j) C[i][j] = INF;
        }
    }
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            if(C[i][j]!=INF)C[i][j]=M-C[i][j];
        }
    }
    return M;
}
void NhanhCanh(int weight[__MAX_N][__MAX_N], int n, char nameIndex[__MAX_N], char startNode, int *best, struct ResultWay* res, int choice){
    int i, j;
    
    show_steps_bb = choice;
    step_count = 0;
    max_steps_display = 0;
    
    if (show_steps_bb) {
        printf("\n" YELLOW "=== PHUONG PHAP NHANH CANH (BRANCH & BOUND) ===" RESET "\n");
        printf("Nhap so buoc toi da muon hien thi (0 = hien thi tat ca): ");
        scanf("%d", &max_steps_display);
        if(max_steps_display == 0) max_steps_display = INF;
        printf("\nKhoi tao ma tran chi phi (duoi dang cuc dai):\n");
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                if (weight[i][j] == -1) printf("  INF ");
                else printf("%5d ", weight[i][j]);
            }
            printf("\n");
        }
    }
    
    int matrix[__MAX_N][__MAX_N];
    for(i = 0; i < n; i++){
        for(j = 0; j < n; j++) matrix[i][j] = weight[i][j];
    }
    int M = createMatraXuli(matrix, n);
    if (show_steps_bb) {
        printf("\nSau khi chuan hoa (dao bai toan - M tru toan bo chi phi voi M la maximum chi phi): %d\n", M);
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                if (matrix[i][j] == INF) printf("  INF ");
                else printf("%5d ", matrix[i][j]);
            }
            printf("\n");
        }
        printf("\nTien hanh giam hang va cot:\n");
    }
    int initial_bound = reduce_matrix(matrix, n);
    if (show_steps_bb)
        printf("Bound ban dau: %d\n\n", initial_bound);

    final_cost = INF;
    step_count = 0;
    branch_and_bound(matrix, n, initial_bound, 0, nameIndex);
    
    *best = final_cost >= INF ? -1 : M * n - final_cost;
    if (show_steps_bb) {
    printf("best = M * n - final_cost\n");
    printf("     = %d * %d - %d\n",
           M,
           n,
           final_cost);
    printf("     = %d\n",
           *best);
    }
    if (show_steps_bb) {
        printf("\n" YELLOW "=== KET QUA CUOI CUNG ===" RESET "\n");
        printf("Chi phi toi uu: %d\n", *best);
    }
    int cur = 0;
    struct ResultWay* cur_way = res;
    if (show_steps_bb) {

    printf("\n========== TRUY VET CHU TRINH ==========\n");

    printf("Bat dau tu dinh: %d (%c)\n", cur, nameIndex[cur]);

    printf("Su dung best_edges:\n");

    for (int t = 0; t < n; t++) {

        printf("best_edges[%d] = (%d -> %d) (%c -> %c)\n",
            t,
            best_edges[t][0],
            best_edges[t][1],
            nameIndex[best_edges[t][0]],
            nameIndex[best_edges[t][1]]);
    }

    printf("\n");
    }
    n--;
    for (i = 0; i < n; i++) {
        if (show_steps_bb) {
        printf("\nDang tai dinh %d (%c)\n", cur, nameIndex[cur]);
        printf("Duyet best_edges de tim canh bat dau tu %d\n", cur);
        }
        cur_way->name = nameIndex[cur];
        for (j = 0; j <= n; j++) {
        if (show_steps_bb) {
        printf("  Xet best_edges[%d] = (%d -> %d)\n",j, best_edges[j][0],best_edges[j][1]);
        }
        if (best_edges[j][0] == cur) {
           if (show_steps_bb) {
           printf("  --> MATCH: (%d -> %d)\n", best_edges[j][0],best_edges[j][1]);
           }
        cur = best_edges[j][1];
           if (show_steps_bb) {
           printf("  Di chuyen sang %d (%c)\n", cur, nameIndex[cur]);
           }
        break;
        }
        }
        cur_way->next = (struct ResultWay*)malloc(sizeof(struct ResultWay));
        cur_way = cur_way->next;
    }
    cur_way->name = nameIndex[cur];
    cur_way->next = res;
    n++;

    show_steps_bb = 0;
    return;
}
void printFileContent(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Khong mo duoc file %s\n", filename);
        return;
    }
    char ch;
    printf("\n===== NOI DUNG FILE %s =====\n", filename);
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }
    printf("\n============================\n");
    fclose(file);
}
int main(void) {
    printMenu();
    int weight[__MAX_N][__MAX_N], indexMap[__MAX_CHAR], n;
    char nameIndex[__MAX_N], startNode;
    struct ResultWay *result = (struct ResultWay*)malloc(sizeof(struct ResultWay));
    result->next = NULL;
    int best;
    int choice;
    FILE *TypeRead;
    clearDataFile(nameFileOut);
    printf("Ban muon nhap tu ban phim (0) hay nhap tu file (!=0): ");
    scanf("%d", &choice);
    if(choice == 0) {
        TypeRead = stdin;
        printf("Du lieu se duoc doc tu ban phim (Voi window thi lap lai 2 lan (nhan ctrl + z -> enter) de thoat loop input [thay ctrl+z thanh ctrl + d voi linux(mac)]) \n");
    }
    else {
    TypeRead = fopen(nameFileIn, "r");

    if (TypeRead == NULL) {
        printf("Khong mo duoc file data.txt\n");
        return 1;
    }
    printf("Du lieu se duoc doc tu file 'data.txt' \n");

    printFileContent(nameFileIn);
    }
    readInput(TypeRead, weight, indexMap, nameIndex, &n, &startNode);
    printf("Du lieu da duoc doc va kiem tra thanh cong. So dinh: "GREEN "%d" RESET" \n", n);
    printMatrix(weight, n);
    kTraMTran(weight, n);

    LARGE_INTEGER tic, toc, freq;
    
    do {   
        printf("\n============= MENU =============\n");
        printf("1. QHD + Bitmask (Uu tien : N<=21)\n");
        printf("2. Nhanh canh (Branch & Bound) (Uu tien : N<=28)\n");
        printf("0. Thoat\n");
        printf("================================\n");
        printf("Nhap lua chon: ");
        scanf("%d", &choice);
        QueryPerformanceFrequency(&freq);

        switch(choice) {
            case 1: {
                printf("\nBan co muon hien thi cac buoc giai hay khong (y[!=0]/n[0]) : ");
                scanf("%d", &choice);
                QueryPerformanceCounter(&tic);
                solve_QDH_BMask(weight, n, nameIndex, startNode, &best, result, choice);
                QueryPerformanceCounter(&toc);
                writeResult(nameFileOut, result, best, startNode,"QDH+BitMask");
                printf("Thoi gian chinh xac: %.6f ms (mili giay)\n", (double)(toc.QuadPart - tic.QuadPart) * 1000.0 / freq.QuadPart);
                break;
                }
            case 2: {
                printf("\nBan co muon hien thi cac buoc giai hay khong (y[!=0]/n[0]) : ");
                scanf("%d", &choice);
                QueryPerformanceCounter(&tic);
                NhanhCanh(weight, n, nameIndex, startNode, &best, result, choice);
                QueryPerformanceCounter(&toc);
                writeResult(nameFileOut, result, best, startNode,"Nhanh Canh");
                printf("Thoi gian chinh xac: %.6f ms (mili giay)\n",
                     (double)(toc.QuadPart - tic.QuadPart) * 1000.0 / freq.QuadPart);
                break;
                }

            case 0:
                printf("\nThoat chuong trinh.\n");
                break;

            default:
                printf("\nLua chon khong hop le!\n");
        }
        printf("\nBan co muon thuc hien them phuong phap gia khac khong (y[!=0]/n[0]): ");
        scanf("%d",&choice);
        if(!choice){
            printf("\nThoat chuong trinh.\n");
            break;
        }
    }
    while(choice);
    return 0;
}
