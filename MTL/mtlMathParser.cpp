//
//  mtlMathParser.cpp
//  MiniLib
//
//  Created by Jonathan Karlsson on 10/14/14.
//  Copyright (c) 2014 Jonathan Karlsson. All rights reserved.
//

#include "mtlMathParser.h"
#include "mtlParser.h"
#include "mtlArray.h"
#include <cmath>

float mtlMathParser::OperationNode::Evaluate( void ) const
{
	float lval = left->Evaluate();
	float rval = right->Evaluate();
	float result;
	
	switch (operation) {
		case '+':
			result = lval + rval;
			break;
		case '-':
			result = lval - rval;
			break;
		case '*':
			result = lval * rval;
			break;
		case '/':
			result = lval / rval;
			break;
		case '^':
			result = pow(lval, rval);
			break;
	}
	
	return result;
}

bool mtlMathParser::OperationNode::IsConstant( void ) const
{
	return left->IsConstant() && right->IsConstant();
}

int mtlMathParser::OperationNode::GetTermDepth(int depth) const
{
	int ldepth = left->GetTermDepth(depth);
	int rdepth = right->GetTermDepth(depth+1);
	return ldepth < rdepth ? rdepth : ldepth;
}

int mtlMathParser::OperationNode::GetOrder(int depth, mtlString &out, float *temp_var, bool *temp_var_init, bool show_symbols) const
{
	int ldepth = left->GetOrder(depth, out, temp_var, temp_var_init, show_symbols);
	int rdepth = right->GetOrder(depth + 1, out, temp_var, temp_var_init, show_symbols);

	mtlString num;

	out.Append('[');
	num.FromInt(depth);
	out.Append(num);
	out.Append(']');
	switch (operation) {
		case '+':
			out.Append("+=");
			break;
		case '-':
			out.Append("-=");
			break;
		case '*':
			out.Append("*=");
			break;
		case '/':
			out.Append("/=");
			break;
		case '^':
			out.Append("^=");
			break;
	}
	out.Append('[');
	num.FromInt(depth + 1);
	out.Append(num);
	out.Append(']');
	out.Append(';');
	return ldepth < rdepth ? rdepth : ldepth;
}

float mtlMathParser::ValueNode::Evaluate( void ) const
{
	return sym.value;
}

bool mtlMathParser::ValueNode::IsConstant( void ) const
{
	return sym.constant;
}

int mtlMathParser::ValueNode::GetTermDepth(int depth) const
{
	return depth;
}

int mtlMathParser::ValueNode::GetOrder(int depth, mtlString &out, float *temp_var, bool *temp_var_init, bool show_symbols) const
{
	if ((temp_var_init[depth] && temp_var[depth] != sym.value) || !temp_var_init[depth]) {
		mtlString num;
		num.FromInt(depth);
		out.Append('[');
		out.Append(num);
		out.Append(']');
		out.Append('=');
		if (!show_symbols || sym.name.GetSize() <= 0) {
			num.FromFloat(sym.value);
			out.Append(num);
		} else {
			out.Append(sym.name);
		}
		out.Append(';');

		temp_var_init[depth] = true;
		temp_var[depth] = sym.value;
	}
	return depth;
}

mtlMathParser::mtlMathParser( void ) : m_scope_stack()
{}

bool mtlMathParser::IsBraceBalanced(const mtlChars &expression) const
{
	int stack = 0;
	for (int i = 0; i < expression.GetSize(); ++i) {
		char ch = expression.GetChars()[i];
		if (ch == '(') {
			++stack;
		} else if (ch == ')') {
			if (stack > 0) {
				--stack;
			} else {
				return false;
			}
		}
	}
	return stack == 0;
}

bool mtlMathParser::IsLegalChars(const mtlChars &expression) const
{
	for (int i = 0; i < expression.GetSize(); ++i) {
		char ch = expression.GetChars()[i];
		if (!mtlChars::IsWhitespace(ch) && !mtlChars::IsAlphanumeric(ch) && ch != '_' && ch != '(' && ch != ')' && ch != '.' && !mtlChars::IsMath(ch)) {
			return false;
		}
	}
	return true;
}

