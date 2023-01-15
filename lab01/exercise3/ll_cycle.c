#include <stddef.h>
#include "ll_cycle.h"

bool advancePtr(node** p) {
    if ((*p)->next) {
        *p = (*p)->next;
        return true;
    }
    return false;
}

int ll_has_cycle(node *head) {
    if (!head) return false;
    /* TODO: Implement ll_has_cycle */
    node* fastPtr = head;
    node* slowPtr = head;

    while(true) {
        bool advanced2 = advancePtr(&fastPtr);
        advanced2 = advancePtr(&fastPtr);
        if (!advanced2) return 0;

        advancePtr(&slowPtr);
        if (slowPtr == fastPtr) return 1;
    }
}
