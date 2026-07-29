#!/usr/bin/env bash

_git_complete() {
    local cur="${COMP_WORDS[COMP_CWORD]}"
    local candidates=()

    case "$cur" in
        re*)
            candidates=(rebase remote reset)
            ;;
        st*)
            candidates=(status stash)
            ;;
        *)
            candidates=(rebase remote reset status stash)
            ;;
    esac

    COMPREPLY=( $(compgen -W "${candidates[*]}" -- "$cur") )
}

complete -F _git_complete git
