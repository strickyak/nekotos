trigger != "" {
    if ($1 == ".word") {
        print trigger "=" $2
    } else {
        print "ERROR: GOT '", $0, "' for ", trigger
    }
    trigger = ""
}

$1 == "_n1pre_regions:" { trigger = "REGIONS_ADDR" }

$1 == "_n1pre_screens:" { trigger = "SCREENS_ADDR" }
