# AdvancedCompilers
# Bril CFG Assignment


## Files
- `cfg.py` – main code implementing:
  - `get_path_lengths`: shortest path lengths from entry
  - `reverse_postorder`: reverse postorder traversal
  - `find_back_edges`: detect back edges with DFS
  - `is_reducible`: check reducibility of a CFG
- `test/cfg_test.py` - pytest-based unit tests
- `test/sample01*.json` - example input CFGs
- `test/sample02.json` - irreducible CFG

### Running on JSON input
```bash
cat test/sample1.json | python3 cfg.py

## To Run tests
pip install pytest
pytest test/