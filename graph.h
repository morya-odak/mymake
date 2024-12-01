/*
* File: graph.h
* Name: Morya Odak
* Purpose: Defines the structs and functions to be implemented in graph.c
*/

#ifndef GRAPH_H // include guard
#define GRAPH_H

#include <stdio.h>

// Linked list node used for dependencies and commands of the same target
typedef struct ll {
    char* name;
    struct ll* next;
} ll;

// Node in the adjacency list
typedef struct Node {
    char* name;              
    ll* dependencies;        
    ll* commands;            
    int visited;             
    struct Node* next;       
} Node;

// Stores several nodes, comprising the graph structure
typedef struct Graph {
    Node* head; 
} Graph;

// Functions
Graph* parse_rules(FILE* file, char** line); 
Node* find_or_create_node(Graph* graph, char* name);
void add_dependency(Node* node, char* dep_name);
void add_command(Node* node, char* cmd);
void postorder_traversal(Node* node, Graph* graph);

// Memory management
void free_memory(Graph* graph, char* line); 
void free_ll(ll* head);                

#endif 
