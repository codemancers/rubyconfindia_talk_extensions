klass, meth = ENV["METHOD"].split("#")

COUNTER = lambda do
  @count ||= 0
  @count += 1
end

string = <<-EOF
class #{klass}
  alias_method :old_method, :#{meth.to_sym}
  def #{meth}
    COUNTER.call
    self.old_method
  end
end
EOF

eval(string)

at_exit { puts "#{ENV['METHOD']} was called #{@count} times" }
