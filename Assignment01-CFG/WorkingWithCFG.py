def get_path_lengths(cfg, entry):
    dist = {entry: 0}
    q = deque([entry])

    while q:
        u = q.popleft()
        for v in cfg.get(u, []):
            if v not in dist:
                dist[v] = dist[u] + 1
                q.append(v)
    return dist
#Compute the shortest path length (in edges) from the entry node to each node in the CFG.

# Parameters:
# cfg (dict): mapping {node: [successors]}
# entry (str): starting node

# Returns:
# dict: {node: distance from entry}, unreachable nodes are omitted




def reverse_postorder(cfg, entry):
    visited = set()
    order = []

    def dfs(u):
        if u in visited:
            return
        visited.add(u)
        for v in cfg.get(u, []):
            dfs(v)
        order.append(u)

    dfs(entry)
    return list(reversed(order))
# Compute reverse postorder (RPO) for a CFG.

# Parameters:
# cfg (dict): mapping {node: [successors]}
# entry (str): starting node


# Returns:
# list: nodes in reverse postorder




def find_back_edges(cfg, entry):
    visited = set()
    stack = set()
    back_edges = []

    def dfs(u):
        visited.add(u)
        stack.add(u)
        for v in cfg.get(u, []):
            if v not in visited:
                dfs(v)
            elif v in stack:
                back_edges.append((u, v))
        stack.remove(u)

    dfs(entry)
    return back_edges
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