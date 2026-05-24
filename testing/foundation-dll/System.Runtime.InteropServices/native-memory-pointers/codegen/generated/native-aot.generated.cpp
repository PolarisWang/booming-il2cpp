#include <chaos/common.h>
#include <chaos/type_info.h>
#include "runtime_core.h"
#include <chaos/eh.h>
#include "com_ccw.h"
#include "codegen_bridge.h"
#include "module_registry.h"
#include "abi_manifest.h"
#include "hotpatch_table.h"
#include "runtime_vtable.h"
#include "runtime_instantiation.h"
#include "reflection_query_model.h"
#include "load_store_chaos_bridge.h"
#include "interpreter_entry.h"
#include <gc/gc_bgc_inline.h>
#include <gc/gc_card_table.h>
#include <ChaosGeneratedRuntimePrelude.h>
#include "enum_metadata.generated.h"
#include "runtime_stubs/enum_stubs.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_121(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_123(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_133(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_138(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_167(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_168(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_195(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_196(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_25(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_27(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_29(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_37(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_41(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_49(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_60(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_63(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_65(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_76(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_90(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_91(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_0(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_1(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_10(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_100(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_101(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_102(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_103(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_104(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_105(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_106(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_107(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_108(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_109(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_11(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_110(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_111(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_112(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_113(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_114(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_115(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_116(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_117(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_118(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_119(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_12(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_120(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_122(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_124(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_125(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_126(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_127(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_128(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_129(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_13(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_130(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_131(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_132(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_134(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_135(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_136(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_137(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_139(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_14(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_140(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_141(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_142(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_143(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_144(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_145(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_146(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_147(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_148(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_149(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_15(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_150(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_151(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_152(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_153(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_154(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_155(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_156(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_157(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_158(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_159(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_16(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_160(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_161(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_162(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_163(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_164(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_165(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_166(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_169(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_17(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_170(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_171(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_172(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_173(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_174(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_175(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_176(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_177(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_178(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_179(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_18(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_180(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_181(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_182(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_183(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_184(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_185(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_186(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_187(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_188(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_189(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_19(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_190(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_191(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_192(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_193(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_194(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_197(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_198(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_2(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_20(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_21(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_22(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_23(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_24(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_26(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_28(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_3(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_30(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_31(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_32(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_33(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_34(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_35(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_36(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_38(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_39(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_4(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_40(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_42(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_43(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_44(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_45(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_46(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_47(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_48(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_5(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_50(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_51(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_52(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_53(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_54(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_55(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_56(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_57(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_58(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_59(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_6(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_61(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_62(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_64(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_66(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_67(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_68(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_69(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_7(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_70(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_71(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_72(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_73(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_74(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_75(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_77(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_78(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_79(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_8(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_80(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_81(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_82(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_83(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_84(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_85(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_86(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_87(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_88(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_89(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_9(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_92(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_93(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_94(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_95(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_96(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_97(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_98(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_99(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_121(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_123(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_133(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_138(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_167(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_168(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_195(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_196(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_25(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_27(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_29(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_37(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_41(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_49(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_60(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_63(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_65(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_76(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_90(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_91(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_0(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_1(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_10(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_100(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_101(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_102(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_103(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_104(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_105(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_106(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_107(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_108(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_109(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_11(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_110(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_111(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_112(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_113(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_114(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_115(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_116(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_117(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_118(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_119(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_12(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_120(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_122(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_124(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_125(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_126(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_127(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_128(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_129(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_13(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_130(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_131(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_132(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_134(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_135(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_136(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_137(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_139(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_14(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_140(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_141(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_142(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_143(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_144(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_145(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_146(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_147(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_148(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_149(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_15(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_150(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_151(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_152(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_153(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_154(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_155(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_156(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_157(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_158(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_159(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_16(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_160(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_161(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_162(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_163(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_164(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_165(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_166(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_169(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_17(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_170(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_171(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_172(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_173(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_174(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_175(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_176(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_177(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_178(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_179(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_18(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_180(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_181(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_182(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_183(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_184(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_185(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_186(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_187(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_188(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_189(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_19(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_190(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_191(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_192(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_193(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_194(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_197(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_198(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_2(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_20(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_21(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_22(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_23(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_24(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_26(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_28(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_3(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_30(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_31(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_32(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_33(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_34(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_35(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_36(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_38(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_39(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_4(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_40(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_42(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_43(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_44(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_45(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_46(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_47(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_48(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_5(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_50(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_51(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_52(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_53(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_54(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_55(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_56(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_57(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_58(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_59(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_6(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_61(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_62(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_64(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_66(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_67(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_68(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_69(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_7(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_70(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_71(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_72(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_73(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_74(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_75(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_77(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_78(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_79(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_8(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_80(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_81(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_82(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_83(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_84(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_85(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_86(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_87(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_88(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_89(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_9(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_92(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_93(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_94(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_95(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_96(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_97(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_98(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_99(void);

namespace chaos::il2cpp::codegen::NativeMemoryPointersSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects = {{nullptr, nullptr, 16729351636441173064ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(16729351636441173064ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Decimal = {{nullptr, nullptr, 1126810584598443822ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Decimal = static_cast<CHAOS_IL2CPP_INTPTR>(1126810584598443822ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Guid = {{nullptr, nullptr, 17983019571512764410ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Guid = static_cast<CHAOS_IL2CPP_INTPTR>(17983019571512764410ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_IntPtr = {{nullptr, nullptr, 15622295425673451578ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_IntPtr = static_cast<CHAOS_IL2CPP_INTPTR>(15622295425673451578ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {{nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {{nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {{nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {{nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Type = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 3222174513575444759ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Type = static_cast<CHAOS_IL2CPP_INTPTR>(3222174513575444759ULL);
inline TypeInfoV0 chaos_mt_System_Runtime_InteropServices_System_IO_UnmanagedMemoryAccessor = {{nullptr, nullptr, 13170727875684058665ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Runtime_InteropServices_System_IO_UnmanagedMemoryAccessor = static_cast<CHAOS_IL2CPP_INTPTR>(13170727875684058665ULL);

// ── Virtual method table arrays ──
bool chaos_is_array_store_compatible(const chaos_managed_array* chaos_array, CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
	if (chaos_array == nullptr)
	{
		return false;
	}

	if (chaos_value == 0)
	{
		return true;
	}

	// StringId fast path: materialized value carries a heap pointer;
	// this line is reached only if caller skipped materialization.
	if (chaos_is_string_id(chaos_value))
	{
		return chaos_array->element_type_shape == chaos_type_shape_reference            && chaos_is_type_compatible(&chaos_mt_System_Private_CoreLib_System_String.hot, chaos_array->element_type_info);
	}

	auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
	if (chaos_array->element_type_shape == chaos_type_shape_interface)
	{
		return chaos_does_type_implement_interface(chaos_object_get_type_info(chaos_header), chaos_array->element_type_info);
	}

	if (chaos_array->element_type_shape == chaos_type_shape_reference)
	{
		return chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_array->element_type_info);
	}

	return false;
}

struct chaos_type_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Decimal
{
	PureTypeHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Guid
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_IntPtr
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Reflection_Assembly
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR runtime_assembly_name_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_method_handle = 0;
	CHAOS_IL2CPP_INTPTR generic_definition_method_handle = 0;
	CHAOS_IL2CPP_INTPTR generic_argument_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
	CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
	CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_method_handle = 0;
	CHAOS_IL2CPP_INTPTR generic_definition_method_handle = 0;
	CHAOS_IL2CPP_INTPTR generic_argument_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
	CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_String
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR length = 0;
	const char* utf8_data = nullptr;
	CHAOS_IL2CPP_UINT64 string_id = 0u;  // stable StringId, 0 = not yet materialized
};

struct chaos_type_System_Private_CoreLib_System_Type : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR runtime_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
	CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Runtime_InteropServices_System_IO_UnmanagedMemoryAccessor
{
	ThinLockableHeader header{};
};

const char* chaos_reflection_get_string_utf8(CHAOS_IL2CPP_INTPTR chaos_string_value) noexcept
{
	if (chaos_string_value == 0)
	{
		return nullptr;
	}

	if (chaos_is_string_id(chaos_string_value))
	{
		const auto chaos_view = chaos::il2cpp::string_table::Resolve(
			chaos_extract_string_id(chaos_string_value));
		return chaos_view.utf8_data;
	}

	auto* chaos_string = reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_string_value);
	return chaos_string->utf8_data;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_string_utf8_copy(const char* chaos_utf8_data, CHAOS_IL2CPP_SIZE chaos_length)
{
	if (chaos_utf8_data == nullptr)
	{
		return 0;
	}

	const auto chaos_id = chaos::il2cpp::string_table::Intern(
		chaos_utf8_data, static_cast<CHAOS_IL2CPP_UINT32>(chaos_length));
	return chaos_make_string_id_value(chaos_id);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionConcatStringPairValues(CHAOS_IL2CPP_INTPTR chaos_left_string_value, CHAOS_IL2CPP_INTPTR chaos_right_string_value)
{
	const auto chaos_left_length =
		chaos_left_string_value == 0
			? static_cast<CHAOS_IL2CPP_SIZE>(0)
			: chaos_is_string_id(chaos_left_string_value)
				? static_cast<CHAOS_IL2CPP_SIZE>(chaos::il2cpp::string_table::Resolve(
					chaos_extract_string_id(chaos_left_string_value)).byte_count)
				: static_cast<CHAOS_IL2CPP_SIZE>(reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_left_string_value)->length);
	const auto chaos_right_length =
		chaos_right_string_value == 0
			? static_cast<CHAOS_IL2CPP_SIZE>(0)
			: chaos_is_string_id(chaos_right_string_value)
				? static_cast<CHAOS_IL2CPP_SIZE>(chaos::il2cpp::string_table::Resolve(
					chaos_extract_string_id(chaos_right_string_value)).byte_count)
				: static_cast<CHAOS_IL2CPP_SIZE>(reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_right_string_value)->length);
	const char* chaos_left_utf8 = chaos_reflection_get_string_utf8(chaos_left_string_value);
	const char* chaos_right_utf8 = chaos_reflection_get_string_utf8(chaos_right_string_value);
	if ((chaos_left_string_value != 0 && chaos_left_utf8 == nullptr && chaos_left_length != 0) ||
		(chaos_right_string_value != 0 && chaos_right_utf8 == nullptr && chaos_right_length != 0))
	{
		CHAOS_IL2CPP_FAIL();
	}

	const auto chaos_combined_length = chaos_left_length + chaos_right_length;
	auto* chaos_combined_utf8 = new char[chaos_combined_length + 1];
	if (chaos_left_length != 0)
	{
		CHAOS_IL2CPP_MEMCPY(chaos_combined_utf8, chaos_left_utf8, chaos_left_length);
	}
	if (chaos_right_length != 0)
	{
		CHAOS_IL2CPP_MEMCPY(chaos_combined_utf8 + chaos_left_length, chaos_right_utf8, chaos_right_length);
	}
	chaos_combined_utf8[chaos_combined_length] = '\0';
	const auto chaos_result = chaos_reflection_create_string_utf8_copy(chaos_combined_utf8, chaos_combined_length);
	delete[] chaos_combined_utf8;
	return chaos_result;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_string_literal(const char* chaos_utf8_data)
{
	if (chaos_utf8_data == nullptr)
	{
		return 0;
	}

	return chaos_reflection_create_string_utf8_copy(chaos_utf8_data, CHAOS_IL2CPP_STRLEN(chaos_utf8_data));
}


CHAOS_IL2CPP_INTPTR chaos_string_materialize(CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
	if (chaos_value == 0)
	{
		return 0;
	}

	if (!chaos_is_string_id(chaos_value))
	{
		return chaos_value;
	}

	const auto chaos_id = chaos_extract_string_id(chaos_value);
	const auto chaos_view = chaos::il2cpp::string_table::Resolve(chaos_id);

	auto* chaos_string = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_String, {});
	chaos_string->header.type_info = &chaos_mt_System_Private_CoreLib_System_String.hot;
	chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_view.byte_count);
	auto* owned_utf8 = static_cast<char*>(chaos::il2cpp::runtime_core::GcAllocateAtomic(chaos_view.byte_count + 1));
	CHAOS_IL2CPP_MEMCPY(owned_utf8, chaos_view.utf8_data, chaos_view.byte_count);
	owned_utf8[chaos_view.byte_count] = '\0';
	chaos_string->utf8_data = owned_utf8;
	chaos_string->string_id = chaos_id;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_reference_array(const TypeInfo* chaos_element_type_info, CHAOS_IL2CPP_SIZE chaos_length)
{
	auto* chaos_array = CHAOS_IL2CPP_NEW_GC(chaos_managed_array);
	chaos_array->header.type_info = &chaos_type_info_managed_array.hot;
	chaos_array->element_type_shape = chaos_type_shape_reference;
	chaos_array->element_type_info = chaos_element_type_info;
	chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
	chaos_array->elements = chaos_length == 0 ? nullptr : static_cast<CHAOS_IL2CPP_INTPTR*>(CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, chaos_length));
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_name_value_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
	switch (chaos_type_handle)
	{
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INT32 chaos_reflection_get_type_metadata_token_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
	switch (chaos_type_handle)
	{
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_type_value(CHAOS_IL2CPP_INTPTR chaos_type_handle)
{
	auto* chaos_type = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Type);
	chaos_type->header.type_info = &chaos_mt_System_Private_CoreLib_System_Type.hot;
	chaos_type->runtime_type_handle = chaos_type_handle;
	chaos_type->runtime_name_value = chaos_reflection_get_type_name_value_from_handle(chaos_type_handle);
	chaos_type->runtime_metadata_token_value = chaos_reflection_get_type_metadata_token_from_handle(chaos_type_handle);
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_type);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethod_name_value_from_handle(CHAOS_IL2CPP_INTPTR chaos_method_handle) noexcept
{
	switch (chaos_method_handle)
	{
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INT32 ChaosReflectionGetMethod_metadata_token_from_handle(CHAOS_IL2CPP_INTPTR chaos_method_handle) noexcept
{
	switch (chaos_method_handle)
	{
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetDeclaringType_handle_from_type_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
	switch (chaos_type_handle)
	{
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFromHandle(CHAOS_IL2CPP_INTPTR chaos_type_handle)
{
	return chaos_reflection_create_type_value(chaos_type_handle);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeHandle(CHAOS_IL2CPP_INTPTR chaos_type_value) noexcept
{
	auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
	return chaos_type->runtime_type_handle;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_object_type(CHAOS_IL2CPP_INTPTR chaos_object_value)
{
	if (chaos_object_value == 0)
	{
		return 0;
	}
	
	const auto* chaos_ti = chaos_object_get_type_info(reinterpret_cast<const void*>(chaos_object_value));
	switch (chaos_ti->stable_id)
	{
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_instance(CHAOS_IL2CPP_INTPTR chaos_type_value)
{
	auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
	if (chaos_type == nullptr)
	{
		return 0;
	}
	
	switch (chaos_type->runtime_type_handle)
	{
		default:
			return 0;
	}
}

CHAOS_IL2CPP_INTPTR chaos_reflection_resolve_method_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle, const char* chaos_method_name) noexcept
{
	if (chaos_method_name == nullptr)
	{
		return 0;
	}

	switch (chaos_type_handle)
	{
		case static_cast<CHAOS_IL2CPP_INTPTR>(50075720u):
			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_121") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110911588u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_123") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117426662u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_133") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117136323u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_138") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113219086u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_167") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102906062u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_168") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114913487u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_195") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102609235u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_196") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110158054u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_25") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113511603u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_27") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112674653u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_29") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102902967u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_37") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113972608u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_41") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107029865u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_49") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101074529u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_60") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106634612u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_63") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104185293u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_65") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111700015u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_76") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112291115u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_90") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104002995u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "CustomEntrySubject_91") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110462404u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_0") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113169025u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_100") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112456586u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_101") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114897345u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_102") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102009016u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_103") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109693159u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_104") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105561326u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_105") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107498629u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_106") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108182252u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_107") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116444571u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_108") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112466642u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_109") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114920905u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_10") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114896746u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_110") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110395447u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_111") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104850120u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_112") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116856721u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_113") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116993050u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_114") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105009387u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_115") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104103548u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_116") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116796309u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_117") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107916734u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_118") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117158079u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_119") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107245552u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_11") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112598049u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_120") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109596940u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_122") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104373390u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_124") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108095064u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_125") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107092231u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_126") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112806698u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_127") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115835617u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_128") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109598164u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_129") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108290627u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_12") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116340120u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_130") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114530057u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_131") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117202962u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_132") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114793519u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_134") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107278797u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_135") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111278966u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_136") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104503523u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_137") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109728116u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_139") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117057994u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_13") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117078087u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_140") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103391110u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_141") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104955677u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_142") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109451140u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_143") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101000819u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_144") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101957410u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_145") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112839577u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_146") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105326384u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_147") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117427711u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_148") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104073982u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_149") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102085589u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_14") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113622478u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_150") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111956099u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_151") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105621524u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_152") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103312365u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_153") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115459094u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_154") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101668815u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_155") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116250688u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_156") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110667817u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_157") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(100823474u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_158") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104936827u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_159") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113438284u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_15") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115470565u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_160") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102017064u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_161") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111151767u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_162") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103245050u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_163") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102628209u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_164") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(100989788u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_165") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108486987u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_166") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116848158u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_169") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112604447u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_16") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103844940u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_170") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114767237u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_171") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112829934u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_172") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106935963u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_173") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115450860u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_174") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105388737u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_175") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102947978u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_176") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116961767u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_177") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109277624u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_178") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108703885u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_179") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111359286u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_17") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112120699u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_180") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109884098u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_181") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110913337u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_182") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116277840u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_183") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114844447u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_184") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115009702u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_185") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101093693u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_186") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109231396u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_187") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116142099u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_188") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105485050u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_189") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104868209u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_18") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108007346u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_190") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112062191u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_191") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105363296u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_192") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107652297u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_193") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105198034u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_194") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105888419u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_197") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113599286u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_198") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102424551u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_19") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101074473u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_1") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(100883530u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_20") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107798633u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_21") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105874930u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_22") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108464911u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_23") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107472768u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_24") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112719917u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_26") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111075523u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_28") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101843297u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_2") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110878631u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_30") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105857516u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_31") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114119835u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_32") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103236590u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_33") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105173893u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_34") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116461496u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_35") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107368423u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_36") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110131850u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_38") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102577972u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_39") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110832291u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_3") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105953656u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_40") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102178787u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_42") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104954061u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_43") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(108954230u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_44") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112468783u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_45") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116963488u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_46") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112205321u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_47") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114878226u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_48") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117435099u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_4") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115554117u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_50") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111365862u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_51") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106527357u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_52") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104850788u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_53") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103817043u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_54") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107469826u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_55") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(100812665u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_56") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104052624u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_57") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107433567u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_58") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114443102u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_59") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110013237u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_5") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111862958u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_61") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104113870u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_62") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(102612091u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_64") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103089441u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_66") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107569479u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_67") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106831512u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_68") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(100987629u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_69") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113134358u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_6") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103585371u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_70") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104071432u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_71") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(107153783u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_72") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103914074u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_73") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(112725969u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_74") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(100975292u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_75") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(113059883u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_77") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116538069u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_78") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103001648u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_79") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115102975u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_7") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111613612u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_80") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101584079u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_81") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(116165952u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_82") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(110583081u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_83") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(100738738u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_84") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(111871363u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_85") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105536788u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_86") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103227629u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_87") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115374358u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_88") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109809479u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_89") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109071512u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_8") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(115132809u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_92") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(105241648u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_93") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(117342975u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_94") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(103306374u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_95") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(104870941u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_96") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(109366404u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_97") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(100916083u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_98") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(106154074u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_99") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(114965969u);
			}

			if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "Subject_9") == 0)
			{
				return static_cast<CHAOS_IL2CPP_INTPTR>(101028498u);
			}

			break;
		default:
			break;
	}

	return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethod(CHAOS_IL2CPP_INTPTR chaos_type_value, CHAOS_IL2CPP_INTPTR chaos_name_value, CHAOS_IL2CPP_INT32 chaos_binding_flags = CHAOS_IL2CPP_INT32(0))
{
	(void)chaos_binding_flags;
	auto* chaos_type = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_value);
	const auto* chaos_method_name = chaos_reflection_get_string_utf8(chaos_name_value);
	const auto chaos_method_handle = chaos_reflection_resolve_method_handle(chaos_type->runtime_type_handle, chaos_method_name);
	if (chaos_method_handle == 0)
	{
		return 0;
	}

	auto* chaos_method = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo);
	chaos_method->header.type_info = &chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo.hot;
	chaos_method->declaring_type_handle = chaos_type->runtime_type_handle;
	chaos_method->runtime_method_handle = chaos_method_handle;
	chaos_method->generic_definition_method_handle = chaos_method_handle;
	chaos_method->runtime_name_value = ChaosReflectionGetMethod_name_value_from_handle(chaos_method_handle);
	chaos_method->runtime_metadata_token_value = ChaosReflectionGetMethod_metadata_token_from_handle(chaos_method_handle);
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_method);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_closed_generic_method_handle(CHAOS_IL2CPP_INTPTR chaos_definition_handle, CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept
{
	CHAOS_IL2CPP_UINT32 hash = 2166136261u;
	const auto chaos_definition_bits = static_cast<CHAOS_IL2CPP_UINT64>(chaos_definition_handle);
	const auto chaos_type_bits = static_cast<CHAOS_IL2CPP_UINT64>(chaos_type_handle);
	hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_definition_bits & 0xFFFFFFFFu);
	hash *= 16777619u;
	hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_definition_bits >> 32);
	hash *= 16777619u;
	hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_type_bits & 0xFFFFFFFFu);
	hash *= 16777619u;
	hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_type_bits >> 32);
	hash *= 16777619u;
	auto row_index = hash & 0x00FFFFFFu;
	if (row_index == 0u)
	{
		row_index = 1u;
	}

	return static_cast<CHAOS_IL2CPP_INTPTR>(0x06000000u | row_index);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionMakeGenericMethod(CHAOS_IL2CPP_INTPTR chaos_method_value, CHAOS_IL2CPP_INTPTR chaos_type_array_value)
{
	if (chaos_method_value == 0 || chaos_type_array_value == 0)
	{
		return 0;
	}

	auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_method_value);
	auto* chaos_type_array = reinterpret_cast<chaos_managed_array*>(chaos_type_array_value);
	if (chaos_type_array->length <= 0)
	{
		return 0;
	}

	const auto chaos_type_argument_value = chaos_type_array->elements[0];
	if (chaos_type_argument_value == 0)
	{
		return 0;
	}

	auto* chaos_type_argument = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Type*>(chaos_type_argument_value);
	auto* chaos_closed_method = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo);
	chaos_closed_method->header.type_info = &chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo.hot;
	chaos_closed_method->declaring_type_handle = chaos_method->declaring_type_handle;
	chaos_closed_method->generic_definition_method_handle = chaos_method->generic_definition_method_handle != 0
		? chaos_method->generic_definition_method_handle
		: chaos_method->runtime_method_handle;
	chaos_closed_method->generic_argument_type_handle = chaos_type_argument->runtime_type_handle;
	chaos_closed_method->runtime_method_handle = chaos_reflection_create_closed_generic_method_handle(
		chaos_closed_method->generic_definition_method_handle,
		chaos_closed_method->generic_argument_type_handle);
	chaos_closed_method->runtime_name_value = chaos_method->runtime_name_value == 0
		? ChaosReflectionGetMethod_name_value_from_handle(chaos_closed_method->generic_definition_method_handle)
		: chaos_method->runtime_name_value;
	chaos_closed_method->runtime_metadata_token_value = chaos_method->runtime_metadata_token_value != 0
		? chaos_method->runtime_metadata_token_value
		: ChaosReflectionGetMethod_metadata_token_from_handle(chaos_closed_method->generic_definition_method_handle);
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_closed_method);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethod_handle(CHAOS_IL2CPP_INTPTR chaos_method_value) noexcept
{
	auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_method_value);
	return chaos_method->runtime_method_handle;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionInvokeMethod(CHAOS_IL2CPP_INTPTR chaos_method_value, CHAOS_IL2CPP_INTPTR chaos_target_value, CHAOS_IL2CPP_INTPTR chaos_args_array_value)
{
	if (chaos_method_value == 0)
	{
		return 0;
	}

	auto* chaos_method = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(chaos_method_value);
	auto* chaos_args = reinterpret_cast<chaos_managed_array*>(chaos_args_array_value);

	return 0;
}

