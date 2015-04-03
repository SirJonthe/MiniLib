//
//  mtlExpression.h
//  MiniLib
//
//  Created by Jonathan Karlsson on 10/14/14.
//  Copyright (c) 2014 Jonathan Karlsson. All rights reserved.
//

#ifndef MTL_EXPRESSION_H_INCLUDED__
#define MTL_EXPRESSION_H_INCLUDED__

#include "mtlString.h"
#include "mtlStringMap.h"

class mtlExpression
{
private:
	struct TermNode
	{
		TermNode *left;
		TermNode *right;

		virtual float Evaluate( void ) const = 0;
	};

	struct OperationNode : public TermNode
	{
		char operation;

		float Evaluate( void ) const;
	};

	struct ValueNode : public TermNode
	{
		float value;

		float Evaluate( void ) const;
	};

	struct Scope
	{
		mtlStringMap<float> m_constants;
		mtlStringMap<float> m_variables;
	};

private:
	mtlString			m_expression;
	float				m_result;
	TermNode			*m_root;
	mtlStringMap<float>	m_constants;
	mtlList<Scope>		m_scope_stack;
	// mtlReference to parent constants (this implements scopes)
	// requires mtlStringMap<float> to be a mtlShared<mtlStringMap<float>>

private:
	void		SanitizeExpression( void );
	bool		IsBraceBalanced( void ) const;
	bool		IsLegalChars( void ) const;
	void		DestroyTermTree(TermNode *node);
	bool		GenerateTermTree(TermNode *& node, const mtlChars &expression);
	int			FindOperation(const mtlChars &operation, const mtlChars &expression) const;
	int			FindOperationReverse(const mtlChars &operations, const mtlChars &expression) const;
	bool		IsLegalNameConvention(const mtlChars &name) const;
	
public:
					mtlExpression( void );
					~mtlExpression( void );

	void			SetConstant(const mtlChars &name, float value);
	float			GetConstant(const mtlChars &name) const;

	//bool			SetConstant(const mtlChars &name, float value);
	//bool			GetConstant(const mtlChars &name, float &value);

	bool			SetVariable(const mtlChars &name, float value);
	bool			GetVariable(const mtlChars &name, float &value);

	//bool			Evaluate(const mtlChars &expression, float &value);

	bool			SetExpression(const mtlChars &expression);
	const mtlString	&GetExpression( void ) const;
	
	void			CopyConstants(const mtlExpression &expr);

	void			PushScope( void );
	void			PopScope( void );
	void			ClearAllScopes( void );
	void			ClearLocalScopes( void );

	float			Evaluate( void ) const;
};

#endif