mtlChars mtlMathParser::TrimBraces(const mtlChars &str) const
{
	mtlSyntaxParser parser;
	parser.SetBuffer(str);
	mtlArray<mtlChars> params;
	if (parser.Match("(%s)%0", params) != mtlSyntaxParser::ExpressionNotFound) {
		return params[0];
	}
	return str;
}

void mtlMathParser::DestroyTermTree(mtlMathParser::TermNode *node)
{
	if (node == NULL) { return; }
	DestroyTermTree(node->left);
	DestroyTermTree(node->right);
	delete node;
}

bool mtlMathParser::GenerateTermTree(mtlMathParser::TermNode *& node, mtlChars expression)
{
	static const char zero_str[] = "0";

	expression = TrimBraces(expression);

	if (expression.GetSize() == 0) {
		node = NULL;
		return false;
	}
	
	bool retval = true;
	static const int OperationClasses = 3;
	static const char *Operations[OperationClasses] = {
		"+-", "*/", "^"
	};
	
	int opIndex = -1;

	// NOTE: Maybe parse everything in right-to-left? Don't think it will affect +- at all
	// NOTE: 4/3 * pi * r^3 does not work when parsing left-to-right (does not parse as (4/3)*pi*r^3, but as 4 / (3*pi*r^3))
	
	for (int i = 0; i < OperationClasses; ++i) {
		mtlChars ops = mtlChars::FromDynamic(Operations[i]);
		if (!ops.Compare("^")) {
			opIndex = FindOperationReverse(ops, expression);
		} else { // exponents are parsed top-down (exception from PEMDAS)
			opIndex = FindOperation(ops, expression);
		}
		if (opIndex != -1) {
			break;
		}
	}
	
	if (opIndex != -1) {
		
		OperationNode *opNode = new OperationNode;
		opNode->operation = expression[opIndex];
		opNode->left = NULL;
		opNode->right = NULL;
		
		mtlChars lexpr = mtlChars(expression, 0, opIndex);
		mtlChars rexpr = mtlChars(expression, opIndex + 1, expression.GetSize());

		if (expression[opIndex] == '-' && lexpr.GetSize() == 0) {
			lexpr = zero_str;
		}
		
		retval = GenerateTermTree(opNode->left, lexpr) && GenerateTermTree(opNode->right, rexpr);

		node = opNode;
	} /*else if (expression.GetChars()[0] == '(' && expression.GetChars()[expression.GetSize() - 1] == ')') {
		
		retval = GenerateTermTree(node, mtlChars(expression, 1, expression.GetSize() - 1));
		
	}*/ else { // STOPPING CONDITION
		
		ValueNode *valNode = new ValueNode;
		valNode->left = NULL;
		valNode->right = NULL;
		valNode->sym.value = 0.0f;
		valNode->sym.constant = true;
		if (!expression.ToFloat(valNode->sym.value)) {
			const Symbol *val = GetSymbol(expression);
			if (val != NULL) {
				valNode->sym.name     = val->name;
				valNode->sym.value    = val->value;
				valNode->sym.constant = val->constant;
			} else {
				retval = false;
			}
		}
		node = valNode;
	
	}
	return retval;
}

int mtlMathParser::FindOperation(const mtlChars &operations, const mtlChars &expression) const
{
	int braceStack = 0;
	for (int i = 0; i < expression.GetSize(); ++i) {
		char ch = expression.GetChars()[i];
		if (ch == '(') {
			++braceStack;
		} else if (ch == ')') {
			--braceStack;
		} else if (braceStack == 0 && operations.SameAsAny(ch)) { // contents of parenthesis are not parsed
			return i;
		}
	}
	return -1;
}

