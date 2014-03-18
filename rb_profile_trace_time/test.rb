$:.unshift 'ext'
require 'rb_profile_trace'

class A
  def to_string
    sleep(0.00004)
  end
end

a = A.new

ProfileTrace.start
1_00_000.times { a.to_string }
ProfileTrace.stop

ProfileTrace.print
