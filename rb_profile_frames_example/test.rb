$:.unshift 'ext'
require 'rb_profile_trace'

class B
  def a
  end
end

class A < B
end

ProfileTrace.start
A.new.a
ProfileTrace.stop
