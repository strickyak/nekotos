package textdb

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"path"
	"path/filepath"
	"sort"
	"strings"
	"time"

	// for Assert, Value
	. "github.com/strickyak/frobio/frob3/lemma/util"
)

const TIME_FORMAT = "2006-01-02-150506"
const FILENAME_FORMAT = "db-%s.tsv"
const FILENAME_GLOB = "db-*.tsv"

type TextDB struct {
	recs    map[string]*TextRec
	dirname string
	w       *os.File
}

type TextRec struct {
	key       string
	timestamp string
	value     string
}

func Open(dirname string) *TextDB {
	globPattern := filepath.Join(dirname, FILENAME_GLOB)

	db := &TextDB{
		recs:    make(map[string]*TextRec),
		dirname: dirname,
	}

	// Hint: filepath.Glob(globPattern string) (matches []string, err error)
	for _, filename := range Value(filepath.Glob(globPattern)) {
		db.SlurpFile(filename)
	}
	return db
}

func (db *TextDB) SlurpFile(filename string) {
	file := Value(os.Open(filename))
	defer file.Close()

	i := 1
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		db.AddLine(scanner.Text(), i, filename)
		i++
	}

	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}
}

func (db *TextDB) AddLine(line string, i int, filename string) {
	ww := strings.Split(line, "\t")
	// Want: ( key ts value )
	if len(ww) < 5 || ww[0] != "(" || !strings.HasPrefix(ww[4], ")") {
		if strings.HasPrefix("#", line) {
			return // Comment line starting with # is OK.
		}
		log.Fatalf("Bad TextDB line %q:%d:  %q", filename, i, line)
	}

	key, ts, value := ww[1], ww[2], ww[3]
	Assert(key != "")
	Assert(ts != "")
	Assert(value != "")

	if old, ok := db.recs[key]; ok {
		if old.timestamp > ts {
			return // newer record already exists
		}
	}
	db.recs[key] = &TextRec{key, ts, value}
}

// Get returns "" if not found.
func (db *TextDB) Get(key string) string {
	if rec, ok := db.recs[key]; ok {
		return rec.value // Found.
	}
	return "" // Not found.
}

func (db *TextDB) Put(key, value, remark string) {
	ts := time.Now().UTC().Format(TIME_FORMAT)
	db.recs[key] = &TextRec{key, ts, value}
	db.WriteToLogFile(key, ts, value, remark)
}

func (db *TextDB) WriteToLogFile(key, ts, value, remark string) {
	Assert(key != "")
	Assert(ts != "")
	Assert(value != "")
	if db.w == nil {
		filename := filepath.Join(db.dirname, Format(FILENAME_FORMAT, ts[:7]))
		db.w = Value(os.OpenFile(filename, os.O_WRONLY|os.O_APPEND|os.O_CREATE|os.O_SYNC, 0644))
	}
	_ = Value(fmt.Fprintf(db.w, "(\t%s\t%s\t%s\t)\t# %s\n", key, ts, value, remark))
}

func (db *TextDB) Keys(prefix string) []string {
	var keys []string
	for k := range db.recs {
		if strings.HasPrefix(k, prefix) {
			keys = append(keys, k)
		}
	}
	sort.Strings(keys)
	return keys
}
func (db *TextDB) MatchKeys(pattern string) []string {
	var keys []string
	for k := range db.recs {
		if Value(path.Match(pattern, k)) {
			keys = append(keys, k)
		}
	}
	sort.Strings(keys)
	return keys
}
