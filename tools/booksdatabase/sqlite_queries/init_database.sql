CREATE TABLE IF NOT EXISTS Files (
	file_id INTEGER PRIMARY KEY,
	name TEXT NOT NULL,
	parent_id INTEGER REFERENCES Files (file_id),
	size INTEGER,
	CONSTRAINT Files_Unique UNIQUE (name, parent_id)
);

CREATE TABLE IF NOT EXISTS Books (
	book_id INTEGER PRIMARY KEY,
	encoding TEXT,
	language TEXT,
	title TEXT NOT NULL,
	file_id INTEGER UNIQUE NOT NULL REFERENCES Files (file_id)
);

CREATE TABLE IF NOT EXISTS Authors (
	author_id INTEGER PRIMARY KEY,
	name TEXT NOT NULL,
	sort_key TEXT NOT NULL,
	CONSTRAINT Authors_Unique UNIQUE (name, sort_key)
);

CREATE TABLE IF NOT EXISTS Series (
	series_id INTEGER PRIMARY KEY,
	name TEXT UNIQUE NOT NULL
);

CREATE TABLE IF NOT EXISTS Tags (
	tag_id INTEGER PRIMARY KEY,
	name TEXT NOT NULL,
	parent_id INTEGER REFERENCES Tags (tag_id),
	CONSTRAINT Tags_Unique UNIQUE (parent_id, name)
);

CREATE TABLE IF NOT EXISTS BookAuthor (
	author_id INTEGER NOT NULL REFERENCES Authors (author_id),
	book_id INTEGER NOT NULL REFERENCES Books (book_id),
	author_index INTEGER NOT NULL,
	CONSTRAINT BookAuthor_Unique0 UNIQUE (author_id, book_id),
	CONSTRAINT BookAuthor_Unique1 UNIQUE (book_id, author_index)
);

CREATE TABLE IF NOT EXISTS BookSeries (
	book_id INTEGER UNIQUE NOT NULL REFERENCES Books (book_id),
	series_id INTEGER NOT NULL REFERENCES Series (series_id),
	book_index INTEGER
);

CREATE TABLE IF NOT EXISTS BookTag (
	book_id INTEGER NOT NULL REFERENCES Books (book_id),
	tag_id INTEGER NOT NULL REFERENCES Tags (tag_id),
	CONSTRAINT BookTag_Unique UNIQUE (book_id, tag_id)
);

CREATE TABLE IF NOT EXISTS State.RecentBooks (
	book_index INTEGER PRIMARY KEY,
	book_id INTEGER UNIQUE REFERENCES Books (book_id)
);

CREATE TABLE IF NOT EXISTS State.BookStateStack (
	book_id INTEGER NOT NULL REFERENCES Books (book_id),
	position INTEGER NOT NULL,
	paragraph INTEGER NOT NULL,
	word INTEGER NOT NULL,
	char INTEGER NOT NULL,
	CONSTRAINT BookStateStack_Unique UNIQUE (book_id, position)
);

CREATE TABLE IF NOT EXISTS PalmType (
	file_id INTEGER UNIQUE REFERENCES Files (file_id),
	type TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS Net.NetFiles (
	url TEXT PRIMARY KEY, 
	file_id INTEGER UNIQUE REFERENCES Files (file_id),
);

CREATE TABLE IF NOT EXISTS State.StackPosition (
	book_id INTEGER UNIQUE NOT NULL REFERENCES Books (book_id),
	stack_pos INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS State.BookList (
	book_id INTEGER UNIQUE NOT NULL REFERENCES Books (book_id)
);

