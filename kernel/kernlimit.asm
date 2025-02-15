; This area is scripted to be the last.
; That lets us determine the last address
;   used by the kernel (the Kern_Limit).

    .area .kernlimit
    fcb 0,0                ; two bytes for a canary
    .globl _Kern_Limit
_Kern_Limit:
