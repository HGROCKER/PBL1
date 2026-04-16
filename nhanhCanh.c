#include <stdio.h>
#include <stdlib.h>

#define INF 1e9
#define MAX 10

int n;
int final_cost = INF;
int best_edges[MAX][2]; // Lưu danh sách các cạnh của chu trình tốt nhất

typedef struct {
    int u, v;
} Edge;

Edge current_edges[MAX]; // Lưu các cạnh đang chọn trong nhánh hiện tại

// Bước 1: Rút gọn ma trận và tính cận dưới
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

// Bước 2: Tìm cạnh phân nhánh có chi phí hối tiếc lớn nhất
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

// Hàm in chu trình từ danh sách các cạnh rời rạc
void print_cycle() {
    int start_node = best_edges[0][0];
    int current_node = start_node;
    printf("Chu trinh: ");
    for (int i = 0; i < n; i++) {
        printf("%d -> ", current_node + 1);
        for (int j = 0; j < n; j++) {
            if (best_edges[j][0] == current_node) {
                current_node = best_edges[j][1];
                break;
            }
        }
    }
    printf("%d\n", start_node + 1);
}

// Bước 3 & 4: Phân nhánh và Đệ quy
void branch_and_bound(int matrix[MAX][MAX], int bound, int edge_count) {
    if (bound >= final_cost) return;

    if (edge_count == n) {
        final_cost = bound;
        for (int i = 0; i < n; i++) {
            best_edges[i][0] = current_edges[i].u;
            best_edges[i][1] = current_edges[i].v;
        }
        return;
    }

    int r, s;
    find_best_edge(matrix, &r, &s);
    if (r == -1) return;

    // --- Nhánh 1: CHỌN cạnh (r, s) ---
    int next_matrix[MAX][MAX];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) next_matrix[i][j] = matrix[i][j];

    for (int k = 0; k < n; k++) {
        next_matrix[r][k] = INF;
        next_matrix[k][s] = INF;
    }
    next_matrix[s][r] = INF; // Chống chu trình ngược 2 điểm

    current_edges[edge_count].u = r;
    current_edges[edge_count].v = s;

    int res_reduction = reduce_matrix(next_matrix);
    branch_and_bound(next_matrix, bound + res_reduction, edge_count + 1);

    // --- Nhánh 2: KHÔNG CHỌN cạnh (r, s) ---
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) next_matrix[i][j] = matrix[i][j];
    
    next_matrix[r][s] = INF;
    res_reduction = reduce_matrix(next_matrix);
    branch_and_bound(next_matrix, bound + res_reduction, edge_count);
}

int main() {
    int original_matrix[MAX][MAX];
    printf("--- Giai Bai Toan Nguoi Du Lich (TSP - Branch & Bound) ---\n");
    printf("Nhap so thanh pho: ");
    scanf("%d", &n);

    int M=-1;
    printf("Nhap ma tran chi phi (Nhap -1 cho cac duong cheo hoac o khong co duong di):\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%d", &original_matrix[i][j]);
            if (original_matrix[i][j] == -1 || i == j) original_matrix[i][j] = INF;
            if(M<original_matrix[i][j])M=original_matrix[i][j];
        }
    }
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            if(original_matrix[i][j]!=INF)original_matrix[i][j]=M-original_matrix[i][j];
        }
    }

    // Tạo bản sao ma trận để tính toán
    int matrix[MAX][MAX];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) matrix[i][j] = original_matrix[i][j];

    int initial_bound = reduce_matrix(matrix);
    branch_and_bound(matrix, initial_bound, 0);

    if (final_cost >= INF) {
        printf("\nKhong tim thay hanh trinh.\n");
    } else {
        printf("\n====================================\n");
        printf("Ket qua toi uu:\n");
        printf("Tong chi phi (Best): %d\n",M*n - final_cost);
        print_cycle();
        printf("====================================\n");
    }

    return 0;
}
