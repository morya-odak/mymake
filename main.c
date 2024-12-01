/*
* File: main.c
* Name: Morya Odak
* Purpose: This file runs the functions specified in the header file and 
* implemented in the graph.c file, as well as checking for several fatal errors. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <makefile> <target>\n", argv[0]);
        return 1;
    }

    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Error opening file");
        return 1;
    }

    char* line = NULL; 
    Graph* graph = parse_rules(fp, &line);
    fclose(fp);

    Node* target_node = graph->head;
    while (target_node && strcmp(target_node->name, argv[2]) != 0) {
        target_node = target_node->next;
    }

    if (!target_node) {
        fprintf(stderr, "Error: Target '%s' not found.\n", argv[2]);
        free_memory(graph, line);
        return 1;
    }

    postorder_traversal(target_node, graph);

    free_memory(graph, line);

    return 0;
}
