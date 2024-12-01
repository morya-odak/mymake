/*
* File: graph.c
* Author: Morya Odak
* Purpose: This file is responsible for the bread and butter of the program. 
* Including reading through the makefile, setting up the dependency graph, 
* performing the postorder traversal, and freeing the memory when the output
* has been printed or an error is encountered. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"


/*
 * add_dependency(node, dep_name) -- adds a dependency to a node, which is stored in a linked
 * list structure specific to the node. 
 */
void add_dependency(Node* node, char* dep_name) {
    ll* new_dep = malloc(sizeof(ll));
    if (!new_dep) {
        perror("Failed to allocate memory for dependency");
        exit(1);
    }
    new_dep->name = strdup(dep_name);
    new_dep->next = NULL;

    if (!node->dependencies) {
        node->dependencies = new_dep;
    } else {
        ll* temp = node->dependencies;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = new_dep;
    }
}

/*
 * add_command(node, cmd) -- adds a command to a node, which is stored in a linked
 * list structure specific to the node. 
 */
void add_command(Node* node, char* cmd) {
    ll* new_cmd = malloc(sizeof(ll));
    if (!new_cmd) {
        perror("Failed to allocate memory for command");
        exit(1);
    }
    new_cmd->name = strdup(cmd);
    new_cmd->next = NULL;

    if (!node->commands) {
        node->commands = new_cmd;
    } else {
        ll* temp = node->commands;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = new_cmd;
    }
}

/*
 * find_or_create_node(graph, name) -- searches for a node in the graph with the 
 * the given name. If the node is in the graph, it is returned. Otherwise, it
 * creates a new node and adds it to the graph.
 */
Node* find_or_create_node(Graph* graph, char* name) {
    Node* current = graph->head;

    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current; 
        }
        current = current->next;
    }

    Node* new_node = malloc(sizeof(Node));
    if (!new_node) {
        perror("Failed to allocate memory for node");
        exit(1);
    }
    new_node->name = strdup(name);
    new_node->dependencies = NULL;
    new_node->commands = NULL;
    new_node->visited = 0; 
    new_node->next = graph->head;
    graph->head = new_node;

    return new_node;
}

/*
 * parse_rules(file, line) -- Takes the makefile pointer as a parameter and
 * constructs the graph structure -- adding targets and adependencies as necesary
 * along with the commands associated with them.
 */
Graph* parse_rules(FILE* file, char** line) {
    Graph* graph = malloc(sizeof(Graph));
    if (!graph) {
        perror("Failed to allocate memory for graph");
        exit(1);
    }
    graph->head = NULL;

    size_t len = 0;
    Node* current_node = NULL;

    while (getline(line, &len, file) != -1) {
        if ((*line)[strlen(*line) - 1] == '\n') {
            (*line)[strlen(*line) - 1] = '\0'; 
        }

        if (**line == '\t') {
            if (!current_node) {
                fprintf(stderr, "Error: Command without a target: '%s'\n", *line);
                exit(1);
            }
            add_command(current_node, *line + 1); 
            continue;
        }

        while (**line == ' ' || **line == '\t') (*line)++;

        if (**line == '\0') continue;

        char target_buffer[256]; 
        int index = 0;

        while ((*line)[index] != ':' && (*line)[index] != '\0' && index < 255) {
            target_buffer[index] = (*line)[index];
            index++;
        }
        target_buffer[index] = '\0'; 

        if ((*line)[index] != ':') {
            fprintf(stderr, "Error: Missing colon in target line: '%s'\n", *line);
            exit(1);
        }
        index++; 

        int target_len = strlen(target_buffer);
        while (target_len > 0 && target_buffer[target_len - 1] == ' ') {
            target_buffer[--target_len] = '\0';
        }

        char* target = malloc((target_len + 1) * sizeof(char));
        if (!target) {
            perror("Failed to allocate memory for target");
            exit(1);
        }
        strcpy(target, target_buffer);

        while ((*line)[index] == ' ') index++;

        current_node = find_or_create_node(graph, target);
        free(target);

        while ((*line)[index] != '\0') {
            char dep_buffer[256]; 
            int dep_len = 0;

            while ((*line)[index] != ' ' && (*line)[index] != '\0' && dep_len < 255) {
                dep_buffer[dep_len++] = (*line)[index++];
            }
            dep_buffer[dep_len] = '\0'; 

            if (dep_len > 0) {
                find_or_create_node(graph, dep_buffer); 
                add_dependency(current_node, dep_buffer);
            }

            while ((*line)[index] == ' ') index++;
        }
    }

    return graph;
}

/*
 * postorder_traversal(node, graph) -- performs the posorder traversal of the graph
 * in accordance with the spec, printing them in the order necessary. 
 */
void postorder_traversal(Node* node, Graph* graph) {
    if (node->visited) return;

    node->visited = 1; 

    ll* dep_ll = node->dependencies;
    while (dep_ll) {
        Node* dep_node = graph->head;
        while (dep_node && strcmp(dep_node->name, dep_ll->name) != 0) {
            dep_node = dep_node->next;
        }
        if (dep_node) {
            postorder_traversal(dep_node, graph); 
        }
        dep_ll = dep_ll->next;
    }

    printf("%s\n", node->name);

    ll* cmd = node->commands;
    while (cmd) {
        printf("  %s\n", cmd->name); 
        cmd = cmd->next;
    }
}

/*
 * free_memory(graph, line) -- frees the dynamically allocated memory used 
 * to construct the graph and store the necessary targets, dependencies, and 
 * commands. The buffer used to read the lines using getline is freed as well.
 */
void free_memory(Graph* graph, char* line) {
    if (graph) {
        Node* current = graph->head;
        while (current) {
            free_ll(current->dependencies);
            free_ll(current->commands);
            free(current->name);

            Node* temp = current;
            current = current->next;
            free(temp);
        }
        free(graph);
    }
    free(line);
}

/*
 * free_linked_list(head) -- helper function for free_memory that frees the linked
 * lists used to store dependencies and commands that pertain to a node. 
 */
void free_ll(ll* head) {
    while (head) {
        free(head->name);
        ll* temp = head;
        head = head->next;
        free(temp);
    }
}