; ModuleID = 'output_mem2reg.ll'
source_filename = "test/loop_sum.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx13.0.0"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @sum_plus_tripled_val(ptr noundef %0, i32 noundef %1, i32 noundef %2) #0 {
  %4 = mul nsw i32 %1, 3
  br label %5

5:                                                ; preds = %13, %3
  %.01 = phi i32 [ 0, %3 ], [ %12, %13 ]
  %.0 = phi i32 [ 0, %3 ], [ %14, %13 ]
  %6 = icmp slt i32 %.0, %2
  br i1 %6, label %7, label %15

7:                                                ; preds = %5
  %8 = sext i32 %.0 to i64
  %9 = getelementptr inbounds i32, ptr %0, i64 %8
  %10 = load i32, ptr %9, align 4
  %11 = add nsw i32 %.01, %10
  %12 = add nsw i32 %11, %4
  br label %13

13:                                               ; preds = %7
  %14 = add nsw i32 %.0, 1
  br label %5, !llvm.loop !10

15:                                               ; preds = %5
  %.01.lcssa = phi i32 [ %.01, %5 ]
  ret i32 %.01.lcssa
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 {
  %1 = alloca [4 x i32], align 4
  %2 = bitcast ptr %1 to ptr
  call void @llvm.memset.p0.i64(ptr align 4 %2, i8 0, i64 16, i1 false)
  %3 = getelementptr inbounds [4 x i32], ptr %1, i64 0, i64 0
  %4 = call i32 @sum_plus_tripled_val(ptr noundef %3, i32 noundef 2, i32 noundef 4)
  %5 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %4)
  ret i32 0
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "min-legal-vector-width"="0" "no-trapping-math"="true" "probe-stack"="__chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }
attributes #1 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "probe-stack"="__chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: write) }

!llvm.module.flags = !{!0, !1, !2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 13, i32 3]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 8, !"branch-target-enforcement", i32 0}
!3 = !{i32 8, !"sign-return-address", i32 0}
!4 = !{i32 8, !"sign-return-address-all", i32 0}
!5 = !{i32 8, !"sign-return-address-with-bkey", i32 0}
!6 = !{i32 8, !"PIC Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 1}
!8 = !{i32 7, !"frame-pointer", i32 1}
!9 = !{!"Apple clang version 14.0.3 (clang-1403.0.22.14.1)"}
!10 = distinct !{!10, !11}
!11 = !{!"llvm.loop.mustprogress"}
