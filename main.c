#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define __N 256


int weight[__N][__N];       // ma trận trọng số
int indexMap[__N];          // ánh xạ ký tự -> chỉ số
char vertexChars[__N];      // danh sách ký tự theo chỉ số
int vertexCount = 0;

void getIndex(unsigned char uc) {
    if (indexMap[uc] == -1) {
        indexMap[uc] = vertexCount;
        vertexChars[vertexCount] = uc;
        vertexCount++;
    }
    return ;
}

void wait() {
    printf("\nPress any key to continue...\n");
    getchar();
}

void showResultConsole(int *path, int length, int best) {
    printf("Hanh trinh toi uu (%d mat):\n", best);
    for (int i = 0; i <= length; i++) {
        printf("%c", vertexChars[path[i]]);
        if (i < length) printf(" -> ");
    }
    printf("\n");
}

void writeResult(const char *filename, int *path, int length, int best, char startNode) {
    FILE *out = fopen(filename, "w");
    if (!out) out = stdout;

    if (best == INT_MIN) {
        fprintf(out, "Không tồn tại chu trình Hamilton từ %c\n", startNode);
    } else {
        fprintf(out, "Hành trình tối ưu (%d mật):\n", best);
        for (int i = 0; i <= vertexCount; i++) {
            fprintf(out, "%c", vertexChars[path[i]]);
            if (i < vertexCount) fprintf(out, " -> ");
        }
        fprintf(out, "\n");
    }

    if (out != stdout) fclose(out);
    
    int dk = 0;
    printf("Nhap 1 de xem ket qua tren console: ");
    scanf("%d", &dk);
    if(dk == 1) {
        showResultConsole(path, vertexCount, best);
    }
    wait();
}

void readInput(const char *filename, unsigned char *startNode) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Không thể mở file dữ liệu\n");
        fclose(file);
        exit(1);
    }

    fscanf(file, " %c", startNode);
    getIndex(*startNode);
    memset(weight, -1, sizeof(weight));
    memset(indexMap, -1, sizeof(indexMap));

    unsigned char u, v;
    int w;

    while (fscanf(file, " %c %c %d", &u, &v, &w) == 3) {
        getIndex(u);
        getIndex(v);
        int iu = indexMap[u];
        int iv = indexMap[v];
        weight[iu][iv] = weight[iv][iu] = weight[iu][iv]>w?weight[iu][iv]:w;
    }
    fclose(file);
}

int solve_QDH_BMask(int startIndex, int *outPath) {
    int n = vertexCount;
    int fullMask = (1 << n) - 1;
    int **dp = malloc((1 << n) * sizeof(int *));
    int **parent = malloc((1 << n) * sizeof(int *));
    int mask;
    for (mask = 0; mask < (1 << n); mask++) {
        dp[mask] = malloc(n * sizeof(int));
        parent[mask] = malloc(n * sizeof(int));
        memset(dp[mask], INT_MIN, n * sizeof(int));
        memset(parent[mask], -1, n * sizeof(int));
    }

    dp[1 << startIndex][startIndex] = 0;

    for (mask = 1 << startIndex ; mask < (1 << n); mask++) {

        if (!(mask & (1 << startIndex))) {mask|= (1<<startIndex - 1); continue;} 

        for (int i = 0; i < n; i++) { 
            if (!(mask & (1 << i))) continue; 
            if (dp[mask][i] == INT_MIN) continue;  
            for (int j = 0; j < n; j++) {
                if (mask & (1 << j) || weight[i][j] < 0) continue;
                int valNew = dp[mask][i] + weight[i][j];
                int maskNew = mask | (1 << j);
                if (valNew > dp[maskNew][j]) { 
                    dp[maskNew][j] = valNew;
                    parent[maskNew][j] = i; 
                }
            }

        }

    }

    int best = INT_MIN;
    int last = -1;
     
    for (int i = 0; i < n; i++) {
        if (dp[fullMask][i] == INT_MIN || weight[i][startIndex] < 0 ) continue;
        int val = dp[fullMask][i] + weight[i][startIndex];
        if (val > best) {
            best = val;
            last = i;
        }
    }

    if (last == -1) {
        for (int mask = 0; mask < (1 << n); mask++) {
            free(dp[mask]);
            free(parent[mask]);
        }
        free(dp);
        free(parent);
        return INT_MIN;
    }

    mask = fullMask;
    int idx = n;
    outPath[idx] = startIndex;
    idx--;
    int x = last;
    
    while (x != startIndex) {
        outPath[idx--] = x;
        int p = parent[mask][x]; 
        mask ^= (1 << x); // tắt đỉnh hiện tại (x) tạo trường hợp tiền đề trước đs
        x = p; //gán đỉnh đi đến đây trong truong hop hiẹn tại
    }
    outPath[idx] = startIndex; // điểm bắt đầu là startIndex (kết thúc cũng là nó, thường thì thúc này đang gán cho outPath[0])

    for (int m = 0; m < (1 << n); m++) {//giai phong bo nho
        free(dp[m]);
        free(parent[m]);
    }
    free(dp);
    
    free(parent);
    return best;
}
//END TT

//Thuat toan cay khung nho (lon) nhat + ...


//END TT


int main(void) {
    //input
    unsigned char startNode;
    readInput("data.txt", (unsigned char*)&startNode);

    //solve
    int *path = malloc(__N * sizeof(int));
    int best = solve_QDH_BMask(indexMap[startNode], path);

    //output
    writeResult("output.txt", path, vertexCount, best, startNode);

    //free memory
    free(path);
    return 0;
}