CHAOS_IL2CPP_INT32 chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = 0;
CHAOS_IL2CPP_INTPTR chaos_static_System_Private_CoreLib_System_Guid__Empty = 0;
CHAOS_IL2CPP_INTPTR chaos_static_System_Private_CoreLib_System_IntPtr__Zero = 0;


	// AOT-baked string table: sorted by StringId for binary search at runtime.
	constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
		{ 2607821981565500683U, "hello", 5u },
	};

	constexpr CHAOS_IL2CPP_UINT32 chaos_aot_string_entry_count = sizeof(chaos_aot_string_entries) / sizeof(chaos_aot_string_entries[0]);

	// Register the AOT-baked string table with the runtime before any code uses it.
	static const CHAOS_IL2CPP_UINT32 s_aot_string_table_registered = []()
	{
		::chaos::il2cpp::string_table::InitializeFromAot(
			chaos_aot_string_entries,
			chaos_aot_string_entry_count);
		return 0u;
	}();

static void chaos_external_runtime_System_Private_CoreLib_System_Decimal___ctor_System_Void_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	chaos_decimal_ctor_int32(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Decimal__op_Explicit_System_Int32_System_Decimal_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	return ChaosDecimalToInt32(chaos_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Guid__GetHashCode_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	return ChaosGuidGetHashCode(chaos_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosObjectGetHashCode(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	if (chaos_arg_0 == 0)
	{
		CHAOS_IL2CPP_FAIL();
	}

	if (chaos_is_string_id(chaos_arg_0))
	{
		return static_cast<CHAOS_IL2CPP_INT32>(
			chaos::il2cpp::string_table::Resolve(
				chaos_extract_string_id(chaos_arg_0)).byte_count);
	}

	auto* chaos_string = reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_0);
	return static_cast<CHAOS_IL2CPP_INT32>(chaos_string->length);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosReflectionGetTypeFromHandle(chaos_fn_arg_0);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_0(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_1(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_2(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_3(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_4(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_5(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_6(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_7(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_8(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_9(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_10(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_11(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_12(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_13(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_14(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_15(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_16(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_17(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_18(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_19(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_20(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_21(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_22(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_23(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_24(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_25(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_26(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_27(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_28(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_29(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_30(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_31(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_32(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_33(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_34(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_35(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_36(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_37(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_38(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_39(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_40(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_41(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_42(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_43(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_44(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_45(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_46(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_47(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_48(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_49(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_50(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_51(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_52(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_53(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_54(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_55(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_56(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_57(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_58(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_59(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_60(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_61(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_62(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_63(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_64(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_65(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_66(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_67(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_68(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_69(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_70(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_71(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_72(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_73(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_74(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_75(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_76(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_77(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_78(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_79(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_80(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_81(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_82(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_83(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_84(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_85(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_86(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_87(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_88(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_89(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_90(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_91(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_92(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_93(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_94(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_95(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_96(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_97(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_98(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_99(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_100(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_101(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_102(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_103(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_104(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_105(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_106(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_107(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_108(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_109(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_110(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_111(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_112(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_113(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_114(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_115(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_116(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_117(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_118(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_119(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_120(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_121(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_122(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_123(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_124(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_125(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_126(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_127(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_128(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_129(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_130(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_131(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_132(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_133(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_134(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_135(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_136(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_137(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_138(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_139(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_140(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_141(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_142(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_143(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_144(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_145(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_146(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_147(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_148(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_149(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_150(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_151(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_152(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_153(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_154(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_155(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_156(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_157(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_158(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_159(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_160(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_161(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_162(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_163(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_164(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_165(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_166(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_167(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_168(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_169(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_170(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_171(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_172(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_173(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_174(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_175(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_176(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_177(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_178(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_179(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_180(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_181(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_182(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_183(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_184(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_185(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_186(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_187(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_188(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_189(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_190(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_191(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_192(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_193(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_194(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_195(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_196(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_197(void);
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_198(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[200] = {
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[199];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		199u,
		0u,
		2016271421u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_0
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_1
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_2
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_3
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_4
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_5
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_6
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_7
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_8
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_9
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_10
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_11
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_12
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_13
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_14
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_15
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_16
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_17
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_18
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_19
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_20
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_21
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_22
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_23
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_24
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_25
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_26
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_27
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_28
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_29
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_30
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_31
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_32
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_33
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_34
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_35
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_36
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_37
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_38
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_39
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_40
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_41
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_42
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_43
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_44
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_45
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_46
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_47
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_48
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_49
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_50
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_51
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_52
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_53
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_54
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_55
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_56
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_57
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_58
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_59
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_60
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_61
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_62
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_63
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_64
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_65
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_66
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_67
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_68
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_69
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_70
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_71
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_72
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_73
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_74
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_75
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_76
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_77
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_78
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_79
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_80
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_81
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_82
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_83
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_84
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_85
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_86
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_87
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_88
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_89
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_90
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_91
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_92
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_93
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_94
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_95
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_96
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_97
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_98
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_99
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_100
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_101
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_102
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_103
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_104
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_105
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_106
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_107
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_108
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_109
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_110
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_111
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_112
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_113
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_114
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_115
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_116
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_117
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_118
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_119
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_120
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_121
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_122
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_123
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_124
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_125
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_126
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_127
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_128
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_129
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_130
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_131
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_132
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_133
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_134
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_135
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_136
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_137
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_138
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_139
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_140
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_141
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_142
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_143
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_144
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_145
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_146
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_147
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_148
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_149
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_150
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_151
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_152
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_153
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_154
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_155
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_156
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_157
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_158
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_159
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_160
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_161
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_162
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_163
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_164
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_165
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_166
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_167
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_168
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_169
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_170
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_171
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_172
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_173
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_174
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_175
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_176
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_177
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_178
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_179
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_180
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_181
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_182
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_183
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_184
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_185
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_186
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_187
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_188
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_189
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_190
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_191
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_192
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_193
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_194
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_195
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_196
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_197
		{ 0u, 0u },  // NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_198
	},
	{
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[2] = {
		0u,
		1548u,
	};

	static constexpr const char* s_type_names[2] = {
		"<Module>",
		"NativeMemoryPointersSubjects",
	};

	static constexpr const char* s_type_namespaces[2] = {
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[2] = {
		0u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[2] = {
		nullptr,
		&chaos_mt_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects.hot,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[3] = {
		0u,
		0u,
		0u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[3] = {
		0u,
		0u,
		0u,
	};



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "NativeMemoryPointersSubjects",
		/* .image             = */ &kReflImage,
		/* .type_flags        = */ s_type_flags,
		/* .type_names        = */ s_type_names,
		/* .type_namespaces   = */ s_type_namespaces,
		/* .type_parent_tokens= */ s_type_parent_tokens,
		/* .type_info_ptrs    = */ s_type_info_ptrs,
		/* .nested_type_children= */ nullptr,
		/* .nested_type_offset = */ s_nested_type_offset,
		/* .generic_param_constraint_data= */ nullptr,
		/* .generic_param_constraint_offset= */ s_generic_param_constraint_offset,
		/* .type_count        = */ 2u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 -- deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,
	/* .custom_attribute_method_offset   = */ nullptr,
	/* .custom_attribute_field_offset    = */ nullptr,
	/* .custom_attribute_property_offset = */ nullptr,
	/* .custom_attribute_param_offset    = */ nullptr,
	/* .custom_attribute_method_count    = */ 0u,
	/* .custom_attribute_field_count     = */ 0u,
	/* .custom_attribute_property_count  = */ 0u,
	/* .custom_attribute_param_count   = */ 0u,
		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("NativeMemoryPointersSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[199] = {
	{ "Subject_0", 0x00000003u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_1", 0x00000004u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_2", 0x00000005u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_3", 0x00000006u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_4", 0x00000007u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_5", 0x00000008u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_6", 0x00000009u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_7", 0x0000000Au, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_8", 0x0000000Bu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_9", 0x0000000Cu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_10", 0x0000000Du, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_11", 0x0000000Eu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_12", 0x0000000Fu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_13", 0x00000010u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_14", 0x00000011u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_15", 0x00000012u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_16", 0x00000013u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_17", 0x00000014u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_18", 0x00000015u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_19", 0x00000016u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_20", 0x00000017u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_21", 0x00000018u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_22", 0x00000019u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_23", 0x0000001Au, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_24", 0x0000001Bu, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_25", 0x0000001Cu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_26", 0x0000001Du, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_27", 0x0000001Eu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_28", 0x0000001Fu, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_29", 0x00000020u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_30", 0x00000021u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_31", 0x00000022u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_32", 0x00000023u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_33", 0x00000024u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_34", 0x00000025u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_35", 0x00000026u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_36", 0x00000027u, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_37", 0x00000028u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_38", 0x00000029u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_39", 0x0000002Au, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_40", 0x0000002Bu, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_41", 0x0000002Cu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_42", 0x0000002Du, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_43", 0x0000002Eu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_44", 0x0000002Fu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_45", 0x00000030u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_46", 0x00000031u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_47", 0x00000032u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_48", 0x00000033u, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_49", 0x00000034u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_50", 0x00000035u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_51", 0x00000036u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_52", 0x00000037u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_53", 0x00000038u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_54", 0x00000039u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_55", 0x0000003Au, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_56", 0x0000003Bu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_57", 0x0000003Cu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_58", 0x0000003Du, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_59", 0x0000003Eu, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_60", 0x0000003Fu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_61", 0x00000040u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_62", 0x00000041u, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_63", 0x00000042u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_64", 0x00000043u, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_65", 0x00000044u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_66", 0x00000045u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_67", 0x00000046u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_68", 0x00000047u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_69", 0x00000048u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_70", 0x00000049u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_71", 0x0000004Au, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_72", 0x0000004Bu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_73", 0x0000004Cu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_74", 0x0000004Du, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_75", 0x0000004Eu, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_76", 0x0000004Fu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_77", 0x00000050u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_78", 0x00000051u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_79", 0x00000052u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_80", 0x00000053u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_81", 0x00000054u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_82", 0x00000055u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_83", 0x00000056u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_84", 0x00000057u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_85", 0x00000058u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_86", 0x00000059u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_87", 0x0000005Au, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_88", 0x0000005Bu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_89", 0x0000005Cu, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_90", 0x0000005Du, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_91", 0x0000005Eu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_92", 0x0000005Fu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_93", 0x00000060u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_94", 0x00000061u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_95", 0x00000062u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_96", 0x00000063u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_97", 0x00000064u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_98", 0x00000065u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_99", 0x00000066u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_100", 0x00000067u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_101", 0x00000068u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_102", 0x00000069u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_103", 0x0000006Au, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_104", 0x0000006Bu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_105", 0x0000006Cu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_106", 0x0000006Du, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_107", 0x0000006Eu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_108", 0x0000006Fu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_109", 0x00000070u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_110", 0x00000071u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_111", 0x00000072u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_112", 0x00000073u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_113", 0x00000074u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_114", 0x00000075u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_115", 0x00000076u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_116", 0x00000077u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_117", 0x00000078u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_118", 0x00000079u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_119", 0x0000007Au, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_120", 0x0000007Bu, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_121", 0x0000007Cu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_122", 0x0000007Du, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_123", 0x0000007Eu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_124", 0x0000007Fu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_125", 0x00000080u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_126", 0x00000081u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_127", 0x00000082u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_128", 0x00000083u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_129", 0x00000084u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_130", 0x00000085u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_131", 0x00000086u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_132", 0x00000087u, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_133", 0x00000088u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_134", 0x00000089u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_135", 0x0000008Au, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_136", 0x0000008Bu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_137", 0x0000008Cu, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_138", 0x0000008Du, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_139", 0x0000008Eu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_140", 0x0000008Fu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_141", 0x00000090u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_142", 0x00000091u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_143", 0x00000092u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_144", 0x00000093u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_145", 0x00000094u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_146", 0x00000095u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_147", 0x00000096u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_148", 0x00000097u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_149", 0x00000098u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_150", 0x00000099u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_151", 0x0000009Au, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_152", 0x0000009Bu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_153", 0x0000009Cu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_154", 0x0000009Du, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_155", 0x0000009Eu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_156", 0x0000009Fu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_157", 0x000000A0u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_158", 0x000000A1u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_159", 0x000000A2u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_160", 0x000000A3u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_161", 0x000000A4u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_162", 0x000000A5u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_163", 0x000000A6u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_164", 0x000000A7u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_165", 0x000000A8u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_166", 0x000000A9u, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_167", 0x000000AAu, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_168", 0x000000ABu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_169", 0x000000ACu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_170", 0x000000ADu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_171", 0x000000AEu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_172", 0x000000AFu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_173", 0x000000B0u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_174", 0x000000B1u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_175", 0x000000B2u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_176", 0x000000B3u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_177", 0x000000B4u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_178", 0x000000B5u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_179", 0x000000B6u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_180", 0x000000B7u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_181", 0x000000B8u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_182", 0x000000B9u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_183", 0x000000BAu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_184", 0x000000BBu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_185", 0x000000BCu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_186", 0x000000BDu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_187", 0x000000BEu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_188", 0x000000BFu, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_189", 0x000000C0u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_190", 0x000000C1u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_191", 0x000000C2u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_192", 0x000000C3u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_193", 0x000000C4u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_194", 0x000000C5u, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_195", 0x000000C6u, 0u },  // NativeMemoryPointersSubjects
	{ "CustomEntrySubject_196", 0x000000C7u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_197", 0x000000C8u, 0u },  // NativeMemoryPointersSubjects
	{ "Subject_198", 0x000000C9u, 0u },  // NativeMemoryPointersSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "NativeMemoryPointersSubjects", "", 0u, 199u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[199] = {
	{ 0x00000003u, 0u },
	{ 0x00000004u, 1u },
	{ 0x00000005u, 2u },
	{ 0x00000006u, 3u },
	{ 0x00000007u, 4u },
	{ 0x00000008u, 5u },
	{ 0x00000009u, 6u },
	{ 0x0000000Au, 7u },
	{ 0x0000000Bu, 8u },
	{ 0x0000000Cu, 9u },
	{ 0x0000000Du, 10u },
	{ 0x0000000Eu, 11u },
	{ 0x0000000Fu, 12u },
	{ 0x00000010u, 13u },
	{ 0x00000011u, 14u },
	{ 0x00000012u, 15u },
	{ 0x00000013u, 16u },
	{ 0x00000014u, 17u },
	{ 0x00000015u, 18u },
	{ 0x00000016u, 19u },
	{ 0x00000017u, 20u },
	{ 0x00000018u, 21u },
	{ 0x00000019u, 22u },
	{ 0x0000001Au, 23u },
	{ 0x0000001Bu, 24u },
	{ 0x0000001Cu, 25u },
	{ 0x0000001Du, 26u },
	{ 0x0000001Eu, 27u },
	{ 0x0000001Fu, 28u },
	{ 0x00000020u, 29u },
	{ 0x00000021u, 30u },
	{ 0x00000022u, 31u },
	{ 0x00000023u, 32u },
	{ 0x00000024u, 33u },
	{ 0x00000025u, 34u },
	{ 0x00000026u, 35u },
	{ 0x00000027u, 36u },
	{ 0x00000028u, 37u },
	{ 0x00000029u, 38u },
	{ 0x0000002Au, 39u },
	{ 0x0000002Bu, 40u },
	{ 0x0000002Cu, 41u },
	{ 0x0000002Du, 42u },
	{ 0x0000002Eu, 43u },
	{ 0x0000002Fu, 44u },
	{ 0x00000030u, 45u },
	{ 0x00000031u, 46u },
	{ 0x00000032u, 47u },
	{ 0x00000033u, 48u },
	{ 0x00000034u, 49u },
	{ 0x00000035u, 50u },
	{ 0x00000036u, 51u },
	{ 0x00000037u, 52u },
	{ 0x00000038u, 53u },
	{ 0x00000039u, 54u },
	{ 0x0000003Au, 55u },
	{ 0x0000003Bu, 56u },
	{ 0x0000003Cu, 57u },
	{ 0x0000003Du, 58u },
	{ 0x0000003Eu, 59u },
	{ 0x0000003Fu, 60u },
	{ 0x00000040u, 61u },
	{ 0x00000041u, 62u },
	{ 0x00000042u, 63u },
	{ 0x00000043u, 64u },
	{ 0x00000044u, 65u },
	{ 0x00000045u, 66u },
	{ 0x00000046u, 67u },
	{ 0x00000047u, 68u },
	{ 0x00000048u, 69u },
	{ 0x00000049u, 70u },
	{ 0x0000004Au, 71u },
	{ 0x0000004Bu, 72u },
	{ 0x0000004Cu, 73u },
	{ 0x0000004Du, 74u },
	{ 0x0000004Eu, 75u },
	{ 0x0000004Fu, 76u },
	{ 0x00000050u, 77u },
	{ 0x00000051u, 78u },
	{ 0x00000052u, 79u },
	{ 0x00000053u, 80u },
	{ 0x00000054u, 81u },
	{ 0x00000055u, 82u },
	{ 0x00000056u, 83u },
	{ 0x00000057u, 84u },
	{ 0x00000058u, 85u },
	{ 0x00000059u, 86u },
	{ 0x0000005Au, 87u },
	{ 0x0000005Bu, 88u },
	{ 0x0000005Cu, 89u },
	{ 0x0000005Du, 90u },
	{ 0x0000005Eu, 91u },
	{ 0x0000005Fu, 92u },
	{ 0x00000060u, 93u },
	{ 0x00000061u, 94u },
	{ 0x00000062u, 95u },
	{ 0x00000063u, 96u },
	{ 0x00000064u, 97u },
	{ 0x00000065u, 98u },
	{ 0x00000066u, 99u },
	{ 0x00000067u, 100u },
	{ 0x00000068u, 101u },
	{ 0x00000069u, 102u },
	{ 0x0000006Au, 103u },
	{ 0x0000006Bu, 104u },
	{ 0x0000006Cu, 105u },
	{ 0x0000006Du, 106u },
	{ 0x0000006Eu, 107u },
	{ 0x0000006Fu, 108u },
	{ 0x00000070u, 109u },
	{ 0x00000071u, 110u },
	{ 0x00000072u, 111u },
	{ 0x00000073u, 112u },
	{ 0x00000074u, 113u },
	{ 0x00000075u, 114u },
	{ 0x00000076u, 115u },
	{ 0x00000077u, 116u },
	{ 0x00000078u, 117u },
	{ 0x00000079u, 118u },
	{ 0x0000007Au, 119u },
	{ 0x0000007Bu, 120u },
	{ 0x0000007Cu, 121u },
	{ 0x0000007Du, 122u },
	{ 0x0000007Eu, 123u },
	{ 0x0000007Fu, 124u },
	{ 0x00000080u, 125u },
	{ 0x00000081u, 126u },
	{ 0x00000082u, 127u },
	{ 0x00000083u, 128u },
	{ 0x00000084u, 129u },
	{ 0x00000085u, 130u },
	{ 0x00000086u, 131u },
	{ 0x00000087u, 132u },
	{ 0x00000088u, 133u },
	{ 0x00000089u, 134u },
	{ 0x0000008Au, 135u },
	{ 0x0000008Bu, 136u },
	{ 0x0000008Cu, 137u },
	{ 0x0000008Du, 138u },
	{ 0x0000008Eu, 139u },
	{ 0x0000008Fu, 140u },
	{ 0x00000090u, 141u },
	{ 0x00000091u, 142u },
	{ 0x00000092u, 143u },
	{ 0x00000093u, 144u },
	{ 0x00000094u, 145u },
	{ 0x00000095u, 146u },
	{ 0x00000096u, 147u },
	{ 0x00000097u, 148u },
	{ 0x00000098u, 149u },
	{ 0x00000099u, 150u },
	{ 0x0000009Au, 151u },
	{ 0x0000009Bu, 152u },
	{ 0x0000009Cu, 153u },
	{ 0x0000009Du, 154u },
	{ 0x0000009Eu, 155u },
	{ 0x0000009Fu, 156u },
	{ 0x000000A0u, 157u },
	{ 0x000000A1u, 158u },
	{ 0x000000A2u, 159u },
	{ 0x000000A3u, 160u },
	{ 0x000000A4u, 161u },
	{ 0x000000A5u, 162u },
	{ 0x000000A6u, 163u },
	{ 0x000000A7u, 164u },
	{ 0x000000A8u, 165u },
	{ 0x000000A9u, 166u },
	{ 0x000000AAu, 167u },
	{ 0x000000ABu, 168u },
	{ 0x000000ACu, 169u },
	{ 0x000000ADu, 170u },
	{ 0x000000AEu, 171u },
	{ 0x000000AFu, 172u },
	{ 0x000000B0u, 173u },
	{ 0x000000B1u, 174u },
	{ 0x000000B2u, 175u },
	{ 0x000000B3u, 176u },
	{ 0x000000B4u, 177u },
	{ 0x000000B5u, 178u },
	{ 0x000000B6u, 179u },
	{ 0x000000B7u, 180u },
	{ 0x000000B8u, 181u },
	{ 0x000000B9u, 182u },
	{ 0x000000BAu, 183u },
	{ 0x000000BBu, 184u },
	{ 0x000000BCu, 185u },
	{ 0x000000BDu, 186u },
	{ 0x000000BEu, 187u },
	{ 0x000000BFu, 188u },
	{ 0x000000C0u, 189u },
	{ 0x000000C1u, 190u },
	{ 0x000000C2u, 191u },
	{ 0x000000C3u, 192u },
	{ 0x000000C4u, 193u },
	{ 0x000000C5u, 194u },
	{ 0x000000C6u, 195u },
	{ 0x000000C7u, 196u },
	{ 0x000000C8u, 197u },
	{ 0x000000C9u, 198u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[199] = {
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_0
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_1
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_2
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_3
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_4
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_5
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_6
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_7
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_8
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_9
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_10
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_11
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_12
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_13
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_14
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_15
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_16
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_17
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_18), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_18
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_19), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_19
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_20), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_20
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_21), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_21
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_22), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_22
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_23), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_23
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_24), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_24
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_25), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_25
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_26), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_26
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_27), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_27
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_28), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_28
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_29), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_29
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_30), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_30
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_31), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_31
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_32
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_33), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_33
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_34), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_34
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_35), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_35
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_36), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_36
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_37), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_37
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_38), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_38
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_39), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_39
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_40), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_40
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_41), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_41
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_42), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_42
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_43), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_43
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_44), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_44
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_45), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_45
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_46), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_46
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_47), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_47
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_48), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_48
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_49), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_49
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_50), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_50
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_51), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_51
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_52), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_52
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_53), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_53
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_54), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_54
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_55), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_55
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_56), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_56
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_57), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_57
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_58), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_58
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_59), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_59
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_60), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_60
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_61), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_61
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_62), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_62
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_63), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_63
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_64), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_64
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_65), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_65
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_66), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_66
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_67), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_67
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_68), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_68
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_69), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_69
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_70), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_70
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_71), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_71
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_72), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_72
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_73), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_73
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_74), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_74
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_75), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_75
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_76), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_76
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_77), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_77
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_78), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_78
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_79), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_79
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_80), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_80
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_81), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_81
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_82), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_82
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_83), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_83
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_84), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_84
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_85), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_85
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_86), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_86
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_87), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_87
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_88), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_88
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_89), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_89
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_90), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_90
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_91), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_91
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_92), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_92
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_93), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_93
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_94), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_94
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_95), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_95
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_96), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_96
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_97), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_97
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_98), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_98
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_99), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_99
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_100), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_100
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_101), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_101
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_102), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_102
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_103), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_103
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_104), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_104
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_105), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_105
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_106), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_106
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_107), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_107
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_108), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_108
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_109), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_109
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_110), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_110
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_111), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_111
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_112), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_112
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_113), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_113
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_114), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_114
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_115), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_115
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_116), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_116
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_117), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_117
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_118), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_118
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_119), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_119
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_120), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_120
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_121), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_121
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_122), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_122
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_123), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_123
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_124), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_124
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_125), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_125
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_126), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_126
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_127), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_127
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_128), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_128
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_129), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_129
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_130), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_130
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_131), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_131
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_132), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_132
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_133), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_133
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_134), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_134
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_135), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_135
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_136), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::Subject_136
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_137), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_137
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_138), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_138
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_139), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_139
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_140), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_140
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_141), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_141
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_142), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_142
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_143), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_143
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_144), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_144
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_145), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_145
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_146), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_146
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_147), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_147
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_148), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_148
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_149), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_149
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_150), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_150
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_151), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_151
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_152), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_152
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_153), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_153
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_154), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_154
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_155), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_155
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_156), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_156
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_157), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_157
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_158), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_158
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_159), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_159
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_160), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_160
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_161), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_161
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_162), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_162
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_163), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_163
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_164), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_164
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_165), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_165
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_166), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_166
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_167), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_167
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_168), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_168
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_169), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_169
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_170), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_170
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_171), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_171
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_172), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_172
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_173), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_173
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_174), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_174
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_175), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_175
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_176), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_176
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_177), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_177
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_178), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_178
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_179), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_179
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_180), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_180
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_181), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_181
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_182), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_182
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_183), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_183
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_184), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_184
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_185), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_185
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_186), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_186
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_187), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_187
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_188), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_188
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_189), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_189
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_190), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_190
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_191), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_191
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_192), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_192
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_193), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_193
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_194), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_194
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_195), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_195
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_196), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NativeMemoryPointersSubjects::CustomEntrySubject_196
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_197), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_197
	{ reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_198), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // NativeMemoryPointersSubjects::Subject_198
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"NativeMemoryPointersSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	199u,
	s_hotpatch_slots,
	199u,
	s_hotpatch_entries,
	199u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[171] = {
	"System.Private.CoreLib/System.IntPtr::Zero",
	"System.Runtime.InteropServices/Marshal::AddRef:System.Int32(System.IntPtr)",
	"NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::_exitCode",
	"System.Runtime.InteropServices/Marshal::AllocCoTaskMem:System.IntPtr(System.Int32)",
	"System.Runtime.InteropServices/Marshal::AllocHGlobal:System.IntPtr(System.Int32)",
	"System.Runtime.InteropServices/Marshal::AllocHGlobal:System.IntPtr(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::AreComObjectsAvailableForCleanup:System.Boolean()",
	"System.Runtime.InteropServices/Marshal::BindToMoniker:System.Object(System.String)",
	"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()",
	"System.Runtime.InteropServices/Marshal::ChangeWrapperHandleStrength:System.Void(System.Object,System.Boolean)",
	"System.Runtime.InteropServices/Marshal::CleanupUnusedObjectsInCurrentContext:System.Void()",
	"System.Runtime.InteropServices/Marshal::CreateAggregatedObject:System.IntPtr(System.IntPtr,System.Object)",
	"System.Private.CoreLib/System.Byte",
	"System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)",
	"System.Runtime.InteropServices/Marshal::CreateWrapperOfType:System.Object(System.Object,System.Type)",
	"System.Runtime.InteropServices/Marshal::DestroyStructure:System.Void(System.IntPtr,System.Type)",
	"System.Runtime.InteropServices/Marshal::FinalReleaseComObject:System.Int32(System.Object)",
	"System.Runtime.InteropServices/Marshal::FreeBSTR:System.Void(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::FreeCoTaskMem:System.Void(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::FreeHGlobal:System.Void(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::GenerateGuidForType:System.Guid(System.Type)",
	"System.Private.CoreLib/System.Guid::GetHashCode:System.Int32()",
	"System.Runtime.InteropServices/Marshal::GenerateProgIdForType:System.String(System.Type)",
	"System.Private.CoreLib/System.String::get_Length:System.Int32()",
	"System.Runtime.InteropServices/Marshal::GetComInterfaceForObject:System.IntPtr(System.Object,System.Type)",
	"System.Runtime.InteropServices/Marshal::GetComInterfaceForObject:System.IntPtr(System.Object,System.Type,CustomQueryInterfaceMode)",
	"System.Runtime.InteropServices/Marshal::GetComObjectData:System.Object(System.Object,System.Object)",
	"System.Runtime.InteropServices/Marshal::GetDelegateForFunctionPointer:System.Delegate(System.IntPtr,System.Type)",
	"System.Runtime.InteropServices/Marshal::GetEndComSlot:System.Int32(System.Type)",
	"System.Runtime.InteropServices/Marshal::GetExceptionCode:System.Int32()",
	"System.Runtime.InteropServices/Marshal::GetExceptionForHR:System.Exception(System.Int32)",
	"System.Runtime.InteropServices/Marshal::GetExceptionForHR:System.Exception(System.Int32,System.IntPtr)",
	"System.Private.CoreLib/System.Guid",
	"System.Runtime.InteropServices/Marshal::GetExceptionForHR:System.Exception(System.Int32,System.Guid&,System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::GetExceptionPointers:System.IntPtr()",
	"System.Runtime.InteropServices/Marshal::GetFunctionPointerForDelegate:System.IntPtr(System.Delegate)",
	"System.Runtime.InteropServices/Marshal::GetHINSTANCE:System.IntPtr(System.Reflection.Module)",
	"System.Runtime.InteropServices/Marshal::GetHRForException:System.Int32(System.Exception)",
	"System.Runtime.InteropServices/Marshal::GetHRForLastWin32Error:System.Int32()",
	"System.Runtime.InteropServices/Marshal::GetIDispatchForObject:System.IntPtr(System.Object)",
	"System.Runtime.InteropServices/Marshal::GetIUnknownForObject:System.IntPtr(System.Object)",
	"System.Runtime.InteropServices/Marshal::GetLastPInvokeError:System.Int32()",
	"System.Runtime.InteropServices/Marshal::GetLastPInvokeErrorMessage:System.String()",
	"System.Runtime.InteropServices/Marshal::GetLastSystemError:System.Int32()",
	"System.Runtime.InteropServices/Marshal::GetLastWin32Error:System.Int32()",
	"System.Runtime.InteropServices/Marshal::GetNativeVariantForObject:System.Void(System.Object,System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::GetObjectForIUnknown:System.Object(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::GetObjectForNativeVariant:System.Object(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::GetObjectsForNativeVariants:System.Object[](System.IntPtr,System.Int32)",
	"System.Runtime.InteropServices/Marshal::GetPInvokeErrorMessage:System.String(System.Int32)",
	"System.Runtime.InteropServices/Marshal::GetStartComSlot:System.Int32(System.Type)",
	"System.Runtime.InteropServices/Marshal::GetTypedObjectForIUnknown:System.Object(System.IntPtr,System.Type)",
	"System.Private.CoreLib/System.Guid::Empty",
	"System.Runtime.InteropServices/Marshal::GetTypeFromCLSID:System.Type(System.Guid)",
	"System.Runtime.InteropServices/Marshal::GetTypeInfoName:System.String(System.Runtime.InteropServices.ComTypes.ITypeInfo)",
	"System.Runtime.InteropServices/Marshal::GetUniqueObjectForIUnknown:System.Object(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::InitHandle:System.Void(System.Runtime.InteropServices.SafeHandle,System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::IsComObject:System.Boolean(System.Object)",
	"System.Runtime.InteropServices/Marshal::IsTypeVisibleFromCom:System.Boolean(System.Type)",
	"System.Runtime.InteropServices/Marshal::OffsetOf:System.IntPtr(System.Type,System.String)",
	"System.Runtime.InteropServices/Marshal::Prelink:System.Void(System.Reflection.MethodInfo)",
	"System.Runtime.InteropServices/Marshal::PrelinkAll:System.Void(System.Type)",
	"System.Runtime.InteropServices/Marshal::PtrToStringAnsi:System.String(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::PtrToStringAnsi:System.String(System.IntPtr,System.Int32)",
	"System.Runtime.InteropServices/Marshal::PtrToStringAuto:System.String(System.IntPtr,System.Int32)",
	"System.Runtime.InteropServices/Marshal::PtrToStringAuto:System.String(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::PtrToStringBSTR:System.String(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::PtrToStringUni:System.String(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::PtrToStringUni:System.String(System.IntPtr,System.Int32)",
	"System.Runtime.InteropServices/Marshal::PtrToStringUTF8:System.String(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::PtrToStringUTF8:System.String(System.IntPtr,System.Int32)",
	"System.Runtime.InteropServices/Marshal::PtrToStructure:System.Object(System.IntPtr,System.Type)",
	"System.Runtime.InteropServices/Marshal::ReadByte:System.Byte(System.Object,System.Int32)",
	"System.Runtime.InteropServices/Marshal::ReadByte:System.Byte(System.IntPtr,System.Int32)",
	"System.Runtime.InteropServices/Marshal::ReadByte:System.Byte(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::ReadInt16:System.Int16(System.Object,System.Int32)",
	"System.Runtime.InteropServices/Marshal::ReadInt16:System.Int16(System.IntPtr,System.Int32)",
	"System.Runtime.InteropServices/Marshal::ReadInt16:System.Int16(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::ReadInt32:System.Int32(System.Object,System.Int32)",
	"System.Runtime.InteropServices/Marshal::ReadInt32:System.Int32(System.IntPtr,System.Int32)",
	"System.Runtime.InteropServices/Marshal::ReadInt32:System.Int32(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::ReadInt64:System.Int64(System.Object,System.Int32)",
	"System.Runtime.InteropServices/Marshal::ReadInt64:System.Int64(System.IntPtr,System.Int32)",
	"System.Runtime.InteropServices/Marshal::ReadInt64:System.Int64(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::ReadIntPtr:System.IntPtr(System.Object,System.Int32)",
	"System.Runtime.InteropServices/Marshal::ReadIntPtr:System.IntPtr(System.IntPtr,System.Int32)",
	"System.Runtime.InteropServices/Marshal::ReadIntPtr:System.IntPtr(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::ReAllocCoTaskMem:System.IntPtr(System.IntPtr,System.Int32)",
	"System.Runtime.InteropServices/Marshal::ReAllocHGlobal:System.IntPtr(System.IntPtr,System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::Release:System.Int32(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::ReleaseComObject:System.Int32(System.Object)",
	"System.Runtime.InteropServices/Marshal::SecureStringToBSTR:System.IntPtr(System.Security.SecureString)",
	"System.Runtime.InteropServices/Marshal::SecureStringToCoTaskMemAnsi:System.IntPtr(System.Security.SecureString)",
	"System.Runtime.InteropServices/Marshal::SecureStringToCoTaskMemUnicode:System.IntPtr(System.Security.SecureString)",
	"System.Runtime.InteropServices/Marshal::SecureStringToGlobalAllocAnsi:System.IntPtr(System.Security.SecureString)",
	"System.Runtime.InteropServices/Marshal::SecureStringToGlobalAllocUnicode:System.IntPtr(System.Security.SecureString)",
	"System.Runtime.InteropServices/Marshal::SetComObjectData:System.Boolean(System.Object,System.Object,System.Object)",
	"System.Runtime.InteropServices/Marshal::SetLastPInvokeError:System.Void(System.Int32)",
	"System.Runtime.InteropServices/Marshal::SetLastSystemError:System.Void(System.Int32)",
	"System.Runtime.InteropServices/Marshal::SizeOf:System.Int32(System.Type)",
	"System.Runtime.InteropServices/Marshal::StringToBSTR:System.IntPtr(System.String)",
	"System.Runtime.InteropServices/Marshal::StringToCoTaskMemAnsi:System.IntPtr(System.String)",
	"System.Runtime.InteropServices/Marshal::StringToCoTaskMemAuto:System.IntPtr(System.String)",
	"System.Runtime.InteropServices/Marshal::StringToCoTaskMemUni:System.IntPtr(System.String)",
	"System.Runtime.InteropServices/Marshal::StringToCoTaskMemUTF8:System.IntPtr(System.String)",
	"System.Runtime.InteropServices/Marshal::StringToHGlobalAnsi:System.IntPtr(System.String)",
	"System.Runtime.InteropServices/Marshal::StringToHGlobalAuto:System.IntPtr(System.String)",
	"System.Runtime.InteropServices/Marshal::StringToHGlobalUni:System.IntPtr(System.String)",
	"System.Runtime.InteropServices/Marshal::StructureToPtr:System.Void(System.Object,System.IntPtr,System.Boolean)",
	"System.Runtime.InteropServices/Marshal::ThrowExceptionForHR:System.Void(System.Int32)",
	"System.Runtime.InteropServices/Marshal::ThrowExceptionForHR:System.Void(System.Int32,System.IntPtr)",
	"System.Private.CoreLib/System.Array::Empty<System.Byte>:System.Byte[]()",
	"System.Runtime.InteropServices/Marshal::UnsafeAddrOfPinnedArrayElement<System.Byte>:System.IntPtr(System.Byte[],System.Int32)",
	"System.Runtime.InteropServices/Marshal::WriteByte:System.Void(System.Object,System.Int32,System.Byte)",
	"System.Runtime.InteropServices/Marshal::WriteByte:System.Void(System.IntPtr,System.Int32,System.Byte)",
	"System.Runtime.InteropServices/Marshal::WriteByte:System.Void(System.IntPtr,System.Byte)",
	"System.Runtime.InteropServices/Marshal::WriteInt16:System.Void(System.Object,System.Int32,System.Int16)",
	"System.Runtime.InteropServices/Marshal::WriteInt16:System.Void(System.IntPtr,System.Int32,System.Int16)",
	"System.Runtime.InteropServices/Marshal::WriteInt16:System.Void(System.IntPtr,System.Int16)",
	"System.Runtime.InteropServices/Marshal::WriteInt16:System.Void(System.IntPtr,System.Int32,System.Char)",
	"System.Runtime.InteropServices/Marshal::WriteInt16:System.Void(System.Object,System.Int32,System.Char)",
	"System.Runtime.InteropServices/Marshal::WriteInt16:System.Void(System.IntPtr,System.Char)",
	"System.Runtime.InteropServices/Marshal::WriteInt32:System.Void(System.Object,System.Int32,System.Int32)",
	"System.Runtime.InteropServices/Marshal::WriteInt32:System.Void(System.IntPtr,System.Int32,System.Int32)",
	"System.Runtime.InteropServices/Marshal::WriteInt32:System.Void(System.IntPtr,System.Int32)",
	"System.Runtime.InteropServices/Marshal::WriteInt64:System.Void(System.Object,System.Int32,System.Int64)",
	"System.Runtime.InteropServices/Marshal::WriteInt64:System.Void(System.IntPtr,System.Int32,System.Int64)",
	"System.Runtime.InteropServices/Marshal::WriteInt64:System.Void(System.IntPtr,System.Int64)",
	"System.Runtime.InteropServices/Marshal::WriteIntPtr:System.Void(System.IntPtr,System.Int32,System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::WriteIntPtr:System.Void(System.Object,System.Int32,System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::WriteIntPtr:System.Void(System.IntPtr,System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::ZeroFreeBSTR:System.Void(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::ZeroFreeCoTaskMemAnsi:System.Void(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::ZeroFreeCoTaskMemUnicode:System.Void(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::ZeroFreeCoTaskMemUTF8:System.Void(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::ZeroFreeGlobalAllocAnsi:System.Void(System.IntPtr)",
	"System.Runtime.InteropServices/Marshal::ZeroFreeGlobalAllocUnicode:System.Void(System.IntPtr)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Dispose:System.Void()",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_CanRead:System.Boolean()",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_CanWrite:System.Boolean()",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_Capacity:System.Int64()",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadBoolean:System.Boolean(System.Int64)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadByte:System.Byte(System.Int64)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadChar:System.Char(System.Int64)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadDecimal:System.Decimal(System.Int64)",
	"System.Private.CoreLib/System.Decimal::op_Explicit:System.Int32(System.Decimal)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadDouble:System.Double(System.Int64)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt16:System.Int16(System.Int64)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt32:System.Int32(System.Int64)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt64:System.Int64(System.Int64)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadSByte:System.SByte(System.Int64)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadSingle:System.Single(System.Int64)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt16:System.UInt16(System.Int64)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt32:System.UInt32(System.Int64)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt64:System.UInt64(System.Int64)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Boolean)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Byte)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Char)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int16)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int32)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int64)",
	"System.Private.CoreLib/System.Decimal::.ctor:System.Void(System.Int32)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Decimal)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Single)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Double)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.SByte)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt16)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt32)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt64)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::.ctor:System.Void(System.Runtime.InteropServices.SafeBuffer,System.Int64,System.Int64)",
	"System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::.ctor:System.Void(System.Runtime.InteropServices.SafeBuffer,System.Int64,System.Int64,System.IO.FileAccess)",
};

extern "C" void* kChaosExternalRuntimeFnTable[171] = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Type__GetTypeFromHandle_System_Type_System_RuntimeTypeHandle_),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Guid__GetHashCode_System_Int32__),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Decimal__op_Explicit_System_Int32_System_Decimal_),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Decimal___ctor_System_Void_System_Int32_),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 171;
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[199])() = {
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_10),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_11),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_12),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_13),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_14),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_15),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_16),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_17),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_18),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_19),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_20),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_21),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_22),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_23),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_24),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_25),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_26),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_27),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_28),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_29),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_30),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_31),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_32),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_33),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_34),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_35),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_36),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_37),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_38),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_39),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_40),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_41),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_42),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_43),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_44),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_45),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_46),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_47),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_48),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_49),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_50),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_51),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_52),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_53),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_54),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_55),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_56),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_57),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_58),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_59),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_60),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_61),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_62),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_63),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_64),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_65),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_66),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_67),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_68),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_69),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_70),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_71),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_72),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_73),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_74),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_75),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_76),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_77),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_78),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_79),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_80),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_81),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_82),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_83),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_84),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_85),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_86),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_87),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_88),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_89),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_90),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_91),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_92),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_93),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_94),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_95),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_96),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_97),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_98),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_99),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_100),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_101),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_102),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_103),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_104),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_105),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_106),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_107),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_108),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_109),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_110),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_111),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_112),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_113),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_114),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_115),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_116),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_117),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_118),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_119),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_120),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_121),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_122),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_123),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_124),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_125),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_126),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_127),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_128),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_129),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_130),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_131),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_132),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_133),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_134),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_135),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_136),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_137),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_138),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_139),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_140),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_141),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_142),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_143),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_144),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_145),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_146),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_147),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_148),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_149),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_150),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_151),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_152),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_153),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_154),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_155),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_156),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_157),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_158),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_159),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_160),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_161),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_162),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_163),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_164),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_165),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_166),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_167),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_168),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_169),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_170),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_171),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_172),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_173),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_174),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_175),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_176),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_177),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_178),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_179),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_180),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_181),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_182),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_183),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_184),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_185),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_186),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_187),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_188),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_189),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_190),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_191),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_192),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_193),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_194),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_195),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_196),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_197),
	reinterpret_cast<void(*)()>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_198),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[199])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {kAotMethods[3]();},
	[]() {kAotMethods[4]();},
	[]() {kAotMethods[5]();},
	[]() {kAotMethods[6]();},
	[]() {kAotMethods[7]();},
	[]() {kAotMethods[8]();},
	[]() {kAotMethods[9]();},
	[]() {kAotMethods[10]();},
	[]() {kAotMethods[11]();},
	[]() {kAotMethods[12]();},
	[]() {kAotMethods[13]();},
	[]() {kAotMethods[14]();},
	[]() {kAotMethods[15]();},
	[]() {kAotMethods[16]();},
	[]() {kAotMethods[17]();},
	[]() {kAotMethods[18]();},
	[]() {kAotMethods[19]();},
	[]() {kAotMethods[20]();},
	[]() {kAotMethods[21]();},
	[]() {kAotMethods[22]();},
	[]() {kAotMethods[23]();},
	[]() {kAotMethods[24]();},
	[]() {kAotMethods[25]();},
	[]() {kAotMethods[26]();},
	[]() {kAotMethods[27]();},
	[]() {kAotMethods[28]();},
	[]() {kAotMethods[29]();},
	[]() {kAotMethods[30]();},
	[]() {kAotMethods[31]();},
	[]() {kAotMethods[32]();},
	[]() {kAotMethods[33]();},
	[]() {kAotMethods[34]();},
	[]() {kAotMethods[35]();},
	[]() {kAotMethods[36]();},
	[]() {kAotMethods[37]();},
	[]() {kAotMethods[38]();},
	[]() {kAotMethods[39]();},
	[]() {kAotMethods[40]();},
	[]() {kAotMethods[41]();},
	[]() {kAotMethods[42]();},
	[]() {kAotMethods[43]();},
	[]() {kAotMethods[44]();},
	[]() {kAotMethods[45]();},
	[]() {kAotMethods[46]();},
	[]() {kAotMethods[47]();},
	[]() {kAotMethods[48]();},
	[]() {kAotMethods[49]();},
	[]() {kAotMethods[50]();},
	[]() {kAotMethods[51]();},
	[]() {kAotMethods[52]();},
	[]() {kAotMethods[53]();},
	[]() {kAotMethods[54]();},
	[]() {kAotMethods[55]();},
	[]() {kAotMethods[56]();},
	[]() {kAotMethods[57]();},
	[]() {kAotMethods[58]();},
	[]() {kAotMethods[59]();},
	[]() {kAotMethods[60]();},
	[]() {kAotMethods[61]();},
	[]() {kAotMethods[62]();},
	[]() {kAotMethods[63]();},
	[]() {kAotMethods[64]();},
	[]() {kAotMethods[65]();},
	[]() {kAotMethods[66]();},
	[]() {kAotMethods[67]();},
	[]() {kAotMethods[68]();},
	[]() {kAotMethods[69]();},
	[]() {kAotMethods[70]();},
	[]() {kAotMethods[71]();},
	[]() {kAotMethods[72]();},
	[]() {kAotMethods[73]();},
	[]() {kAotMethods[74]();},
	[]() {kAotMethods[75]();},
	[]() {kAotMethods[76]();},
	[]() {kAotMethods[77]();},
	[]() {kAotMethods[78]();},
	[]() {kAotMethods[79]();},
	[]() {kAotMethods[80]();},
	[]() {kAotMethods[81]();},
	[]() {kAotMethods[82]();},
	[]() {kAotMethods[83]();},
	[]() {kAotMethods[84]();},
	[]() {kAotMethods[85]();},
	[]() {kAotMethods[86]();},
	[]() {kAotMethods[87]();},
	[]() {kAotMethods[88]();},
	[]() {kAotMethods[89]();},
	[]() {kAotMethods[90]();},
	[]() {kAotMethods[91]();},
	[]() {kAotMethods[92]();},
	[]() {kAotMethods[93]();},
	[]() {kAotMethods[94]();},
	[]() {kAotMethods[95]();},
	[]() {kAotMethods[96]();},
	[]() {kAotMethods[97]();},
	[]() {kAotMethods[98]();},
	[]() {kAotMethods[99]();},
	[]() {kAotMethods[100]();},
	[]() {kAotMethods[101]();},
	[]() {kAotMethods[102]();},
	[]() {kAotMethods[103]();},
	[]() {kAotMethods[104]();},
	[]() {kAotMethods[105]();},
	[]() {kAotMethods[106]();},
	[]() {kAotMethods[107]();},
	[]() {kAotMethods[108]();},
	[]() {kAotMethods[109]();},
	[]() {kAotMethods[110]();},
	[]() {kAotMethods[111]();},
	[]() {kAotMethods[112]();},
	[]() {kAotMethods[113]();},
	[]() {kAotMethods[114]();},
	[]() {kAotMethods[115]();},
	[]() {kAotMethods[116]();},
	[]() {kAotMethods[117]();},
	[]() {kAotMethods[118]();},
	[]() {kAotMethods[119]();},
	[]() {kAotMethods[120]();},
	[]() {kAotMethods[121]();},
	[]() {kAotMethods[122]();},
	[]() {kAotMethods[123]();},
	[]() {kAotMethods[124]();},
	[]() {kAotMethods[125]();},
	[]() {kAotMethods[126]();},
	[]() {kAotMethods[127]();},
	[]() {kAotMethods[128]();},
	[]() {kAotMethods[129]();},
	[]() {kAotMethods[130]();},
	[]() {kAotMethods[131]();},
	[]() {kAotMethods[132]();},
	[]() {kAotMethods[133]();},
	[]() {kAotMethods[134]();},
	[]() {kAotMethods[135]();},
	[]() {kAotMethods[136]();},
	[]() {kAotMethods[137]();},
	[]() {kAotMethods[138]();},
	[]() {kAotMethods[139]();},
	[]() {kAotMethods[140]();},
	[]() {kAotMethods[141]();},
	[]() {kAotMethods[142]();},
	[]() {kAotMethods[143]();},
	[]() {kAotMethods[144]();},
	[]() {kAotMethods[145]();},
	[]() {kAotMethods[146]();},
	[]() {kAotMethods[147]();},
	[]() {kAotMethods[148]();},
	[]() {kAotMethods[149]();},
	[]() {kAotMethods[150]();},
	[]() {kAotMethods[151]();},
	[]() {kAotMethods[152]();},
	[]() {kAotMethods[153]();},
	[]() {kAotMethods[154]();},
	[]() {kAotMethods[155]();},
	[]() {kAotMethods[156]();},
	[]() {kAotMethods[157]();},
	[]() {kAotMethods[158]();},
	[]() {kAotMethods[159]();},
	[]() {kAotMethods[160]();},
	[]() {kAotMethods[161]();},
	[]() {kAotMethods[162]();},
	[]() {kAotMethods[163]();},
	[]() {kAotMethods[164]();},
	[]() {kAotMethods[165]();},
	[]() {kAotMethods[166]();},
	[]() {kAotMethods[167]();},
	[]() {kAotMethods[168]();},
	[]() {kAotMethods[169]();},
	[]() {kAotMethods[170]();},
	[]() {kAotMethods[171]();},
	[]() {kAotMethods[172]();},
	[]() {kAotMethods[173]();},
	[]() {kAotMethods[174]();},
	[]() {kAotMethods[175]();},
	[]() {kAotMethods[176]();},
	[]() {kAotMethods[177]();},
	[]() {kAotMethods[178]();},
	[]() {kAotMethods[179]();},
	[]() {kAotMethods[180]();},
	[]() {kAotMethods[181]();},
	[]() {kAotMethods[182]();},
	[]() {kAotMethods[183]();},
	[]() {kAotMethods[184]();},
	[]() {kAotMethods[185]();},
	[]() {kAotMethods[186]();},
	[]() {kAotMethods[187]();},
	[]() {kAotMethods[188]();},
	[]() {kAotMethods[189]();},
	[]() {kAotMethods[190]();},
	[]() {kAotMethods[191]();},
	[]() {kAotMethods[192]();},
	[]() {kAotMethods[193]();},
	[]() {kAotMethods[194]();},
	[]() {kAotMethods[195]();},
	[]() {kAotMethods[196]();},
	[]() {kAotMethods[197]();},
	[]() {kAotMethods[198]();},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 199;
