count = lambda do
  @count ||= 0
  @count += 1
end


trace = TracePoint.new(:c_call) do |tp|
  if tp.method_id  == :to_s
    count.call
  end
end

trace.enable

at_exit do
  puts "The method Fixnum#to_s was called #{@count} times"
  trace.disable
end
