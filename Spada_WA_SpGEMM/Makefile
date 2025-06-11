all: spada trace_logger extract_mapper

spada: spada_wadf_final.c
	$(CC) -O2 -o spada spada_wadf_final.c

trace_logger: trace_logger.c
	$(CC) -O2 -o trace_logger trace_logger.c

extract_mapper: extract_mapper.c
	$(CC) -O2 -o extract_mapper extract_mapper.c

clean:
	rm -f spada trace_logger extract_mapper trace.txt a_row_index.txt addr_to_arow.map