extern "C" const int kSubjectEntryIndices[199] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
	17,
	18,
	19,
	20,
	21,
	22,
	23,
	24,
	25,
	26,
	27,
	28,
	29,
	30,
	31,
	32,
	33,
	34,
	35,
	36,
	37,
	38,
	39,
	40,
	41,
	42,
	43,
	44,
	45,
	46,
	47,
	48,
	49,
	50,
	51,
	52,
	53,
	54,
	55,
	56,
	57,
	58,
	59,
	60,
	61,
	62,
	63,
	64,
	65,
	66,
	67,
	68,
	69,
	70,
	71,
	72,
	73,
	74,
	75,
	76,
	77,
	78,
	79,
	80,
	81,
	82,
	83,
	84,
	85,
	86,
	87,
	88,
	89,
	90,
	91,
	92,
	93,
	94,
	95,
	96,
	97,
	98,
	99,
	100,
	101,
	102,
	103,
	104,
	105,
	106,
	107,
	108,
	109,
	110,
	111,
	112,
	113,
	114,
	115,
	116,
	117,
	118,
	119,
	120,
	121,
	122,
	123,
	124,
	125,
	126,
	127,
	128,
	129,
	130,
	131,
	132,
	133,
	134,
	135,
	136,
	137,
	138,
	139,
	140,
	141,
	142,
	143,
	144,
	145,
	146,
	147,
	148,
	149,
	150,
	151,
	152,
	153,
	154,
	155,
	156,
	157,
	158,
	159,
	160,
	161,
	162,
	163,
	164,
	165,
	166,
	167,
	168,
	169,
	170,
	171,
	172,
	173,
	174,
	175,
	176,
	177,
	178,
	179,
	180,
	181,
	182,
	183,
	184,
	185,
	186,
	187,
	188,
	189,
	190,
	191,
	192,
	193,
	194,
	195,
	196,
	197,
	198,
};

