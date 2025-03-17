#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <arpa/inet.h> // 用于 IP 地址转换

#define HASH_COUNT 5  // 哈希函数数量  
#define TABLE_SIZE 1000 // 哈希表大小（适当加大，减少冲突）

// 哈希函数
int hash1(uint32_t x) {  
    return x % TABLE_SIZE;  
}  

int hash2(uint32_t x) {  
    return (x / TABLE_SIZE) % TABLE_SIZE;  
}  

// 计数矩阵
typedef struct {  
    int counts[HASH_COUNT][TABLE_SIZE]; // 计数矩阵  
} CountSketch;  

// 初始化 CountSketch
void initCountSketch(CountSketch *cs) {  
    memset(cs->counts, 0, sizeof(cs->counts));  
}  

// 将 (IP, 端口) 组合转换为一个唯一整数
uint32_t combineIPPort(const char *ip, uint16_t port) {
    struct in_addr addr;
    if (inet_pton(AF_INET, ip, &addr) != 1) {
        fprintf(stderr, "Invalid IP address: %s\n", ip);
        return 0;
    }
    return (addr.s_addr ^ port);  // 简单哈希组合（可以换成更好的哈希方式）
}

// 插入数据 (IP, 端口)
void insert(CountSketch *cs, const char *ip, uint16_t port) {  
    uint32_t value = combineIPPort(ip, port);
    for (int i = 0; i < HASH_COUNT; i++) {  
        int index = (i % 2 == 0) ? hash1(value) : hash2(value); // 交替使用哈希函数  
        if (i % 2 == 0) {  
            cs->counts[i][index]++; // 正向计数  
        } else {  
            cs->counts[i][index]--; // 反向计数  
        }  
    }  
}

// 估算 (IP, 端口) 组合的频率
int estimateFrequency(CountSketch *cs, const char *ip, uint16_t port) {  
    uint32_t value = combineIPPort(ip, port);
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

    // 插入 (IP, 端口) 组合数据
    insert(&cs, "192.168.1.1", 80);
    insert(&cs, "192.168.1.1", 80);
    insert(&cs, "10.0.0.1", 22);
    insert(&cs, "10.0.0.1", 22);
    insert(&cs, "10.0.0.1", 22);
    
    // 查询频率  
    printf("Estimated frequency of 192.168.1.1:80 -> %d\n", estimateFrequency(&cs, "192.168.1.1", 80));  
    printf("Estimated frequency of 10.0.0.1:22 -> %d\n", estimateFrequency(&cs, "10.0.0.1", 22));  
    printf("Estimated frequency of 8.8.8.8:53 -> %d\n", estimateFrequency(&cs, "8.8.8.8", 53));  

    return 0;  
}

