# include <stdio.h>
# include <string.h>

typedef struct ll {
    char* name;
    ll *next;
}ll;


typedef struct Node {
    char* name;              // Target name
    ll *dependencies;
    int num_dependencies;    // Number of dependencies
    ll *commands;           // Array of commands for target
    int num_commands;        // Number of commands
    int visited;             // Flag for traversal (0 = visited)
} Node;

typedef struct Graph {
    Node** nodes;          // Array of pointers to all nodes
    int num_nodes;         // Total number of nodes
} Graph;


// add a command to a node
void add_command(Node* n, char *cmd) {
    ll* new_cmd = malloc(sizeof(ll));
    if (n->commands == NULL) {
        n->commands = malloc(sizeof(ll));
    }
    new_cmd->name = strdup(cmd);
    new_cmd->next = NULL;
}


Graph construct_graph(FILE *fp) {
    Graph *g = malloc(sizeof(Graph));
    g->nodes = NULL;
    g->num_nodes = 0;

    size_t line_size = 0;
    char *line = NULL;
    Node* current_node = NULL;
    while (getline(&line, &line_size, fp)) {
        if(*line == '\t') {
            if (current_node ==  NULL) {
                fprintf(stderr, "Bash commands must  be preceded by a rule\n");
                continue;
            }
        }
    } 

}