/*
 * main.c
 *
 *  Created on: 19 Apr 2018
 *      Author: adtonks
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* below graph structs and functions based on Adam's submission for POSIX midterm assignment */

typedef struct m_node {
	int ID; /* ID of node */
	/* co-ordinates of node */
	double x;
	double y;
	int conn_count; /* number of connected nodes */
	struct m_node **connected_nodes; /* array of pointers to connected nodes */
	double *dists;  /* array of distances to connected nodes */
} node;

typedef struct m_graph {
	int size;
	node **node_list;
} graph;

graph *create_new_graph() {
	graph *g = malloc(sizeof(graph));
	g->size = 0;
	g->node_list = NULL;
	return(g);
}

node *create_node(graph *g, int ID, double x, double y) {
	/* initialize the node */
	node *node_ptr = malloc(sizeof(node));
	/* initialize the known node components */
	node_ptr->ID = ID;
	node_ptr->x = x;
	node_ptr->y = y;
	/* initialize unknown node components */
	node_ptr->conn_count = 0;
	node_ptr->connected_nodes = NULL;
	node_ptr->dists = NULL;
	/* put pointer in node_list */
	g->node_list = realloc(g->node_list, sizeof(node *)*(g->size+1));
	g->node_list[g->size] = node_ptr;
	/* increment graph size */
	g->size++;
	return(node_ptr);
}

void create_edge(graph *g, int ID1, int ID2) {
	node* node_ptr = g->node_list[ID1];
	/* input node pointer into adjacency lists */
	node_ptr->connected_nodes = realloc(node_ptr->connected_nodes,
			sizeof(node *)*(node_ptr->conn_count+1));
	node_ptr->connected_nodes[node_ptr->conn_count] = g->node_list[ID2];
	node_ptr->conn_count++;
	return;
}

void delete_graph(graph *g) {
	int i;
	if(g == NULL)
		return;
	for(i=0; i<g->size; i++) {
		/* free components of the node */
		free(g->node_list[i]->connected_nodes); /* free array of connected nodes */
		free(g->node_list[i]->dists); /* free array of distances */
		/* free the node */
		free(g->node_list[i]);
	}
	/* free components of the node */
	free(g->node_list); /* free the array of nodes */
	/* free the node */
	free(g);
	return;
}

void print_graph(graph *g) {
	int i, j;
	int root_ID;
	node *node_ptr;
	if(g->size == 0) {
		printf("Graph is empty\n");
		return;
	}
	printf("#### BEGIN PRINTING GRAPH ####\n");
	printf("Graph contains %d nodes\n", g->size);
	for(i=0; i<g->size; i++) {
		node_ptr = g->node_list[i];
		printf(" ______________________________________ \n");
		printf("|            #### Node ####            |\n");
		printf(" ID: %d\n", node_ptr->ID);
		printf(" x: %f; y: %f\n", node_ptr->x, node_ptr->y);
		/* connected nodes */
		printf(" Connected nodes: ");
		for(j=0; j<node_ptr->conn_count; j++) {
			printf("%d ", node_ptr->connected_nodes[j]->ID);
		}
		printf("\n");
		printf(" Distances: ");
		for(j=0; j<node_ptr->conn_count; j++) {
			printf("%f ", node_ptr->dists[j]);
		}
		printf("\n");
		printf("|______________________________________|\n");
	}
	printf("#### END PRINTING GRAPH ####\n");
	return;
}

/* load the nodes into graph */
void load_nodes(graph *g) {
	FILE *fp = fopen("graph.txt", "r");
	int curr_node;
	double x, y;
	char curr_char;
	for(curr_node = 0; curr_node != -1; curr_node++) {
		/* read the node information */
		/* read first character of line */
		curr_char = fgetc(fp);
		/* read x co-ordinate */
		if(curr_char == '-') {
			fscanf(fp, "%lf", &x);
			x = -x;
		} else {
			ungetc(curr_char, fp);
			fscanf(fp, "%lf", &x);
		}
		/* read y co-ordinate */
		fscanf(fp, "%lf", &y);
		/* enter node */
		create_node(g, curr_node, x, y);
		/* increment fp until newline */
		for(;;curr_char = fgetc(fp)) {
			if(curr_char == '\n') {
				/* look out for EOF */
				curr_char = fgetc(fp);
				if(curr_char == EOF) {
					/* set exit flag (will be incremented) */
					curr_node = -2;
					break;
				} else {
					ungetc(curr_char, fp);
					break;
				}
			}
		}
	}
	fclose(fp);
}

/* load the edges into graph */
void load_edges(graph *g, int neg_space) {
	FILE *fp = fopen("graph.txt", "r");
	int i, spaces;
	int curr_node, conn_node;
	char curr_char, exit_char;
	for(curr_node = 0; curr_node != -1; curr_node++) {
		curr_char = fgetc(fp);
		if((curr_char == '-') && (neg_space == 1)) {
			spaces = 4;
		} else {
			spaces = 2;
		}
		/* jump spaces */
		for(i=0; i<spaces; curr_char = fgetc(fp)) {
			if(curr_char == ' ')
				i++;
		}

		for(;;curr_char = fgetc(fp)) {
			if(curr_char == '\n') {
				/* look out for EOF */
				curr_char = fgetc(fp);
				if(curr_char == EOF) {
					/* set exit flag (will be incremented) */
					curr_node = -2;
					break;
				} else {
					ungetc(curr_char, fp);
					break;
				}
			} else {
				/* connect the nodes */
				ungetc(curr_char, fp);
				fscanf(fp, "%d", &conn_node);
				printf("Creating edge between %d and %d\n", curr_node, conn_node);
				create_edge(g, curr_node, conn_node);
				/* get the space */
				curr_char = fgetc(fp);
				if(curr_char == '\n')
					ungetc(curr_char, fp);
			}
		}
	}
	fclose(fp);
}

/* calculate distances in graph */
void load_dists(graph *g) {
	int i, j;
	int n_size;
	double n_x, n_y;
	node **n_conn;
	double *n_dists;
	/* loop over nodes in graph */
	for(i=0; i<g->size; i++) {
		n_size = g->node_list[i]->conn_count;
		n_x = g->node_list[i]->x;
		n_y = g->node_list[i]->y;
		n_conn = g->node_list[i]->connected_nodes;
		/* malloc the distance array */
		n_dists = malloc(sizeof(double) * n_size);
		/* loop over connected nodes */
		for(j=0; j<n_size; j++) {
			/* store Euclidian distance */
			n_dists[j] = sqrt(pow(n_x-(n_conn[j]->x), 2.0) + pow(n_y-(n_conn[j]->y), 2.0));
		}
		g->node_list[i]->dists = n_dists;
	}
	/* sort the distances */
}

void greedy_path(graph *g, int start, int end) {
	/* find the shortest path using a greedy algorithm */
}

int main(int argc, char* argv[]) {
	graph *g = create_new_graph();
	load_nodes(g);
	load_edges(g, 0);
	load_dists(g);
	print_graph(g);
	delete_graph(g);
	return(0);
}
