#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// -----------------------------------------------------------------------------
// Mô tả bài toán:
// Cho một đồ thị vô hướng, mỗi cạnh có trọng số (lượng mật).
// Ong bắt đầu từ một đỉnh cố định, đi qua mỗi đỉnh chính xác một lần
// và quay về điểm xuất phát sao cho tổng lượng mật thu được là tối đa.
// Đây là phiên bản tối đa của bài toán TSP (Traveling Salesman Problem).
// -----------------------------------------------------------------------------

int **weight;               // ma trận trọng số
int indexMap[256];          // ánh xạ ký tự -> chỉ số
char vertexChars[256];      // danh sách ký tự theo chỉ số
int vertexCount = 0;

int getIndex(char c) {
    unsigned char uc = (unsigned char)c;
    if (indexMap[uc] == -1) {
        indexMap[uc] = vertexCount;
        vertexChars[vertexCount] = c;
        vertexCount++;
    }
    return indexMap[uc];
}

// Hàm giải TSP bằng lập trình động bitmask, trả về tổng mật lớn nhất.
// outPath phải có kích thước ≥ vertexCount+1 để lưu đường đi (kết thúc trở về
// đỉnh bắt đầu).
int solveTSP(int startIndex, int *outPath) {
    int n = vertexCount;
    int fullMask = (1 << n) - 1;

    int **dp = malloc((1 << n) * sizeof(int *));
    int **parent = malloc((1 << n) * sizeof(int *));
    for (int mask = 0; mask < (1 << n); mask++) {
        dp[mask] = malloc(n * sizeof(int));
        parent[mask] = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            dp[mask][i] = INT_MIN / 2;
            parent[mask][i] = -1;
        }
    }

    dp[1 << startIndex][startIndex] = 0;

    for (int mask = 0; mask < (1 << n); mask++) {
        if (!(mask & (1 << startIndex))) continue;
        for (int i = 0; i < n; i++) {
            if (!(mask & (1 << i))) continue;
            if (dp[mask][i] <= INT_MIN / 4) continue;
            for (int j = 0; j < n; j++) {
                if (mask & (1 << j)) continue;
                if (weight[i][j] < 0) continue;
                int nm = mask | (1 << j);
                int val = dp[mask][i] + weight[i][j];
                if (val > dp[nm][j]) {
                    dp[nm][j] = val;
                    parent[nm][j] = i;
                }
            }
        }
    }

    int best = INT_MIN;
    int last = -1;
    for (int i = 0; i < n; i++) {
        if (i == startIndex) continue;
        if (dp[fullMask][i] <= INT_MIN / 4) continue;
        if (weight[i][startIndex] < 0) continue;
        int val = dp[fullMask][i] + weight[i][startIndex];
        if (val > best) {
            best = val;
            last = i;
        }
    }

    if (last == -1) {
        // không có chu trình Hamilton
        for (int mask = 0; mask < (1 << n); mask++) {
            free(dp[mask]);
            free(parent[mask]);
        }
        free(dp);
        free(parent);
        return INT_MIN;
    }

    // dựng lại đường đi
    int mask = fullMask;
    int idx = n;
    outPath[idx] = startIndex; // chốt vòng
    idx--;
    int cur = last;
    while (cur != startIndex) {
        outPath[idx] = cur;
        int p = parent[mask][cur];
        mask ^= (1 << cur);
        cur = p;
        idx--;
    }
    outPath[idx] = startIndex;

    for (int m = 0; m < (1 << n); m++) {
        free(dp[m]);
        free(parent[m]);
    }
    free(dp);
    free(parent);
    return best;
}

int main(void) {
    FILE *file = fopen("data.txt", "r");
    if (!file) {
        fprintf(stderr, "Không thể mở file dữ liệu\n");
        return 1;
    }

    char startNode;
    fscanf(file, " %c", &startNode);
    memset(indexMap, -1, sizeof(indexMap));

    // đọc ban đầu để xác định tất cả các đỉnh
    char u, v;
    int w;
    while (fscanf(file, " %c %c %d", &u, &v, &w) == 3) {
        getIndex(u);
        getIndex(v);
    }
    fclose(file);

    int n = vertexCount;
    // khởi ma trận trọng số
    weight = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        weight[i] = malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) weight[i][j] = -1;
    }

    file = fopen("data.txt", "r");
    fscanf(file, " %c", &startNode);
    while (fscanf(file, " %c %c %d", &u, &v, &w) == 3) {
        int iu = indexMap[(unsigned char)u];
        int iv = indexMap[(unsigned char)v];
        weight[iu][iv] = weight[iv][iu] = weight[iu][iv]>w?weight[iu][iv]:w;
    }
    fclose(file);

    int startIdx = indexMap[(unsigned char)startNode];
    int *path = malloc((n + 1) * sizeof(int));
    int best = solveTSP(startIdx, path);

    FILE *out = fopen("output.txt", "w");
    if (!out) out = stdout;

    if (best == INT_MIN) {
        fprintf(out, "Không tồn tại chu trình Hamilton từ %c\n", startNode);
    } else {
        fprintf(out, "Hành trình tối ưu (%d mật):\n", best);
        for (int i = 0; i <= n; i++) {
            fprintf(out, "%c", vertexChars[path[i]]);
            if (i < n) fprintf(out, " -> ");
        }
        fprintf(out, "\n");
    }

    if (out != stdout) fclose(out);
    for (int i = 0; i < n; i++) free(weight[i]);
    free(weight);
    free(path);
    return 0;
}
