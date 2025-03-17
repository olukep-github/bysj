#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <arpa/inet.h> // ���� IP ��ַת��

#define HASH_COUNT 5  // ��ϣ��������  
#define TABLE_SIZE 1000 // ��ϣ���С���ʵ��Ӵ󣬼��ٳ�ͻ��

// ��ϣ����
int hash1(uint32_t x) {  
    return x % TABLE_SIZE;  
}  

int hash2(uint32_t x) {  
    return (x / TABLE_SIZE) % TABLE_SIZE;  
}  

// ��������
typedef struct {  
    int counts[HASH_COUNT][TABLE_SIZE]; // ��������  
} CountSketch;  

// ��ʼ�� CountSketch
void initCountSketch(CountSketch *cs) {  
    memset(cs->counts, 0, sizeof(cs->counts));  
}  

// �� (IP, �˿�) ���ת��Ϊһ��Ψһ����
uint32_t combineIPPort(const char *ip, uint16_t port) {
    struct in_addr addr;
    if (inet_pton(AF_INET, ip, &addr) != 1) {
        fprintf(stderr, "Invalid IP address: %s\n", ip);
        return 0;
    }
    return (addr.s_addr ^ port);  // �򵥹�ϣ��ϣ����Ի��ɸ��õĹ�ϣ��ʽ��
}

// �������� (IP, �˿�)
void insert(CountSketch *cs, const char *ip, uint16_t port) {  
    uint32_t value = combineIPPort(ip, port);
    for (int i = 0; i < HASH_COUNT; i++) {  
        int index = (i % 2 == 0) ? hash1(value) : hash2(value); // ����ʹ�ù�ϣ����  
        if (i % 2 == 0) {  
            cs->counts[i][index]++; // �������  
        } else {  
            cs->counts[i][index]--; // �������  
        }  
    }  
}

// ���� (IP, �˿�) ��ϵ�Ƶ��
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
    return min_count; // ���ع��Ƶ�Ƶ��  
}

int main() {  
    CountSketch cs;  
    initCountSketch(&cs);  

    // ���� (IP, �˿�) �������
    insert(&cs, "192.168.1.1", 80);
    insert(&cs, "192.168.1.1", 80);
    insert(&cs, "10.0.0.1", 22);
    insert(&cs, "10.0.0.1", 22);
    insert(&cs, "10.0.0.1", 22);
    
    // ��ѯƵ��  
    printf("Estimated frequency of 192.168.1.1:80 -> %d\n", estimateFrequency(&cs, "192.168.1.1", 80));  
    printf("Estimated frequency of 10.0.0.1:22 -> %d\n", estimateFrequency(&cs, "10.0.0.1", 22));  
    printf("Estimated frequency of 8.8.8.8:53 -> %d\n", estimateFrequency(&cs, "8.8.8.8", 53));  

    return 0;  
}

