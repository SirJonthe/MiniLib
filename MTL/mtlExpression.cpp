//
//  mtlExpression.cpp
//  MiniLib
//
//  Created by Jonathan Karlsson on 10/14/14.
//  Copyright (c) 2014 Jonathan Karlsson. All rights reserved.
//

#include "mtlExpression.h"
#include <cmath>

float mtlExpression::OperationNode::Evaluate( void ) const
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

float mtlExpression::ValueNode::Evaluate( void ) const
{
	return value;
}

mtlExpression::mtlExpression( void ) : m_expression(), m_result(0.0f), m_root(NULL)
{}

mtlExpression::~mtlExpression( void )
{
	DestroyTermTree(m_root);
}

void mtlExpression::SanitizeExpression( void )
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
}

bool mtlExpression::IsBraceBalanced( void ) const
{
	int stack = 0;
	for (int i = 0; i < m_expression.GetSize(); ++i) {
		char ch = m_expression.GetChars()[i];
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

bool mtlExpression::IsLegalChars( void ) const
{
	for (int i = 0; i < m_expression.GetSize(); ++i) {
		char ch = m_expression.GetChars()[i];
		if (!mtlChars::IsAlphanumeric(ch) && ch != '_' && ch != '(' && ch != ')' && !mtlChars::IsMath(ch)) {
			return false;
		}
	}
	return true;
}

void mtlExpression::DestroyTermTree(mtlExpression::TermNode *node)
{
	if (node == NULL) { return; }
	DestroyTermTree(node->left);
	DestroyTermTree(node->right);
	delete node;
}

bool mtlExpression::GenerateTermTree(mtlExpression::TermNode *& node, const mtlChars &expression)
{
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
		
		retval = GenerateTermTree(opNode->left, lexpr) && GenerateTermTree(opNode->right, rexpr);

		node = opNode;
	} else if (expression.GetChars()[0] == '(' && expression.GetChars()[expression.GetSize() - 1] == ')') {
		
		retval = GenerateTermTree(node, mtlChars(expression, 1, expression.GetSize() - 1));
		
	} else { // STOPPING CONDITION
		
		ValueNode *valNode = new ValueNode;
		valNode->left = NULL;
		valNode->right = NULL;
		valNode->value = 0.0f;
		if (!expression.ToFloat(valNode->value)) {
			const float *constant = m_constants.GetEntry(expression);
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

int mtlExpression::FindOperation(const mtlChars &operations, const mtlChars &expression) const
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

int mtlExpression::FindOperationReverse(const mtlChars &operations, const mtlChars &expression) const
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

bool mtlExpression::IsLegalNameConvention(const mtlChars &name) const
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

void mtlExpression::SetConstant(const mtlChars &name, float value)
{
	if (IsLegalNameConvention(name)) {
		(*m_constants.CreateEntry(name)) = value;
	}
}

float mtlExpression::GetConstant(const mtlChars &name) const
{
	const float *value = m_constants.GetEntry(name);
	return value != NULL ? *value : 0.0f;
}

bool mtlExpression::SetExpression(const mtlChars &expression)
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

const mtlString &mtlExpression::GetExpression( void ) const
{
	return m_expression;
}

void mtlExpression::CopyConstants(const mtlExpression &expr)
{
	m_constants.Copy(expr.m_constants);
}

float mtlExpression::Evaluate( void ) const
{
	float result = 0.0f;
	if (m_root != NULL) {
		result = m_root->Evaluate();
	}
	return result;
}
