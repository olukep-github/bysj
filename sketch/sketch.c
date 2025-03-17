#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <limits.h>  

#define HASH_COUNT 5  // 哈希函数数量  
#define TABLE_SIZE 10 // 哈希表大小  

// 哈希函数  
int hash1(int x) {  
    return x % TABLE_SIZE;  
}  

int hash2(int x) {  
    return (x / TABLE_SIZE) % TABLE_SIZE;  
}  

typedef struct {  
    int counts[HASH_COUNT][TABLE_SIZE]; // 计数矩阵  
} CountSketch;  

void initCountSketch(CountSketch *cs) {  
    memset(cs->counts, 0, sizeof(cs->counts));  
}  

void insert(CountSketch *cs, int value) {  
    for (int i = 0; i < HASH_COUNT; i++) {  
        int index = (i % 2 == 0) ? hash1(value) : hash2(value); // 交替使用哈希函数  
        if (i % 2 == 0) {  
            cs->counts[i][index]++; // 正向计数  
        } else {  
            cs->counts[i][index]--; // 反向计数  
        }  
    }  
}  

int estimateFrequency(CountSketch *cs, int value) {  
    int min_count = INT_MAX;  
    for (int i = 0; i < HASH_COUNT; i++) {  
        int index = (i % 2 == 0) ? hash1(value) : hash2(value);  
        if (i % 2 == 0) {  
            min_count = (cs->counts[i][index] < min_count) ? cs->counts[i][index] : min_count;  
        } else {  
            min_count = (cs->counts[i][index] > min_count) ? cs->counts[i][index] : min_count;  
        }  
    }  
    return min_count; // 返回估计的频率  
}  

int main() {  
    CountSketch cs;  
    initCountSketch(&cs);  
	int i;
    // 插入一些测试数据  
	while((scanf("%d", &i)) != EOF){
		insert(&cs, i);
	}
    
    // 查询频率  
    printf("Estimated frequency of 5: %d\n", estimateFrequency(&cs, 5));  
    printf("Estimated frequency of 3: %d\n", estimateFrequency(&cs, 3));  
    printf("Estimated frequency of 8: %d\n", estimateFrequency(&cs, 8));  

    return 0;  
}  