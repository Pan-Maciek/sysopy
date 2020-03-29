#define FORK_HEADER \
"                       │ fork() │       ignore       │      chandler      │        mask        │       pending      │\n"\
"┌────┬─────────────────┴────────┼─────────┬──────────┼─────────┬──────────┼─────────┬──────────┼─────────┬──────────┤\n"\
"│ no │                   signal │  child  │  parent  │  child  │  parent  │  child  │  parent  │  child  │  parent  │\n"\
"├────┼──────────────────────────┼─────────┼──────────┼─────────┼──────────┼─────────┼──────────┼─────────┼──────────┤\n"\

#define FORK_FOOTER \
"└────┴──────────────────────────┴─────────┴──────────┴─────────┴──────────┴─────────┴──────────┴─────────┴──────────┘\n"

#define EXEC_HEADER \
"                       │ exec() │       ignore       │        mask        │       pending      │\n"\
"┌────┬─────────────────┴────────┼─────────┬──────────┼─────────┬──────────┼─────────┬──────────┤\n"\
"│ no │                   signal │  child  │  parent  │  child  │  parent  │  child  │  parent  │\n"\
"├────┼──────────────────────────┼─────────┼──────────┼─────────┼──────────┼─────────┼──────────┤\n"

#define EXEC_FOOTER \
"└────┴──────────────────────────┴─────────┴──────────┴─────────┴──────────┴─────────┴──────────┘\n"
