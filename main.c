#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int **weight;               // ma trận trọng số
int indexMap[256];          // ánh xạ ký tự -> chỉ số
char vertexChars[256];      // danh sách ký tự theo chỉ số
int vertexCount = 0;

//Đây là hàm nhập các đỉnh mới với các chỉ số mới. VD: nhập A B B D C -> ['A','B','D','C']
// Tác dụng cuẻ indexMap : giống hashtable (chỉ số là các cụm khóa dạng string, tuy nhiên ở đây sẽ đơn giản là các char -> lưu được từ 0->255 (ví dụ a có giá trị 97))
int getIndex(char c) {
    unsigned char uc = (unsigned char)c;
    if (indexMap[uc] == -1) {
        indexMap[uc] = vertexCount;
        vertexChars[vertexCount] = c;
        vertexCount++;
    }
    return indexMap[uc];
}

int solve(int startIndex, int *outPath) {
    //startIndex là chỉ số của đỉnh bắt đầu, outPath là mảng chứa đường đi tối ưu sau khi giải xong
    int n = vertexCount;//...Count là số lượng đỉnh được tính từ các lệnh getIndex
    int fullMask = (1 << n) - 1; //n đính -> mỗi đỉnh có 2 trạng thái là đi qua hoặc chưa đi qua 
                                 //-> ánh xạ tới 1 bit -> n bít -> biêu diễn qua số nguyên thì là 2^n trường hợp (có giá trị 0 -> giá trị max = 2^n-1)
                                 // số này thể hiện sang bit thì là 11111...1 với n số 1.
    int **dp = malloc((1 << n) * sizeof(int *));  //mảng chứa giá trị lớn nhất của các trường hợp bit ví dụ 111011 thì có giá trị lớn nhất
                                                 //khi đi qua các đỉnh 1 đó(với thứ tự bất kì) đã được tìm trước đó
    int **parent = malloc((1 << n) * sizeof(int *));// Mảng truy vết, khi có một giá trị lớn nhất thì đồng thời sẽ lưu lại đỉnh đi đến hiện tại ở trước đó.
    for (int mask = 0; mask < (1 << n); mask++) { //quét qua mọi trường hơp có thể xã ra từ 00000.. -> 1111 hay từ giá trị 0->2^n-1
        dp[mask] = malloc(n * sizeof(int)); // mỗi trường hợp sẽ chứa 1 mảng n ptu
        parent[mask] = malloc(n * sizeof(int));// tương tự
        for (int i = 0; i < n; i++) {
            dp[mask][i] = INT_MIN / 2; //INT_MIN / 2  để khi cộng 2 phần tử dp[i][j] với nhau thì sẽ không bị tràn, đồng thời cũng biểu thị giá trị nhỏ nhất
                                        //thể hiện tại trường các đỉnh giá trị bit là 1 trong với giá trị nguyên mask đã được đi qua có 1 mảng của giá trị gì đây của các đỉnh
            parent[mask][i] = -1;      //thể hiện các trường hợp hiện tại chưa có đỉnh nào đi tới.
        }
    }
  
    dp[1 << startIndex][startIndex] = 0; //1 << startIndex thể hiện bit vị trí start... được bật, còn lại thì không, ý trường hợp chỉ bit bắt 
    printf("Starting DP with start index: %d\n", startIndex);
    for (int mask = 0; mask < (1 << n); mask++) { //duyet qua mọi trường hợp có thể xảy ra từ 0000.. -> 1111..
        if (!(mask & (1 << startIndex))) {mask|= (1<<startIndex - 1); continue;} //nếu trường hợp hiện tại chưa có bit startIndex thì bật nó lên và bỏ qua trường hợp này luôn vì không hợp lệ(vi du: 1110000 la chua bat bit 3 thi ta chinh len 1110111 thi loop tiep se la 1110111 [bo qua loop khong can thiet])
        for (int i = 0; i < n; i++) { //
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
    int best = solve(startIdx, path);

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
    
    int dk = 0;
    printf("Nhap 1 de xem ket qua tren console: ");
    scanf("%d", &dk);
    if(dk == 1) {
    printf("Hanh trinh toi uu (%d mat):\n", best);
    for (int i = 0; i <= n; i++) {
        printf("%c", vertexChars[path[i]]);
        if (i < n) printf(" -> ");
    }
    printf("\n");
    }
    scanf("%d", &dk);

    if (out != stdout) fclose(out);
    for (int i = 0; i < n; i++) free(weight[i]);
    free(weight);
    free(path);
    return 0;
}
