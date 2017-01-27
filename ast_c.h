
#ifndef __AST_C_H__
#define __AST_C_H__


#define NODE_PTR  void *


/****************/
/* expressions: */
/****************/

NODE_PTR createTensorExpr(const NODE_PTR left, const NODE_PTR right);
NODE_PTR createDotExpr(const NODE_PTR left, const NODE_PTR right);
NODE_PTR createIdentifier(const char *name);
NODE_PTR createInteger(int value);
NODE_PTR createBrackExpr(const NODE_PTR list);
NODE_PTR createParenExpr(const NODE_PTR expr);


/**************/
/* statement: */
/**************/

NODE_PTR createStmt(const NODE_PTR id, const NODE_PTR expr);


/****************/
/* declaration: */
/****************/

NODE_PTR createVarDecl(const NODE_PTR id, const NODE_PTR expr);
NODE_PTR createTypeDecl(const NODE_PTR id, const NODE_PTR expr);


/*************/
/* program:: */
/*************/

NODE_PTR createProgram(const NODE_PTR decls, const NODE_PTR stmts);


/**********/
/* lists: */
/**********/

NODE_PTR createDeclList(const NODE_PTR d);
NODE_PTR appendDeclList(NODE_PTR list, const NODE_PTR d);
NODE_PTR createStmtList(const NODE_PTR s);
NODE_PTR appendStmtList(NODE_PTR list, const NODE_PTR s);
NODE_PTR createExprList();
NODE_PTR appendExprList(NODE_PTR list, const NODE_PTR e);


void printNode(const NODE_PTR n);

void printAST(const NODE_PTR p);
void destroyAST(const NODE_PTR p);

#endif /* __AST_C_H__ */