// Single-method dispatch via hotpatch dispatch table.
// NOTE: Uses kBenchmarkWrappers (not raw kAotMethods) to ensure instance
// methods receive a valid this-pointer sentinel and default argument values.
extern "C" CHAOS_IL2CPP_INT32 RunNativeAot(
	CHAOS_IL2CPP_INT32 chaos_entry_index)
{
	if (chaos_entry_index < 0 || chaos_entry_index >= kAotMethodCount)
		return -1;
	auto& entry = s_hotpatch_entries[chaos_entry_index];
	if (chaos::il2cpp::runtime_core::HotpatchIsActive(entry)
		&& !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(entry))
	{
		uint64_t __chaos_args[4] = {}; uint64_t __chaos_ret[2] = {};
		chaos::il2cpp::runtime_core::InterpreterEntryDirect(
			entry.method_key, __chaos_args, __chaos_ret);
	} else {
		kBenchmarkWrappers[chaos_entry_index]();
	}
	return 0;
}

// All-methods loop: run every method and return a bitmask of failures.
extern "C" CHAOS_IL2CPP_INT32 RunNativeAotAll()
{
	CHAOS_IL2CPP_INT32 result = 0;
	for (int i = 0; i < kAotMethodCount; i++) {
		auto& entry = s_hotpatch_entries[i];
		if (chaos::il2cpp::runtime_core::HotpatchIsActive(entry)
			&& !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(entry))
		{
			uint64_t __chaos_args[4] = {}; uint64_t __chaos_ret[2] = {};
			chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				entry.method_key, __chaos_args, __chaos_ret);
		} else {
			// Use kBenchmarkWrappers which supply correct default argument values
			kBenchmarkWrappers[i]();
		}
	}
	return result;
}

