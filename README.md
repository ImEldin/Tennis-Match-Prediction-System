# Tennis Match Prediction

A C++ program for predicting the outcome of a tennis match based on statistical probabilities.

## 🚀 Features

- Calculates the probability of winning a **game**, **set**, and **match**
- Uses **hash functions** for memoization to improve performance
- Supports **deuce**, **tiebreaks**, and **set logic**
- Reads match scenarios from an **input file**
- Implements **recursive dynamic programming** for probability calculations

## 📜 How It Works

The program models a **tennis match** by breaking it down into:
1. **Game probability calculation** (`dp_gem`) - Determines the probability of winning a single game.
2. **Tiebreak probability calculation** (`dp_tiebreak`) - Determines probabilities during a tiebreak.
3. **Set probability calculation** (`dp_set`) - Computes the probability of winning a set based on game results.
4. **Match probability calculation** (`dp_match`) - Uses set probabilities to estimate the final match outcome.

## 🛠 Technologies Used
- **C++** (Efficient performance and memory management)
- **Dynamic Programming** (Optimized probability calculations)
- **Hash Maps (unordered_map)** (Memoization for fast lookups)



 
