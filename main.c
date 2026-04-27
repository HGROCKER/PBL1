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

void solve_QDH_BMask(int weight[__MAX_N][__MAX_N], int n, char nameIndex[__MAX_N], char startNode, int *best, struct ResultWay *result, int choice) {
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
    
    mask = 0;
    int countLoop = 0;
    if(choice){
        printf("\nKhoi tao bang dp[mask][i] {gia tri max qua cac dinh truong hop bit cua mask va ket thuc tai dinh i} " );
        printf("\n V e!=(0,i) thuoc dp = INF ;\n e=(0,i) thuoc dp = weight[0][i]\n");
        
        printf("\nBat dau duyet dp[mask][] voi mask tu 00..00 toi 11..11 (0 toi 2^(n-1)-1) \n");
        printf("Nhap so loop ban muon chay (so luong co the rat lon, toi da la %d):",fullMask);
        scanf("%d",&countLoop);
        if(countLoop>fullMask)countLoop=fullMask;

        mask = 0;
        for (mask; mask < countLoop; mask++) {
            int nextMask, cand;
            for (j = 1; j < n; j++) {
                if (!(mask & (1 << (j - 1))) || dp[mask][j] == INT_MIN) continue;
                printf("\nDang xet mask %d va dinh %c (gia tri dp[%d][%d] = %d)\n", mask, nameIndex[j], mask, j, dp[mask][j]);

                for (k = 1; k < n; k++) {
                    if (mask & (1 << (k - 1)) || weight[j][k] <= 0) continue;
                    printf("  Dang xet dinh %c (gia tri canh tu %c den %c la %d)\n", nameIndex[k], nameIndex[j], nameIndex[k], weight[j][k]);
                    nextMask = mask | (1 << (k - 1));
                    cand = dp[mask][j] + weight[j][k];
                    printf("Tu mask %d, dinh %c den %c: %d + %d = %d\n", mask, nameIndex[j], nameIndex[k], dp[mask][j], weight[j][k], cand);
                    if (cand > dp[nextMask][k]) {
                        dp[nextMask][k] = cand;
                        printf("Cap nhat dp[%d][%d] = %d\n", nextMask, k, dp[nextMask][k]);
                        parent[nextMask][k] = j;
                    }
                }
                printf("Ket thuc xet dinh %c cho mask %d\n", nameIndex[j], mask);
            }
            printf("\nBang dp sau khi xet xong mask %d:\n", mask);
            for (j = 0; j < n; j++) {
                if (dp[mask][j] == INT_MIN) printf(" -INF ");
                else printf("%5d ", dp[mask][j]);
            }
            printf("\n\n");
        }
    }
    
    for (mask; mask < fullMask; mask++) {
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
    if(choice){
        printf("\nSau khi tinh xong bang dp, tim duong di tot nhat tu cac dinh j ve dinh 0 (voi j tu 1 den n-1) de hoan thanh chu trinh va cap nhat ket qua vao bien best neu tim thay duong di tot hon:\n");
        for (j = 1; j < n; j++) {
            if (dp[fullMask][j] == INT_MIN || weight[j][0] <= 0) {
                printf("Dinh %c khong the tro ve dinh %c de hoan thanh chu trinh (dp[%d][%d] = %d, canh tu %c den %c = %d)\n", nameIndex[j], nameIndex[0], fullMask, j, dp[fullMask][j], nameIndex[j], nameIndex[0], weight[j][0]);
                continue;
            }
            total = dp[fullMask][j] + weight[j][0];
            printf("Duong di tu dinh %c den %c qua mask %d co gia tri: dp[%d][%d] + canh tu %c den %c = %d + %d = %d\n", nameIndex[j], nameIndex[0], fullMask, fullMask, j, dp[fullMask][j], nameIndex[j], nameIndex[0], dp[fullMask][j], weight[j][0], total);
            if (total > *best) {
                *best = total;
                last = j;
                printf("Cap nhat best = %d va last = %d\n", *best, last);
            }
        }
    }
    else{
        for (j = 1; j < n; j++) {
            if (dp[fullMask][j] == INT_MIN || weight[j][0] <= 0) continue;
            total = dp[fullMask][j] + weight[j][0];
            if (total > *best) {
                *best = total;
                last = j;
            }
        }
    }

    if(choice){
        if (*best == -1) {
            printf("\nKhong tim thay chu trinh di qua tat ca cac dinh.\n");
        } else {
            printf("\nKet qua tim duoc: best = %d, last = %d (duong di tot nhat ket thuc tai dinh %c)\n", *best, last, nameIndex[last]);
        }
    }

    if (*best == -1) {
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

    if(choice){
        printf("\nTruy vet ket qua tu last = %d va mask = %d de xay dung ket qua cuoi cung:\n", last, fullMask);
            while (cur != 0) {
                printf("Dinh %c duoc chon, parent[%d][%d] = %d (%c)\n", nameIndex[cur], mask, cur, parent[mask][cur], nameIndex[parent[mask][cur]]);
                current->next = (struct ResultWay *)malloc(sizeof(struct ResultWay));
                current = current->next;
                current->name = nameIndex[cur];
                temp = parent[mask][cur];
                mask ^= (1 << (cur - 1));
                cur = temp;
            }
            current->next = result;
            
    }
    else{
        while (cur != 0) {
            current->next = (struct ResultWay *)malloc(sizeof(struct ResultWay));
            current = current->next;
            current->name = nameIndex[cur];
            temp = parent[mask][cur];
            mask ^= (1 << (cur - 1));
            cur = temp;
        }
        current->next = result;
    }
    
    
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

void solve_backtracking(int n, int weight[__MAX_N][__MAX_N], int indexMap[__MAX_CHAR], char nameIndex[__MAX_N] , char startNode, char *filename, int choice) {
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
//Nhanh canh
int final_cost = INF;
int best_edges[__MAX_N][2];
int current_edges[__MAX_N][2];
int show_steps_bb = 0;  // Biến điều khiển in chi tiết
int step_count = 0;     // Bộ đếm bước
int max_steps_display = 0;  // Số bước tối đa cần in

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
        return;
    }

    int r, s;
    find_best_edge(matrix, n, &r, &s, nameIndex);
    if (r == -1) return;

    // --- Nhánh 1: CHỌN cạnh (r, s) ---
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
        printf("  Khoa hang %d, cot %d, va vi tri (%c -> %c):\n", r, s, nameIndex[s], nameIndex[r]);
        print_matrix_bb(next_matrix, n, "  Ma tran sau khi chon canh:");
    }

    int res_reduction = reduce_matrix(next_matrix, n);
    
    if (show_steps_bb && step_count <= max_steps_display)
        printf("  Tong giam: %d, bound moi: %d\n", res_reduction, bound + res_reduction);
    
    branch_and_bound(next_matrix, n, bound + res_reduction, edge_count + 1, nameIndex);

    // --- Nhánh 2: KHÔNG CHỌN cạnh (r, s) ---
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
        printf("\n" YELLOW "=== KET QUA CUOI CUNG ===" RESET "\n");
        printf("Chi phi toi uu: %d\n", *best);
    }

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
    
    show_steps_bb = 0;
    return;
}
//END TT
//END TT

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
        printf("Du lieu se duoc doc tu file 'data.txt' \n");
    }
    readInput(TypeRead, weight, indexMap, nameIndex, &n, &startNode);
    printf("Du lieu da duoc doc va kiem tra thanh cong. So dinh: "GREEN "%d" RESET" \n", n);
    printMatrix(weight, n);
    kTraMTran(weight, n);

    LARGE_INTEGER tic, toc, freq;
    
    do {   
        printf("\n============= MENU =============\n");
        printf("1. QHD + Bitmask (Uu tien : N<=21)\n");
        printf("2. Backtracking (Quay lui) (Uu tien : N<=12):\n");
        printf("3. Nhanh canh (Branch & Bound) (Uu tien : N<=28)\n");
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

            case 2:
                printf("\nBan co muon hien thi cac buoc giai hay khong (y[!=0]/n[0]) : ");
                scanf("%d", &choice);
                QueryPerformanceCounter(&tic);
                solve_backtracking(n, weight, indexMap, nameIndex, startNode, nameFileOut, choice);
                QueryPerformanceCounter(&toc);

                printf("Thoi gian chinh xac: %.6f ms (mili giay)\n", (double)(toc.QuadPart - tic.QuadPart) * 1000.0 / freq.QuadPart);
                break;

            case 3: {
                printf("\nBan co muon hien thi cac buoc giai hay khong (y[!=0]/n[0]) : ");
                scanf("%d", &choice);
                QueryPerformanceCounter(&tic);
                NhanhCanh(weight, n, nameIndex, startNode, &best, result, choice);
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
