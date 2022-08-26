../test/test.sy
[32m5.500000[0m
[32m5.500000[0m
[32m3.141593[0m
[32m3.141593[0m
[32m0.000001[0m
[32m0.000001[0m
[32m0.000000[0m
[32m0.000000[0m
[32m0.000000[0m
[32m0.000000[0m
[32m-33000.000000[0m
[32m-33000.000000[0m
[32m95.033188[0m
[32m95.033188[0m
[32m0.000000[0m
[32m0.000000[0m
[32m34.557522[0m
[32m34.557522[0m
[32m233[0m
[32m233.000000[0m
[32m4095[0m
[32m4095.000000[0m
[32m1000000000.000000[0m
[32m1000000000[0m
[32m2.900000[0m
[32m2[0m
[32m3.200000[0m
[32m3[0m
[32m5[0m
[32m5[0m
[32mBasic IR[0m
@RADIUS = global float 5.500000

@PI = global float 3.141593

@EPS = global float 0.000001

@PI_HEX = global float 0.000000

@HEX2 = global float 0.000000

@FACT = global float -33000.000000

@EVAL1 = global float 95.033188

@EVAL2 = global float 0.000000

@EVAL3 = global float 34.557522

@CONV1 = global float 233.000000

@CONV2 = global float 4095.000000

@MAX = global i32 1000000000

@TWO = global i32 2

@THREE = global i32 3

@FIVE = global i32 5

@main.ifnext.arr = global [10 x float] [float 1.000000, float 2.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000]

declare i32 @getint()

declare i32 @getch()

declare float @getfloat()

declare i32 @getarray(i32*)

declare i32 @getfarray(float*)

declare void @putint(i32)

declare void @putch(i32)

declare void @putfloat(float)

declare void @putarray(i32, i32*)

declare void @putfarray(float, float*)

declare void @putf(i8*)

declare void @starttime()

declare void @stoptime()

declare void @llvm.memcpy.inline.p0.p0.i32(i8*, i8*, i32, i1)

define float @float_abs(float %x) {
entry:				
  %x.alloca = alloca float, align 4
  store float %x, float* %x.alloca, align 4
  %x.cpy = load float, float* %x.alloca
  %int2float = sitofp i32  0 to float
  %lestmp = fcmp ult float %x.cpy, %int2float
  %cond = icmp ne i1 %lestmp, 0
  br i1 %cond, label %then, label %ifnext

then:				; preds = %entry, 
  %x.cpy1 = load float, float* %x.alloca
  %negtmp = fsub float 0.000000, %x.cpy1
  ret float %negtmp

ifnext:				; preds = %entry, 
  %x.cpy2 = load float, float* %x.alloca
  ret float %x.cpy2

}

define float @circle_area(i32 %radius) {
entry:				
  %radius.alloca = alloca i32, align 4
  store i32 %radius, i32* %radius.alloca, align 4
  %radius.cpy = load i32, i32* %radius.alloca
  %int2float = sitofp i32  %radius.cpy to float
  %multmp = fmul float 3.141593, %int2float
  %radius.cpy1 = load i32, i32* %radius.alloca
  %int2float1 = sitofp i32  %radius.cpy1 to float
  %multmp1 = fmul float %multmp, %int2float1
  %radius.cpy2 = load i32, i32* %radius.alloca
  %radius.cpy3 = load i32, i32* %radius.alloca
  %multmp2 = mul i32 %radius.cpy2, %radius.cpy3
  %int2float2 = sitofp i32  %multmp2 to float
  %multmp3 = fmul float %int2float2, 3.141593
  %addtmp = fadd float %multmp1, %multmp3
  %int2float3 = sitofp i32  2 to float
  %divtmp = fdiv float %addtmp, %int2float3
  ret float %divtmp

}

define i32 @float_eq(float %a, float %b) {
entry:				
  %b.alloca = alloca float, align 4
  %a.alloca = alloca float, align 4
  store float %a, float* %a.alloca, align 4
  store float %b, float* %b.alloca, align 4
  %a.cpy = load float, float* %a.alloca
  %b.cpy = load float, float* %b.alloca
  %subtmp = fsub float %a.cpy, %b.cpy
  %funcCall = call float @float_abs(float %subtmp)
  %lestmp = fcmp ult float %funcCall, 0.000001
  %cond = icmp ne i1 %lestmp, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  %float2int = fptosi float  1.000000 to i32
  ret i32 %float2int

else:				; preds = %entry, 
  ret i32 0

ifnext:				
  retvoid

}

define void @error() {
entry:				
  call void @putch(i32 101)
  call void @putch(i32 114)
  call void @putch(i32 114)
  call void @putch(i32 111)
  call void @putch(i32 114)
  call void @putch(i32 10)
  retvoid

}

define void @ok() {
entry:				
  call void @putch(i32 111)
  call void @putch(i32 107)
  call void @putch(i32 10)
  retvoid

}

define void @assert(i32 %cond) {
entry:				
  %cond.alloca = alloca i32, align 4
  store i32 %cond, i32* %cond.alloca, align 4
  %cond.cpy = load i32, i32* %cond.alloca
  %nottmp = icmp eq i32 %cond.cpy, 0
  %cond = icmp ne i1 %nottmp, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  call void @error()
  br label %ifnext

else:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %then, %else, 
  retvoid

}

define void @assert_not(i32 %cond) {
entry:				
  %cond.alloca = alloca i32, align 4
  store i32 %cond, i32* %cond.alloca, align 4
  %cond.cpy = load i32, i32* %cond.alloca
  %cond = icmp ne i32 %cond.cpy, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  call void @error()
  br label %ifnext

else:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %then, %else, 
  retvoid

}

define i32 @main() {
entry:				
  %area_trunc = alloca float, align 4
  %area = alloca float, align 4
  %input = alloca float, align 4
  %len = alloca i32, align 4
  %p = alloca i32, align 4
  %i = alloca i32, align 4
  %funcCall = call i32 @float_eq(float 0.000000, float -33000.000000)
  call void @assert_not(i32 %funcCall)
  %funcCall1 = call i32 @float_eq(float 95.033188, float 0.000000)
  call void @assert_not(i32 %funcCall1)
  %funcCall2 = call i32 @float_eq(float 0.000000, float 34.557522)
  call void @assert(i32 %funcCall2)
  %float2int = fptosi float  5.500000 to i32
  %funcCall3 = call float @circle_area(i32 %float2int)
  %float2int1 = fptosi float  5.000000 to i32
  %funcCall4 = call float @circle_area(i32 %float2int1)
  %funcCall5 = call i32 @float_eq(float %funcCall3, float %funcCall4)
  call void @assert(i32 %funcCall5)
  %int2float = sitofp i32  233 to float
  %int2float1 = sitofp i32  4095 to float
  %funcCall6 = call i32 @float_eq(float %int2float, float %int2float1)
  call void @assert_not(i32 %funcCall6)
  %ifcond = fcmp une float 1.500000, 0.000000
  br i1 %ifcond, label %then, label %ifnext

then:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %entry, %then, 
  %ifcond1 = fcmp une float 1.000000, 0.000000
  br i1 %ifcond1, label %then1, label %ifnext1

then1:				; preds = %ifnext, 
  call void @ok()
  br label %ifnext1

ifnext1:				; preds = %ifnext, %then1, 
  %cond = icmp ne i32 0, 0
  br i1 %cond, label %then2, label %ifnext2

then2:				; preds = %ifnext1, 
  call void @error()
  br label %ifnext2

ifnext2:				; preds = %ifnext1, %then2, 
  %cond1 = icmp ne i32 0, 0
  br i1 %cond1, label %then3, label %ifnext3

then3:				; preds = %ifnext2, 
  call void @ok()
  br label %ifnext3

ifnext3:				; preds = %ifnext2, %then3, 
  store i32 1, i32* %i, align 4
  store i32 0, i32* %p, align 4
  %arrayptr = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 0
  %funcCall7 = call i32 @getfarray(float* %arrayptr)
  store i32 %funcCall7, i32* %len, align 4
  br label %condBlock

condBlock:				; preds = %ifnext3, %whileloop, 
  %i.cpy = load i32, i32* %i
  %int2float2 = sitofp i32  %i.cpy to float
  %lestmp = fcmp ult float %int2float2, 1000000000.000000
  %cond2 = icmp ne i1 %lestmp, 0
  br i1 %cond2, label %whileloop, label %afterloop

whileloop:				; preds = %condBlock, 
  %funcCall8 = call float @getfloat()
  store float %funcCall8, float* %input, align 4
  %input.cpy = load float, float* %input
  %multmp = fmul float 3.141593, %input.cpy
  %input.cpy1 = load float, float* %input
  %multmp1 = fmul float %multmp, %input.cpy1
  store float %multmp1, float* %area, align 4
  %input.cpy2 = load float, float* %input
  %float2int2 = fptosi float  %input.cpy2 to i32
  %funcCall9 = call float @circle_area(i32 %float2int2)
  store float %funcCall9, float* %area_trunc, align 4
  %p.cpy = load i32, i32* %p
  %p.cpy1 = load i32, i32* %p
  %arrayitem = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 %p.cpy1
  %item = load float, float* %arrayitem
  %input.cpy3 = load float, float* %input
  %addtmp = fadd float %item, %input.cpy3
  %p.cpy2 = load i32, i32* %p
  %p.cpy3 = load i32, i32* %p
  %arrayitem1 = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 %p.cpy3
  store float %addtmp, float* %arrayitem1, align 4
  %area.cpy = load float, float* %area
  call void @putfloat(float %area.cpy)
  call void @putch(i32 32)
  %area_trunc.cpy = load float, float* %area_trunc
  %float2int3 = fptosi float  %area_trunc.cpy to i32
  call void @putint(i32 %float2int3)
  call void @putch(i32 10)
  %i.cpy1 = load i32, i32* %i
  %int2float3 = sitofp i32  %i.cpy1 to float
  %multmp2 = fmul float %int2float3, 10.000000
  %float2int4 = fptosi float  %multmp2 to i32
  store i32 %float2int4, i32* %i, align 4
  %p.cpy4 = load i32, i32* %p
  %addtmp1 = add i32 %p.cpy4, 1
  store i32 %addtmp1, i32* %p, align 4
  br label %condBlock

afterloop:				; preds = %condBlock, 
  %len.cpy = load i32, i32* %len
  %int2float4 = sitofp i32  %len.cpy to float
  %arrayptr1 = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 0
  call void @putfarray(float %int2float4, float* %arrayptr1)
  ret i32 0

}


[32m	load%i.cpy = load i32, i32* %i [0m
[32m	load%p.cpy = load i32, i32* %p [0m
[32m	load%p.cpy1 = load i32, i32* %p [0m
[32m	load%p.cpy2 = load i32, i32* %p [0m
[32m	load%p.cpy3 = load i32, i32* %p [0m
[32m	load%p.cpy4 = load i32, i32* %p [0m
[32m	load%i.cpy1 = load i32, i32* %i [0m
[32mAfter Mem2Reg[0m
@RADIUS = global float 5.500000

@PI = global float 3.141593

@EPS = global float 0.000001

@PI_HEX = global float 0.000000

@HEX2 = global float 0.000000

@FACT = global float -33000.000000

@EVAL1 = global float 95.033188

@EVAL2 = global float 0.000000

@EVAL3 = global float 34.557522

@CONV1 = global float 233.000000

@CONV2 = global float 4095.000000

@MAX = global i32 1000000000

@TWO = global i32 2

@THREE = global i32 3

@FIVE = global i32 5

@main.ifnext.arr = global [10 x float] [float 1.000000, float 2.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000]

declare i32 @getint()

declare i32 @getch()

declare float @getfloat()

declare i32 @getarray(i32*)

declare i32 @getfarray(float*)

declare void @putint(i32)

declare void @putch(i32)

declare void @putfloat(float)

declare void @putarray(i32, i32*)

declare void @putfarray(float, float*)

declare void @putf(i8*)

declare void @starttime()

declare void @stoptime()

declare void @llvm.memcpy.inline.p0.p0.i32(i8*, i8*, i32, i1)

define float @float_abs(float %x) {
entry:				
  %int2float = sitofp i32  0 to float
  %lestmp = fcmp ult float %x, %int2float
  %cond = icmp ne i1 %lestmp, 0
  br i1 %cond, label %then, label %ifnext

then:				; preds = %entry, 
  %negtmp = fsub float 0.000000, %x
  ret float %negtmp

ifnext:				; preds = %entry, 
  ret float %x

}

define float @circle_area(i32 %radius) {
entry:				
  %int2float = sitofp i32  %radius to float
  %multmp = fmul float 3.141593, %int2float
  %int2float1 = sitofp i32  %radius to float
  %multmp1 = fmul float %multmp, %int2float1
  %multmp2 = mul i32 %radius, %radius
  %int2float2 = sitofp i32  %multmp2 to float
  %multmp3 = fmul float %int2float2, 3.141593
  %addtmp = fadd float %multmp1, %multmp3
  %int2float3 = sitofp i32  2 to float
  %divtmp = fdiv float %addtmp, %int2float3
  ret float %divtmp

}

define i32 @float_eq(float %a, float %b) {
entry:				
  %subtmp = fsub float %a, %b
  %funcCall = call float @float_abs(float %subtmp)
  %lestmp = fcmp ult float %funcCall, 0.000001
  %cond = icmp ne i1 %lestmp, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  %float2int = fptosi float  1.000000 to i32
  ret i32 %float2int

else:				; preds = %entry, 
  ret i32 0

ifnext:				
  retvoid

}

define void @error() {
entry:				
  call void @putch(i32 101)
  call void @putch(i32 114)
  call void @putch(i32 114)
  call void @putch(i32 111)
  call void @putch(i32 114)
  call void @putch(i32 10)
  retvoid

}

define void @ok() {
entry:				
  call void @putch(i32 111)
  call void @putch(i32 107)
  call void @putch(i32 10)
  retvoid

}

define void @assert(i32 %cond) {
entry:				
  %nottmp = icmp eq i32 %cond, 0
  %cond = icmp ne i1 %nottmp, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  call void @error()
  br label %ifnext

else:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %then, %else, 
  retvoid

}

define void @assert_not(i32 %cond) {
entry:				
  %cond = icmp ne i32 %cond, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  call void @error()
  br label %ifnext

else:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %then, %else, 
  retvoid

}

define i32 @main() {
entry:				
  %funcCall = call i32 @float_eq(float 0.000000, float -33000.000000)
  call void @assert_not(i32 %funcCall)
  %funcCall1 = call i32 @float_eq(float 95.033188, float 0.000000)
  call void @assert_not(i32 %funcCall1)
  %funcCall2 = call i32 @float_eq(float 0.000000, float 34.557522)
  call void @assert(i32 %funcCall2)
  %float2int = fptosi float  5.500000 to i32
  %funcCall3 = call float @circle_area(i32 %float2int)
  %float2int1 = fptosi float  5.000000 to i32
  %funcCall4 = call float @circle_area(i32 %float2int1)
  %funcCall5 = call i32 @float_eq(float %funcCall3, float %funcCall4)
  call void @assert(i32 %funcCall5)
  %int2float = sitofp i32  233 to float
  %int2float1 = sitofp i32  4095 to float
  %funcCall6 = call i32 @float_eq(float %int2float, float %int2float1)
  call void @assert_not(i32 %funcCall6)
  %ifcond = fcmp une float 1.500000, 0.000000
  br i1 %ifcond, label %then, label %ifnext

then:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %entry, %then, 
  %ifcond1 = fcmp une float 1.000000, 0.000000
  br i1 %ifcond1, label %then1, label %ifnext1

then1:				; preds = %ifnext, 
  call void @ok()
  br label %ifnext1

ifnext1:				; preds = %ifnext, %then1, 
  %cond = icmp ne i32 0, 0
  br i1 %cond, label %then2, label %ifnext2

then2:				; preds = %ifnext1, 
  call void @error()
  br label %ifnext2

ifnext2:				; preds = %ifnext1, %then2, 
  %cond1 = icmp ne i32 0, 0
  br i1 %cond1, label %then3, label %ifnext3

then3:				; preds = %ifnext2, 
  call void @ok()
  br label %ifnext3

ifnext3:				; preds = %ifnext2, %then3, 
  %arrayptr = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 0
  %funcCall7 = call i32 @getfarray(float* %arrayptr)
  br label %condBlock

condBlock:				; preds = %ifnext3, %whileloop, 
  %i.phi = phi i32 [1, %ifnext3], [%float2int4, %whileloop]
  %p.phi = phi i32 [0, %ifnext3], [%addtmp1, %whileloop]
  %int2float2 = sitofp i32  %i.phi to float
  %lestmp = fcmp ult float %int2float2, 1000000000.000000
  %cond2 = icmp ne i1 %lestmp, 0
  br i1 %cond2, label %whileloop, label %afterloop

whileloop:				; preds = %condBlock, 
  %funcCall8 = call float @getfloat()
  %multmp = fmul float 3.141593, %funcCall8
  %multmp1 = fmul float %multmp, %funcCall8
  %float2int2 = fptosi float  %funcCall8 to i32
  %funcCall9 = call float @circle_area(i32 %float2int2)
  %arrayitem = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 %p.phi
  %item = load float, float* %arrayitem
  %addtmp = fadd float %item, %funcCall8
  %arrayitem1 = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 %p.phi
  store float %addtmp, float* %arrayitem1, align 4
  call void @putfloat(float %multmp1)
  call void @putch(i32 32)
  %float2int3 = fptosi float  %funcCall9 to i32
  call void @putint(i32 %float2int3)
  call void @putch(i32 10)
  %int2float3 = sitofp i32  %i.phi to float
  %multmp2 = fmul float %int2float3, 10.000000
  %float2int4 = fptosi float  %multmp2 to i32
  %addtmp1 = add i32 %p.phi, 1
  br label %condBlock

afterloop:				; preds = %condBlock, 
  %int2float4 = sitofp i32  %funcCall7 to float
  %arrayptr1 = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 0
  call void @putfarray(float %int2float4, float* %arrayptr1)
  ret i32 0

}


[32mIPA[0m
[32mRuning GVN[0m
[32mMemory SSA at float_abs[0m
[32mInsert the phi instructions(MemSSA-def-use)[0m
[32mRename the variable(MemSSA-def-use)[0m
[32mInsert the phi instructions(MemSSA-def-use)GV[0m
[32mRename the variable(MemSSA-def-use)GV[0m
[32mInsert the phi instructions(MemSSA-use-def)[0m
[32mRename the variable(MemSSA-use-def)[0m
[32mInsert the phi instructions(MemSSA-use-def)GV[0m
[32mRename the variable(MemSSA-use-def)GV[0m
define float @float_abs(float %x) {
entry:				
  %int2float = sitofp i32  0 to float
  %lestmp = fcmp ult float %x, %int2float
  %cond = icmp ne i1 %lestmp, 0
  br i1 %cond, label %then, label %ifnext

then:				; preds = %entry, 
  %negtmp = fsub float 0.000000, %x
  ret float %negtmp

ifnext:				; preds = %entry, 
  ret float %x

}
[32mGVN in float_abs[0m
define float @float_abs(float %x) {
entry:				
  %lestmp = fcmp ult float %x, 0.000000
  %cond = icmp ne i1 %lestmp, 0
  br i1 %cond, label %then, label %ifnext

then:				; preds = %entry, 
  %negtmp = fsub float 0.000000, %x
  ret float %negtmp

ifnext:				; preds = %entry, 
  ret float %x

}
[32mclear Memory SSA at float_abs[0m
[32mMemory SSA at circle_area[0m
[32mInsert the phi instructions(MemSSA-def-use)[0m
[32mRename the variable(MemSSA-def-use)[0m
[32mInsert the phi instructions(MemSSA-def-use)GV[0m
[32mRename the variable(MemSSA-def-use)GV[0m
[32mInsert the phi instructions(MemSSA-use-def)[0m
[32mRename the variable(MemSSA-use-def)[0m
[32mInsert the phi instructions(MemSSA-use-def)GV[0m
[32mRename the variable(MemSSA-use-def)GV[0m
define float @circle_area(i32 %radius) {
entry:				
  %int2float = sitofp i32  %radius to float
  %multmp = fmul float 3.141593, %int2float
  %int2float1 = sitofp i32  %radius to float
  %multmp1 = fmul float %multmp, %int2float1
  %multmp2 = mul i32 %radius, %radius
  %int2float2 = sitofp i32  %multmp2 to float
  %multmp3 = fmul float %int2float2, 3.141593
  %addtmp = fadd float %multmp1, %multmp3
  %int2float3 = sitofp i32  2 to float
  %divtmp = fdiv float %addtmp, %int2float3
  ret float %divtmp

}
[32mGVN in circle_area[0m
define float @circle_area(i32 %radius) {
entry:				
  %int2float = sitofp i32  %radius to float
  %multmp = fmul float 3.141593, %int2float
  %multmp1 = fmul float %multmp, %int2float
  %multmp2 = mul i32 %radius, %radius
  %int2float2 = sitofp i32  %multmp2 to float
  %multmp3 = fmul float %int2float2, 3.141593
  %addtmp = fadd float %multmp1, %multmp3
  %divtmp = fdiv float %addtmp, 2.000000
  ret float %divtmp

}
[32mclear Memory SSA at circle_area[0m
[32mMemory SSA at float_eq[0m
[32mInsert the phi instructions(MemSSA-def-use)[0m
[32mRename the variable(MemSSA-def-use)[0m
[32mInsert the phi instructions(MemSSA-def-use)GV[0m
[32mRename the variable(MemSSA-def-use)GV[0m
[32mInsert the phi instructions(MemSSA-use-def)[0m
[32mRename the variable(MemSSA-use-def)[0m
[32mInsert the phi instructions(MemSSA-use-def)GV[0m
[32mRename the variable(MemSSA-use-def)GV[0m
define i32 @float_eq(float %a, float %b) {
entry:				
  %subtmp = fsub float %a, %b
  %funcCall = call float @float_abs(float %subtmp)
  %lestmp = fcmp ult float %funcCall, 0.000001
  %cond = icmp ne i1 %lestmp, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  %float2int = fptosi float  1.000000 to i32
  ret i32 %float2int

else:				; preds = %entry, 
  ret i32 0

ifnext:				
  retvoid

}
[32mGVN in float_eq[0m
define i32 @float_eq(float %a, float %b) {
entry:				
  %subtmp = fsub float %a, %b
  %funcCall = call float @float_abs(float %subtmp)
  %lestmp = fcmp ult float %funcCall, 0.000001
  %cond = icmp ne i1 %lestmp, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  ret i32 1

else:				; preds = %entry, 
  ret i32 0

ifnext:				
  retvoid

}
[32mclear Memory SSA at float_eq[0m
[32mMemory SSA at error[0m
[32mInsert the phi instructions(MemSSA-def-use)[0m
[32mRename the variable(MemSSA-def-use)[0m
[32mInsert the phi instructions(MemSSA-def-use)GV[0m
[32mRename the variable(MemSSA-def-use)GV[0m
[32mInsert the phi instructions(MemSSA-use-def)[0m
[32mRename the variable(MemSSA-use-def)[0m
[32mInsert the phi instructions(MemSSA-use-def)GV[0m
[32mRename the variable(MemSSA-use-def)GV[0m
define void @error() {
entry:				
  call void @putch(i32 101)
  call void @putch(i32 114)
  call void @putch(i32 114)
  call void @putch(i32 111)
  call void @putch(i32 114)
  call void @putch(i32 10)
  retvoid

}
[32mGVN in error[0m
define void @error() {
entry:				
  call void @putch(i32 101)
  call void @putch(i32 114)
  call void @putch(i32 114)
  call void @putch(i32 111)
  call void @putch(i32 114)
  call void @putch(i32 10)
  retvoid

}
[32mclear Memory SSA at error[0m
[32mMemory SSA at ok[0m
[32mInsert the phi instructions(MemSSA-def-use)[0m
[32mRename the variable(MemSSA-def-use)[0m
[32mInsert the phi instructions(MemSSA-def-use)GV[0m
[32mRename the variable(MemSSA-def-use)GV[0m
[32mInsert the phi instructions(MemSSA-use-def)[0m
[32mRename the variable(MemSSA-use-def)[0m
[32mInsert the phi instructions(MemSSA-use-def)GV[0m
[32mRename the variable(MemSSA-use-def)GV[0m
define void @ok() {
entry:				
  call void @putch(i32 111)
  call void @putch(i32 107)
  call void @putch(i32 10)
  retvoid

}
[32mGVN in ok[0m
define void @ok() {
entry:				
  call void @putch(i32 111)
  call void @putch(i32 107)
  call void @putch(i32 10)
  retvoid

}
[32mclear Memory SSA at ok[0m
[32mMemory SSA at assert[0m
[32mInsert the phi instructions(MemSSA-def-use)[0m
[32mRename the variable(MemSSA-def-use)[0m
[32mInsert the phi instructions(MemSSA-def-use)GV[0m
[32mRename the variable(MemSSA-def-use)GV[0m
[32mInsert the phi instructions(MemSSA-use-def)[0m
[32mRename the variable(MemSSA-use-def)[0m
[32mInsert the phi instructions(MemSSA-use-def)GV[0m
[32mRename the variable(MemSSA-use-def)GV[0m
define void @assert(i32 %cond) {
entry:				
  %nottmp = icmp eq i32 %cond, 0
  %cond = icmp ne i1 %nottmp, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  call void @error()
  br label %ifnext

else:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %then, %else, 
  retvoid

}
[32mGVN in assert[0m
define void @assert(i32 %cond) {
entry:				
  %nottmp = icmp eq i32 %cond, 0
  %cond = icmp ne i1 %nottmp, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  call void @error()
  br label %ifnext

else:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %then, %else, 
  retvoid

}
[32mclear Memory SSA at assert[0m
[32mMemory SSA at assert_not[0m
[32mInsert the phi instructions(MemSSA-def-use)[0m
[32mRename the variable(MemSSA-def-use)[0m
[32mInsert the phi instructions(MemSSA-def-use)GV[0m
[32mRename the variable(MemSSA-def-use)GV[0m
[32mInsert the phi instructions(MemSSA-use-def)[0m
[32mRename the variable(MemSSA-use-def)[0m
[32mInsert the phi instructions(MemSSA-use-def)GV[0m
[32mRename the variable(MemSSA-use-def)GV[0m
define void @assert_not(i32 %cond) {
entry:				
  %cond = icmp ne i32 %cond, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  call void @error()
  br label %ifnext

else:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %then, %else, 
  retvoid

}
[32mGVN in assert_not[0m
define void @assert_not(i32 %cond) {
entry:				
  %cond = icmp ne i32 %cond, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  call void @error()
  br label %ifnext

else:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %then, %else, 
  retvoid

}
[32mclear Memory SSA at assert_not[0m
[32mMemory SSA at main[0m
[32mInsert the phi instructions(MemSSA-def-use)[0m
[32mRename the variable(MemSSA-def-use)[0m
[32mInsert the phi instructions(MemSSA-def-use)GV[0m
[32mRename the variable(MemSSA-def-use)GV[0m
[32mInsert the phi instructions(MemSSA-use-def)[0m
[32mRename the variable(MemSSA-use-def)[0m
[32mInsert the phi instructions(MemSSA-use-def)GV[0m
[32mRename the variable(MemSSA-use-def)GV[0m
define i32 @main() {
entry:				
  %funcCall = call i32 @float_eq(float 0.000000, float -33000.000000)
  call void @assert_not(i32 %funcCall)
  %funcCall1 = call i32 @float_eq(float 95.033188, float 0.000000)
  call void @assert_not(i32 %funcCall1)
  %funcCall2 = call i32 @float_eq(float 0.000000, float 34.557522)
  call void @assert(i32 %funcCall2)
  %float2int = fptosi float  5.500000 to i32
  %funcCall3 = call float @circle_area(i32 %float2int)
  %float2int1 = fptosi float  5.000000 to i32
  %funcCall4 = call float @circle_area(i32 %float2int1)
  %funcCall5 = call i32 @float_eq(float %funcCall3, float %funcCall4)
  call void @assert(i32 %funcCall5)
  %int2float = sitofp i32  233 to float
  %int2float1 = sitofp i32  4095 to float
  %funcCall6 = call i32 @float_eq(float %int2float, float %int2float1)
  call void @assert_not(i32 %funcCall6)
  %ifcond = fcmp une float 1.500000, 0.000000
  br i1 %ifcond, label %then, label %ifnext

then:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %entry, %then, 
  %ifcond1 = fcmp une float 1.000000, 0.000000
  br i1 %ifcond1, label %then1, label %ifnext1

then1:				; preds = %ifnext, 
  call void @ok()
  br label %ifnext1

ifnext1:				; preds = %ifnext, %then1, 
  %cond = icmp ne i32 0, 0
  br i1 %cond, label %then2, label %ifnext2

then2:				; preds = %ifnext1, 
  call void @error()
  br label %ifnext2

ifnext2:				; preds = %ifnext1, %then2, 
  %cond1 = icmp ne i32 0, 0
  br i1 %cond1, label %then3, label %ifnext3

then3:				; preds = %ifnext2, 
  call void @ok()
  br label %ifnext3

ifnext3:				; preds = %ifnext2, %then3, 
  %arrayptr = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 0
  %funcCall7 = call i32 @getfarray(float* %arrayptr)
  br label %condBlock

condBlock:				; preds = %ifnext3, %whileloop, 
  %main.ifnext.arr.memphi = memphi void [undef, %ifnext3], [%item, %whileloop]
  %main.ifnext.arr.memphi1 = memphi void [%funcCall7, %ifnext3], [%, %whileloop]
  %i.phi = phi i32 [1, %ifnext3], [%float2int4, %whileloop]
  %p.phi = phi i32 [0, %ifnext3], [%addtmp1, %whileloop]
  %int2float2 = sitofp i32  %i.phi to float
  %lestmp = fcmp ult float %int2float2, 1000000000.000000
  %cond2 = icmp ne i1 %lestmp, 0
  br i1 %cond2, label %whileloop, label %afterloop

whileloop:				; preds = %condBlock, 
  %funcCall8 = call float @getfloat()
  %multmp = fmul float 3.141593, %funcCall8
  %multmp1 = fmul float %multmp, %funcCall8
  %float2int2 = fptosi float  %funcCall8 to i32
  %funcCall9 = call float @circle_area(i32 %float2int2)
  %arrayitem = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 %p.phi
  %item = load float, float* %arrayitem, storeis %main.ifnext.arr.memphi1 = memphi void [%funcCall7, %ifnext3], [%, %whileloop]
  %addtmp = fadd float %item, %funcCall8
  %arrayitem1 = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 %p.phi
  dead instr
  store float %addtmp, float* %arrayitem1, align 4
  call void @putfloat(float %multmp1)
  call void @putch(i32 32)
  %float2int3 = fptosi float  %funcCall9 to i32
  call void @putint(i32 %float2int3)
  call void @putch(i32 10)
  %int2float3 = sitofp i32  %i.phi to float
  %multmp2 = fmul float %int2float3, 10.000000
  %float2int4 = fptosi float  %multmp2 to i32
  %addtmp1 = add i32 %p.phi, 1
  br label %condBlock

afterloop:				; preds = %condBlock, 
  %int2float4 = sitofp i32  %funcCall7 to float
  %arrayptr1 = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 0
  call void @putfarray(float %int2float4, float* %arrayptr1)
  ret i32 0

}
[32mGVN in main[0m
define i32 @main() {
entry:				
  %funcCall = call i32 @float_eq(float 0.000000, float -33000.000000)
  call void @assert_not(i32 %funcCall)
  %funcCall1 = call i32 @float_eq(float 95.033188, float 0.000000)
  call void @assert_not(i32 %funcCall1)
  %funcCall2 = call i32 @float_eq(float 0.000000, float 34.557522)
  call void @assert(i32 %funcCall2)
  %funcCall3 = call float @circle_area(i32 5)
  %funcCall4 = call float @circle_area(i32 5)
  %funcCall5 = call i32 @float_eq(float %funcCall3, float %funcCall4)
  call void @assert(i32 %funcCall5)
  %funcCall6 = call i32 @float_eq(float 233.000000, float 4095.000000)
  call void @assert_not(i32 %funcCall6)
  br i1 1, label %then, label %ifnext

then:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %entry, %then, 
  br i1 1, label %then1, label %ifnext1

then1:				; preds = %ifnext, 
  call void @ok()
  br label %ifnext1

ifnext1:				; preds = %ifnext, %then1, 
  br i1 0, label %then2, label %ifnext2

then2:				; preds = %ifnext1, 
  call void @error()
  br label %ifnext2

ifnext2:				; preds = %ifnext1, %then2, 
  br i1 0, label %then3, label %ifnext3

then3:				; preds = %ifnext2, 
  call void @ok()
  br label %ifnext3

ifnext3:				; preds = %ifnext2, %then3, 
  %arrayptr = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 0
  %funcCall7 = call i32 @getfarray(float* %arrayptr)
  br label %condBlock

condBlock:				; preds = %ifnext3, %whileloop, 
  %main.ifnext.arr.memphi = memphi void [undef, %ifnext3], [%item, %whileloop]
  %main.ifnext.arr.memphi1 = memphi void [%funcCall7, %ifnext3], [%, %whileloop]
  %i.phi = phi i32 [1, %ifnext3], [%float2int4, %whileloop]
  %p.phi = phi i32 [0, %ifnext3], [%addtmp1, %whileloop]
  %int2float2 = sitofp i32  %i.phi to float
  %lestmp = fcmp ult float %int2float2, 1000000000.000000
  %cond2 = icmp ne i1 %lestmp, 0
  br i1 %cond2, label %whileloop, label %afterloop

whileloop:				; preds = %condBlock, 
  %funcCall8 = call float @getfloat()
  %multmp = fmul float 3.141593, %funcCall8
  %multmp1 = fmul float %multmp, %funcCall8
  %float2int2 = fptosi float  %funcCall8 to i32
  %funcCall9 = call float @circle_area(i32 %float2int2)
  %arrayitem = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 %p.phi
  %item = load float, float* %arrayitem, storeis %main.ifnext.arr.memphi1 = memphi void [%funcCall7, %ifnext3], [%, %whileloop]
  %addtmp = fadd float %item, %funcCall8
  dead instr
  store float %addtmp, float* %arrayitem, align 4
  call void @putfloat(float %multmp1)
  call void @putch(i32 32)
  %float2int3 = fptosi float  %funcCall9 to i32
  call void @putint(i32 %float2int3)
  call void @putch(i32 10)
  %multmp2 = fmul float %int2float2, 10.000000
  %float2int4 = fptosi float  %multmp2 to i32
  %addtmp1 = add i32 %p.phi, 1
  br label %condBlock

afterloop:				; preds = %condBlock, 
  %int2float4 = sitofp i32  %funcCall7 to float
  call void @putfarray(float %int2float4, float* %arrayptr)
  ret i32 0

}
[32mclear Memory SSA at main[0m
[32mAfter GVN[0m
@RADIUS = global float 5.500000

@PI = global float 3.141593

@EPS = global float 0.000001

@PI_HEX = global float 0.000000

@HEX2 = global float 0.000000

@FACT = global float -33000.000000

@EVAL1 = global float 95.033188

@EVAL2 = global float 0.000000

@EVAL3 = global float 34.557522

@CONV1 = global float 233.000000

@CONV2 = global float 4095.000000

@MAX = global i32 1000000000

@TWO = global i32 2

@THREE = global i32 3

@FIVE = global i32 5

@main.ifnext.arr = global [10 x float] [float 1.000000, float 2.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000]

declare i32 @getint()

declare i32 @getch()

declare float @getfloat()

declare i32 @getarray(i32*)

declare i32 @getfarray(float*)

declare void @putint(i32)

declare void @putch(i32)

declare void @putfloat(float)

declare void @putarray(i32, i32*)

declare void @putfarray(float, float*)

declare void @putf(i8*)

declare void @starttime()

declare void @stoptime()

declare void @llvm.memcpy.inline.p0.p0.i32(i8*, i8*, i32, i1)

define float @float_abs(float %x) {
entry:				
  %lestmp = fcmp ult float %x, 0.000000
  %cond = icmp ne i1 %lestmp, 0
  br i1 %cond, label %then, label %ifnext

then:				; preds = %entry, 
  %negtmp = fsub float 0.000000, %x
  ret float %negtmp

ifnext:				; preds = %entry, 
  ret float %x

}

define float @circle_area(i32 %radius) {
entry:				
  %int2float = sitofp i32  %radius to float
  %multmp = fmul float 3.141593, %int2float
  %multmp1 = fmul float %multmp, %int2float
  %multmp2 = mul i32 %radius, %radius
  %int2float2 = sitofp i32  %multmp2 to float
  %multmp3 = fmul float %int2float2, 3.141593
  %addtmp = fadd float %multmp1, %multmp3
  %divtmp = fdiv float %addtmp, 2.000000
  ret float %divtmp

}

define i32 @float_eq(float %a, float %b) {
entry:				
  %subtmp = fsub float %a, %b
  %funcCall = call float @float_abs(float %subtmp)
  %lestmp = fcmp ult float %funcCall, 0.000001
  %cond = icmp ne i1 %lestmp, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  ret i32 1

else:				; preds = %entry, 
  ret i32 0

ifnext:				
  retvoid

}

define void @error() {
entry:				
  call void @putch(i32 101)
  call void @putch(i32 114)
  call void @putch(i32 114)
  call void @putch(i32 111)
  call void @putch(i32 114)
  call void @putch(i32 10)
  retvoid

}

define void @ok() {
entry:				
  call void @putch(i32 111)
  call void @putch(i32 107)
  call void @putch(i32 10)
  retvoid

}

define void @assert(i32 %cond) {
entry:				
  %nottmp = icmp eq i32 %cond, 0
  %cond = icmp ne i1 %nottmp, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  call void @error()
  br label %ifnext

else:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %then, %else, 
  retvoid

}

define void @assert_not(i32 %cond) {
entry:				
  %cond = icmp ne i32 %cond, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  call void @error()
  br label %ifnext

else:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %then, %else, 
  retvoid

}

define i32 @main() {
entry:				
  %funcCall = call i32 @float_eq(float 0.000000, float -33000.000000)
  call void @assert_not(i32 %funcCall)
  %funcCall1 = call i32 @float_eq(float 95.033188, float 0.000000)
  call void @assert_not(i32 %funcCall1)
  %funcCall2 = call i32 @float_eq(float 0.000000, float 34.557522)
  call void @assert(i32 %funcCall2)
  %funcCall3 = call float @circle_area(i32 5)
  %funcCall4 = call float @circle_area(i32 5)
  %funcCall5 = call i32 @float_eq(float %funcCall3, float %funcCall4)
  call void @assert(i32 %funcCall5)
  %funcCall6 = call i32 @float_eq(float 233.000000, float 4095.000000)
  call void @assert_not(i32 %funcCall6)
  br i1 1, label %then, label %ifnext

then:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %entry, %then, 
  br i1 1, label %then1, label %ifnext1

then1:				; preds = %ifnext, 
  call void @ok()
  br label %ifnext1

ifnext1:				; preds = %ifnext, %then1, 
  br i1 0, label %then2, label %ifnext2

then2:				; preds = %ifnext1, 
  call void @error()
  br label %ifnext2

ifnext2:				; preds = %ifnext1, %then2, 
  br i1 0, label %then3, label %ifnext3

then3:				; preds = %ifnext2, 
  call void @ok()
  br label %ifnext3

ifnext3:				; preds = %ifnext2, %then3, 
  %arrayptr = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 0
  %funcCall7 = call i32 @getfarray(float* %arrayptr)
  br label %condBlock

condBlock:				; preds = %ifnext3, %whileloop, 
  %i.phi = phi i32 [1, %ifnext3], [%float2int4, %whileloop]
  %p.phi = phi i32 [0, %ifnext3], [%addtmp1, %whileloop]
  %int2float2 = sitofp i32  %i.phi to float
  %lestmp = fcmp ult float %int2float2, 1000000000.000000
  %cond2 = icmp ne i1 %lestmp, 0
  br i1 %cond2, label %whileloop, label %afterloop

whileloop:				; preds = %condBlock, 
  %funcCall8 = call float @getfloat()
  %multmp = fmul float 3.141593, %funcCall8
  %multmp1 = fmul float %multmp, %funcCall8
  %float2int2 = fptosi float  %funcCall8 to i32
  %funcCall9 = call float @circle_area(i32 %float2int2)
  %arrayitem = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 %p.phi
  %item = load float, float* %arrayitem
  %addtmp = fadd float %item, %funcCall8
  store float %addtmp, float* %arrayitem, align 4
  call void @putfloat(float %multmp1)
  call void @putch(i32 32)
  %float2int3 = fptosi float  %funcCall9 to i32
  call void @putint(i32 %float2int3)
  call void @putch(i32 10)
  %multmp2 = fmul float %int2float2, 10.000000
  %float2int4 = fptosi float  %multmp2 to i32
  %addtmp1 = add i32 %p.phi, 1
  br label %condBlock

afterloop:				; preds = %condBlock, 
  %int2float4 = sitofp i32  %funcCall7 to float
  call void @putfarray(float %int2float4, float* %arrayptr)
  ret i32 0

}


[32mAfter DCE[0m
@RADIUS = global float 5.500000

@PI = global float 3.141593

@EPS = global float 0.000001

@PI_HEX = global float 0.000000

@HEX2 = global float 0.000000

@FACT = global float -33000.000000

@EVAL1 = global float 95.033188

@EVAL2 = global float 0.000000

@EVAL3 = global float 34.557522

@CONV1 = global float 233.000000

@CONV2 = global float 4095.000000

@MAX = global i32 1000000000

@TWO = global i32 2

@THREE = global i32 3

@FIVE = global i32 5

@main.ifnext.arr = global [10 x float] [float 1.000000, float 2.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000, float 0.000000]

declare i32 @getint()

declare i32 @getch()

declare float @getfloat()

declare i32 @getarray(i32*)

declare i32 @getfarray(float*)

declare void @putint(i32)

declare void @putch(i32)

declare void @putfloat(float)

declare void @putarray(i32, i32*)

declare void @putfarray(float, float*)

declare void @putf(i8*)

declare void @starttime()

declare void @stoptime()

declare void @llvm.memcpy.inline.p0.p0.i32(i8*, i8*, i32, i1)

define float @float_abs(float %x) {
entry:				
  %lestmp = fcmp ult float %x, 0.000000
  %cond = icmp ne i1 %lestmp, 0
  br i1 %cond, label %then, label %ifnext

then:				; preds = %entry, 
  %negtmp = fsub float 0.000000, %x
  ret float %negtmp

ifnext:				; preds = %entry, 
  ret float %x

}

define float @circle_area(i32 %radius) {
entry:				
  %int2float = sitofp i32  %radius to float
  %multmp = fmul float 3.141593, %int2float
  %multmp1 = fmul float %multmp, %int2float
  %multmp2 = mul i32 %radius, %radius
  %int2float2 = sitofp i32  %multmp2 to float
  %multmp3 = fmul float %int2float2, 3.141593
  %addtmp = fadd float %multmp1, %multmp3
  %divtmp = fdiv float %addtmp, 2.000000
  ret float %divtmp

}

define i32 @float_eq(float %a, float %b) {
entry:				
  %subtmp = fsub float %a, %b
  %funcCall = call float @float_abs(float %subtmp)
  %lestmp = fcmp ult float %funcCall, 0.000001
  %cond = icmp ne i1 %lestmp, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  ret i32 1

else:				; preds = %entry, 
  ret i32 0

ifnext:				

}

define void @error() {
entry:				
  call void @putch(i32 101)
  call void @putch(i32 114)
  call void @putch(i32 114)
  call void @putch(i32 111)
  call void @putch(i32 114)
  call void @putch(i32 10)
  retvoid

}

define void @ok() {
entry:				
  call void @putch(i32 111)
  call void @putch(i32 107)
  call void @putch(i32 10)
  retvoid

}

define void @assert(i32 %cond) {
entry:				
  %nottmp = icmp eq i32 %cond, 0
  %cond = icmp ne i1 %nottmp, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  call void @error()
  br label %ifnext

else:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %then, %else, 
  retvoid

}

define void @assert_not(i32 %cond) {
entry:				
  %cond = icmp ne i32 %cond, 0
  br i1 %cond, label %then, label %else

then:				; preds = %entry, 
  call void @error()
  br label %ifnext

else:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %then, %else, 
  retvoid

}

define i32 @main() {
entry:				
  %funcCall = call i32 @float_eq(float 0.000000, float -33000.000000)
  call void @assert_not(i32 %funcCall)
  %funcCall1 = call i32 @float_eq(float 95.033188, float 0.000000)
  call void @assert_not(i32 %funcCall1)
  %funcCall2 = call i32 @float_eq(float 0.000000, float 34.557522)
  call void @assert(i32 %funcCall2)
  %funcCall3 = call float @circle_area(i32 5)
  %funcCall4 = call float @circle_area(i32 5)
  %funcCall5 = call i32 @float_eq(float %funcCall3, float %funcCall4)
  call void @assert(i32 %funcCall5)
  %funcCall6 = call i32 @float_eq(float 233.000000, float 4095.000000)
  call void @assert_not(i32 %funcCall6)
  br i1 1, label %then, label %ifnext

then:				; preds = %entry, 
  call void @ok()
  br label %ifnext

ifnext:				; preds = %entry, %then, 
  br i1 1, label %then1, label %ifnext1

then1:				; preds = %ifnext, 
  call void @ok()
  br label %ifnext1

ifnext1:				; preds = %ifnext, %then1, 
  br i1 0, label %then2, label %ifnext2

then2:				; preds = %ifnext1, 
  call void @error()
  br label %ifnext2

ifnext2:				; preds = %ifnext1, %then2, 
  br i1 0, label %then3, label %ifnext3

then3:				; preds = %ifnext2, 
  call void @ok()
  br label %ifnext3

ifnext3:				; preds = %ifnext2, %then3, 
  %arrayptr = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 0
  %funcCall7 = call i32 @getfarray(float* %arrayptr)
  br label %condBlock

condBlock:				; preds = %ifnext3, %whileloop, 
  %i.phi = phi i32 [1, %ifnext3], [%float2int4, %whileloop]
  %p.phi = phi i32 [0, %ifnext3], [%addtmp1, %whileloop]
  %int2float2 = sitofp i32  %i.phi to float
  %lestmp = fcmp ult float %int2float2, 1000000000.000000
  %cond2 = icmp ne i1 %lestmp, 0
  br i1 %cond2, label %whileloop, label %afterloop

whileloop:				; preds = %condBlock, 
  %funcCall8 = call float @getfloat()
  %multmp = fmul float 3.141593, %funcCall8
  %multmp1 = fmul float %multmp, %funcCall8
  %float2int2 = fptosi float  %funcCall8 to i32
  %funcCall9 = call float @circle_area(i32 %float2int2)
  %arrayitem = getelementptr inbounds [10 x float], [10 x float]* @main.ifnext.arr, i32 0, i32 %p.phi
  %item = load float, float* %arrayitem
  %addtmp = fadd float %item, %funcCall8
  store float %addtmp, float* %arrayitem, align 4
  call void @putfloat(float %multmp1)
  call void @putch(i32 32)
  %float2int3 = fptosi float  %funcCall9 to i32
  call void @putint(i32 %float2int3)
  call void @putch(i32 10)
  %multmp2 = fmul float %int2float2, 10.000000
  %float2int4 = fptosi float  %multmp2 to i32
  %addtmp1 = add i32 %p.phi, 1
  br label %condBlock

afterloop:				; preds = %condBlock, 
  %int2float4 = sitofp i32  %funcCall7 to float
  call void @putfarray(float %int2float4, float* %arrayptr)
  ret i32 0

}


