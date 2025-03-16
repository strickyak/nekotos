package main

import (
    "fmt"
    "flag"
    "log"

    "github.com/strickyak/nekot-coco-microkernel/mcp/textdb"
)

var DIR = flag.String("dir", ".", "directory where to find database files")

func main() {
	flag.Parse()
    log.SetPrefix("textdb: ")

     db := textdb.Open(*DIR)

    switch flag.Arg(0) {
    case "get":
        key := flag.Arg(1)
        fmt.Println(db.Get(key))
    case "put":
        key, value := flag.Arg(1), flag.Arg(2)
        db.Put(key, value, "cli")
    case "keys":
        for _, k := range db.Keys("") {
            fmt.Println(k)
        }
    case "all":
        for _, k := range db.Keys("") {
            fmt.Printf("%s\t%s\n", k, db.Get(k))
        }
    case "prefix":
        prefix := flag.Arg(1)
        for _, k := range db.Keys(prefix) {
            fmt.Printf("%s\t%s\n", k, db.Get(k))
        }
    case "match":
        match := flag.Arg(1)
        for _, k := range db.MatchKeys(match) {
            fmt.Printf("%s\t%s\n", k, db.Get(k))
        }
    default:
        log.Fatalf(`Usage:
  textdb_cli get    KEY
  textdb_cli put    KEY VALUE
  textdb_cli keys
  textdb_cli prefix KEY_PREFIX
  textdb_cli match  KEY_PATTERN
`)
    }
}
