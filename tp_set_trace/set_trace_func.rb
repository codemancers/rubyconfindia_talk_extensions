count = lambda do
  @count ||= 0
  @count += 1
end

set_trace_func proc { |event, _, _, id, _, _|
  if id == :to_s && event == "c-call"
    count.call
  end
}

at_exit do
  puts "The method Fixnum#to_s was called #{@count} times"
end
