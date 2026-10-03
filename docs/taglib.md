
# Database
Basic:    TITLE*, ALBUM*, ARTIST*, ALBUMARTIST*, TRACKNUMBER*,
          DISCNUMBER*, DATE*, GENRE*, COMMENT
Sort:     TITLESORT, ALBUMSORT, ARTISTSORT, ALBUMARTISTSORT, COMPOSERSORT
Credits:  COMPOSER*, LYRICIST, CONDUCTOR, REMIXER, PERFORMER:<role>
Other:    ISRC, ASIN, BPM, COPYRIGHT, ENCODEDBY, MOOD, MEDIA, LABEL,
          CATALOGNUMBER, BARCODE, RELEASECOUNTRY, RELEASESTATUS, RELEASETYPE

MUSICBRAINZ_TRACKID, MUSICBRAINZ_ALBUMID

COMPILATION

NAVIDROME_ID


Reading files: taglib is the only one that reads and writes to the db.

Api: reads all, only writes to the changes table
tag: reads/writes tracks, updates changes (specific fields)
sql: manages schema, manages read/writes, controls full flow of albums/artists tables (updates id FK on those).




