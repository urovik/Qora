#ifndef AST_H
#define AST_H



typedef struct {
    
    char* key;
    char* value;
    int TTL;

} SetAST;


SetAST* create_set(char *key, char* value, int TTL);
void freeSetAST(SetAST* ast);



#endif // AST_H