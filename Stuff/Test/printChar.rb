#!/usr/bin/ruby
while line=gets
  /\{(.*)\}/.match(line) or raise "doit trouver {}"
  a=$1.split(/,/).map{|x|x.to_i}
  for y in 0..11
    for x in 0..6
      print "·#"[(a[x]>>y)&1] if a[x]!=-1
    end
    puts
  end
  puts
end

