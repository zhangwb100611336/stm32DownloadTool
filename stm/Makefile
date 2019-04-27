CURRENT_PATH = $(shell pwd)
CURRENT_SOURCE_FILE = $(subst ./,,$(shell find  -name \*.c))
CURRENT_DEP_FILE =  $(patsubst %.c,%.d,$(CURRENT_SOURCE_FILE))
CURRENT_OBJ_FILE =  $(patsubst %.c,%.o,$(CURRENT_SOURCE_FILE))

CURRENT_OBJ_PATH = $(addprefix $(BASE_OBJ_PATH),$(subst $(BASE_PATH),,$(CURRENT_PATH)))
CURRENT_DEP_PATH = $(addprefix $(BASE_DEP_PATH),$(subst $(BASE_PATH),,$(CURRENT_PATH)))
CURRENT_OBJ_PATH_K = $(subst /,\/,$(CURRENT_OBJ_PATH))
CURRENT_DEP_PATH_K = $(subst /,\/,$(CURRENT_DEP_PATH))

CURRENT_OBJ_PATH_FILE = $(addprefix $(CURRENT_OBJ_PATH)/,$(CURRENT_OBJ_FILE))
CURRENT_DEP_PATH_FILE = $(addprefix $(CURRENT_DEP_PATH)/,$(CURRENT_DEP_FILE))

.PHONY : all debug
#all:$(CURRENT_OBJ_FILE) 
#all:$(CURRENT_OBJ_PATH_FILE) $(CURRENT_DEP_FILE)
all: $(CURRENT_DEP_PATH_FILE)

#$(CURRENT_OBJ_FILE):%.o:%.c
$(CURRENT_OBJ_PATH_FILE):
	$(CC) $(CFLAGS) -c $< -o $@ 

#-include $(CURRENT_DEP_PATH_FILE)


##$(CURRENT_DEP_FILE):%.d:%.c
$(CURRENT_DEP_PATH_FILE) : $(CURRENT_DEP_PATH)/%.d : $(CURRENT_PATH)/%.c
#	test -d $(CURRENT_OBJ_PATH) || mkdir -p $(CURRENT_OBJ_PATH)
#	test -d $(CURRENT_DEP_PATH) || mkdir -p $(CURRENT_DEP_PATH)
#	$(CC) -MM $< $(CFLAGS) > $@.$$$$;\
#	sed "s/$*\.o/$(CURRENT_OBJ_PATH_K)\/$*\.o $@/" <$@.$$$$ >$(CURRENT_DEP_PATH)/$@;\
#	rm $@.$$$$
	@echo "hello"


all:
	@echo $(CURRENT_SOURCE_FILE)
	@echo $(CURRENT_DEP_FILE)
	@echo $(CURRENT_OBJ_FILE)
	@echo $(CURRENT_PATH)
	@echo $(BASE_OBJ_PATH)
	@echo $(CURRENT_OBJ_PATH)
	@echo $(CURRENT_OBJ_PATH_FILE)
	@echo $(BASE_DEP_PATH)
	@echo $(CURRENT_DEP_PATH)
	@echo $(CURRENT_DEP_PATH_FILE)
