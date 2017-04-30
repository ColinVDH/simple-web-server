/*
 * This has not been implemented but does contain the
 * theoretical implementation of a web cache.
 * DOES NOT WORK
 */

/*

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define SIZE 1024

struct cachenode {
    char *req;
    void *val;
    struct cachenode *next;

};

struct cachetable {
    struct cachenode *size[SIZE];
    void ( *free_req )( char * );
    void ( *free_val )( void* );
};
 
void *get( struct cachetable *table, char *req )
{
    int store = table->( req )%SIZE;
    struct cacheode *node;

    node = table->store[store];

    while( node )
    {
        if( table->cmp( req, node->req ) == 0 )
            return node->value;
            
        node = node->next;
    }
    return NULL;
}


int insert( struct cacheable *table, char *req, void *val )
{
    int store = table->hash( req )%SIZE;
    struct cachenode **tmp;
    struct cachenode *node ;

    tmp = &table->store[store];
    
    while( *tmp )
    {
        if( table->cmp( req,( *tmp )->req ) == 0 )
            break;

        tmp = &( *tmp )->next;
    }
    
    if( *tmp )
    {
        if( table->free_req != NULL )
            table->free_req( ( *tmp )->req );

        if( table->free_val != NULL )
            table->free_val( ( *tmp )->val );

        node = *tmp;
    } else {
        node = malloc( sizeof *node );

        if( node == NULL )
            return -1;

        node->next = NULL;
        *tmp = node;
    }

    node->req = req;
    node->val = val;

    return 0;
}

int cache( const char *str )
{
   int tmp = 0;
    for( ; *str; str++ )
        tmp = 31*tmp + *str;
    return tmp;
}


int main(int argc,char *argv[])
{
    struct cachetable tbl = { {0}, NULL, NULL, cache, strcmp};

    insert( &tbl, "Test", "TestVal" );
    
    insert( &tbl, "Test2", "TestVal" );
    
    puts( get( &tbl, "Test" ) );
    
    insert( &tbl, "Test", "TestValUpdate" );
    
    puts( get( &tbl, "Test" ) );

    return 0;
}

*/
