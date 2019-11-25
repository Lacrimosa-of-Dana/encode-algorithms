//
//  main.c
//  Encode
//
//  Created by Wang Jianmin on 2019/5/3.
//  Copyright © 2019年 Zhang Tianchen. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

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
    FILE *fp, *out, *logg;
    if (!(fp = fopen(address, "rb"))) {
        printf("Failed to open file.\n");
        return 1;
    }
    out = fopen(strcat(address, ".huffman"), "wb");
    logg = fopen(strcat(address, ".log"), "w");
    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    uchar *src = (uchar*)malloc(sizeof(uchar) * length);
    fread(src, sizeof(uchar), length, fp);
    
    long p;
    w[0] = 1;
    for (p = 0; p < length; ++p) ++w[src[p]];
    int i;
    int sum = 0;
    double h = 0, prob;
    for (i = 0; i < MAX; ++i) {
        fprintf(logg, "%d %d\n", i, w[i]);
        sum += w[i];
    }
    for (i = 0; i < MAX; ++i) {
        prob = (double)w[i] / sum;
        if (w[i] > 0.0)
            h -= prob * (log(prob) / log(2));
    }
    printf("H(X) = %.4lf.\n", h);
    node *head = (node*)malloc(sizeof(node));
    node *leaf[MAX];
    int top = -1;
    memset(head, 0, sizeof(node));
    
    for (i = 0; i < MAX; ++i) {
        if (w[i]) {
            node *p = head;
            while (p->next && (p->next->weight < w[i] || (p->next->weight == w[i] && p->next->chara < i))) p = p->next;
            node *q = (node*)malloc(sizeof(node));
            leaf[++top] = q;
            q->left = q->right = q->parent = NULL;
            q->chara = i;
            // q->times = 0;
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
    int code[MAX][MAX];
    int start[MAX];
    for (i = 0; i <= top; ++i) {
        start[leaf[i]->chara] = top;
        node *f = leaf[i]->parent;
        node *s = leaf[i];
        while (f) {
            if (f->left == s) code[leaf[i]->chara][start[leaf[i]->chara]] = 0;
            else code[leaf[i]->chara][start[leaf[i]->chara]] = 1;
            --start[leaf[i]->chara];
            s = f;
            f = f->parent;
        }
    }
    fseek(fp, 0, SEEK_SET);
    int flag;
    uchar buffer = 0;
    int time = 0;
    for (p = 0; p < length; ++p) {
        for (flag = start[src[p]] + 1; flag <= top; ++flag) {
            ++time;
            buffer <<= 1;
            buffer += code[src[p]][flag];
            if (time == 8) {
                fwrite(&buffer, 1, 1, out);
                //fprintf(out, "%x", buffer);
                buffer = 0;
                time = 0;
            }
        }
    }
    for (flag = start[0] + 1; flag <= top; ++flag) {
        ++time;
        buffer <<= 1;
        buffer += code[0][flag];
        if (time == 8) {
            fwrite(&buffer, 1, 1, out);
            //fprintf(out, "%x", buffer);
            buffer = 0;
            time = 0;
        }
    }
    
    if (time > 0 && time < 8) {
        int j;
        for (j = 0; j < 8 - time; ++j) buffer <<= 1;
        fwrite(&buffer, 1, 1, out);
        //fprintf(out, "%x", buffer);
        //printf("\n");
    }
    fclose(logg);
    fclose(fp);
    fclose(out);
    endt = clock();
    printf("Encode finished.\n");
    printf("Runtime is %lfs.\n", (double)(endt - startt) / CLOCKS_PER_SEC);
    return 0;
}
