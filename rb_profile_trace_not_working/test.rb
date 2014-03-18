$:.unshift 'ext'
require 'rb_profile_trace'

ProfileTrace.start
1_00_000.times { |number| number.to_s }
ProfileTrace.stop
