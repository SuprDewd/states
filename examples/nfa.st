/*
    A state machine over the language {a,b}* that matches any string beginning
    with an 'a' and containing exactly one 'b'.
*/

alphabet a b;               /* Specify the alphabet */

/* This is an NFA starting at the 'empty' state */
nfa empty {

    /* Definition of the 'empty' state */
    state empty {

        /* When we get an 'a' on the empty state, we progress to the 'need_b'
         * state. */
        on a need_b;
    }

    state need_b {
        on a need_b;
        on b has_b;
    }

    /* This is an accept state */
    accept state has_b {
        on a has_b;
    }
}
