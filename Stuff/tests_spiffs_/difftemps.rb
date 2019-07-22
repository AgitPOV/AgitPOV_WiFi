#!/usr/bin/env ruby
f=File.open ARGV[0],"r"
thresh=Integer(ARGV[1])
ot=nil; i=0; oi=0
while line=f.gets
  a=line.split(/\s+/)
  break if a.length<=1
  t=Integer(a[4])
  if ot and t-ot>=thresh
    printf "t=%6d ot=%6d diff=%4d ligne=%6d lignediff=%4d\n", t, ot, t-ot, i, i-oi
    oi=i
  end
  ot=t
  i+=1
end

