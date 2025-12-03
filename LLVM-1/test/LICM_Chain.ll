define i32 @licm_chain_test(i32 %a) {
entry:
  %X = mul i32 %a, 2  ; Loop-invariant (uses argument %a)
  br label %loop.header

loop.header:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop.body ]
  %A = add i32 %X, 3    ; INVARIANT: Uses %X (from outside)
  %B = sub i32 %A, 1    ; INVARIANT: Uses %A (now in InvariantSet)
  
  %i.next = add i32 %i, 1
  %cmp = icmp slt i32 %i.next, 10
  br i1 %cmp, label %loop.body, label %loop.exit
  
loop.body:
  br label %loop.header

loop.exit:
  ret i32 %B
}