define void @ive_gep_test(ptr %array_ptr) {
entry:
  br label %loop.header

loop.header:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop.header ] ; Basic IV
  
  ; Derived IV: %addr.derived = Base + 4*i (assuming i32 is 4 bytes)
  %addr.derived = getelementptr inbounds i32, ptr %array_ptr, i32 %i 

  %val = load i32, ptr %addr.derived ; Use of the derived IV
  
  %i.next = add i32 %i, 1
  %cmp = icmp slt i32 %i.next, 10
  br i1 %cmp, label %loop.header, label %loop.exit

loop.exit:
  ret void
}