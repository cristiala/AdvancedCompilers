# Bril CFG Assignment

## Main File and Test Cases
- `WorkingWithCFG.py` – contains main code implementing methods:
  - get_path_lengths: shortest path lengths from entry with a breadth first search from the entry node
  - reverse_postorder: reverse postorder traversal with depth first search
  - find_back_edges: runs DFS to find back edges, where edge (u, v) points back to an previous v in the stack
  - is_reducible: check reducibility of a CFG
- `test/cfg_test.py` - pytest based unit tests
- `test/sample01.json` - example input CFGs
- `test/sample02.json` - irreducible CFG


CFGs are represented as Python dictionaries:
```python
cfg = {
    "A": ["B", "C"],
    "B": ["D"],
    "C": ["D"],
    "D": []
}
```
### JSON files

The JSON files in test/ are example Bril programs converted to JSON using bril2json.
These files serve as input to WorkingWithCFG.py

### Running on JSON input

```bash
cat test/sample1.json | python3 WorkingWithCFG.py
```
- Reads the contents of the JSON file (sample01.json).
- That JSON file is just a Bril program converted into JSON with bril2json.
- Inside WorkingWithCFG.py, the function mycfg() runs by default:
```python
prog = json.load(sys.stdin)
print(prog)
```
- This loads the JSON from stdin and stores it in the prog dictionary
- Then it prints the whole JSON object back out (in Python dictionary format)


### To Run tests
```bash
pip install pytest
pytest test/
```
- Pytest finds test file
- Each dot (.) when ran means one test passed
- CFG analysis code is tested independently of the Bril JSON input/output