int mtlMathParser::FindOperationReverse(const mtlChars &operations, const mtlChars &expression) const
{
	int braceStack = 0;
	for (int i = expression.GetSize() - 1; i >= 0; --i) {
		char ch = expression.GetChars()[i];
		if (ch == ')') {
			++braceStack;
		} else if (ch == '(') {
			--braceStack;
		} else if (braceStack == 0 && operations.SameAsAny(ch)) { // contents of parenthesis are not parsed
			return i;
		}
	}
	return -1;
}

bool mtlMathParser::IsLegalNameConvention(const mtlChars &name) const
{
	if (name.GetSize() == 0) {
		return false;
	}
	for (int i = 1; i < name.GetSize(); ++i) {
		char ch = name.GetChars()[i];
		if (!mtlChars::IsAlphanumeric(ch) && ch != '_') { return false; }
	}
	char ch = name.GetChars()[0];
	if (!mtlChars::IsAlpha(ch) && ch != '_') {
		return false;
	}
	return true;
}

mtlMathParser::Symbol *mtlMathParser::GetSymbol(const mtlChars &name)
{
	mtlItem<Scope> *i = m_scope_stack.GetLast();
	Symbol *found_value = NULL;
	while (i != NULL) {
		found_value = i->GetItem().m_defs.GetEntry(name);
		if (found_value != NULL) { break; }
		i = i->GetPrev();
	}
	return found_value;
}

const mtlMathParser::Symbol *mtlMathParser::GetSymbol(const mtlChars &name) const
{
	const mtlItem<Scope> *i = m_scope_stack.GetLast();
	const Symbol *found_value = NULL;
	while (i != NULL) {
		found_value = i->GetItem().m_defs.GetEntry(name);
		if (found_value != NULL) { break; }
		i = i->GetPrev();
	}
	return found_value;
}

float *mtlMathParser::GetValue(const mtlChars &name)
{
	Symbol *sym = GetSymbol(name);
	return sym != NULL ? &sym->value : NULL;
}

const float *mtlMathParser::GetValue(const mtlChars &name) const
{
	const Symbol *sym = GetSymbol(name);
	return sym != NULL ? &sym->value : NULL;
}

bool mtlMathParser::SetConstant(const mtlChars &name, float value)
{
	Symbol *sym = GetSymbol(name);
	bool ret_val = false;
	if (sym != NULL) {
		ret_val = sym->constant;
	} else {
		ret_val = IsLegalNameConvention(name);
		if (ret_val) {
			if (m_scope_stack.GetSize() == 0) { PushScope(); }
			sym = m_scope_stack.GetLast()->GetItem().m_defs.CreateEntry(name);
			sym->name = name;
			sym->constant = true;
		}
	}
	if (ret_val) {
		sym->value = value;
	}
	return ret_val;
}

bool mtlMathParser::GetConstant(const mtlChars &name, float &value) const
{
	const Symbol *sym = GetSymbol(name);
	bool ret_val = sym != NULL && sym->constant;
	if (ret_val) {
		value = sym->value;
	}
	return ret_val;
}

bool mtlMathParser::SetVariable(const mtlChars &name, float value)
{
	Symbol *sym = GetSymbol(name);
	bool ret_val = false;
	if (sym != NULL) {
		ret_val = !sym->constant;
	} else {
		ret_val = IsLegalNameConvention(name);
		if (ret_val) {
			if (m_scope_stack.GetSize() == 0) { PushScope(); }
			sym = m_scope_stack.GetLast()->GetItem().m_defs.CreateEntry(name);
			sym->name = name;
			sym->constant = false;
		}
	}
	if (ret_val) {
		sym->value = value;
	}
	return ret_val;
}

bool mtlMathParser::GetVariable(const mtlChars &name, float &value) const
{
	const Symbol *sym = GetSymbol(name);
	bool ret_val = sym != NULL && !sym->constant;
	if (ret_val) {
		value = sym->value;
	}
	return ret_val;
}

