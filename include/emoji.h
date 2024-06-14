#ifndef EMOJI_H
#define EMOJI_H


#include <stddef.h>

typedef struct {
    const char *emoji;
    const char *keywords;
} EmojiEntry;


// Declaration of extern variables
extern const EmojiEntry emojis[];
extern const size_t NUM_EMOJIS;

#endif // EMOJI_H
