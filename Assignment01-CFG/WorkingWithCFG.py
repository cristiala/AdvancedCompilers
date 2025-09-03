def get_path_lengths(cfg, entry):
#Compute the shortest path length (in edges) from the entry node to each node in the CFG.

# Parameters:
# cfg (dict): mapping {node: [successors]}
# entry (str): starting node

# Returns:
# dict: {node: distance from entry}, unreachable nodes are omitted




def reverse_postorder(cfg, entry):
# Compute reverse postorder (RPO) for a CFG.

# Parameters:
# cfg (dict): mapping {node: [successors]}
# entry (str): starting node


# Returns:
# list: nodes in reverse postorder




def find_back_edges(cfg, entry):
# Find back edges in a CFG using DFS.

# Parameters:
# cfg(dict): mapping {node: [successors]}
# entry(str): starting node


# Returns: list of edges (u,v) where u->v is a back edge




def is_reducible(cfg, entry):
# Determine whether a CFG is reducible.


# Parameters:
# cfg(dict): mapping {node: [successors]}
# entry(str): starting node

# Returns: True if the CFG is reducible or False if the CFG is irreducible
