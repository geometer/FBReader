CREATE TRIGGER IF NOT EXISTS Books_Delete BEFORE DELETE 
ON Books FOR EACH ROW 
BEGIN
	DELETE FROM BookAuthor WHERE book_id = OLD.book_id;
	DELETE FROM BookSeries WHERE book_id = OLD.book_id;
	DELETE FROM BookTag WHERE book_id = OLD.book_id;
END;

CREATE TRIGGER IF NOT EXISTS Files_Delete BEFORE DELETE 
ON Files FOR EACH ROW 
BEGIN
	DELETE FROM Books WHERE file_id = OLD.file_id;
END;


CREATE TRIGGER IF NOT EXISTS Files_Unique_Insert BEFORE INSERT 
ON Files FOR EACH ROW 
WHEN NEW.parent_id IS NULL 
	AND 0 != (SELECT count(*) FROM Files AS f WHERE f.parent_id IS NULL AND f.name = NEW.name)
BEGIN
	SELECT RAISE(ABORT, "columns name, parent_id are not unique");
END;

CREATE TRIGGER IF NOT EXISTS Files_Directory_Insert BEFORE INSERT 
ON Files FOR EACH ROW 
WHEN NEW.parent_id IS NULL AND NEW.size IS NOT NULL
BEGIN
	SELECT RAISE(ABORT, "size is not null for Directory entry");
END;

CREATE TRIGGER IF NOT EXISTS Files_ArchEntry_Insert BEFORE INSERT 
ON Files FOR EACH ROW 
WHEN NEW.parent_id IS NOT NULL 
	AND 0 != (SELECT count(*) FROM Files AS f WHERE f.file_id = NEW.parent_id AND f.parent_id IS NOT NULL)
	AND NEW.size IS NOT NULL
BEGIN
	SELECT RAISE(ABORT, "size is not null for Archive Entry entry");
END;

CREATE TRIGGER IF NOT EXISTS Files_Unique_Update BEFORE UPDATE 
ON Files FOR EACH ROW 
WHEN NEW.parent_id IS NULL 
	AND 0 != (SELECT count(*) FROM Files AS f WHERE f.parent_id IS NULL AND f.name = NEW.name AND f.file_id != NEW.file_id)
BEGIN
	SELECT RAISE(ABORT, "columns name, parent_id are not unique");
END;

CREATE TRIGGER IF NOT EXISTS Files_Directory_Update BEFORE UPDATE 
ON Files FOR EACH ROW 
WHEN NEW.parent_id IS NULL AND NEW.size IS NOT NULL
BEGIN
	SELECT RAISE(ABORT, "size is not null for Directory entry");
END;

CREATE TRIGGER IF NOT EXISTS Files_ArchEntry_Update BEFORE UPDATE 
ON Files FOR EACH ROW 
WHEN NEW.parent_id IS NOT NULL 
	AND 0 != (SELECT count(*) FROM Files AS f WHERE f.file_id = NEW.parent_id AND f.parent_id IS NOT NULL)
	AND NEW.size IS NOT NULL
BEGIN
	SELECT RAISE(ABORT, "size is not null for Archive Entry entry");
END;

