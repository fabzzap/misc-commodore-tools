BIN = makeat64 fixconv
MAKEAT64_OBJS = makeat64.o t64utils.o create_t64.o process_input_files.o prg2wav_utils.o block_list.o
FIXCONV_OBJS = fixconv.o t64utils.o

all: $(BIN)

makeat64: $(MAKEAT64_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

fixconv: $(FIXCONV_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(BIN) $(MAKEAT64_OBJS) $(FIXCONV_OBJS) *~