// Fast benchmark dispatch: no setjmp, inline slot access.
extern "C" CHAOS_IL2CPP_INT32 RunNativeAotBench(
	CHAOS_IL2CPP_INT32 chaos_entry_index)
{
	if (chaos_entry_index < 0 || chaos_entry_index >= kAotMethodCount)
		return -1;
	auto& entry = s_hotpatch_entries[chaos_entry_index];
	if (chaos::il2cpp::runtime_core::HotpatchIsActive(entry)
		&& !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(entry))
	{
		chaos::il2cpp::runtime_core::InterpreterEntryDirectFast(
			entry.method_key);
	} else {
		kBenchmarkWrappers[chaos_entry_index]();
	}
	return 0;
}

// Pure AOT benchmark: calls kBenchmarkWrappers[i] directly, no hotpatch overhead.
extern "C" double BenchmarkMethod(
	int chaos_entry_index, int iterations) {
	if (chaos_entry_index < 0 || chaos_entry_index >= kAotMethodCount)
		return -1.0;
	auto start = std::chrono::steady_clock::now();
	for (int i = 0; i < iterations; i++) {
		kBenchmarkWrappers[chaos_entry_index]();
	}
	auto end = std::chrono::steady_clock::now();
	return std::chrono::duration<double, std::milli>(
		end - start).count();
}
// ── GC Slot Map Section ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for precise stack root scanning.
// One nested struct entry per method with GC-referencing stack slots.
// Iterated by GcRegisterSlotMapsFromSection() which advances by
// entry_total_size bytes per entry.
#if defined(_MSC_VER)
#pragma pack(push, 1)
#endif
#if defined(__GNUC__) || defined(__clang__)
static const struct __attribute__((packed)) {
#else
static const struct {
#endif

	/* ── Entry 0: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry0;
	/* ── Entry 1: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry1;
	/* ── Entry 2: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry2;
	/* ── Entry 3: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry3;
	/* ── Entry 4: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry4;
	/* ── Entry 5: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry5;
	/* ── Entry 6: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry6;
	/* ── Entry 7: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry7;
	/* ── Entry 8: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_24 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry8;
	/* ── Entry 9: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_26 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry9;
	/* ── Entry 10: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_28 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry10;
	/* ── Entry 11: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_30 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry11;
	/* ── Entry 12: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_31 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry12;
	/* ── Entry 13: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry13;
	/* ── Entry 14: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_33 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry14;
	/* ── Entry 15: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_34 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry15;
	/* ── Entry 16: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_35 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry16;
	/* ── Entry 17: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_36 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry17;
	/* ── Entry 18: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_38 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry18;
	/* ── Entry 19: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_39 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry19;
	/* ── Entry 20: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_40 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry20;
	/* ── Entry 21: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_42 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry21;
	/* ── Entry 22: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_43 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry22;
	/* ── Entry 23: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_44 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry23;
	/* ── Entry 24: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_45 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry24;
	/* ── Entry 25: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_46 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry25;
	/* ── Entry 26: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_47 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry26;
	/* ── Entry 27: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_48 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry27;
	/* ── Entry 28: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_50 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry28;
	/* ── Entry 29: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_51 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry29;
	/* ── Entry 30: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_52 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry30;
	/* ── Entry 31: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_53 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry31;
	/* ── Entry 32: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_54 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry32;
	/* ── Entry 33: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_55 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry33;
	/* ── Entry 34: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_56 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry34;
	/* ── Entry 35: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_57 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry35;
	/* ── Entry 36: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_58 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry36;
	/* ── Entry 37: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_59 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry37;
	/* ── Entry 38: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_61 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry38;
	/* ── Entry 39: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_62 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry39;
	/* ── Entry 40: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_64 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry40;
	/* ── Entry 41: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_66 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry41;
	/* ── Entry 42: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_67 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry42;
	/* ── Entry 43: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_68 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry43;
	/* ── Entry 44: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_69 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry44;
	/* ── Entry 45: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_70 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry45;
	/* ── Entry 46: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_71 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry46;
	/* ── Entry 47: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_72 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry47;
	/* ── Entry 48: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_73 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry48;
	/* ── Entry 49: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_74 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry49;
	/* ── Entry 50: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_75 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry50;
	/* ── Entry 51: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_77 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry51;
	/* ── Entry 52: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_78 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry52;
	/* ── Entry 53: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_79 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry53;
	/* ── Entry 54: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_80 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry54;
	/* ── Entry 55: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_81 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry55;
	/* ── Entry 56: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_82 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry56;
	/* ── Entry 57: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_83 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry57;
	/* ── Entry 58: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_84 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry58;
	/* ── Entry 59: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_85 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry59;
	/* ── Entry 60: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_86 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry60;
	/* ── Entry 61: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_87 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry61;
	/* ── Entry 62: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_88 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry62;
	/* ── Entry 63: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_89 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry63;
	/* ── Entry 64: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_93 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry64;
	/* ── Entry 65: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_94 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry65;
	/* ── Entry 66: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_95 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry66;
	/* ── Entry 67: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_96 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry67;
	/* ── Entry 68: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_97 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry68;
	/* ── Entry 69: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_98 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry69;
	/* ── Entry 70: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_99 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry70;
	/* ── Entry 71: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_100 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry71;
	/* ── Entry 72: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_101 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry72;
	/* ── Entry 73: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_102 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry73;
	/* ── Entry 74: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_103 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry74;
	/* ── Entry 75: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_104 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry75;
	/* ── Entry 76: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_105 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry76;
	/* ── Entry 77: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_106 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry77;
	/* ── Entry 78: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_107 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry78;
	/* ── Entry 79: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_108 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry79;
	/* ── Entry 80: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_109 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry80;
	/* ── Entry 81: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_110 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry81;
	/* ── Entry 82: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_111 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry82;
	/* ── Entry 83: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_112 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry83;
	/* ── Entry 84: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_113 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry84;
	/* ── Entry 85: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_114 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry85;
	/* ── Entry 86: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_115 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry86;
	/* ── Entry 87: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_116 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry87;
	/* ── Entry 88: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_117 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry88;
	/* ── Entry 89: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_118 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry89;
	/* ── Entry 90: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_119 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry90;
	/* ── Entry 91: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_120 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry91;
	/* ── Entry 92: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_122 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry92;
	/* ── Entry 93: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_124 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry93;
	/* ── Entry 94: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_125 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry94;
	/* ── Entry 95: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_126 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry95;
	/* ── Entry 96: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_127 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry96;
	/* ── Entry 97: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_128 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry97;
	/* ── Entry 98: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_129 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry98;
	/* ── Entry 99: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_130 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry99;
	/* ── Entry 100: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_131 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry100;
	/* ── Entry 101: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_132 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry101;
	/* ── Entry 102: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_134 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry102;
	/* ── Entry 103: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_135 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry103;
	/* ── Entry 104: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_137 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry104;
	/* ── Entry 105: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_139 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry105;
	/* ── Entry 106: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_140 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry106;
	/* ── Entry 107: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_141 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry107;
	/* ── Entry 108: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_142 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry108;
	/* ── Entry 109: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_143 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry109;
	/* ── Entry 110: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_144 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry110;
	/* ── Entry 111: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_145 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry111;
	/* ── Entry 112: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_146 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry112;
	/* ── Entry 113: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_147 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry113;
	/* ── Entry 114: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_148 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry114;
	/* ── Entry 115: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_149 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry115;
	/* ── Entry 116: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_150 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry116;
	/* ── Entry 117: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_151 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry117;
	/* ── Entry 118: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_152 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry118;
	/* ── Entry 119: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_153 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry119;
	/* ── Entry 120: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_154 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry120;
	/* ── Entry 121: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_155 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry121;
	/* ── Entry 122: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_156 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry122;
	/* ── Entry 123: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_157 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry123;
	/* ── Entry 124: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_158 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry124;
	/* ── Entry 125: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_159 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry125;
	/* ── Entry 126: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_160 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry126;
	/* ── Entry 127: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_161 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry127;
	/* ── Entry 128: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_162 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry128;
	/* ── Entry 129: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_163 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry129;
	/* ── Entry 130: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_164 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry130;
	/* ── Entry 131: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_165 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry131;
	/* ── Entry 132: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_166 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry132;
	/* ── Entry 133: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_169 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry133;
	/* ── Entry 134: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_170 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry134;
	/* ── Entry 135: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_171 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry135;
	/* ── Entry 136: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_172 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry136;
	/* ── Entry 137: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_173 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry137;
	/* ── Entry 138: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_174 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry138;
	/* ── Entry 139: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_175 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry139;
	/* ── Entry 140: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_176 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry140;
	/* ── Entry 141: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_177 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry141;
	/* ── Entry 142: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_178 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry142;
	/* ── Entry 143: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_179 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry143;
	/* ── Entry 144: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_180 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry144;
	/* ── Entry 145: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_181 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry145;
	/* ── Entry 146: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_182 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry146;
	/* ── Entry 147: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_183 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry147;
	/* ── Entry 148: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_184 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry148;
	/* ── Entry 149: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_185 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry149;
	/* ── Entry 150: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_186 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry150;
	/* ── Entry 151: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_187 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry151;
	/* ── Entry 152: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_188 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry152;
	/* ── Entry 153: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_189 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry153;
	/* ── Entry 154: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_190 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry154;
	/* ── Entry 155: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_191 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry155;
	/* ── Entry 156: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_192 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry156;
	/* ── Entry 157: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_193 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry157;
	/* ── Entry 158: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_194 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry158;
	/* ── Entry 159: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_197 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry159;
	/* ── Entry 160: NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_198 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry160;
} kChaosGcSlotMapsSection = {
	/* entry0 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry1 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_1),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry2 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_2),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry3 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_3),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry4 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_4 */
	.entry4 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_4),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry5 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_5 */
	.entry5 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_5),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry6 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_6 */
	.entry6 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_6),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry7 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_7 */
	.entry7 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_7),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry8 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_24 */
	.entry8 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_24),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry9 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_26 */
	.entry9 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_26),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry10 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_28 */
	.entry10 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_28),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry11 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_30 */
	.entry11 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_30),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry12 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_31 */
	.entry12 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_31),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry13 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_32 */
	.entry13 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_32),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry14 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_33 */
	.entry14 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_33),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry15 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_34 */
	.entry15 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_34),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry16 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_35 */
	.entry16 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_35),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry17 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_36 */
	.entry17 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_36),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry18 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_38 */
	.entry18 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_38),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry19 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_39 */
	.entry19 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_39),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry20 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_40 */
	.entry20 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_40),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry21 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_42 */
	.entry21 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_42),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry22 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_43 */
	.entry22 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_43),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry23 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_44 */
	.entry23 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_44),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry24 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_45 */
	.entry24 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_45),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry25 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_46 */
	.entry25 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_46),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry26 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_47 */
	.entry26 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_47),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry27 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_48 */
	.entry27 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_48),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry28 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_50 */
	.entry28 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_50),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry29 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_51 */
	.entry29 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_51),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry30 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_52 */
	.entry30 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_52),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry31 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_53 */
	.entry31 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_53),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry32 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_54 */
	.entry32 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_54),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry33 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_55 */
	.entry33 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_55),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry34 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_56 */
	.entry34 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_56),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry35 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_57 */
	.entry35 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_57),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry36 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_58 */
	.entry36 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_58),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry37 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_59 */
	.entry37 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_59),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry38 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_61 */
	.entry38 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_61),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry39 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_62 */
	.entry39 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_62),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry40 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_64 */
	.entry40 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_64),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry41 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_66 */
	.entry41 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_66),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry42 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_67 */
	.entry42 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_67),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry43 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_68 */
	.entry43 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_68),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry44 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_69 */
	.entry44 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_69),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry45 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_70 */
	.entry45 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_70),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry46 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_71 */
	.entry46 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_71),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry47 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_72 */
	.entry47 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_72),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry48 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_73 */
	.entry48 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_73),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry49 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_74 */
	.entry49 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_74),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry50 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_75 */
	.entry50 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_75),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry51 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_77 */
	.entry51 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_77),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry52 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_78 */
	.entry52 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_78),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry53 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_79 */
	.entry53 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_79),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry54 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_80 */
	.entry54 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_80),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry55 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_81 */
	.entry55 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_81),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry56 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_82 */
	.entry56 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_82),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry57 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_83 */
	.entry57 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_83),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry58 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_84 */
	.entry58 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_84),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry59 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_85 */
	.entry59 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_85),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry60 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_86 */
	.entry60 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_86),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry61 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_87 */
	.entry61 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_87),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry62 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_88 */
	.entry62 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_88),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry63 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_89 */
	.entry63 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_89),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry64 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_93 */
	.entry64 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_93),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry65 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_94 */
	.entry65 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_94),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry66 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_95 */
	.entry66 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_95),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry67 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_96 */
	.entry67 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_96),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry68 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_97 */
	.entry68 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_97),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry69 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_98 */
	.entry69 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_98),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry70 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_99 */
	.entry70 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_99),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry71 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_100 */
	.entry71 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_100),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry72 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_101 */
	.entry72 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_101),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry73 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_102 */
	.entry73 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_102),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry74 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_103 */
	.entry74 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_103),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry75 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_104 */
	.entry75 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_104),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry76 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_105 */
	.entry76 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_105),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry77 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_106 */
	.entry77 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_106),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry78 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_107 */
	.entry78 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_107),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry79 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_108 */
	.entry79 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_108),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry80 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_109 */
	.entry80 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_109),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry81 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_110 */
	.entry81 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_110),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry82 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_111 */
	.entry82 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_111),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry83 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_112 */
	.entry83 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_112),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry84 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_113 */
	.entry84 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_113),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry85 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_114 */
	.entry85 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_114),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry86 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_115 */
	.entry86 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_115),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry87 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_116 */
	.entry87 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_116),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry88 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_117 */
	.entry88 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_117),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry89 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_118 */
	.entry89 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_118),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry90 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_119 */
	.entry90 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_119),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry91 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_120 */
	.entry91 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_120),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry92 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_122 */
	.entry92 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_122),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry93 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_124 */
	.entry93 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_124),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry94 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_125 */
	.entry94 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_125),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry95 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_126 */
	.entry95 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_126),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry96 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_127 */
	.entry96 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_127),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry97 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_128 */
	.entry97 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_128),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry98 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_129 */
	.entry98 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_129),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry99 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_130 */
	.entry99 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_130),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry100 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_131 */
	.entry100 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_131),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry101 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_132 */
	.entry101 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_132),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry102 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_134 */
	.entry102 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_134),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry103 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_135 */
	.entry103 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_135),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry104 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_137 */
	.entry104 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_137),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry105 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_139 */
	.entry105 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_139),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry106 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_140 */
	.entry106 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_140),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry107 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_141 */
	.entry107 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_141),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry108 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_142 */
	.entry108 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_142),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry109 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_143 */
	.entry109 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_143),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry110 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_144 */
	.entry110 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_144),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry111 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_145 */
	.entry111 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_145),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry112 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_146 */
	.entry112 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_146),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry113 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_147 */
	.entry113 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_147),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry114 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_148 */
	.entry114 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_148),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry115 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_149 */
	.entry115 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_149),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry116 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_150 */
	.entry116 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_150),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry117 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_151 */
	.entry117 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_151),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry118 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_152 */
	.entry118 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_152),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry119 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_153 */
	.entry119 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_153),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry120 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_154 */
	.entry120 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_154),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry121 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_155 */
	.entry121 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_155),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry122 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_156 */
	.entry122 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_156),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry123 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_157 */
	.entry123 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_157),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry124 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_158 */
	.entry124 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_158),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry125 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_159 */
	.entry125 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_159),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry126 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_160 */
	.entry126 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_160),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry127 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_161 */
	.entry127 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_161),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry128 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_162 */
	.entry128 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_162),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry129 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_163 */
	.entry129 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_163),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry130 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_164 */
	.entry130 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_164),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry131 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_165 */
	.entry131 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_165),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry132 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_166 */
	.entry132 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_166),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry133 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_169 */
	.entry133 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_169),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry134 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_170 */
	.entry134 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_170),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry135 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_171 */
	.entry135 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_171),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry136 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_172 */
	.entry136 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_172),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry137 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_173 */
	.entry137 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_173),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry138 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_174 */
	.entry138 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_174),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry139 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_175 */
	.entry139 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_175),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry140 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_176 */
	.entry140 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_176),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry141 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_177 */
	.entry141 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_177),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry142 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_178 */
	.entry142 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_178),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry143 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_179 */
	.entry143 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_179),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry144 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_180 */
	.entry144 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_180),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry145 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_181 */
	.entry145 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_181),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry146 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_182 */
	.entry146 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_182),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry147 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_183 */
	.entry147 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_183),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry148 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_184 */
	.entry148 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_184),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry149 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_185 */
	.entry149 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_185),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry150 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_186 */
	.entry150 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_186),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry151 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_187 */
	.entry151 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_187),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry152 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_188 */
	.entry152 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_188),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry153 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_189 */
	.entry153 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_189),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry154 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_190 */
	.entry154 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_190),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry155 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_191 */
	.entry155 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_191),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry156 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_192 */
	.entry156 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_192),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry157 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_193 */
	.entry157 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_193),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry158 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_194 */
	.entry158 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_194),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry159 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_197 */
	.entry159 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_197),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry160 = NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_198 */
	.entry160 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_198),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 5708u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[199] = {
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_0),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_1),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_2),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_3),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_4),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_5),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_6),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_7),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_8),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_9),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_10),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_11),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_12),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_13),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_14),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_15),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_16),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_17),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_18),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_19),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_20),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_21),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_22),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_23),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_24),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_25),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_26),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_27),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_28),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_29),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_30),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_31),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_32),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_33),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_34),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_35),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_36),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_37),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_38),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_39),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_40),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_41),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_42),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_43),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_44),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_45),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_46),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_47),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_48),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_49),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_50),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_51),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_52),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_53),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_54),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_55),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_56),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_57),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_58),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_59),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_60),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_61),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_62),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_63),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_64),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_65),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_66),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_67),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_68),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_69),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_70),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_71),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_72),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_73),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_74),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_75),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_76),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_77),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_78),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_79),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_80),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_81),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_82),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_83),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_84),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_85),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_86),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_87),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_88),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_89),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_90),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_91),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_92),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_93),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_94),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_95),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_96),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_97),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_98),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_99),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_100),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_101),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_102),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_103),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_104),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_105),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_106),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_107),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_108),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_109),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_110),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_111),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_112),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_113),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_114),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_115),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_116),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_117),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_118),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_119),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_120),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_121),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_122),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_123),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_124),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_125),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_126),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_127),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_128),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_129),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_130),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_131),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_132),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_133),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_134),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_135),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_136),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_137),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_138),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_139),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_140),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_141),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_142),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_143),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_144),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_145),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_146),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_147),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_148),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_149),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_150),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_151),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_152),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_153),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_154),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_155),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_156),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_157),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_158),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_159),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_160),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_161),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_162),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_163),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_164),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_165),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_166),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_167),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_168),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_169),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_170),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_171),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_172),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_173),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_174),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_175),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_176),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_177),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_178),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_179),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_180),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_181),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_182),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_183),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_184),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_185),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_186),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_187),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_188),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_189),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_190),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_191),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_192),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_193),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_194),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_195),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_196),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_197),
	reinterpret_cast<void*>(&NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_198),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 199u,
	.reverse_pinvoke_wrappers  = nullptr,
	.reverse_pinvoke_wrapper_count = 0u,
	.invoker_pointers          = nullptr,
	.invoker_pointer_count     = 0u,
	.unresolved_virtual_calls = nullptr,
	.unresolved_virtual_call_count = 0u,
	.type_capabilities       = nullptr,
	.type_capability_count   = 0u,
	.vtable_descriptors = nullptr,
	.vtable_descriptor_count = 0u,
	.slot_map_section_begin = reinterpret_cast<const void*>(&kChaosGcSlotMapsSection),
	.slot_map_section_end   = reinterpret_cast<const void*>(
		reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&kChaosGcSlotMapsSection) + kChaosGcSlotMapsSize),
};

