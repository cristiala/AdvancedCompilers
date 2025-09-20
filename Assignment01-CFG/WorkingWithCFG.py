import json
import sys

def mycfg():
    prog = json.load(sys.stdin)
    print(prog)


def get_path_lengths(cfg, entry):
    #Compute the shortest path length (in edges) from the entry node to each node in the CFG.

    # Parameters:
    # cfg (dict): mapping {node: [successors]}
    # entry (str): starting node

    # Returns:
    # dict: {node: distance from entry}, unreachable nodes are omitted
    dist = {entry: 0}
    q = deque([entry])

    while q:
        u = q.popleft()
        for v in cfg.get(u, []):
            if v not in dist:
                dist[v] = dist[u] + 1
                q.append(v)
    return dist



def reverse_postorder(cfg, entry):
    # Compute reverse postorder (RPO) for a CFG.

    # Parameters:
    # cfg (dict): mapping {node: [successors]}
    # entry (str): starting node


    # Returns:
    # list: nodes in reverse postorder
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



def find_back_edges(cfg, entry):
    # Find back edges in a CFG using DFS.

    # Parameters:
    # cfg(dict): mapping {node: [successors]}
    # entry(str): starting node


    # Returns: list of edges (u,v) where u->v is a back edge

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




def is_reducible(cfg, entry):
    # Determine whether a CFG is reducible.


    # Parameters:
    # cfg(dict): mapping {node: [successors]}
    # entry(str): starting node

    # Returns: True if the CFG is reducible or False if the CFG is irreducible
    nodes = set(cfg.keys())
    dom = {n: set(nodes) for n in nodes}
    dom[entry] = {entry}

    changed = True
    while changed:
        changed = False
        for n in nodes - {entry}:
            preds = [p for p, succs in cfg.items() if n in succs]
            if not preds:
                continue
            new_dom = set(nodes)
            for p in preds:
                new_dom &= dom[p]
            new_dom.add(n)
            if new_dom != dom[n]:
                dom[n] = new_dom
                changed = True

    for u, v in find_back_edges(cfg, entry):
        if v not in dom[u]:
            return False
    return True


if __name__ == '__main__':
    mycfg()