bool mtlMathParser::Evaluate(const mtlChars &expression, float &value)
{
	TermNode *term_tree = NULL;

	mtlList<mtlChars> expr_sides;
	expression.SplitByChar(expr_sides, '=');
	mtlChars var_part;
	mtlChars expr_part;
	if (expr_sides.GetSize() > 1) {
		var_part = expr_sides.GetFirst()->GetItem().GetTrimmed();
		if (var_part.GetSize() <= 0) {
			value = 0.0f;
			return false;
		}
		expr_part = expr_sides.GetFirst()->GetNext()->GetItem();
	} else {
		expr_part = expr_sides.GetFirst()->GetItem();
	}
	bool success = (expr_sides.GetSize() <= 2) && IsBraceBalanced(expr_part) && IsLegalChars(expr_part) && GenerateTermTree(term_tree, expr_part);

	if (success) {
		value = term_tree->Evaluate();
		if (var_part.GetSize() > 0) {
			success = SetVariable(var_part, value);
		}
	} else {
		value = 0.0f;
	}

	DestroyTermTree(term_tree);
	return success;
}

int mtlMathParser::GetOrderOfOperations(const mtlChars &expression, mtlString &out, bool show_symbols)
{
	out.Free();
	int depth = 0;

	TermNode *term_tree = NULL;

	mtlList<mtlChars> expr_sides;
	expression.SplitByChar(expr_sides, '=');
	mtlChars var_part;
	mtlChars expr_part;
	if (expr_sides.GetSize() > 1) {
		var_part = expr_sides.GetFirst()->GetItem().GetTrimmed();
		if (var_part.GetSize() <= 0) {
			return false;
		}
		expr_part = expr_sides.GetFirst()->GetNext()->GetItem();
	} else {
		expr_part = expr_sides.GetFirst()->GetItem();
	}
	bool success = (expr_sides.GetSize() <= 2) && IsBraceBalanced(expr_part) && IsLegalChars(expr_part) && GenerateTermTree(term_tree, expr_part);

	if (success) {
		depth = term_tree->GetTermDepth(0) + 1;
		mtlArray<float> temp_var(depth);
		mtlArray<bool>  temp_var_init(depth);
		for (int i = 0; i < depth; ++i) {
			temp_var_init[i] = false;
		}
		term_tree->GetOrder(0, out, temp_var, temp_var_init, show_symbols);
	}

	DestroyTermTree(term_tree);
	return depth;
}

bool mtlMathParser::IsConstExpression(const mtlChars &expression)
{
	TermNode *term_tree = NULL;

	mtlList<mtlChars> expr_sides;
	expression.SplitByChar(expr_sides, '=');
	mtlChars var_part;
	mtlChars expr_part;
	if (expr_sides.GetSize() > 1) {
		var_part = expr_sides.GetFirst()->GetItem().GetTrimmed();
		if (var_part.GetSize() <= 0) {
			return false;
		}
		expr_part = expr_sides.GetFirst()->GetNext()->GetItem();
	} else {
		expr_part = expr_sides.GetFirst()->GetItem();
	}
	bool ret_val = (expr_sides.GetSize() <= 2) && IsBraceBalanced(expr_part) && IsLegalChars(expr_part) && GenerateTermTree(term_tree, expr_part);

	if (ret_val) {
		ret_val = term_tree->IsConstant();
	}

	DestroyTermTree(term_tree);
	return ret_val;
}

void mtlMathParser::Copy(const mtlMathParser &parser)
{
	const mtlItem<Scope> *i = parser.m_scope_stack.GetFirst();
	m_scope_stack.RemoveAll();
	while (i != NULL) {
		m_scope_stack.AddLast().m_defs.Copy(i->GetItem().m_defs);
		i = i->GetNext();
	}
}

void mtlMathParser::PushScope( void )
{
	m_scope_stack.AddLast();
}

void mtlMathParser::PopScope( void )
{
	if (m_scope_stack.GetSize() > 1) {
		m_scope_stack.RemoveLast();
	}
}

void mtlMathParser::ClearAllScopes( void )
{
	m_scope_stack.RemoveAll();
	m_scope_stack.AddLast();
}

void mtlMathParser::ClearLocalScopes( void )
{
	while (m_scope_stack.GetSize() > 1) {
		m_scope_stack.RemoveLast();
	}
}