// MetadataRegistrationV0
extern "C" const MetadataRegistrationV0 chaos_codegen_metadata_registration
	= {
	.struct_size              = sizeof(MetadataRegistrationV0),
	.generic_types            = kGenericTypeEntries,
	.generic_type_count       = sizeof(kGenericTypeEntries) / sizeof(kGenericTypeEntries[0]),
	.generic_type_args        = kGenericTypeArgTokens,
	.generic_type_arg_count   = sizeof(kGenericTypeArgTokens) / sizeof(kGenericTypeArgTokens[0]),
	.generic_methods          = kGenericMethodEntries,
	.generic_method_count     = sizeof(kGenericMethodEntries) / sizeof(kGenericMethodEntries[0]),
	.generic_method_args      = kGenericMethodArgTokens,
	.generic_method_arg_count = sizeof(kGenericMethodArgTokens) / sizeof(kGenericMethodArgTokens[0]),
	.method_aot_entries       = s_method_aot_entries,
	.method_aot_entry_count  = sizeof(s_method_aot_entries) / sizeof(s_method_aot_entries[0]),
	.method_aot_entry_args    = s_method_aot_entry_args,
	.method_aot_entry_arg_count = sizeof(s_method_aot_entry_args) / sizeof(s_method_aot_entry_args[0]),
	.field_offsets           = nullptr,
	.field_offset_count      = 0u,
	.metadata_usages         = nullptr,
	.metadata_usage_count    = 0u,
};

