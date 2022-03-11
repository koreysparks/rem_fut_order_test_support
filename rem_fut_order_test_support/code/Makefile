CXX=g++
AR=ar
OUT_NAME=rem_fut_order_test_support

OUT_PATH=../lib/
SRC_PATH=./
OBJ_PATH=./obj/

BOOST=$(BOOST_1_53_0)

DEBUG_PATH=debug/
RELEASE_PATH=release/

PUBLISH_PATH=../bin/linux/

INC_PATH=-I./ -I$(BOOST)

LIB_DEBUG_FILE= -lpthread -lrt -ldl  \
		-L$(BASE_JYS) -lbase_jys -lEESQuoteApi -lEESTraderApi \
		$(BOOST)/stage/lib_linux64/libboost_thread-mt-d.a \
		$(BOOST)/stage/lib_linux64/libboost_system-mt-d.a \

LIB_RELEASE_FILE= -lpthread \

SRC_FILE=$(wildcard $(SRC_PATH) *.cpp)
ALL_FILE=$(notdir $(SRC_FILE))
OBJ_FILE=$(patsubst %.cpp,%.o,$(ALL_FILE))

DEBUG_CXXFLAGS=-g -ggdb3  -Wall -fPIC  
RELEASE_CXXFLAGS=-O2 -fPIC

DEBUG_ARFLAGS=cqs
RELEASE_ARFLAGS=cqs

OUT_DEBUG_FILE=$(OUT_PATH)$(OUT_NAME)
OBJ_DEBUG_FILE=$(addprefix  $(OBJ_PATH)$(DEBUG_PATH),$(notdir $(OBJ_FILE)))

OUT_RELEASE_FILE=$(OUT_PATH)$(RELEASE_PATH)$(OUT_NAME)
OBJ_RELEASE_FILE=$(addprefix  $(OBJ_PATH)$(RELEASE_PATH),$(notdir $(OBJ_FILE)))

debug : pre_debug $(OUT_DEBUG_FILE)

release : per_release $(OUT_RELEASE_FILE)

all : debug release

publish:  pre_publish

pre_debug :
	-$(shell mkdir $(OBJ_PATH) -p)
	-$(shell mkdir $(OBJ_PATH)$(DEBUG_PATH) -p)
	-$(shell mkdir $(OUT_PATH) -p)
	-$(shell cp  $(SL_EFH_API_LIB_PATH)* $(OUT_PATH)$(DEBUG_PATH) -p)

per_release :
	-$(shell mkdir $(OBJ_PATH) -p)
	-$(shell mkdir $(OBJ_PATH)$(RELEASE_PATH) -p)
	-$(shell mkdir $(OUT_PATH) -p)
	-$(shell mkdir $(OUT_PATH)$(RELEASE_PATH) -p)

$(OUT_DEBUG_FILE) : $(OBJ_DEBUG_FILE)
	$(CXX) $(DEBUG_CXXFLAGS) $(addprefix  $(OBJ_PATH)$(DEBUG_PATH),$(notdir $^)) -o $@ $(LIB_DEBUG_FILE)
#	$(AR) $(DEBUG_ARFLAGS) $(OUT_DEBUG_FILE) $(addprefix  $(OBJ_PATH)$(DEBUG_PATH),$(notdir $^)) $@ $(LIB_DEBUG_FILE)

$(OUT_RELEASE_FILE) : $(OBJ_RELEASE_FILE)
	$(CXX) $(RELEASE_CXXFLAGS) $(addprefix  $(OBJ_PATH)$(RELEASE_PATH),$(notdir $^)) -o $@ $(LIB_RELEASE_FILE)
#	$(AR) $(RELEASE_ARFLAGS) $(OUT_RELEASE_FILE) $(addprefix  $(OBJ_PATH)$(RELEASE_PATH),$(notdir $^)) $@ $(LIB_RELEASE_FILE)

$(OBJ_PATH)$(DEBUG_PATH)%.o : %.cpp
	$(CXX) -c $(DEBUG_CXXFLAGS) $(INC_PATH) $(SRC_PATH)$< -o $@
	
$(OBJ_PATH)$(RELEASE_PATH)%.o : %.cpp
	$(CXX) -c $(RELEASE_CXXFLAGS) $(INC_PATH) $(SRC_PATH)$< -o $@

pre_publish:
	$(shell cp $(OUT_PATH)$(DEBUG_PATH)$(OUT_NAME) $(PUBLISH_PATH) -p)
	@echo "publish finish"
	
clean :
	@echo  "make clean"
	$(shell rm $(OBJ_PATH) -rf)
	$(shell rm $(OUT_PATH)$(DEBUG_PATH)$(OUT_NAME) -f)
	$(shell rm $(OUT_PATH)$(RELEASE_PATH)$(OUT_NAME) -f)
