#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define __N 256
#define MAX 100

int weight[__N][__N];       // ma trận trọng số
int indexMap[__N];          // ánh xạ ký tự -> chỉ số
char vertexChars[__N];      // danh sách ký tự theo chỉ số
int vertexCount = 0;

void getIndex(char uc) {
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

void readInput(const char *filename, char *startNode) {
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

    char u, v;
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
    fprintf(g, "Hanh trinh toi uu (%d mat):\n", best1);
    for(int i = 0; i < count_best; i++){
        for(int j = 1; j <= n1; j++){
            fprintf(g, "%c -> ", name[best_path[i][j]]);
        }
        fprintf(g, "%c\n", name[best_path[i][1]]);
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
    FILE *g = fopen("output2.txt", "w+");

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
    for (int i = 0; i < vertexCount; i++) {
        int deg = 0;
        for (int j = 0; j < vertexCount; j++) {
            if (weight[i][j] != -1) {
                deg++;
            }
        }

        if (deg<2){printf("Do thi co dinh treo hoac dinh co lap -> Khong co loi giai\n"); exit(1);}

        }
    return ;

}

int main(void) {
    //input
    char startNode;
    readInput("data.txt", (char*)&startNode);
    kTraMTran();

    //solve
    int *path = malloc(__N * sizeof(int));
    int best = solve_QDH_BMask(indexMap[startNode], path);

    //output
    writeResult("output.txt", path, vertexCount, best, startNode);

    //free memory
    free(path);

    solve_backtracking();

    return 0;
}
