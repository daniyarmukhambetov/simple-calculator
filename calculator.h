#pragma once

#include <sstream>
#include <memory>
#include <stack>

class Tokenizer {
public:
    explicit Tokenizer(std::istream* in) : in_{in} {
        Consume();
    }

    enum TokenType { kNumber, kSymbol, kEnd };

    void Consume() {
        char c;
        bool is_number = false;
        int64_t res = 0;
        while (in_->get(c)) {
            if (is_number) {
                if ('0' <= c && c <= '9') {
                    res = res * 10 + (c - '0');
                } else {
                    type_ = TokenType::kNumber;
                    number_ = res;
                    in_->putback(c);
                    is_number = false;
                    return;
                }
                continue;
            }
            if ('0' <= c && c <= '9') {
                is_number = true;
                res = res * 10 + (c - '0');
            } else {
                if (!(c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')')) {
                    continue;
                }
                type_ = TokenType::kSymbol;
                symbol_ = c;
                return;
            }
        }
        if (is_number) {
            type_ = TokenType::kNumber;
            number_ = res;
            return;
        }
        type_ = TokenType::kEnd;
    }

    TokenType GetType() const {
        return type_;
    }

    int64_t GetNumber() const {
        if (type_ != TokenType::kNumber) {
            throw std::runtime_error{"current token not number"};
        }
        return number_;
    }

    char GetSymbol() const {
        if (type_ != TokenType::kSymbol) {
            throw std::runtime_error{"current token not symbol"};
        }
        return symbol_;
    }

private:
    std::istream* in_;
    TokenType type_;
    int64_t number_;
    char symbol_;
};

class Expression {
public:
    virtual ~Expression() = default;
    virtual int64_t Evaluate() const = 0;
    virtual void ApplyMinus(){};
};

class Constant : public Expression {
public:
    explicit Constant(int64_t value) : value_(value) {
    }
    int64_t Evaluate() const {
        return is_neg_ ? -value_ : value_;
    }
    void ApplyMinus() {
        is_neg_ ^= true;
    }

private:
    int64_t value_;
    bool is_neg_ = false;
};

class Operation : public Expression {
public:
    Operation(char op, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : op_(op), l_(std::move(l)), r_(std::move(r)) {
    }
    int64_t Evaluate() const {
        int64_t l = l_->Evaluate();
        int64_t r = r_->Evaluate();
        int64_t res = 0;
        if (op_ == '+') {
            res = l + r;
        } else if (op_ == '-') {
            res = l - r;
        } else if (op_ == '*') {
            res = l * r;
        } else if (op_ == '/') {
            if (r == 0) {
                throw std::runtime_error{"division by zero"};
            }
            res = l / r;
        }
        return is_neg_ ? res * -1 : res;
    }
    void ApplyMinus() {
        is_neg_ ^= true;
    }

private:
    char op_;
    std::unique_ptr<Expression> l_, r_;
    bool is_neg_ = false;
};

std::unique_ptr<Expression> ReadTerm(Tokenizer* tokenizer);

std::unique_ptr<Expression> ReadExpression(Tokenizer* tokenizer) {
    auto node = ReadTerm(tokenizer);
    while (tokenizer->GetType() == Tokenizer::kSymbol &&
           (tokenizer->GetSymbol() == '+' || tokenizer->GetSymbol() == '-')) {
        char op = tokenizer->GetSymbol();
        tokenizer->Consume();
        auto right = ReadTerm(tokenizer);
        auto new_node = std::make_unique<Operation>(op, std::move(node), std::move(right));
        node = std::move(new_node);
    }
    return node;
}

std::unique_ptr<Expression> ReadFactor(Tokenizer* tokenizer);

std::unique_ptr<Expression> ReadTerm(Tokenizer* tokenizer) {
    auto node = ReadFactor(tokenizer);
    while (tokenizer->GetType() == Tokenizer::kSymbol &&
           (tokenizer->GetSymbol() == '*' || tokenizer->GetSymbol() == '/')) {
        char op = tokenizer->GetSymbol();
        tokenizer->Consume();
        auto node_right = ReadFactor(tokenizer);
        auto new_node = std::make_unique<Operation>(op, std::move(node), std::move(node_right));
        node = std::move(new_node);
    }
    return node;
}

std::unique_ptr<Expression> ReadFactor(Tokenizer* tokenizer) {
    if (tokenizer->GetType() == Tokenizer::kSymbol && tokenizer->GetSymbol() == '-') {
        tokenizer->Consume();
        if (tokenizer->GetType() == Tokenizer::kNumber) {
            auto res = tokenizer->GetNumber();
            tokenizer->Consume();
            auto ret = std::make_unique<Constant>(res);
            ret->ApplyMinus();
            return ret;
        } else if (tokenizer->GetType() == Tokenizer::kSymbol && tokenizer->GetSymbol() == '(') {
            tokenizer->Consume();
            auto res = ReadExpression(tokenizer);
            tokenizer->Consume();
            res->ApplyMinus();
            return res;
        }
        throw std::runtime_error{"invalid factor minus"};
    }
    if (tokenizer->GetType() == Tokenizer::kNumber) {
        auto res = tokenizer->GetNumber();
        tokenizer->Consume();
        return std::make_unique<Constant>(res);
    } else if (tokenizer->GetType() == Tokenizer::kSymbol && tokenizer->GetSymbol() == '(') {
        tokenizer->Consume();
        auto res = ReadExpression(tokenizer);
        tokenizer->Consume();
        return res;
    }
    throw std::runtime_error{"invalid factor"};
}

// лубые выражения выражаются через factor(*|/)factor(*|/)...(*|/)factor --> term
// term(+|-)term

std::unique_ptr<Expression> ParseExpression(Tokenizer* tokenizer) {
    return ReadExpression(tokenizer);
}
