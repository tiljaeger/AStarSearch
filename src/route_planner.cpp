#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
    start_node = &m_Model.FindClosestNode(start_x, start_y);
    end_node = &m_Model.FindClosestNode(end_x, end_y);

}


// calculate h value: use the distance to the end_node for the h value.
// Node objects have a distance method to determine the distance to another node.
float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    float h_value = node->distance(*end_node);
    return h_value;
}


// expand the current node by adding all unvisited neighbors to the open list.
// FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
// CalculateHValue method to implement the h-Value calculation.
// For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.
void RoutePlanner::AddNeighbors(RouteModel::Node* current_node) {
	current_node->FindNeighbors();
	for(auto neighbor : current_node->neighbors) {
		neighbor->parent = current_node;
		neighbor->g_value = current_node->g_value + current_node->distance(*neighbor);
		neighbor->h_value = CalculateHValue(neighbor);

		open_list.push_back(neighbor);
		neighbor->visited = true;
	}
}

// TODO: do I need to declare method in h-file?
// helper method for NextNode()
bool SortCompare (RouteModel::Node* node1, RouteModel::Node* node2) {
    float f_value1 = node1->g_value + node1->h_value;
    float f_value2 = node2->g_value + node2->h_value;
    return f_value1 < f_value2;
}

// NextNode method to sort the open list and return the next node.
// Sort the open_list according to the sum of the h value and g value (use Compare heler method).
// Create a pointer to the node in the list with the lowest sum.
// Remove that node from the open_list.
// Return the pointer.

RouteModel::Node *RoutePlanner::NextNode() {
    // sort with helper method SortCompare
    //std::sort((this->open_list).begin(), (this->open_list).end(), SortCompare);
    // sort with lambda function
    std::sort(open_list.begin(), open_list.end(), 
    [](const RouteModel::Node *a, const RouteModel::Node* b){
        return (a->h_value+a->g_value) < (b->h_value+b->g_value);
    });
    RouteModel::Node * lowest_sum_node = (this->open_list).front();
    (this->open_list).erase(open_list.begin());

    return lowest_sum_node;
}

    


// ConstructFinalPath method to return the final path found from  A* search.
// This method takes the current (final) node as an argument and iteratively follow the 
//   chain of parents of nodes until the starting node is found.
// For each node in the chain, add the distance from the node to its parent to the distance variable.
// The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;
    path_found.push_back(*current_node);
    while (current_node != this->start_node) {

        distance = distance + current_node->distance(*(current_node->parent));
        path_found.push_back(*(current_node->parent));
        current_node = current_node->parent;
    }
    std::reverse(path_found.begin(), path_found.end());
    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;

}


// Use of the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// Use of the NextNode() method to sort the open_list and return the next node.
// When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = nullptr;
    RoutePlanner::start_node->visited = true;
    RoutePlanner::open_list.push_back(RoutePlanner::start_node);
    while (!RoutePlanner::open_list.empty()) {
        current_node = RoutePlanner::NextNode();
        //if (current_node->distance(*RoutePlanner::end_node) == 0) {
        if (current_node == end_node) {  
            RoutePlanner::m_Model.path = RoutePlanner::ConstructFinalPath(RoutePlanner::end_node);
            break;
        } 
        RoutePlanner::AddNeighbors(current_node);
    }


}

