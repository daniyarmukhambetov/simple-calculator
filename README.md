# simple-calculator
Calculator for simple mathematical expressions including +, -, *, / and parentheses
# Expression Tree Parser and Evaluator

A lightweight library for parsing mathematical expressions into an expression tree and evaluating them.

## Features

- **Tokenizer**: Breaks down input expressions into tokens (numbers, operators, parentheses).  
- **Expression Parser**: Builds a syntax tree respecting operator precedence and parentheses.  
- **Expression Nodes**:
  - **`Constant`**: Represents numeric values.
  - **`BinaryOperation`**: Represents operations like addition, subtraction, multiplication, and division.
- **Evaluation**: Calculates the result of the expression by traversing the tree.

## How It Works

1. **Input**: `(3 + 5) * 2`
2. **Tokenization**: `(`, `3`, `+`, `5`, `)`, `*`, `2`
3. **Expression Tree**:  
     *
   /   \
  +     2
 / \
3   5

4. **Evaluation**: `16`

## Use Cases

- Embedding a lightweight math parser in applications.
- Understanding syntax tree construction and traversal.
- Extending for custom operations or functions.
