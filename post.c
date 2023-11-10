#include <common.h>

struct post
{
    int id;
    char content[MAX_LEN_CONTENT];
    int madeBy;
};

struct post initPost(int id, char content, int madeBy){
    struct post newPost;
    newPost.id = id;
    strcpy(newPost.content, content);
    newPost.madeBy = madeBy;
    return newPost;
}