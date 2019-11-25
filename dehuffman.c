//
//  main.c
//  HuffmanDecode
//
//  Created by Wang Jianmin on 2019/5/8.
//  Copyright © 2019年 Zhang Tianchen. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 256
typedef struct node {
    int chara;
    int weight;
    struct node *left, *right, *next, *parent;
} node;
typedef unsigned char uchar;
int w[MAX];

int main(int argc, const char * argv[]) {
    time_t startt, endt;
    startt = clock();
    char address[200];
    printf("Please enter the address to the file.\n");
    fgets(address, 200, stdin);
    int k;
    for (k = (int)strlen(address) - 1; address[k] == '\n'; --k) address[k] = '\0';
    FILE *input, *log, *output;
    if (!(input = fopen(address, "rb"))) {
        printf("Failed to open file.\n");
        return 1;
    }
    if (!(log = fopen(strcat(address, ".log"), "r"))) {
        printf("Failed to open the log file.\n");
        return 2;
    }
    address[strlen(address) - 12] = '\0';
    output = fopen(address, "wb");
    char temp[200];
    int c, num;
    while (fgets(temp, 200, log)) {
        sscanf(temp, "%d %d", &c, &num);
        w[c] = num;
    }
    node *head = (node*)malloc(sizeof(node));
    node *leaf[MAX];
    int top = -1;
    memset(head, 0, sizeof(node));
    int i;
    for (i = 0; i < MAX; ++i) {
        if (w[i]) {
            node *p = head;
            while (p->next && (p->next->weight < w[i] || (p->next->weight == w[i] && p->next->chara < i))) p = p->next;
            node *q = (node*)malloc(sizeof(node));
            leaf[++top] = q;
            q->left = q->right = q->parent = NULL;
            q->chara = i;
            q->weight = w[i];
            q->next = p->next;
            p->next = q;
        }
    }
    while (head->next->next) {
        node *com = (node*)malloc(sizeof(node));
        com->weight = head->next->weight + head->next->next->weight;
        com->left = head->next;
        com->right = head->next->next;
        com->parent = NULL;
        head->next->parent = com;
        head->next->next->parent = com;
        head = head->next->next;
        node *p = head;
        while (p->next && p->next->weight <= com->weight) p = p->next;
        com->next = p->next;
        p->next = com;
    }
    node *root = head->next;
    fseek(input, 0, SEEK_END);
    long length = ftell(input);
    fseek(input, 0, SEEK_SET);
    uchar *src = (uchar*)malloc(length * sizeof(uchar));
    fread(src, sizeof(uchar), length, input);
    long j = 0;
    int read = 8;
    int zero = w[0];
    node *find = root;
    while (j < length) {
        while (find->left || find->right) {
            if ((src[j] >> (--read)) & 1) find = find->right;
            else find = find->left;
            if (!read) {
                ++j;
                read = 8;
            }
        }
        if (find->chara) fwrite(&(find->chara), 1, 1, output);
        else if (--zero) fwrite(&(find->chara), 1, 1, output);
        else break;
        find = root;
    }
    endt = clock();
    printf("Runtime is %lfs.\n", (double)(endt - startt) / CLOCKS_PER_SEC);
    return 0;
}