// CodegenRegistrationOptionsV0
extern "C" const CodegenRegistrationOptionsV0 chaos_codegen_options
	= {
	.struct_size       = sizeof(CodegenRegistrationOptionsV0),
	.registration_flags = 0u,
	.image_name_utf8    = "NativeMemoryPointersSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects[1] = {
	{ 0x04000001u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::_exitCode", "_exitCode", "System.Int32", 0LL, 3u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects[199] = {
	{ 0x00000003u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000004u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000005u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000006u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000007u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000008u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000009u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Au, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Bu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Cu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Du, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_10:System.Void()", "Subject_10", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Eu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_11:System.Void()", "Subject_11", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Fu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000010u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000011u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_14:System.Void()", "Subject_14", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000012u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_15:System.Void()", "Subject_15", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000013u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_16:System.Void()", "Subject_16", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000014u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_17:System.Void()", "Subject_17", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000015u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_18:System.Void()", "Subject_18", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000016u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_19:System.Void()", "Subject_19", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000017u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_20:System.Void()", "Subject_20", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000018u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_21:System.Void()", "Subject_21", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000019u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_22:System.Void()", "Subject_22", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Au, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_23:System.Void()", "Subject_23", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Bu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_24:System.Void()", "Subject_24", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Cu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_25:System.Void()", "CustomEntrySubject_25", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Du, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_26:System.Void()", "Subject_26", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Eu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_27:System.Void()", "CustomEntrySubject_27", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Fu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_28:System.Void()", "Subject_28", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000020u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_29:System.Void()", "CustomEntrySubject_29", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000021u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_30:System.Void()", "Subject_30", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000022u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_31:System.Void()", "Subject_31", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000023u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_32:System.Void()", "Subject_32", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000024u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_33:System.Void()", "Subject_33", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000025u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_34:System.Void()", "Subject_34", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000026u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_35:System.Void()", "Subject_35", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000027u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_36:System.Void()", "Subject_36", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000028u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_37:System.Void()", "CustomEntrySubject_37", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000029u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_38:System.Void()", "Subject_38", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Au, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_39:System.Void()", "Subject_39", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Bu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_40:System.Void()", "Subject_40", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Cu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_41:System.Void()", "CustomEntrySubject_41", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Du, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_42:System.Void()", "Subject_42", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Eu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_43:System.Void()", "Subject_43", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Fu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_44:System.Void()", "Subject_44", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000030u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_45:System.Void()", "Subject_45", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000031u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_46:System.Void()", "Subject_46", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000032u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_47:System.Void()", "Subject_47", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000033u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_48:System.Void()", "Subject_48", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000034u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_49:System.Void()", "CustomEntrySubject_49", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000035u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_50:System.Void()", "Subject_50", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000036u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_51:System.Void()", "Subject_51", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000037u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_52:System.Void()", "Subject_52", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000038u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_53:System.Void()", "Subject_53", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000039u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_54:System.Void()", "Subject_54", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Au, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_55:System.Void()", "Subject_55", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Bu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_56:System.Void()", "Subject_56", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Cu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_57:System.Void()", "Subject_57", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Du, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_58:System.Void()", "Subject_58", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Eu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_59:System.Void()", "Subject_59", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Fu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_60:System.Void()", "CustomEntrySubject_60", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000040u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_61:System.Void()", "Subject_61", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000041u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_62:System.Void()", "Subject_62", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000042u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_63:System.Void()", "CustomEntrySubject_63", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000043u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_64:System.Void()", "Subject_64", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000044u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_65:System.Void()", "CustomEntrySubject_65", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000045u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_66:System.Void()", "Subject_66", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000046u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_67:System.Void()", "Subject_67", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000047u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_68:System.Void()", "Subject_68", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000048u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_69:System.Void()", "Subject_69", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000049u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_70:System.Void()", "Subject_70", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000004Au, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_71:System.Void()", "Subject_71", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000004Bu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_72:System.Void()", "Subject_72", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000004Cu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_73:System.Void()", "Subject_73", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000004Du, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_74:System.Void()", "Subject_74", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000004Eu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_75:System.Void()", "Subject_75", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000004Fu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_76:System.Void()", "CustomEntrySubject_76", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000050u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_77:System.Void()", "Subject_77", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000051u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_78:System.Void()", "Subject_78", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000052u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_79:System.Void()", "Subject_79", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000053u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_80:System.Void()", "Subject_80", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000054u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_81:System.Void()", "Subject_81", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000055u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_82:System.Void()", "Subject_82", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000056u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_83:System.Void()", "Subject_83", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000057u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_84:System.Void()", "Subject_84", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000058u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_85:System.Void()", "Subject_85", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000059u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_86:System.Void()", "Subject_86", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000005Au, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_87:System.Void()", "Subject_87", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000005Bu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_88:System.Void()", "Subject_88", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000005Cu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_89:System.Void()", "Subject_89", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000005Du, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_90:System.Void()", "CustomEntrySubject_90", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000005Eu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_91:System.Void()", "CustomEntrySubject_91", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000005Fu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_92:System.Void()", "Subject_92", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000060u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_93:System.Void()", "Subject_93", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000061u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_94:System.Void()", "Subject_94", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000062u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_95:System.Void()", "Subject_95", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000063u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_96:System.Void()", "Subject_96", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000064u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_97:System.Void()", "Subject_97", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000065u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_98:System.Void()", "Subject_98", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000066u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_99:System.Void()", "Subject_99", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000067u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_100:System.Void()", "Subject_100", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000068u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_101:System.Void()", "Subject_101", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000069u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_102:System.Void()", "Subject_102", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000006Au, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_103:System.Void()", "Subject_103", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000006Bu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_104:System.Void()", "Subject_104", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000006Cu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_105:System.Void()", "Subject_105", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000006Du, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_106:System.Void()", "Subject_106", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000006Eu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_107:System.Void()", "Subject_107", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000006Fu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_108:System.Void()", "Subject_108", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000070u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_109:System.Void()", "Subject_109", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000071u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_110:System.Void()", "Subject_110", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000072u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_111:System.Void()", "Subject_111", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000073u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_112:System.Void()", "Subject_112", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000074u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_113:System.Void()", "Subject_113", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000075u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_114:System.Void()", "Subject_114", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000076u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_115:System.Void()", "Subject_115", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000077u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_116:System.Void()", "Subject_116", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000078u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_117:System.Void()", "Subject_117", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000079u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_118:System.Void()", "Subject_118", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000007Au, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_119:System.Void()", "Subject_119", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000007Bu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_120:System.Void()", "Subject_120", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000007Cu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_121:System.Void()", "CustomEntrySubject_121", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000007Du, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_122:System.Void()", "Subject_122", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000007Eu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_123:System.Void()", "CustomEntrySubject_123", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000007Fu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_124:System.Void()", "Subject_124", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000080u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_125:System.Void()", "Subject_125", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000081u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_126:System.Void()", "Subject_126", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000082u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_127:System.Void()", "Subject_127", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000083u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_128:System.Void()", "Subject_128", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000084u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_129:System.Void()", "Subject_129", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000085u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_130:System.Void()", "Subject_130", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000086u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_131:System.Void()", "Subject_131", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000087u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_132:System.Void()", "Subject_132", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000088u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_133:System.Void()", "CustomEntrySubject_133", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000089u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_134:System.Void()", "Subject_134", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000008Au, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_135:System.Void()", "Subject_135", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000008Bu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_136:System.Void()", "Subject_136", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000008Cu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_137:System.Void()", "Subject_137", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000008Du, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_138:System.Void()", "CustomEntrySubject_138", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000008Eu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_139:System.Void()", "Subject_139", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000008Fu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_140:System.Void()", "Subject_140", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000090u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_141:System.Void()", "Subject_141", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000091u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_142:System.Void()", "Subject_142", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000092u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_143:System.Void()", "Subject_143", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000093u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_144:System.Void()", "Subject_144", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000094u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_145:System.Void()", "Subject_145", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000095u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_146:System.Void()", "Subject_146", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000096u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_147:System.Void()", "Subject_147", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000097u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_148:System.Void()", "Subject_148", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000098u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_149:System.Void()", "Subject_149", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000099u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_150:System.Void()", "Subject_150", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000009Au, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_151:System.Void()", "Subject_151", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000009Bu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_152:System.Void()", "Subject_152", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000009Cu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_153:System.Void()", "Subject_153", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000009Du, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_154:System.Void()", "Subject_154", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000009Eu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_155:System.Void()", "Subject_155", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000009Fu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_156:System.Void()", "Subject_156", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000A0u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_157:System.Void()", "Subject_157", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000A1u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_158:System.Void()", "Subject_158", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000A2u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_159:System.Void()", "Subject_159", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000A3u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_160:System.Void()", "Subject_160", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000A4u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_161:System.Void()", "Subject_161", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000A5u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_162:System.Void()", "Subject_162", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000A6u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_163:System.Void()", "Subject_163", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000A7u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_164:System.Void()", "Subject_164", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000A8u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_165:System.Void()", "Subject_165", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000A9u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_166:System.Void()", "Subject_166", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000AAu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_167:System.Void()", "CustomEntrySubject_167", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000ABu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_168:System.Void()", "CustomEntrySubject_168", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000ACu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_169:System.Void()", "Subject_169", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000ADu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_170:System.Void()", "Subject_170", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000AEu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_171:System.Void()", "Subject_171", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000AFu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_172:System.Void()", "Subject_172", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000B0u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_173:System.Void()", "Subject_173", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000B1u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_174:System.Void()", "Subject_174", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000B2u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_175:System.Void()", "Subject_175", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000B3u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_176:System.Void()", "Subject_176", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000B4u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_177:System.Void()", "Subject_177", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000B5u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_178:System.Void()", "Subject_178", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000B6u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_179:System.Void()", "Subject_179", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000B7u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_180:System.Void()", "Subject_180", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000B8u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_181:System.Void()", "Subject_181", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000B9u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_182:System.Void()", "Subject_182", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000BAu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_183:System.Void()", "Subject_183", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000BBu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_184:System.Void()", "Subject_184", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000BCu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_185:System.Void()", "Subject_185", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000BDu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_186:System.Void()", "Subject_186", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000BEu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_187:System.Void()", "Subject_187", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000BFu, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_188:System.Void()", "Subject_188", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000C0u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_189:System.Void()", "Subject_189", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000C1u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_190:System.Void()", "Subject_190", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000C2u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_191:System.Void()", "Subject_191", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000C3u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_192:System.Void()", "Subject_192", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000C4u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_193:System.Void()", "Subject_193", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000C5u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_194:System.Void()", "Subject_194", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000C6u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_195:System.Void()", "CustomEntrySubject_195", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000C7u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_196:System.Void()", "CustomEntrySubject_196", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000C8u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_197:System.Void()", "Subject_197", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x000000C9u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_198:System.Void()", "Subject_198", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0x00000001u, "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects", "NativeMemoryPointersSubjects/NativeMemoryPointersSubjects", "", "NativeMemoryPointersSubjects", "NativeMemoryPointersSubjects", nullptr, kReflFields_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects, 1u, nullptr, 0u,
/* EVENT_SECTION_START */
nullptr, 0u,    kReflMethods_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects, 199u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "NativeMemoryPointersSubjects", kReflTypePtrs, 1u, 1, 0, 0, 0 };

// Fake ImageHandle that ResolveSubjectId will decode back to kReflImage.
// BootstrapRuntime's aot_image_handle fallback discovers this via
// LookupModule(mid)->image at lines 311-321 of bootstrap.cpp.
// ── Generic registration proof-host helper ─────────────────
// Populate generic registration arrays from this TU.
static void ChaosDoPopulateGenericRegistration(
	CHAOS_IL2CPP_UINT32* out_type_count,
	const GenericTypeRegistrationEntryV0** out_type_entries,
	const CHAOS_IL2CPP_UINT32** out_type_args,
	CHAOS_IL2CPP_UINT32* out_type_arg_count,
	CHAOS_IL2CPP_UINT32* out_method_count,
	const GenericMethodRegistrationEntryV0** out_method_entries,
	const CHAOS_IL2CPP_UINT32** out_method_args,
	CHAOS_IL2CPP_UINT32* out_method_arg_count)
{
	*out_type_count = 0;
	*out_type_entries = kGenericTypeEntries;
	*out_type_args = kGenericTypeArgTokens;
	*out_type_arg_count = 0;
	*out_method_count = 0;
	*out_method_entries = kGenericMethodEntries;
	*out_method_args = kGenericMethodArgTokens;
	*out_method_arg_count = 0;
}

// Static initializer registers the callback.
extern "C" void (*g_chaos_populate_generic_registration)(
	CHAOS_IL2CPP_UINT32*, const GenericTypeRegistrationEntryV0**, const CHAOS_IL2CPP_UINT32**, CHAOS_IL2CPP_UINT32*,
	CHAOS_IL2CPP_UINT32*, const GenericMethodRegistrationEntryV0**, const CHAOS_IL2CPP_UINT32**, CHAOS_IL2CPP_UINT32*);
namespace {
struct ChaosGenericRegistrationInit {
	ChaosGenericRegistrationInit() {
		g_chaos_populate_generic_registration =
			&ChaosDoPopulateGenericRegistration;
	}
} g_chaos_reg_init;
}
// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_0()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[1])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[1])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_1()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_2()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[4])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[4])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_3()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[5])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[5])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_4()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[6])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[6])();
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			chaos_locals[0] = _s4;
			_s4 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s5;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_5()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_6()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_7()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[10])();
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_8()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_9()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_10()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_11()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_12()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_12(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_13()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_13(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_14()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_14(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_15()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_15(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_16()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_16(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_17()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_17(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_18()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_18(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_19()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_19(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_20()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_20(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_21()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_21(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_22()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_22(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_23()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_23(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_24()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_24(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[11])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = 0;
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[11])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_25()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_25(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_26()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_26(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[14])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = 0;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s2;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[14])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_27()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_27(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_28()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_28(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[15])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_29()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_29(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_30()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_30(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[16])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[16])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_31()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_31(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[17])(chaos_arg_0);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_32()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_32(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[18])(chaos_arg_0);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_33()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_33(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[19])(chaos_arg_0);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_34()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_34(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[20])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s0;
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosGuidGetHashCode(ChaosLoadInt64(chaos_arg_0));
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[20])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosGuidGetHashCode(ChaosLoadInt64(chaos_arg_0));
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_35()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_35(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[22])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[22])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_36()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_36(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[24])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = 0;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s2;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[24])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_37()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_37(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_38()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_38(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};
	CHAOS_IL2CPP_INTPTR _s19{};
	CHAOS_IL2CPP_INTPTR _s20{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[25])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = 0;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s2;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[25])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_39()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_39(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[26])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = 0;
			_s2 = 0;
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[26])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_40()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_40(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[27])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s2;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[27])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_41()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_41(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_42()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_42(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[28])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[28])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_43()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_43(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[29])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[29])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_44()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_44(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[30])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[30])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_45()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_45(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[31])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[31])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_46()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_46(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			_s2 = _s1;
			{
				const auto chaos_address = _s2;
				if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
				{
					auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
					*chaos_slot = 0;
				}
				else
				{
					*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
				}
			}
			_s2 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[33])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
			_s3 = _s2;
			{
				const auto chaos_address = _s3;
				if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
				{
					auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
					*chaos_slot = 0;
				}
				else
				{
					*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
				}
			}
			_s3 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_2 = _s3;
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[33])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_47()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_47(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[34])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[34])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_48()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_48(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[35])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[35])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_49()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_49(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_50()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_50(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[36])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[36])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_51()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_51(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[37])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[37])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_52()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_52(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[38])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[38])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_53()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_53(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[39])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[39])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_54()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_54(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[40])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[40])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_55()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_55(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[41])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[41])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_56()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_56(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[42])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[42])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_57()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_57(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[43])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[43])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_58()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_58(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[44])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[44])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_59()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_59(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[45])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_60()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_60(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_61()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_61(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[46])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[46])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_62()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_62(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[47])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[47])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_63()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_63(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_64()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_64(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};
	CHAOS_IL2CPP_INTPTR _s19{};
	CHAOS_IL2CPP_INTPTR _s20{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[48])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = [&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(_s0); return _c_arr ? static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_c_arr->length)) : CHAOS_IL2CPP_INTPTR{}; }();
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[48])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = [&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(_s1); return _c_arr ? static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_c_arr->length)) : CHAOS_IL2CPP_INTPTR{}; }();
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_65()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_65(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_66()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_66(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[49])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[49])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_67()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_67(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[50])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[50])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_68()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_68(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[51])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s2;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[51])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_69()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_69(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_Guid__Empty;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[53])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_Guid__Empty;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[53])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_70()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_70(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[54])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[54])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_71()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_71(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[55])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[55])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_72()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_72(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[56])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_73()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_73(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[57])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s3 = 0;
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[57])(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			chaos_locals[0] = _s4;
			_s4 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s5;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_74()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_74(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[58])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[58])(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			chaos_locals[0] = _s4;
			_s4 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s5;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_75()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_75(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[59])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{{
				_s2 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[59])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_76()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_76(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_77()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_77(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[60])(chaos_arg_0);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_78()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_78(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[61])(chaos_arg_0);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_79()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_79(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[62])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[62])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_80()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_80(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[63])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[63])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_81()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_81(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[64])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[64])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_82()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_82(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[65])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[65])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_83()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_83(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[66])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[66])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_84()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_84(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[67])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[67])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_85()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_85(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[68])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[68])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_86()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_86(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[69])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[69])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_87()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_87(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[70])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[70])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_88()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_88(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[71])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s2;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[71])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosObjectGetHashCode(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_89()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_89(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[71])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_90()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_90(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_91()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_91(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_92()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_92(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_93()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_93(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_UINT8(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[72])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = 0;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_UINT8(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[72])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_94()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_94(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_UINT8(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[73])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_UINT8(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[73])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_95()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_95(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_UINT8(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[74])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_UINT8(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[74])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_96()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_96(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT16(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[75])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = 0;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT16(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[75])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_97()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_97(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT16(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[76])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT16(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[76])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_98()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_98(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT16(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[77])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT16(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[77])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_99()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_99(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[78])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = 0;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[78])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_100()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_100(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[79])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[79])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_101()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_101(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[80])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[80])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_102()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_102(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[81])(chaos_arg_0, chaos_arg_1);
				_s0 = ChaosStoreInt64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = 0;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[81])(chaos_arg_0, chaos_arg_1);
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_103()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_103(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[82])(chaos_arg_0, chaos_arg_1);
				_s0 = ChaosStoreInt64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[82])(chaos_arg_0, chaos_arg_1);
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_104()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_104(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[83])(chaos_arg_0);
				_s0 = ChaosStoreInt64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[83])(chaos_arg_0);
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_105()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_105(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[84])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = 0;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[84])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_106()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_106(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[85])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[85])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_107()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_107(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[86])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[86])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_108()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_108(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[87])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[87])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_109()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_109(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[88])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[88])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_110()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_110(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[89])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[89])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_111()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_111(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[90])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[90])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_112()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_112(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[91])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[91])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_113()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_113(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[92])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[92])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_114()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_114(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[93])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[93])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_115()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_115(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[94])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[94])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_116()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_116(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[95])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[95])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_117()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_117(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = 0;
			_s2 = 0;
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[96])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s3 = 0;
			_s4 = 0;
			_s5 = 0;
			{
				const auto chaos_arg_2 = _s5;
				const auto chaos_arg_1 = _s4;
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[96])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			chaos_locals[0] = _s4;
			_s4 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s5;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_118()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_118(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[97])(chaos_arg_0);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_119()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_119(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[98])(chaos_arg_0);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_120()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_120(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[99])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = 0;
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[99])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_121()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_121(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_122()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_122(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[99])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(41240749u);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosReflectionGetTypeFromHandle(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[99])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_123()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_123(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_124()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_124(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[100])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[100])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_125()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_125(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[101])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[101])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_126()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_126(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[102])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[102])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_127()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_127(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[103])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[103])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_128()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_128(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[104])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[104])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_129()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_129(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[105])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[105])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_130()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_130(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[106])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[106])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_131()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_131(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[107])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			{{
				_s1 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[107])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_132()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_132(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[108])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_133()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_133(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_134()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_134(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[109])(chaos_arg_0);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_135()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_135(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[110])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_136()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_136(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_137()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_137(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[111])();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[112])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[111])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[112])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_138()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_138(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_139()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_139(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[113])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_140()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_140(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[114])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_141()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_141(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[115])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_142()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_142(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[116])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_143()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_143(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[117])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_144()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_144(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[118])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_145()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_145(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(65);
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[119])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_146()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_146(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(65);
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[120])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_147()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_147(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(65);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[121])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_148()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_148(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[122])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_149()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_149(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[123])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_150()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_150(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[124])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_151()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_151(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s2));
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[125])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_152()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_152(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s2));
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[126])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_153()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_153(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[127])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_154()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_154(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[128])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_155()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_155(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_2 = _s2;
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[129])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_156()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_156(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			_s1 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[130])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_157()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_157(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[131])(chaos_arg_0);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_158()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_158(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[132])(chaos_arg_0);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_159()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_159(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[133])(chaos_arg_0);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_160()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_160(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[134])(chaos_arg_0);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_161()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_161(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[135])(chaos_arg_0);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_162()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_162(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = chaos_static_System_Private_CoreLib_System_IntPtr__Zero;
			{
				const auto chaos_arg_0 = _s0;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[136])(chaos_arg_0);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_163()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_163(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[137])();
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_164()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_164(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[138])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[138])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			chaos_locals[0] = _s6;
			_s6 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s7;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_165()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_165(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[139])();
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[139])();
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			chaos_locals[0] = _s6;
			_s6 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s7;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_166()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_166(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(void)>(kChaosExternalRuntimeFnTable[140])();
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s2 = 0;
			{
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(void)>(kChaosExternalRuntimeFnTable[140])();
				_s3 = ChaosStoreInt64(chaos_result);
			}
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s3));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_167()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_167(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_168()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_168(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_169()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_169(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};
	CHAOS_IL2CPP_INTPTR _s19{};
	CHAOS_IL2CPP_INTPTR _s20{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[141])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = 0;
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s5 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s5));
			{
				const auto chaos_arg_0 = _s5;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[141])(chaos_arg_0);
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s6) == static_cast<CHAOS_IL2CPP_INTPTR>(_s7) ? 1 : 0);
			chaos_locals[0] = _s6;
			_s6 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s7;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_170()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_170(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_UINT8(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[142])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_UINT8(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[142])(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_171()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_171(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[143])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[143])(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_172()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_172(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[144])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Decimal__op_Explicit_System_Int32_System_Decimal_(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[144])(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Decimal__op_Explicit_System_Int32_System_Decimal_(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_173()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_173(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};
	CHAOS_IL2CPP_INTPTR _s19{};
	CHAOS_IL2CPP_INTPTR _s20{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[146])(chaos_arg_0);
				_s1 = ChaosStoreFloat64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s1)));
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[146])(chaos_arg_0);
				_s3 = ChaosStoreFloat64(chaos_result);
			}
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s3)));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_174()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_174(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT16(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[147])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT16(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[147])(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_175()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_175(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[148])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[148])(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_176()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_176(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};
	CHAOS_IL2CPP_INTPTR _s19{};
	CHAOS_IL2CPP_INTPTR _s20{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[149])(chaos_arg_0);
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[149])(chaos_arg_0);
				_s3 = ChaosStoreInt64(chaos_result);
			}
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s3));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_177()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_177(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT8(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[150])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT8(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[150])(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_178()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_178(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};
	CHAOS_IL2CPP_INTPTR _s19{};
	CHAOS_IL2CPP_INTPTR _s20{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<float(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[151])(chaos_arg_0);
				_s1 = ChaosStoreFloat32(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_load_float32(_s1)));
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<float(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[151])(chaos_arg_0);
				_s3 = ChaosStoreFloat32(chaos_result);
			}
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_load_float32(_s3)));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_179()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_179(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_UINT16(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[152])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_UINT16(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[152])(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_180()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_180(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[153])(chaos_arg_0);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[153])(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_181()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_181(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};
	CHAOS_IL2CPP_INTPTR _s19{};
	CHAOS_IL2CPP_INTPTR _s20{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_UINT64(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[154])(chaos_arg_0);
				_s1 = chaos_store_uint64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s2 = 0;
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s3));
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_UINT64(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[154])(chaos_arg_0);
				_s3 = chaos_store_uint64(chaos_result);
			}
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s3));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
			chaos_locals[0] = _s2;
			_s2 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s3;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_182()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_182(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[155])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_183()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_183(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[156])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_184()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_184(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(65);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[157])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_185()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_185(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[158])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_186()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_186(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[159])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_187()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_187(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s2));
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[160])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_188()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_188(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Decimal, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Decimal.hot;
				chaos_external_runtime_System_Private_CoreLib_System_Decimal___ctor_System_Void_System_Int32_(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[162])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_189()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_189(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			_s2 = ChaosStoreFloat32(42.0f);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[163])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_190()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_190(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			_s2 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[164])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_191()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_191(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[165])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_192()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_192(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[166])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_193()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_193(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[167])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_194()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_194(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s2));
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[168])(chaos_arg_0, chaos_arg_1);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_195()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_195(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::CustomEntrySubject_196()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_CustomEntrySubject_196(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_197()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_197(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s2));
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Runtime_InteropServices_System_IO_UnmanagedMemoryAccessor, {});
				chaos_object->header.type_info = &chaos_mt_System_Runtime_InteropServices_System_IO_UnmanagedMemoryAccessor.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: NativeMemoryPointersSubjects/NativeMemoryPointersSubjects::Subject_198()
extern "C" void NativeMemoryPointersSubjects_NativeMemoryPointersSubjects_Subject_198(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};


	CHAOS_EH_TRY
			_s0 = 0;
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s2));
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Runtime_InteropServices_System_IO_UnmanagedMemoryAccessor, {});
				chaos_object->header.type_info = &chaos_mt_System_Runtime_InteropServices_System_IO_UnmanagedMemoryAccessor.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_NativeMemoryPointersSubjects_NativeMemoryPointersSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}



}  // namespace chaos::il2cpp::codegen::NativeMemoryPointersSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 199;

extern "C" void ChaosJitRegisterAll() {}