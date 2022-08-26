declare i32 @getint()

declare i32 @getch()

declare float @getfloat()

declare i32 @getarray(i32*)

declare i32 @getfarray(float*)

declare void @putint(i32)

declare void @putch(i32)

declare void @putfloat(float)

declare void @putarray(i32, i32*)

declare void @putfarray(i32, float*)

declare void @putf(i8*)

declare void @starttime()

declare void @stoptime()

declare void @llvm.memcpy.inline.p0.p0.i32(i8*, i8*, i32, i1)

define i32 @main() {
entry:				
  move 0 to %sum.phi = phi i32 [%sum.phi1, %innerBB], [0, %entry]
  move 0 to %i.phi = phi i32 [%addtmp6, %innerBB], [0, %entry]
  br label %header

header:				; preds = %innerBB, %entry, 
  move %sum.phi = phi i32 [%sum.phi1, %innerBB], [0, %entry] to %sum.phi1 = phi i32 [%sum.phi, %header], [%sum.phi4, %exit1]
  move 0 to %j.phi = phi i32 [0, %header], [%addtmp5, %exit1]
  br label %guard1

guard1:				; preds = %header, %exit1, 
  %lestmp1 = icmp slt i32 %j.phi, 10
  br i1 %lestmp1, label %innerBB1, label %latch

header2:				; preds = %innerBB2, %innerBB1, 
  move %sum.phi3 = phi i32 [%sum.phi4, %innerBB2], [%sum.phi1, %innerBB1] to %sum.phi5 = phi i32 [%addtmp3, %innerBB3], [%sum.phi3, %header2]
  move 0 to %m.phi = phi i32 [%addtmp, %innerBB3], [0, %header2]
  br label %header3

header3:				; preds = %innerBB3, %header2, 
  %addtmp = add i32 %m.phi, 1
  %geqtmp = icmp sge i32 %addtmp, 3
  br i1 %geqtmp, label %then, label %innerBB4

then:				; preds = %header3, 
  %cond5 = icmp ne i32 %m.phi, 0
  br i1 %cond5, label %then1, label %innerBB5

then1:				; preds = %then, 
  br i1 %cond5, label %then2, label %still

then2:				; preds = %then1, %still, 
  %subtmp = sub i32 %m.phi, -1
  %geqtmp1 = icmp sge i32 %subtmp, 3
  br i1 %geqtmp1, label %innerBB6, label %innerBB7

still:				; preds = %then1, 
  %nottmp = icmp eq i32 %m.phi, 0
  br i1 %nottmp, label %then2, label %innerBB8

guard4:				; preds = %header4, %innerBB4, %innerBB5, %innerBB8, %innerBB7, 
  %lestmp4 = icmp slt i32 %n.phi, 2
  br i1 %lestmp4, label %header4, label %latch2

header4:				; preds = %guard4, 
  %addtmp1 = add i32 %n.phi, 1
  move %addtmp1 = add i32 %n.phi, 1 to %n.phi = phi i32 [%addtmp1, %header4], [0, %innerBB4], [0, %innerBB5], [0, %innerBB8], [0, %innerBB7]
  br label %guard4

latch2:				; preds = %guard4, 
  %lestmp5 = icmp slt i32 %addtmp, 3
  %addtmp3 = add i32 %sum.phi5, 1
  br i1 %lestmp5, label %innerBB3, label %innerBB9

exit3:				; preds = %innerBB9, %innerBB6, 
  %addtmp4 = add i32 %k.phi, 1
  %lestmp6 = icmp slt i32 %addtmp4, 5
  br i1 %lestmp6, label %innerBB2, label %exit1

exit1:				; preds = %exit3, 
  %addtmp5 = add i32 %j.phi, 1
  move %sum.phi4 = phi i32 [%addtmp3, %innerBB9], [%sum.phi5, %innerBB6] to %sum.phi1 = phi i32 [%sum.phi, %header], [%sum.phi4, %exit1]
  move %addtmp5 = add i32 %j.phi, 1 to %j.phi = phi i32 [0, %header], [%addtmp5, %exit1]
  br label %guard1

latch:				; preds = %guard1, 
  %addtmp6 = add i32 %i.phi, 1
  %lestmp7 = icmp slt i32 %addtmp6, 20
  br i1 %lestmp7, label %innerBB, label %exit

exit:				; preds = %latch, 
  ret i32 %sum.phi1

innerBB1:				; preds = %guard1, 
  move %sum.phi1 = phi i32 [%sum.phi, %header], [%sum.phi4, %exit1] to %sum.phi3 = phi i32 [%sum.phi4, %innerBB2], [%sum.phi1, %innerBB1]
  move 0 to %k.phi = phi i32 [%addtmp4, %innerBB2], [0, %innerBB1]
  br label %header2

innerBB6:				; preds = %then2, 
  move %sum.phi5 = phi i32 [%addtmp3, %innerBB3], [%sum.phi3, %header2] to %sum.phi4 = phi i32 [%addtmp3, %innerBB9], [%sum.phi5, %innerBB6]
  br label %exit3

innerBB2:				; preds = %exit3, 
  move %sum.phi4 = phi i32 [%addtmp3, %innerBB9], [%sum.phi5, %innerBB6] to %sum.phi3 = phi i32 [%sum.phi4, %innerBB2], [%sum.phi1, %innerBB1]
  move %addtmp4 = add i32 %k.phi, 1 to %k.phi = phi i32 [%addtmp4, %innerBB2], [0, %innerBB1]
  br label %header2

innerBB7:				; preds = %then2, 
  move 0 to %n.phi = phi i32 [%addtmp1, %header4], [0, %innerBB4], [0, %innerBB5], [0, %innerBB8], [0, %innerBB7]
  br label %guard4

innerBB3:				; preds = %latch2, 
  move %addtmp3 = add i32 %sum.phi5, 1 to %sum.phi5 = phi i32 [%addtmp3, %innerBB3], [%sum.phi3, %header2]
  move %addtmp = add i32 %m.phi, 1 to %m.phi = phi i32 [%addtmp, %innerBB3], [0, %header2]
  br label %header3

innerBB9:				; preds = %latch2, 
  move %addtmp3 = add i32 %sum.phi5, 1 to %sum.phi4 = phi i32 [%addtmp3, %innerBB9], [%sum.phi5, %innerBB6]
  br label %exit3

innerBB8:				; preds = %still, 
  move 0 to %n.phi = phi i32 [%addtmp1, %header4], [0, %innerBB4], [0, %innerBB5], [0, %innerBB8], [0, %innerBB7]
  br label %guard4

innerBB5:				; preds = %then, 
  move 0 to %n.phi = phi i32 [%addtmp1, %header4], [0, %innerBB4], [0, %innerBB5], [0, %innerBB8], [0, %innerBB7]
  br label %guard4

innerBB4:				; preds = %header3, 
  move 0 to %n.phi = phi i32 [%addtmp1, %header4], [0, %innerBB4], [0, %innerBB5], [0, %innerBB8], [0, %innerBB7]
  br label %guard4

innerBB:				; preds = %latch, 
  move %sum.phi1 = phi i32 [%sum.phi, %header], [%sum.phi4, %exit1] to %sum.phi = phi i32 [%sum.phi1, %innerBB], [0, %entry]
  move %addtmp6 = add i32 %i.phi, 1 to %i.phi = phi i32 [%addtmp6, %innerBB], [0, %entry]
  br label %header

}


