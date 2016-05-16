#pragma once

#include <iostream>
#include <memory>
#include <vector>

namespace Interpreter {
	typedef std::vector<const std::string> vcs;

	class Context {
		std::shared_ptr<const vcs> pText;
		std::vector<std::pair<int, int>> result;
	public:
		Context(std::shared_ptr<const vcs> apText) : pText(apText) {
		}
		const vcs &text() const {
			return *pText;
		}
		friend class RegexPattern;
		friend std::ostream &operator<<(std::ostream &os, const Context &ctx);
	};
	std::ostream &operator<<(std::ostream &os, const Context &ctx) {
		for (auto &p : ctx.result) {
			int beginIdx = p.first;
			int endIdx = p.second;
			if (beginIdx > endIdx - 1)
				continue;

			std::cout << "[" + std::to_string(beginIdx);
			if (beginIdx < endIdx - 1)
				std::cout << ", " + std::to_string(endIdx - 1);
			std::cout << "] : ";

			std::cout << "[...";
			for (int idx = beginIdx; idx < endIdx; ++idx)
				 std::cout << ", " << ctx.text()[idx];
			std::cout << ", ...]" << std::endl;
		}
		return os;
	}

	// abstract expressions
	class Expression {
	public:
		Expression() {
		}
		virtual ~Expression() {
		}
		virtual std::vector<int> interpret(Context &ctx, int idx) = 0;
	};
	class TerminalExpression : public Expression {
	public:
		TerminalExpression() : Expression() {
		}
		virtual ~TerminalExpression() {
		}
	};
	class NonTerminalExpression : public Expression {
	public:
		NonTerminalExpression() : Expression() {
		}
		virtual ~NonTerminalExpression() {
		}
	};

	// concrete terminal expressions
	class Literal : public TerminalExpression {
		std::string lit;
	public:
		Literal(std::string aLit) : TerminalExpression(), lit(aLit) {
		}
		virtual ~Literal() {
		}
		virtual std::vector<int> interpret(Context &ctx, int idx) {
			if (idx >= ctx.text().size() || lit != ctx.text()[idx])
				return {};
			return {idx + 1};
		}
	};

	// non-terminal expressions
	class BinaryExpression : public NonTerminalExpression {
	protected:
		std::unique_ptr<Expression> left;
		std::unique_ptr<Expression> right;
	public:
		BinaryExpression(std::unique_ptr<Expression> &aLeft, std::unique_ptr<Expression> &aRight) : NonTerminalExpression(), left(std::move(aLeft)), right(std::move(aRight)) {
		}
		virtual ~BinaryExpression() {
		}
	};
	class UnaryExpression : public NonTerminalExpression {
	protected:
		std::unique_ptr<Expression> next;
	public:
		UnaryExpression(std::unique_ptr<Expression> &aNext) : NonTerminalExpression(), next(std::move(aNext)) {
		}
		virtual ~UnaryExpression() {
		}
	};

	// concrete non-terminal expressions
	class And : public BinaryExpression {
	public:
		And(std::unique_ptr<Expression> &aLeft, std::unique_ptr<Expression> &aRight) : BinaryExpression(aLeft, aRight) {
		}
		virtual ~And() {
		}
		virtual std::vector<int> interpret(Context &ctx, int idx) {
			std::vector<int> andMatch;
			std::vector<int> leftMatch = left -> interpret(ctx, idx);
			if (leftMatch.empty()) 
				return andMatch;
			for (int iidx : leftMatch) {
				std::vector<int> rightMatch = right -> interpret(ctx, iidx);
				andMatch.insert(andMatch.end(), rightMatch.begin(), rightMatch.end());
			}
			return andMatch;
		}
	};
	class Or : public BinaryExpression {
	public:
		Or(std::unique_ptr<Expression> &aLeft, std::unique_ptr<Expression> &aRight) : BinaryExpression(aLeft, aRight) {
		}
		virtual ~Or() {
		}
		virtual std::vector<int> interpret(Context &ctx, int idx) {
			std::vector<int> orMatch;
			std::vector<int> leftMatch = left -> interpret(ctx, idx);
			orMatch.insert(orMatch.end(), leftMatch.begin(), leftMatch.end());
			std::vector<int> rightMatch = right -> interpret(ctx, idx);
			orMatch.insert(orMatch.end(), rightMatch.begin(), rightMatch.end());
			return orMatch;
		}
	};
	class ZeroOrMany : public UnaryExpression {
	public:
		ZeroOrMany(std::unique_ptr<Expression> &aNext) : UnaryExpression(aNext) {
		}
		virtual ~ZeroOrMany() {
		}
		virtual std::vector<int> interpret(Context &ctx, int idx) {
			std::vector<int> zeroOrManyMatch = {idx};
			std::vector<int> nextMatch = {idx};
			do {
				std::vector<int> tmp = nextMatch; 
				nextMatch.clear();
				for (int iidx : tmp) {
					std::vector<int> match = next -> interpret(ctx, iidx);
					nextMatch.insert(nextMatch.end(), match.begin(), match.end());
				}
				zeroOrManyMatch.insert(zeroOrManyMatch.end(), nextMatch.begin(), nextMatch.end());
			} while (!nextMatch.empty());
			return zeroOrManyMatch;
		}
	};

	// interpreter
	class RegexPattern {
		std::unique_ptr<Expression> root;
	public:
		RegexPattern(std::string pattern) {
			// TODO: parse pattern ...
			// "raining*&((cats&dogs)|cats)*"
			std::unique_ptr<Expression> lit1(new Literal("raining"));
			std::unique_ptr<Expression> lit2(new Literal("cats"));
			std::unique_ptr<Expression> lit3(new Literal("dogs"));
			std::unique_ptr<Expression> lit4(new Literal("cats"));

			std::unique_ptr<Expression> zeroOrMany1(new ZeroOrMany(lit1));
			std::unique_ptr<Expression> and1(new And(lit2, lit3));
			std::unique_ptr<Expression> or1(new Or(and1, lit4));
			std::unique_ptr<Expression> zeroOrMany2(new ZeroOrMany(or1));

			std::unique_ptr<Expression> and2(new And(zeroOrMany1, zeroOrMany2));
			root = std::move(and2);
		}
		void match(Context &ctx) {
			std::vector<int> rootMatch;
			for (int beginIdx = 0; beginIdx < ctx.text().size(); ++beginIdx) {
				rootMatch = root -> interpret(ctx, beginIdx);
				for (int endIdx : rootMatch) {
					ctx.result.push_back({beginIdx, endIdx});
				}
			}
		}
	};

	void TestSuite() {
		std::shared_ptr<const vcs> pText = std::make_shared<const vcs>(vcs{"dogs", "texttext", "raining", "texttexttexttext", "raining", "cats", "dogs", "cats", "dogs", "cats", "texttext"});
		Context ctx(pText);

		RegexPattern pattern("");
		pattern.match(ctx);
		std::cout << ctx;
	}
}
