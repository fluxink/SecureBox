## 🧮 What is SecureBox?

SecureBox represents a 2D grid puzzle where each cell has three states:
- **0** - Fully unlocked (green)
- **1** - Partially locked (yellow) 
- **2** - Fully locked (red)

**Goal:** Automatically find a sequence of toggle operations to unlock all cells (make them all 0).

**Toggle Rules:** When position (x,y) is toggled:
- All cells in row y are incremented by 1 (mod 3)
- All cells in column x are incremented by 1 (mod 3)


**Solution Method:** 
1. **Model as System of Equations**: Each cell (i,j) represents an equation in Galois Field GF(3), where all arithmetic is done modulo 3

2. **Effect Matrix**: Creates a lookup table showing how each possible toggle(x,y) affects every cell in the grid:
   ```
   Example for 2×2 grid:
   Grid:        Possible toggles:
   [A] [B]      toggle(0,0)  toggle(1,0)  toggle(0,1)  toggle(1,1)
   [C] [D]      
   
   Effect Matrix:
              toggle(0,0)  toggle(1,0)  toggle(0,1)  toggle(1,1)
   cell A:         2           1           1           0
   cell B:         1           2           0           1  
   cell C:         1           0           2           1
   cell D:         0           1           1           2
   ```

3. **Target Vector**: Calculates how much to add to each cell to make it 0:
   ```
   Current state:    [1] [2] [0]
                     [2] [1] [1]
   
   Target vector:    [2] [1] [0]  ← what to add to get [0] [0] [0]
                     [1] [2] [2]  ← what to add to get [0] [0] [0]
   ```
   - If cell = 0, add 0 → (0 + 0) % 3 = 0 ✓
   - If cell = 1, add 2 → (1 + 2) % 3 = 0 ✓
   - If cell = 2, add 1 → (2 + 1) % 3 = 0 ✓

4. **Solve**: Uses Gaussian elimination in GF(3) to find how many times to toggle each position

5. **Execute**: Applies the calculated toggles step-by-step until all cells become 0 (unlocked)