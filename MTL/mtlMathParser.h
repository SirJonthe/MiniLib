//
//  mtlMathParser.h
//  MiniLib
//
//  Created by Jonathan Karlsson on 10/14/14.
//  Copyright (c) 2014 Jonathan Karlsson. All rights reserved.
//

#ifndef MTL_MATHPARSER_H_INCLUDED__
#define MTL_MATHPARSER_H_INCLUDED__

#include "mtlString.h"
#include "mtlStringMap.h"

class mtlMathParser
{
private:
	struct TermNode
	{
		TermNode *left;
		TermNode *right;

		TermNode( void ) : left(NULL), right(NULL) {}

		virtual float Evaluate( void )        const = 0;
		virtual bool  IsConstant( void )      const = 0;
		virtual int   GetTermDepth(int depth) const = 0;
		virtual int   GetOrder(int depth, mtlString &out, float *temp_var, bool *temp_var_init, bool show_symbols) const = 0;
	};

	struct OperationNode : public TermNode
	{
		char operation;

		float Evaluate( void )        const;
		bool  IsConstant( void )      const;
		int   GetTermDepth(int depth) const;
		int   GetOrder(int depth, mtlString &out, float *temp_var, bool *temp_var_init, bool show_symbols) const;
	};

	struct Symbol
	{
		mtlChars name;
		float    value;
		bool     constant;
	};

	struct ValueNode : public TermNode
	{
		Symbol sym;

		float Evaluate( void )        const;
		bool  IsConstant( void )      const;
		int   GetTermDepth(int depth) const;
		int   GetOrder(int depth, mtlString &out, float *temp_var, bool *temp_var_init, bool show_symbols) const;
	};

	struct Scope
	{
		mtlStringMap<Symbol> m_defs;
	};

private:
	mtlList<Scope> m_scope_stack;

private:
	bool          IsBraceBalanced(const mtlChars &expression) const;
	bool          IsLegalChars(const mtlChars &expression) const;
	mtlChars      TrimBraces(const mtlChars &str) const;
	void          DestroyTermTree(TermNode *node);
	bool          GenerateTermTree(TermNode *&node, mtlChars expression);
	int           FindOperation(const mtlChars &operation, const mtlChars &expression) const;
	int           FindOperationReverse(const mtlChars &operations, const mtlChars &expression) const;
	bool          IsLegalNameConvention(const mtlChars &name) const;
	Symbol       *GetSymbol(const mtlChars &name);
	const Symbol *GetSymbol(const mtlChars &name) const;
	
public:
	mtlMathParser( void );

	float       *GetValue(const mtlChars &name);
	const float *GetValue(const mtlChars &name) const;

	bool         SetConstant(const mtlChars &name, float value);
	bool         GetConstant(const mtlChars &name, float &value) const;

	bool         SetVariable(const mtlChars &name, float value);
	bool         GetVariable(const mtlChars &name, float &value) const;

	bool         Evaluate(const mtlChars &expression, float &value);
	int          GetOrderOfOperations(const mtlChars &expression, mtlString &out, bool show_symbols = false);
	bool         IsConstExpression(const mtlChars &expression);
	
	void         Copy(const mtlMathParser &parser);

	void         PushScope( void );
	void         PopScope( void );
	void         ClearAllScopes( void );
	void         ClearLocalScopes( void );
};

#endif
