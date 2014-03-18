$:.unshift 'ext'
require 'rb_profile_trace'

class A
  def to_string
  end
end

a = A.new

ProfileTrace.start
1_00_000.times { a.to_string }
ProfileTrace.stop

ProfileTrace.print
