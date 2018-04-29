/*
 * main.c
 *
 *  Created on: 19 Apr 2018
 *      Author: adtonks
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

/* below graph structs and functions based on Adam's submission for POSIX midterm assignment */

typedef struct m_conn_node {
	struct m_node *node;
	double dist;
} conn_node;

typedef struct m_node {
	int ID; /* ID of node */
	/* co-ordinates of node */
	double x;
	double y;
	int conn_count; /* number of connected nodes */
	conn_node *connected_nodes; /* array of conn_nodes structs */
	double* min_dist;
	int *pred; /* predecessor node */
} node;

typedef struct m_graph {
	int size;
	node **node_list;
	int *visited;
} graph;

graph *create_new_graph() {
	graph *g = malloc(sizeof(graph));
	g->size = 0;
	g->node_list = NULL;
	g->visited = NULL;
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
	/* put pointer in node_list */
	g->node_list = realloc(g->node_list, sizeof(node *)*(g->size+1));
	g->node_list[g->size] = node_ptr;
	g->visited = realloc(g->visited, sizeof(int)*(g->size+1));
	/* increment graph size */
	g->size++;
	return(node_ptr);
}

void create_edge(graph *g, int ID1, int ID2) {
	node* node_ptr = g->node_list[ID1];
	/* input node pointer into adjacency lists */
	node_ptr->connected_nodes = realloc(node_ptr->connected_nodes,
			sizeof(conn_node)*(node_ptr->conn_count+1));
	node_ptr->connected_nodes[node_ptr->conn_count].node = g->node_list[ID2];
	node_ptr->conn_count++;
	return;
}

void delete_graph(graph *g) {
	int i;
	if(g == NULL)
		return;
	for(i=0; i<g->size; i++) {
		/* free components of the node */
		free(g->node_list[i]->connected_nodes); /* free array of conn_node structs */
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
			printf("\n Node: %d; Dist: %f", node_ptr->connected_nodes[j].node->ID, node_ptr->connected_nodes[j].dist);
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

int comp_dists(const void *a, const void *b) {
	double dist_a = (*(const conn_node *) a).dist;
	double dist_b = (*(const conn_node *) b).dist;
	if(dist_a < dist_b) {
		printf("%f less than %f\n", dist_a, dist_b);
		return(-1);
	} else if(dist_a > dist_b) {
		printf("%f greater than %f\n", dist_a, dist_b);
		return(1);
	} else {
		return(0);
	}
}

/* calculate distances in graph */
void load_dists(graph *g) {
	int i, j;
	int n_size;
	double n_x, n_y;
	conn_node *n_conn;
	double *n_dists;
	/* loop over nodes in graph */
	for(i=0; i<g->size; i++) {
		n_size = g->node_list[i]->conn_count;
		n_x = g->node_list[i]->x;
		n_y = g->node_list[i]->y;
		n_conn = g->node_list[i]->connected_nodes;
		/* loop over connected nodes */
		for(j=0; j<n_size; j++) {
			/* store Euclidian distance */
			n_conn[j].dist = sqrt(pow(n_x-(n_conn[j].node->x), 2.0) + pow(n_y-(n_conn[j].node->y), 2.0));
			printf("Old %f\n", n_conn[j].dist);
		}
		/* sort the distances */
		qsort(n_conn, n_size, sizeof(*n_conn), &comp_dists);
		for(j=0; j<n_size; j++) {
			printf("New %f\n", n_conn[j].dist);
		}
	}

}

void dijk_init(graph *g, int node_start) {
	int i;
	g->node_list[node_start]->min_dist = malloc(sizeof(double) * g->size);
	g->node_list[node_start]->pred = malloc(sizeof(int) * g->size);
	for(i=0; i<g->size; i++) {
		g->node_list[node_start]->min_dist[i] = DBL_MAX;
		g->node_list[node_start]->pred[i] = node_start;
	}
	for(i=0; i<g->size; i++)
		g->visited[i] = 0;
	return;
}

void dijk(graph *g, int node_start) {
	int i, j;
	double *min_dist;
	int *pred;
	int *visited;
	int node_curr, count_adj;
	int node_adj;
	double node_curr_dist, node_adj_dist;

	dijk_init(g, node_start);
	min_dist = g->node_list[node_start]->min_dist;
	pred = g->node_list[node_start]->pred;
	visited = g->visited;

	/* ensure that node_start is first to be visited */
	min_dist[node_start] = 0;
	for(i=0; i<g->size; i++) {
		node_curr_dist = DBL_MAX;
		/* find the unvisted node with lowest interim distance */
		for(j=0; j<g->size; j++) {
			if((min_dist[j] <= node_curr_dist) && (visited[j] == 0)) {
				node_curr = j;
				node_curr_dist = min_dist[j];
			}
		}
		/* check if a better path exists through this node */
		visited[node_curr] = 1;
		count_adj = g->node_list[node_curr]->conn_count;
		for(j=0; j<count_adj; j++) {
			node_adj = g->node_list[node_curr]->connected_nodes[j].node->ID;
			node_adj_dist = g->node_list[node_curr]->connected_nodes[j].dist;
			if(visited[node_adj] == 0) {
				/* assign new predecessor and distance if better path found */
				if((node_curr_dist+node_adj_dist) < min_dist[node_adj]) {
					min_dist[node_adj] = node_curr_dist + node_adj_dist;
					pred[node_adj] = node_curr;
				}
			}
		}
	}
}

double greedy_path(graph *g, int start) {
	int i, j;
	double dist_cum, node_next_dist;
	int node_curr, node_next;
	/* find the shortest path using a greedy algorithm */
	/* convert into fully connected graph */
	for(i=0; i<g->size; i++) {
		dijk(g, i);
		printf("Running Dijkstra's algorithm on node %d\n", i);
	}
	for(i=0; i<g->size; i++)
		g->visited[i] = 0;
	/* traverse the graph */
	node_curr = start;
	g->visited[node_curr] = 1;
	dist_cum = 0;
	for(i=1; i<g->size; i++) {
		node_next_dist = DBL_MAX;
		for(j=0; j<g->size; j++) {
			if((g->node_list[node_curr]->min_dist[j] <= node_next_dist) && (g->visited[j] == 0)) {
				node_next = j;
				node_next_dist = g->node_list[node_curr]->min_dist[j];
			}
		}
		g->visited[node_next] = 1;
		dist_cum += node_next_dist;
		printf("Visit node %d with distance %f\n", node_next, node_next_dist);
		node_curr = node_next;
	}
	/* return to start */
	dist_cum += g->node_list[node_curr]->min_dist[start];
	return(dist_cum);
}

int main(int argc, char* argv[]) {
	graph *g = create_new_graph();
	load_nodes(g);
	load_edges(g, 0);
	load_dists(g);
	print_graph(g);
	printf("The greedy algorithm finds a walk of length %f\n", greedy_path(g, 0));
	delete_graph(g);
	return(0);
}
