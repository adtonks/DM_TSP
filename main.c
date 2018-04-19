/*
 * main.c
 *
 *  Created on: 19 Apr 2018
 *      Author: adtonks
 */


#include <stdio.h>
#include <stdlib.h>

/* below graph structs and functions based on Adam's submission for POSIX midterm assignment */

typedef struct m_node {
	int ID; /* ID of node */
	/* co-ordinates of node */
	double x;
	double y;
	int conn_count; /* number of connected nodes */
	struct m_node **connected_nodes; /* array of pointers to connected nodes */
	double* dists;  /* array of distances to connected nodes */
} node;

typedef struct m_graph {
	int size;
	node **node_list;
} graph;

graph * create_new_graph() {
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
}

void create_edge(graph *g, int ID1, int ID2) {
	node* node_ptr;
	/* input node pointer into adjacency lists */
	g->node_list[ID1]->connected_nodes = realloc(node_ptr->connected_nodes,
			sizeof(node *)*(node_ptr->conn_count+1));
	g->node_list[ID1]->connected_nodes[g->node_list[ID1]->conn_count] =
			g->node_list[ID2];
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

/* load the nodes into graph */
void load_nodes() {

}

/* load the edges into graph */
void load_edges() {

}

/* calculate distances in graph */
void load_dists() {

}

void greedy_path(graph *g, int start, int end) {
	/* find the shortest path using a greedy algorithm */
}

int main(int argc, char* argv[]) {

}
