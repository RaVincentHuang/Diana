include ./scripts/config.mk
include ./scripts/antlr.mk

vpath %.cpp lib/Frontend:lib/Support:lib/IR/LLVM:lib/IR:lib/Target:lib/Analysis:lib/Transform:lib/Transform/Utils:lib/Transform/Scalar:.
vpath %.h   .

BUILD_DIR = ./build
ANTLR_DIR = antlr
SYSYC_DIR = sysy

BUILD_ANTLR_OBJ = $(addprefix $(BUILD_DIR)/$(ANTLR_DIR)/, $(ANTLR_OBJ))

FRONTEND_OBJ = SysYParser.o SysYLexer.o Frontend.o Ast.o SysYVisitor.o AstDump.o AstTransform.o
SUPPORT_OBJ = strUtils.o TypeUtils.o
IR_OBJ = ValueScope.o Type.o Module.o User.o Value.o Builder.o IRGen.o Constant.o BasicBlock.o \
			GlobalValue.o Instructions.o Use.o Function.o IRDump.o PassManager.o
TARGET_OBJ = Block.o MCFunction.o MCInstruction.o MCModule.o Reg.o Regallocator.o
ANALYSIS_OBJ = DomInfo.o Vertify.o Analysis.o Loop.o LoopInfo.o AliasAnalysis.o IPAnalysis.o
TRANSFORM_OBJ = Memory2Reg.o GVN.o SimplifyInstructions.o DCE.o EliminatePhi.o Transform.o MemorySSA.o

BUILD_SYSYC_FRONTEND_OBJ = $(addprefix $(BUILD_DIR)/$(SYSYC_DIR)/, $(FRONTEND_OBJ))
BULID_SYSYC_SUPPORT_OBJ = $(addprefix $(BUILD_DIR)/$(SYSYC_DIR)/, $(SUPPORT_OBJ))
BULID_SYSYC_IR_OBJ = $(addprefix $(BUILD_DIR)/$(SYSYC_DIR)/, $(IR_OBJ))
BULID_SYSYC_TARGET_OBJ = $(addprefix $(BUILD_DIR)/$(SYSYC_DIR)/, $(TARGET_OBJ))
BUILD_SYSYC_ANALYSIS_OBJ = $(addprefix $(BUILD_DIR)/$(SYSYC_DIR)/, $(ANALYSIS_OBJ))
BUILD_SYSYC_TRANSFORM_OBJ = $(addprefix $(BUILD_DIR)/$(SYSYC_DIR)/, $(TRANSFORM_OBJ))

SYSYC_OBJ = $(BUILD_DIR)/$(SYSYC_DIR)/sysyc.o $(BUILD_SYSYC_FRONTEND_OBJ)  \
	$(BULID_SYSYC_SUPPORT_OBJ) $(BULID_SYSYC_IR_OBJ) $(BULID_SYSYC_TARGET_OBJ) \
	$(BUILD_SYSYC_ANALYSIS_OBJ) $(BUILD_SYSYC_TRANSFORM_OBJ)

SYSYC = sysyc

default : $(SYSYC)

antlr : $(BUILD_ANTLR_OBJ)

$(BUILD_DIR)/$(ANTLR_DIR)/%.o : %.cpp
	@echo CXX +$@
	@$(CXX) -c $(CXXFLAGS) $(COMM_FLAGS) $< -o $@

$(BUILD_DIR)/$(SYSYC_DIR)/%.o : %.cpp
	@echo CXX +$@
	$(CXX) -c $(CXXFLAGS) $(COMM_FLAGS) $< -o $@


%.o : %.cpp
	@echo CXX +$@
	@$(CXX) -c $(CXXFLAGS) $(COMM_FLAGS) $< -o $@

$(SYSYC) : $(SYSYC_OBJ) $(BUILD_ANTLR_OBJ)
	@echo Linking $@
	@$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(COMM_FLAGS) $^ $(LIBFLAGS)

test: $(BUILD_SYSYC_ANALYSIS_OBJ) $(BUILD_SYSYC_FRONTEND_OBJ) $(BULID_SYSYC_SUPPORT_OBJ) $(BULID_SYSYC_IR_OBJ) $(BULID_SYSYC_TARGET_OBJ) $(BUILD_ANTLR_OBJ) test.o
	@echo Linking $@
	@$(CXX) -o a.out $(CXXFLAGS) $(LDFLAGS) $(COMM_FLAGS) $^ $(LIBFLAGS)

clean:
	rm -rf build/sysy/*.o sysyc

clean-all:
	rm -rf build/sysy/*.o build/antlr/*.o sysyc

init:
	$(ANTLR4) lib/Frontend/SysYLexer.g4 lib/Frontend/SysYParser.g4
