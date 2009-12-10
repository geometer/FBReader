TARGETS = StatisticsGenerator PatternGenerator LanguageDetector

CFLAGS = -O0 -g

DIR= ./../../zlibrary/core/src/language 
ISTATDIR= -I /usr/include/zlibrary/core -I $(DIR) 

all: $(TARGETS)

.cpp.o:
	gcc $(CFLAGS) $(ISTATDIR) -c -Wall $<

StatisticsGenerator: generator.o
	g++ $(ISTATDIR) $^ -o $@ -lzlcore

LanguageDetector: detector.o
	g++ $(ISTATDIR) $^ -o $@ -lzlcore

PatternGenerator: pattern.o
	g++ $(ISTATDIR) $^ -o $@ -lzlcore

generator.o: generator.cpp 

detector.o: detector.cpp 

pattern.o: pattern.cpp

clean:
	rm -f $(TARGETS) *.o
