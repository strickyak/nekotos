# works with data from
# https://github.com/open-aviation-data/airports
#   py gen-airports.py < airports.csv > airports.go

import csv, sys

D = dict()
reader = csv.reader(sys.stdin)
first = True
for row in reader:
    airport, timezone = row[3], row[18]
    name, latitude, longitude = row[6], row[7], row[8]
    if first:
        assert airport == 'iata'
        assert timezone == 'timezone'
        assert name == 'name'
        assert latitude == 'latitude'
        assert longitude == 'longitude'
        first = False
    else:
        if len(airport) != 3: continue
        if not timezone: continue
        if not latitude: continue
        if not longitude: continue
        lat = float(latitude) if latitude else 0.0
        long = float(longitude) if longitude else 0.0
        D[airport] = (name.replace("`", "'"), timezone, float(latitude), float(longitude))

print('''
package data

type Airport struct {
    Name        string
    Timezone        string
    Latitude    float32
    Longitude   float32
}

var AirportMap = map[string]*Airport{
''')

for k, v in sorted(D.items()):
    print("  `%s`: {`%s`, `%s`, %f, %f}," % (k, *v))

print('''
}
''')
