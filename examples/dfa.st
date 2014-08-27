alphabet a b;
dfa empty {

    state empty {
        on b dead;
        on a need_b;
    }

    state dead {
        on a dead;
        on b dead;
    }

    state need_b {
        on a need_b;
        on b has_b;
    }

    accept state has_b {
        on b dead;
        on a has_b;
    }
}
