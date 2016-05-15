#pragma once

#include <iostream>
#include <memory>
#include <vector>

namespace Interpreter {
	// abstract expressions
	class Expression {
	public:
		Expression() {
		}
		virtual ~Expression() {
		}
		virtual bool interpret(std::vector<std::string> &text, int idx) = 0;
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
	};
	class Or : public BinaryExpression {
	public:
		Or(std::unique_ptr<Expression> &aLeft, std::unique_ptr<Expression> &aRight) : BinaryExpression(aLeft, aRight) {
		}
		virtual ~Or() {
		}
	};
	class Many : public UnaryExpression {
	public:
		Many(std::unique_ptr<Expression> &aNext) : UnaryExpression(aNext) {
		}
		virtual ~Many() {
		}
	};

	// interpreter
	class RegexPattern {
		std::unique_ptr<Expression> root;
	public:
		RegexPattern(std::string pattern) {
			std::unique_ptr<Expression> lit1(new Literal("raining"));
			std::unique_ptr<Expression> lit2(new Literal("dogs"));
			std::unique_ptr<Expression> lit3(new Literal("cats"));

			std::unique_ptr<Expression> or1(new Or(lit2, lit3));
			std::unique_ptr<Expression> many1(new Many(or1));
			std::unique_ptr<Expression> and1(new And(lit1, many1));
			root = std::move(and1);
		}
		bool match(std::vector<std::string> &text) {

			return true;
		}
	};

	void TestSuite() {
		std::vector<std::string> text =  {"dogs", "texttext", "raining", "texttexttexttext", "raining", "dogs", "cats", "texttext"};
		RegexPattern pattern("raining&(dogs|cats)*");
		std::string res[2] = {"no-match", "match"};
		std::cout << res[pattern.match(text)] << std::endl;
	}
}








