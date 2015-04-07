//
//  mtlMathParser.cpp
//  MiniLib
//
//  Created by Jonathan Karlsson on 10/14/14.
//  Copyright (c) 2014 Jonathan Karlsson. All rights reserved.
//

#include "mtlMathParser.h"
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

float mtlMathParser::ValueNode::Evaluate( void ) const
{
	return value;
}

mtlMathParser::mtlMathParser( void ) : /*m_expression(), m_result(0.0f), m_root(NULL),*/ m_scope_stack()
{
	m_scope_stack.AddLast();
}

/*mtlMathParser::~mtlMathParser( void )
{
	DestroyTermTree(m_root);
}*/

/*void mtlMathParser::SanitizeExpression( void )
{
	int skipped = 0;
	for (int i = 0; i < m_expression.GetSize(); ++i) {
		char ch = m_expression.GetChars()[i];
		if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
			++skipped;
		} else {
			m_expression.GetChars()[i - skipped] = ch;
		}
	}
	m_expression.SetSize(m_expression.GetSize() - skipped);
}*/

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
		if (!mtlChars::IsWhitespace(ch) && !mtlChars::IsAlphanumeric(ch) && ch != '_' && ch != '(' && ch != ')' && !mtlChars::IsMath(ch)) {
			return false;
		}
	}
	return true;
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

	expression.TrimBraces();

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
		valNode->value = 0.0f;
		if (!expression.ToFloat(valNode->value)) {
			const float *constant = GetValue(expression);
			if (constant != NULL) {
				valNode->value = *constant;
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

/*void mtlMathParser::SetConstant(const mtlChars &name, float value)
{
	if (IsLegalNameConvention(name)) {
		(*m_constants.CreateEntry(name)) = value;
	}
}

float mtlMathParser::GetConstant(const mtlChars &name) const
{
	const float *value = m_constants.GetEntry(name);
	return value != NULL ? *value : 0.0f;
}*/

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
			sym = m_scope_stack.GetLast()->GetItem().m_defs.CreateEntry(name);
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
			sym = m_scope_stack.GetLast()->GetItem().m_defs.CreateEntry(name);
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
	TermNode *term_tree;

	mtlList<mtlChars> expr_sides;
	expression.SplitByChar(expr_sides, '=');
	mtlChars var_part;
	mtlChars expr_part;
	if (expr_sides.GetSize() > 1) {
		var_part = expr_sides.GetFirst()->GetItem().GetTrimmed();
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

/*bool mtlMathParser::SetExpression(const mtlChars &expression)
{
	DestroyTermTree(m_root);
	m_root = NULL;

	m_expression.Copy(expression);
	SanitizeExpression();

	bool retVal = IsBraceBalanced() && IsLegalChars() && GenerateTermTree(m_root, m_expression);
	if (!retVal) {
		m_expression.Free();
		DestroyTermTree(m_root);
		m_root = NULL;
	}

	return retVal;
}

const mtlString &mtlMathParser::GetExpression( void ) const
{
	return m_expression;
}*/

/*void mtlMathParser::CopyConstants(const mtlMathParser &expr)
{
	m_constants.Copy(expr.m_constants);
}*/

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

/*float mtlMathParser::Evaluate( void ) const
{
	float result = 0.0f;
	if (m_root != NULL) {
		result = m_root->Evaluate();
	}
	return result;
}*/
